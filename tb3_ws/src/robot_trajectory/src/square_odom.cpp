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

        first_odom_received_ = false;
        current_distance_ = 0.0;
        angle_diff_ = 0.0;

        timer_ = this->create_wall_timer(500ms, std::bind(&SquareOdomNode::display_metrics, this));
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        // Obtener el Yaw actual usando TF2
        tf2::Quaternion q(
            msg->pose.pose.orientation.x,
            msg->pose.pose.orientation.y,
            msg->pose.pose.orientation.z,
            msg->pose.pose.orientation.w);
        tf2::Matrix3x3 m(q);
        double r, p, yaw;
        m.getRPY(r, p, yaw);

        if (!first_odom_received_) {
            initial_x_ = msg->pose.pose.position.x;
            initial_y_ = msg->pose.pose.position.y;
            initial_theta_ = yaw;
            first_odom_received_ = true;
        }

        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;
        current_theta_ = yaw;

        // Cálculo de distancia (Versión 6)
        current_distance_ = std::sqrt(std::pow(current_x_ - initial_x_, 2) + std::pow(current_y_ - initial_y_, 2));

        // 1. Calcular la diferencia entre el ángulo inicial y el actual (Versión 7)
        angle_diff_ = current_theta_ - initial_theta_;

        // Normalización del ángulo (opcional pero recomendado para evitar saltos de 2PI)
        while (angle_diff_ < -M_PI) angle_diff_ += 2.0 * M_PI;
        while (angle_diff_ > M_PI) angle_diff_ -= 2.0 * M_PI;
    }

    void display_metrics() {
        if (first_odom_received_) {
            // 2. Mostrar la diferencia de ángulo
            RCLCPP_INFO(this->get_logger(), "------------------------------------");
            RCLCPP_INFO(this->get_logger(), "Distancia recorrida: [%.3f m]", current_distance_);
            RCLCPP_INFO(this->get_logger(), "Diferencia angular: [%.3f rad] (%.1f°)", 
                        angle_diff_, angle_diff_ * (180.0/M_PI));
        }
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;

    bool first_odom_received_;
    double current_x_, current_y_, current_theta_;
    double initial_x_, initial_y_, initial_theta_;
    double current_distance_;
    double angle_diff_; // Variable Versión 7
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SquareOdomNode>());
    rclcpp::shutdown();
    return 0;
}
