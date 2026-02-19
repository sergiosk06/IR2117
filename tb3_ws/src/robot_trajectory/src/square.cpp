#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("square_node");
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    
    // --- 1. DECLARAR PARÁMETROS (Basado en tus imágenes) ---
    node->declare_parameter("linear_speed", 0.5); // Velocidad lineal aumentada según tu imagen
    node->declare_parameter("angular_speed", 0.785); // Aprox pi/4 para giros más rápidos
    node->declare_parameter("line_steps", 1000); // Pasos para la distancia
    node->declare_parameter("turn_steps", 1000); // Pasos para el ángulo
    
    geometry_msgs::msg::Twist message;
    rclcpp::WallRate loop_rate(10ms);

    // --- 2. OBTENER VALORES ---
    double linear_speed = node->get_parameter("linear_speed").as_double();
    double angular_speed = node->get_parameter("angular_speed").as_double();
    int line_steps = node->get_parameter("line_steps").as_int();
    int turn_steps = node->get_parameter("turn_steps").as_int();

    for (int i = 0; i < 4; i++) {
        int n = 0;
        int total_steps = line_steps + turn_steps;

        while (rclcpp::ok() && (n < total_steps)) {
            if (n < line_steps) {
                // FASE RECTA: Usa el parámetro de velocidad lineal
                message.linear.x = linear_speed;
                message.angular.z = 0.0;
            } else {
                // FASE GIRO: Usa el parámetro de velocidad angular
                message.linear.x = 0.0;
                message.angular.z = angular_speed;
            }
            
            publisher->publish(message);
            rclcpp::spin_some(node);
            loop_rate.sleep();
            n++;
        }
    }

    // Detener al robot al final
    message.linear.x = 0.0;
    message.angular.z = 0.0;
    publisher->publish(message);

    rclcpp::shutdown();
    return 0;
}
