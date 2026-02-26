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

        // Inicializar variables
        current_x_ = 0.0;
        current_y_ = 0.0;
        current_theta_ = 0.0; // Ángulo Yaw

        timer_ = this->create_wall_timer(500ms, std::bind(&SquareOdomNode::display_data, this));
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        // Guardar posición X e Y
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;

        // 1. Convertir Cuaternión a ángulo Theta (Yaw)
        tf2::Quaternion q(
            msg->pose.pose.orientation.x,
            msg->pose.pose.orientation.y,
            msg->pose.pose.orientation.z,
            msg->pose.pose.orientation.w);
        
        tf2::Matrix3x3 m(q);
        double roll, pitch, yaw;
        m.getRPY(roll, pitch, yaw);
        
        current_theta_ = yaw; // Almacenar en variable global del nodo
    }

    void display_data() {
        // 2. Mostrar todas las variables almacenadas
        RCLCPP_INFO(this->get_logger(), "ESTADO ACTUAL:");
        RCLCPP_INFO(this->get_logger(), "Pos: [%.2f, %.2f] | Ang: [%.2f rad] (%.1f°)", 
                    current_x_, current_y_, current_theta_, current_theta_ * (180.0/M_PI));
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;

    // Variables globales del nodo
    double current_x_;
    double current_y_;
    double current_theta_; 
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SquareOdomNode>());
    rclcpp::shutdown();
    return 0;
}
