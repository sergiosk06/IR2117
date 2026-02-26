#include <chrono>
#include <memory>
#include <cmath>
#include <thread> // Necesario para ejecutar la trayectoria sin bloquear los callbacks
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>

using namespace std::chrono_literals;

class SquareOdomNode : public rclcpp::Node {
public:
    SquareOdomNode() : Node("square_odom") {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
        subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 10, std::bind(&SquareOdomNode::odom_callback, this, std::placeholders::_1));

        this->declare_parameter("linear_speed", 0.15);
        this->declare_parameter("square_length", 1.0);

        first_odom_received_ = false;
        current_distance_ = 0.0;
        
        // Iniciamos la trayectoria en un hilo separado para no bloquear el spin del nodo
        std::thread(&SquareOdomNode::execute_trajectory, this).detach();
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        if (!first_odom_received_) {
            initial_x_ = msg->pose.pose.position.x;
            initial_y_ = msg->pose.pose.position.y;
            first_odom_received_ = true;
        }

        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;

        // Cálculo de distancia euclidiana (Versión 6)
        current_distance_ = std::sqrt(std::pow(current_x_ - initial_x_, 2) + std::pow(current_y_ - initial_y_, 2));
    }

    void execute_trajectory() {
        // Esperar a recibir la primera lectura de odometría
        while (!first_odom_received_ && rclcpp::ok()) {
            std::this_thread::sleep_for(100ms);
        }

        double linear_speed = this->get_parameter("linear_speed").as_double();
        double square_length = this->get_parameter("square_length").as_double();
        geometry_msgs::msg::Twist msg;

        RCLCPP_INFO(this->get_logger(), "Iniciando avance por distancia...");

        // --- LÓGICA VERSIÓN 8: Bucle basado en distancia ---
        rclcpp::Rate loop_rate(20); 
        while (rclcpp::ok() && current_distance_ < square_length) {
            msg.linear.x = linear_speed;
            msg.angular.z = 0.0;
            publisher_->publish(msg);
            
            RCLCPP_INFO(this->get_logger(), "Distancia: %.2f / %.2f", current_distance_, square_length);
            loop_rate.sleep();
        }

        // Parar el robot al llegar
        msg.linear.x = 0.0;
        publisher_->publish(msg);
        RCLCPP_INFO(this->get_logger(), "¡Objetivo alcanzado! Distancia final: %.2f", current_distance_);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    
    bool first_odom_received_;
    double current_x_, current_y_;
    double initial_x_, initial_y_;
    double current_distance_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SquareOdomNode>());
    rclcpp::shutdown();
    return 0;
}
