#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  
  // Creación del nodo llamado "avoidance"
  auto node = rclcpp::Node::make_shared("avoidance");
  
  // Publicador en el tópico /cmd_vel con un histórico de 10 mensajes
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  
  geometry_msgs::msg::Twist message;
  rclcpp::WallRate loop_rate(50ms);

  while (rclcpp::ok()) {
    // En la versión 0, el mensaje se publica vacío (velocidad 0) o con valores por defecto
    publisher->publish(message);
    
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  rclcpp::shutdown();
  return 0;
}
