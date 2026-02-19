#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("square_node");
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    
    // 1. DEFINIR PARÁMETROS (Como en la imagen)
    // Se eliminó "speed" para usar nombres más específicos
    node->declare_parameter("linear_speed", 0.1);
    node->declare_parameter("angular_speed", 3.1416 / 20.0);
    // Parámetros adicionales para corregir distancia y ángulo sin recompilar
    node->declare_parameter("line_iterations", 1000);
    node->declare_parameter("turn_iterations", 1000);

    geometry_msgs::msg::Twist message;
    rclcpp::WallRate loop_rate(10ms);

    // 2. OBTENER VALORES DE LOS PARÁMETROS
    double linear_speed = node->get_parameter("linear_speed").as_double();
    double angular_speed = node->get_parameter("angular_speed").as_double();
    int line_iterations = node->get_parameter("line_iterations").as_int();
    int turn_iterations = node->get_parameter("turn_iterations").as_int();

    // Ciclo para los 4 lados
    for (int i = 0; i < 4; i++) {
        int n = 0;
        int total_iterations = line_iterations + turn_iterations;

        // Bucle unificado (Fix: distance/angle -> number of iterations)
        while (rclcpp::ok() && (n < total_iterations)) {
            if (n < line_iterations) {
                // FASE RECTA
                message.linear.x = linear_speed;
                message.angular.z = 0.0;
            } else {
                // FASE GIRO
                message.linear.x = 0.0;
                message.angular.z = angular_speed;
            }
            
            publisher->publish(message);
            rclcpp::spin_some(node);
            loop_rate.sleep();
            n++;
        }
    }

    // Detener al robot al finalizar
    message.linear.x = 0.0;
    message.angular.z = 0.0;
    publisher->publish(message);

    rclcpp::shutdown();
    return 0;
}
    }

    // Detener al robot al final
    message.linear.x = 0.0;
    message.angular.z = 0.0;
    publisher->publish(message);

    rclcpp::shutdown();
    return 0;
}
