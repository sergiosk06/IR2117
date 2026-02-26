#include <chrono>
#include <memory>
#include <cmath> // Para sqrt y pow
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

        // Timer para mostrar el cálculo de distancia
        timer_ = this->create_wall_timer(500ms, std::bind(&SquareOdomNode::display_metrics, this));
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        // Extraer Yaw
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

        // 1. Calcular la distancia euclidiana entre la posición inicial y la actual
        current_distance_ = std::sqrt(
            std::pow(current_x_ - initial_x_, 2) + 
            std::pow(current_y_ - initial_y_, 2)
        );
    }

    void display_metrics() {
        if (first_odom_received_) {
            // 2. Mostrar la distancia calculada
            RCLCPP_INFO(this->get_logger(), ">> Distancia desde el origen: [%.3f m]", current_distance_);
            RCLCPP_INFO(this->get_logger(), ">> Ángulo actual: [%.2f°]", current_theta_ * (180.0/M_PI));
        }
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;

    bool first_odom_received_;
    double current_x_, current_y_, current_theta_;
    double initial_x_, initial_y_, initial_theta_;
    double current_distance_; // Variable Versión 6
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SquareOdomNode>());
    rclcpp::shutdown();
    return 0;
}
