 #include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "geometry_msgs/msg/twist.hpp"


using namespace std::chrono_literals;

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
			message_cmd_vel.linear.x = 0.0;
			message_cmd_vel.angular.z = 0.5;
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




