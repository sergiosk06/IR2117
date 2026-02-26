#include <chrono>
#include <memory>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp" //

using namespace std::chrono_literals;

class SquareOdomNode : public rclcpp::Node {
public:
    SquareOdomNode() : Node("square_odom") {
        // Publicador para mover el robot
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

        // 1. Crear la suscripción al tópico /odom
        subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 
            10, 
            std::bind(&SquareOdomNode::odom_callback, this, std::placeholders::_1) //
        );

        // Parámetros
        this->declare_parameter("linear_speed", 0.2);   
        this->declare_parameter("angular_speed", 0.8); 
        this->declare_parameter("square_length", 1.0); 

        // Temporizador para ejecutar el movimiento cuadrado
        timer_ = this->create_wall_timer(100ms, std::bind(&SquareOdomNode::run_square, this));
    }

private:
    // 2. Callback para mostrar los valores (x, y) de la posición
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) const {
        double x = msg->pose.pose.position.x; //
        double y = msg->pose.pose.position.y; //
        RCLCPP_INFO(this->get_logger(), "Posición actual: x=[%.2f], y=[%.2f]", x, y);
    }

    void run_square() {
        // Aquí iría la lógica de movimiento (opcionalmente puedes mantener 
        // tu bucle for original, pero en ROS2 es mejor usar máquinas de estado)
        static int step = 0;
        if (step == 0) {
            execute_trajectory();
            step++;
        }
    }

    void execute_trajectory() {
        double linear_speed = this->get_parameter("linear_speed").as_double();
        double angular_speed = this->get_parameter("angular_speed").as_double();
        double square_length = this->get_parameter("square_length").as_double();
        
        geometry_msgs::msg::Twist msg;
        const double dt = 0.01;
        rclcpp::WallRate loop_rate(10ms);

        int line_steps = static_cast<int>(square_length / (dt * linear_speed));
        int turn_steps = static_cast<int>(M_PI_2 / (dt * angular_speed));

        for (int i = 0; i < 4 && rclcpp::ok(); i++) {
            // Avanzar
            for (int n = 0; n < line_steps && rclcpp::ok(); n++) {
                msg.linear.x = linear_speed;
                msg.angular.z = 0.0;
                publisher_->publish(msg);
                rclcpp::spin_some(this->get_node_base_interface());
                loop_rate.sleep();
            }
            // Girar
            for (int n = 0; n < turn_steps && rclcpp::ok(); n++) {
                msg.linear.x = 0.0;
                msg.angular.z = angular_speed;
                publisher_->publish(msg);
                rclcpp::spin_some(this->get_node_base_interface());
                loop_rate.sleep();
            }
        }
        // Parar
        msg.linear.x = 0.0;
        msg.angular.z = 0.0;
        publisher_->publish(msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SquareOdomNode>());
    rclcpp::shutdown();
    return 0;
}
