#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("square_node");
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    
    // 1. DEFINIR PARÁMETROS (Imagen 1 y 2)
    // Cambiamos "speed" por "linear_speed" y añadimos "angular_speed"
    node->declare_parameter("linear_speed", 0.1);
    node->declare_parameter("angular_speed", 3.1416 / 20.0);
    
    // Parámetros para el número de iteraciones (Imagen 3)
    // Ajusta estos valores para "Fix: distance/angle"
    node->declare_parameter("line_steps", 1000); 
    node->declare_parameter("turn_steps", 1000);

    geometry_msgs::msg::Twist message;
    rclcpp::WallRate loop_rate(10ms); // Frecuencia de 100Hz

    // 2. OBTENER VALORES (Imagen 1)
    double linear_speed = node->get_parameter("linear_speed").as_double();
    double angular_speed = node->get_parameter("angular_speed").as_double();
    int line_steps = node->get_parameter("line_steps").as_int();
    int turn_steps = node->get_parameter("turn_steps").as_int();

    for (int i = 0; i < 4; i++) {
        int n = 0;
        int total_cycle = line_steps + turn_steps;

        // Bucle único por lado (Imagen 1: while (n < 2000))
        while (rclcpp::ok() && (n < total_cycle)) {
            if (n < line_steps) {
                // FASE: MOVE FORWARD 1m (Imagen 4)
                message.linear.x = linear_speed;
                message.angular.z = 0.0;
            } else {
                // FASE: TURNING 90º (Imagen 5)
                message.linear.x = 0.0;
                message.angular.z = angular_speed;
            }
            
            publisher->publish(message);
            rclcpp::spin_some(node);
            loop_rate.sleep();
            n++;
        }
    }

    // Detener al robot al terminar
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
