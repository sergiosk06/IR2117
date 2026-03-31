#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp" // Cambio de tipo de mensaje
#include <iostream>

// Función callback actualizada para recibir LaserScan
void callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
    // Por ahora solo imprimimos que recibimos datos del láser
    std::cout << "He recibido un escaneo con " << msg->ranges.size() << " puntos." << std::endl;
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);

    // 1. Nombre del nodo cambiado a "detector"
    auto node = rclcpp::Node::make_shared("detector");

    // 2. Suscripción actualizada: tipo LaserScan, tópico "/scan"
    auto subscription = node->create_subscription<sensor_msgs::msg::LaserScan>(
        "scan", 
        10, 
        callback
    );

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
