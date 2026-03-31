#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "example_interfaces/msg/bool.hpp"

class ObstacleDetector : public rclcpp::Node
{
public:
    ObstacleDetector() : Node("detector")
    {
        // Parámetros para hacer el nodo genérico
        this->declare_parameter("obs_threshold", 1.0);     // Distancia de detección
        this->declare_parameter("obs_angle_min", -0.3927); // Ángulo inicial (rad)
        this->declare_parameter("obs_angle_max", 0.3927);  // Ángulo final (rad)

        publisher_ = this->create_publisher<example_interfaces::msg::Bool>("obstacle", 10);
        subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "scan", 10, std::bind(&ObstacleDetector::scan_callback, this, std::placeholders::_1));
    }

private:
    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
    {
        float threshold = this->get_parameter("obs_threshold").as_double();
        float angle_min = this->get_parameter("obs_angle_min").as_double();
        float angle_max = this->get_parameter("obs_angle_max").as_double();

        auto obstacle_msg = example_interfaces::msg::Bool();
        obstacle_msg.data = false;

        // Iterar sobre los rangos del láser
        for (size_t i = 0; i < msg->ranges.size(); ++i) {
            float angle = msg->angle_min + i * msg->angle_increment;
            
            // Verificar si el ángulo está dentro del rango configurado
            if (angle >= angle_min && angle <= angle_max) {
                if (msg->ranges[i] < threshold) {
                    obstacle_msg.data = true;
                    break; // Optimización: salir al encontrar el primer obstáculo 
                }
            }
        }

        publisher_->publish(obstacle_msg);
    }

    rclcpp::Publisher<example_interfaces::msg::Bool>::SharedPtr publisher_;
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ObstacleDetector>());
    rclcpp::shutdown();
    return 0;
}
