 #include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/twist.hpp"


using namespace std::chrono_literals;

float min_range09 = std::numeric_limits<float>::max();
float min_range3509 = std::numeric_limits<float>::max();

void MinRanges(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
	// Calcular el mínimo de ranges[0..9]
	min_range09 = std::numeric_limits<float>::max();
	for (int i = 0; i < 10; ++i) {
    	if (msg->ranges[i] < min_range09) {
        	min_range09 = msg->ranges[i];
    	}
	}

	// Calcular el mínimo de ranges[350..359]
	min_range3509 = std::numeric_limits<float>::max();
	for (int i = 350; i < 360; ++i) {
    	if (msg->ranges[i] < min_range3509) {
        	min_range3509 = msg->ranges[i];
    	}
	}

	// Mostrar los mínimos en pantalla
	std::cout << "Mínimo delantero: " << min_range09 << std::endl;
	std::cout << "Mínimo trasero: " << min_range3509 << std::endl;
}


int main(int argc, char * argv[])
{
	rclcpp::init(argc, argv);
	auto node = rclcpp::Node::make_shared("wandering");
	auto publisher_cmd_vel = node->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
	geometry_msgs::msg::Twist message_cmd_vel;
    
	message_cmd_vel.linear.x = 0.5;
	message_cmd_vel.angular.z = 0.0;
    
	bool obs_detectado = false;
    
		auto subscription_scan = node->create_subscription<sensor_msgs::msg::LaserScan>(
    	"/scan", 10, [&](const sensor_msgs::msg::LaserScan::SharedPtr msg) {     
	 
        	MinRanges(msg);
       	 
        	obs_detectado = false;
        	for (int i = 0; i < 18; i++) {
			if (msg->ranges[i] < 1.0) {
			 obs_detectado = true;
			 break;
			}
        	}
    	});

	rclcpp::WallRate loop_rate(10ms);

	while (rclcpp::ok()) {
		if (obs_detectado) {
			if (min_range09 > min_range3509) {
			// Girar a la izquierda
			message_cmd_vel.linear.x = 0.0;
			message_cmd_vel.angular.z = 0.5;
			} else {
			// Girar a la derecha
			message_cmd_vel.linear.x = 0.0;
			message_cmd_vel.angular.z = -0.5;
		}
   	 } else {
   		 message_cmd_vel.linear.x = 0.5;
   		 message_cmd_vel.angular.z = 0.0;
   	 }
   	 publisher_cmd_vel->publish(message_cmd_vel);
		rclcpp::spin_some(node);
		loop_rate.sleep();
	}


	rclcpp::shutdown();
	return 0;
}




