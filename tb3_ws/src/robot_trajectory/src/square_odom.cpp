#include <chrono>
#include <memory>
#include <cmath>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"

using namespace std::chrono_literals;

class SquareOdomNode : public rclcpp::Node {
public:
    SquareOdomNode() : Node("square_odom") {
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);

        subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 
            10, 
            std::bind(&SquareOdomNode::odom_callback, this, std::placeholders::_1)
        );

        this->declare_parameter("linear_speed", 0.2);   
        this->declare_parameter("angular_speed", 0.8); 
        this->declare_parameter("square_length", 1.0); 

        // Inicializar variables de posición
        current_x_ = 0.0;
        current_y_ = 0.0;

        // Timer para ejecutar la lógica y mostrar variables
        timer_ = this->create_wall_timer(500ms, std::bind(&SquareOdomNode::display_and_run, this));
    }

private:
    // 1. Almacenar los valores de (x,y) en variables de la clase (globales al nodo)
    void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
        current_x_ = msg->pose.pose.position.x;
        current_y_ = msg->pose.pose.position.y;
    }

    // 2. Mostrar las variables desde una función distinta al callback
    void display_and_run() {
        RCLCPP_INFO(this->get_logger(), "--- ODOMETRÍA ALMACENADA ---");
        RCLCPP_INFO(this->get_logger(), "X: [%.3f] | Y: [%.3f]", current_x_, current_y_);
        
        // Aquí podrías añadir lógica que use estas variables para decidir cuándo girar
        static bool started = false;
        if (!started) {
            std::thread(&SquareOdomNode::execute_trajectory, this).detach();
            started = true;
        }
    }

    void execute_trajectory() {
        // ... (mismo código de trayectoria que tenías antes)
    }

    // Miembros de la clase
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
    rclcpp::TimerBase::SharedPtr timer_;

    // Variables globales del nodo para almacenar posición
    double current_x_;
    double current_y_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SquareOdomNode>());
    rclcpp::shutdown();
    return 0;
}
