#include <chrono>
#include <functional>
#include <memory>
#include <cmath>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

using namespace std::chrono_literals;

class IsoscelesNode : public rclcpp::Node {
public:
    IsoscelesNode() : Node("isosceles"), step_(0) {
        this->declare_parameter("angular_speed", 0.5);
        this->declare_parameter("linear_speed", 1.0);
        this->declare_parameter("base_length", 1.0);
        this->declare_parameter("lateral_length", 2.0);

        double base = this->get_parameter("base_length").as_double();
        double lateral = this->get_parameter("lateral_length").as_double();
        double lin_speed = this->get_parameter("linear_speed").as_double();
        double ang_speed = this->get_parameter("angular_speed").as_double();

        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

        // Cálculos trigonométricos (Ley del Coseno)
        // Ángulo entre la base y el lado lateral
        double angle_base_lateral = std::acos(base / (2.0 * lateral));
        // Ángulo en el vértice superior
        double angle_vertex = std::acos(1.0 - (base * base) / (2.0 * lateral * lateral));

        // Tiempos de movimiento
        t_base_ = base / lin_speed;
        t_lateral_ = lateral / lin_speed;
        
        // Tiempos de giro (usamos el suplementario para el giro exterior)
        t_turn_base_ = (M_PI - angle_base_lateral) / ang_speed;
        t_turn_vertex_ = (M_PI - angle_vertex) / ang_speed;

        timer_ = this->create_wall_timer(20ms, std::bind(&IsoscelesNode::control_loop, this));
        start_time_ = this->now();
    }

private:
    void control_loop() {
        auto elapsed = (this->now() - start_time_).seconds();
        geometry_msgs::msg::Twist msg;
        double lin = this->get_parameter("linear_speed").as_double();
        double ang = this->get_parameter("angular_speed").as_double();

        // Secuencia: Base -> Giro1 -> Lateral1 -> Giro2 -> Lateral2
        if (step_ == 0) { // Avanzar Base
            if (elapsed < t_base_) msg.linear.x = lin;
            else next_step();
        } else if (step_ == 1) { // Giro Base-Lateral
            if (elapsed < t_turn_base_) msg.angular.z = ang;
            else next_step();
        } else if (step_ == 2) { // Avanzar Lateral 1
            if (elapsed < t_lateral_) msg.linear.x = lin;
            else next_step();
        } else if (step_ == 3) { // Giro Vértice
            if (elapsed < t_turn_vertex_) msg.angular.z = ang;
            else next_step();
        } else if (step_ == 4) { // Avanzar Lateral 2
            if (elapsed < t_lateral_) msg.linear.x = lin;
            else { 
                publish_stop();
                timer_->cancel();
                RCLCPP_INFO(this->get_logger(), "Isósceles completado.");
            }
        }
        if (step_ < 5) publisher_->publish(msg);
    }

    void next_step() {
        publish_stop();
        step_++;
        start_time_ = this->now();
    }

    void publish_stop() {
        geometry_msgs::msg::Twist stop_msg;
        publisher_->publish(stop_msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_time_;
    double t_base_, t_lateral_, t_turn_base_, t_turn_vertex_;
    int step_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<IsoscelesNode>());
    rclcpp::shutdown();
    return 0;
}
