#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "example_interfaces/msg/bool.hpp" // Asegúrate de que esta línea esté así

using namespace std::chrono_literals;

// Variables globales para los estados de los obstáculos
bool obs_front = false;
bool obs_left = false;
bool obs_right = false;

// CORRECCIÓN: El tipo de mensaje es msg::Bool (con B mayúscula)
void callback_front(const example_interfaces::msg::Bool::SharedPtr msg) {
  obs_front = msg->data;
}

void callback_left(const example_interfaces::msg::Bool::SharedPtr msg) {
  obs_left = msg->data;
}

void callback_right(const example_interfaces::msg::Bool::SharedPtr msg) {
  obs_right = msg->data;
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("avoidance");
  
  auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
  
  // CORRECCIÓN: Se han corregido los nombres de los tópicos para que no todos sean /front/obstacle
  auto subs_front = node->create_subscription<example_interfaces::msg::Bool>("/front/obstacle", 10, callback_front);
  auto subs_left = node->create_subscription<example_interfaces::msg::Bool>("/left/obstacle", 10, callback_left);
  auto subs_right = node->create_subscription<example_interfaces::msg::Bool>("/right/obstacle", 10, callback_right);
  
  geometry_msgs::msg::Twist message;
  rclcpp::WallRate loop_rate(50ms);

  while (rclcpp::ok()) {
    // Aquí iría tu lógica de la FSM (Forward, Turn, etc.)
    publisher->publish(message);
    
    rclcpp::spin_some(node);
    loop_rate.sleep();
  }

  rclcpp::shutdown();
  return 0;
}
