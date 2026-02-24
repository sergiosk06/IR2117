#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <cmath> // Necesario para M_PI_2 (Imagen 5)

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("square");
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    
    // DECLARACIÓN DE PARÁMETROS (Imagen 3 y 4)
    node->declare_parameter("linear_speed", 0.5);   
    node->declare_parameter("angular_speed", 1.2); 
    node->declare_parameter("square_length", 1.5); 
    
    geometry_msgs::msg::Twist message;
    const double dt = 0.01; // Ciclo de 10ms (Imagen 4)
    rclcpp::WallRate loop_rate(10ms); 

    // OBTENER VALORES ACTUALES
    double linear_speed = node->get_parameter("linear_speed").as_double();
    double angular_speed = node->get_parameter("angular_speed").as_double();
    double square_length = node->get_parameter("square_length").as_double();

    // CÁLCULO DE ITERACIONES SEGÚN TUS FÓRMULAS (Imagen 1 y 2)
    // line_iterations = length / (0.01 * linear_speed)
    int line_steps = static_cast<int>(square_length / (dt * linear_speed));
    
    // Para girar 90 grados exactos usando M_PI_2 (Imagen 5)
    int turn_steps = static_cast<int>(M_PI_2 / (dt * angular_speed));

    for (int i = 0; i < 4; i++) {
        // FASE: AVANZAR
        for (int n = 0; n < line_steps && rclcpp::ok(); n++) {
            message.linear.x = linear_speed;
            message.angular.z = 0.0;
            publisher->publish(message);
            rclcpp::spin_some(node);
            loop_rate.sleep();
        }

        // FASE: GIRAR
        for (int n = 0; n < turn_steps && rclcpp::ok(); n++) {
            message.linear.x = 0.0;
            message.angular.z = angular_speed;
            publisher->publish(message);
            rclcpp::spin_some(node);
            loop_rate.sleep();
        }
    }

    // Parada final
    message.linear.x = 0.0;
    message.angular.z = 0.0;
    publisher->publish(message);

    rclcpp::shutdown();
    return 0;
}
