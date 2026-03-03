#include <chrono>
#include <memory>
#include <cmath>
#include <thread>
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
        this->declare_parameter("angular_speed", 0.4);
        this->declare_parameter("square_length", 1.0);

        first_odom_received_ = false;
        current_distance_ = 0.0;
        angle_diff_ = 0.0;
        
        // Hilo principal para no bloquear el spin de ROS
        worker_thread_ = std::thread(&SquareOdomNode::execute_full_square, this);
    }

    // Destructor para asegurar que el hilo termine bien
    ~SquareOdomNode() {
        if (worker_thread_.joinable()) worker_thread_.join();
    }

private:
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        tf2::Quaternion q(
            msg->pose.pose.orientation.x, msg->pose.pose.orientation.y,
            msg->pose.pose.orientation.z, msg->pose.pose.orientation.w);
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

        // Cálculo de distancia desde el último punto de referencia
        current_distance_ = std::sqrt(std::pow(current_x_ - initial_x_, 2) + std::pow(current_y_ - initial_y_, 2));
        
        // Diferencia de ángulo normalizada
        angle_diff_ = current_theta_ - initial_theta_;
        while (angle_diff_ < -M_PI) angle_diff_ += 2.0 * M_PI;
        while (angle_diff_ > M_PI) angle_diff_ -= 2.0 * M_PI;
    }

    void execute_full_square() {
        // Esperar a tener datos de odometría
        while (rclcpp::ok() && !first_odom_received_) {
            std::this_thread::sleep_for(100ms);
        }

        double l_speed = this->get_parameter("linear_speed").as_double();
        double a_speed = this->get_parameter("angular_speed").as_double();
        double side = this->get_parameter("square_length").as_double();
        geometry_msgs::msg::Twist msg;
        rclcpp::Rate loop_rate(20);

        for (int i = 0; i < 4; i++) {
            if (!rclcpp::ok()) break;

            // --- 1. AVANZAR ---
            initial_x_ = current_x_;
            initial_y_ = current_y_;
            current_distance_ = 0.0; 

            while (rclcpp::ok() && current_distance_ < side) {
                msg.linear.x = l_speed;
                msg.angular.z = 0.0;
                publisher_->publish(msg);
                loop_rate.sleep();
            }
            stop_robot();

            // --- 2. GIRAR ---
            initial_theta_ = current_theta_;
            angle_diff_ = 0.0; 

            // Girar hasta 90 grados (PI/2)
            while (rclcpp::ok() && std::abs(angle_diff_) < (M_PI / 2.0)) { 
                msg.linear.x = 0.0;
                msg.angular.z = a_speed;
                publisher_->publish(msg);
                loop_rate.sleep();
            }
            stop_robot();
            
            RCLCPP_INFO(this->get_logger(), "Lado %d completado", i + 1);
        }
        RCLCPP_INFO(this->get_logger(), "¡Trayectoria terminada!");
    }

    void stop_robot() {
        geometry_msgs::msg::Twist stop_msg;
        publisher_->publish(stop_msg);
        std::this_thread::sleep_for(500ms); 
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    std::thread worker_thread_;
    bool first_odom_received_;
    double current_x_, current_y_, current_theta_;
    double initial_x_, initial_y_, initial_theta_;
    double current_distance_, angle_diff_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SquareOdomNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
