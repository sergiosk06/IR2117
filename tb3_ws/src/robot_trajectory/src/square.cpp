#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("square");
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    
    // --- 1. DECLARAR PARÁMETROS (Como en la imagen) ---
    // Se definen nombres descriptivos y valores por defecto
    node->declare_parameter("linear_speed", 0.1);
    node->declare_parameter("angular_speed", 3.1416 / 20.0);

    geometry_msgs::msg::Twist message;
    rclcpp::WallRate loop_rate(10ms);

    // --- 2. OBTENER VALORES DE PARÁMETROS ---
    // Esto lee los valores actuales (o los por defecto) al iniciar
    double linear_speed = node->get_parameter("linear_speed").get_parameter_value().get<double>();
    double angular_speed = node->get_parameter("angular_speed").get_parameter_value().get<double>();

    // Bucle principal para los 4 lados del cuadrado
    for (int j = 0; j < 4; j++) {
        
        int n = 0;
        // Bucle unificado de movimiento (basado en la lógica de la imagen)
        // n < 1000: Recto | 1000 <= n < 2000: Giro
        while (rclcpp::ok() && (n < 2000)) {
            
            if (n < 1000) {
                // Movimiento lineal usando el parámetro
                message.linear.x = linear_speed;
                message.angular.z = 0.0;
            } else {
                // Movimiento angular usando el parámetro
                message.linear.x = 0.0;
                message.angular.z = angular_speed;
            }
            
            publisher->publish(message);
            rclcpp::spin_some(node);
            loop_rate.sleep();
            n++;
        }

        // Parada de seguridad al terminar cada ciclo de lado/giro
        message.linear.x = 0.0;
        message.angular.z = 0.0;
        publisher->publish(message);
        rclcpp::spin_some(node);
        loop_rate.sleep();
    }

    rclcpp::shutdown();
    return 0;
}

