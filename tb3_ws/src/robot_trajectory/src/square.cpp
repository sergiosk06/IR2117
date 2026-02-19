#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("square_node");
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    
    // 1. DECLARAR PARÁMETROS (Como en tus imágenes 1 y 2)
    node->declare_parameter("linear_speed", 0.5);   // Velocidad para avanzar
    node->declare_parameter("angular_speed", 0.78); // Velocidad para girar (~pi/4 rad/s)
    
    // 2. PARÁMETROS DE ITERACIONES (Fix: distance/angle -> Imagen 3)
    // Ajustar estos números permite arreglar el cuadrado sin tocar el código
    node->declare_parameter("line_steps", 1000); 
    node->declare_parameter("turn_steps", 1000);

    geometry_msgs::msg::Twist message;
    rclcpp::WallRate loop_rate(10ms); // Frecuencia fija (100Hz)

    // 3. OBTENER VALORES ACTUALES
    double linear_speed = node->get_parameter("linear_speed").as_double();
    double angular_speed = node->get_parameter("angular_speed").as_double();
    int line_steps = node->get_parameter("line_steps").as_int();
    int turn_steps = node->get_parameter("turn_steps").as_int();

    for (int i = 0; i < 4; i++) {
        int n = 0;
        int total_cycle = line_steps + turn_steps;

        // Bucle unificado (Imagen 1: while (n < 2000))
        while (rclcpp::ok() && (n < total_cycle)) {
            if (n < line_steps) {
                // FASE: MOVE FORWARD (Imagen 4)
                message.linear.x = linear_speed;
                message.angular.z = 0.0;
            } else {
                // FASE: TURNING (Imagen 5)
                message.linear.x = 0.0;
                message.angular.z = angular_speed;
            }
            
            publisher->publish(message);
            rclcpp::spin_some(node);
            loop_rate.sleep();
            n++;
        }
    }

    // Parada final de seguridad
    message.linear.x = 0.0;
    message.angular.z = 0.0;
    publisher->publish(message);

    rclcpp::shutdown();
    return 0;
}
