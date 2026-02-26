#include <chrono>
#include <memory>
#include <cmath>
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

        // Inicializar flags
        first_odom_received_ = false;

        // Timer para mostrar datos
        timer_ = this->create_wall_timer(1s, std::bind(&SquareOdomNode::display_data, this));
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        // Convertir cuaternión a Euler para obtener Yaw
        tf2::Quaternion q(
            msg->pose.pose.orientation.x,
            msg->pose.pose.orientation.y,
            msg->pose.pose.orientation.z,
            msg->pose.pose.orientation.w);
        tf2::Matrix3x3 m(q);
        double r, p, yaw;
        m.getRPY(r, p, yaw);

        // 1. Almacenar posición y ángulo inicial (solo la primera vez)
        if (!first_odom_received_) {
            initial_x_ = msg->pose.pose.position.x;
            initial_y_ = msg->pose.pose.position.y;
            initial_theta_ = yaw;
            first_odom_received_ = true;
            RCLCPP_INFO(this->get_logger(), "Punto inicial fijado.");
        }

        // Actualizar valores actuales
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;
        current_theta_ = yaw;
    }

    void display_data() {
        if (first_odom_received_) {
            // 2. Mostrar valores iniciales vs actuales
            RCLCPP_INFO(this->get_logger(), "--- COMPARATIVA DE ESTADO ---");
            RCLCPP_INFO(this->get_logger(), "INICIAL -> X: %.2f, Y: %.2f, θ: %.2f°", 
                        initial_x_, initial_y_, initial_theta_ * (180.0/M_PI));
            RCLCPP_INFO(this->get_logger(), "ACTUAL  -> X: %.2f, Y: %.2f, θ: %.2f°", 
                        current_x_, current_y_, current_theta_ * (180.0/M_PI));
        }
    }

    // Miembros del nodo
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;

    // Variables de estado
    bool first_odom_received_;
    double current_x_, current_y_, current_theta_;
    double initial_x_, initial_y_, initial_theta_; // Variables Versión 5
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SquareOdomNode>());
    rclcpp::shutdown();
    return 0;
}
