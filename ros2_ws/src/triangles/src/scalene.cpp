#include <chrono>
#include <functional>
#include <memory>
#include <cmath>
#include <vector>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

using namespace std::chrono_literals;

class ScaleneNode : public rclcpp::Node {
public:
    ScaleneNode() : Node("scalene"), step_(0) {
        this->declare_parameter("angular_speed", 0.5);
        this->declare_parameter("linear_speed", 1.0);
        this->declare_parameter("a_side_length", 1.5);
        this->declare_parameter("b_side_length", 2.0);
        this->declare_parameter("c_side_length", 2.5);

        double a = this->get_parameter("a_side_length").as_double();
        double b = this->get_parameter("b_side_length").as_double();
        double c = this->get_parameter("c_side_length").as_double();
        double v = this->get_parameter("linear_speed").as_double();
        double w = this->get_parameter("angular_speed").as_double();

        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

        // Ley de Cosenos para ángulos internos
        double alpha = std::acos((b*b + c*c - a*a) / (2.0 * b * c));
        double beta  = std::acos((a*a + c*c - b*b) / (2.0 * a * c));
        double gamma = std::acos((a*a + b*b - c*c) / (2.0 * a * b));

        // Tiempos: Avance -> Giro (exterior)
        // Secuencia: Lado A -> Giro (PI-gamma) -> Lado B -> Giro (PI-alpha) -> Lado C
        durations_ = {
            a/v, (M_PI - gamma)/w, 
            b/v, (M_PI - alpha)/w, 
            c/v
        };

        timer_ = this->create_wall_timer(20ms, std::bind(&ScaleneNode::control_loop, this));
        start_time_ = this->now();
    }

private:
    void control_loop() {
        if (step_ >= durations_.size()) {
            publish_stop();
            timer_->cancel();
            return;
        }

        auto elapsed = (this->now() - start_time_).seconds();
        geometry_msgs::msg::Twist msg;

        if (elapsed < durations_[step_]) {
            if (step_ % 2 == 0) msg.linear.x = this->get_parameter("linear_speed").as_double();
            else msg.angular.z = this->get_parameter("angular_speed").as_double();
            publisher_->publish(msg);
        } else {
            publish_stop();
            step_++;
            start_time_ = this->now();
        }
    }

    void publish_stop() {
        geometry_msgs::msg::Twist s;
        publisher_->publish(s);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_time_;
    std::vector<double> durations_;
    size_t step_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ScaleneNode>());
    rclcpp::shutdown();
    return 0;
}
