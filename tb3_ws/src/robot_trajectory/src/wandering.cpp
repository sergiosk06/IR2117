#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
	rclcpp::init(argc, argv);
	auto node = rclcpp::Node::make_shared("wandering");
	auto publisher_cmd_vel = node->create_publisher<std_msgs::msg::String>("/cmd_vel", 10);
	std_msgs::msg::String message_cmd_vel;

	message_cmd_vel.data = "0";
    
	auto subscription_scan = node->create_subscription<sensor_msgs::msg::LaserScan>(
    	"/scan", 10, [&](const sensor_msgs::msg::LaserScan::SharedPtr msg) {
       	 
        	std::cout << "[0] " << msg->ranges[0] << std::endl;
		std::cout << "[90] " << msg->ranges[msg->ranges.size() / 4] << std::endl;
		std::cout << "[180] " << msg->ranges[msg->ranges.size() / 2] << std::endl;
		std::cout << "[270] " << msg->ranges[(msg->ranges.size() * 3) / 4] << std::endl;

    	});

	rclcpp::WallRate loop_rate(10ms);

	while (rclcpp::ok()) {
    		publisher_cmd_vel->publish(message_cmd_vel);
    		rclcpp::spin_some(node);
    		loop_rate.sleep();
	}

	rclcpp::shutdown();
	return 0;
}



