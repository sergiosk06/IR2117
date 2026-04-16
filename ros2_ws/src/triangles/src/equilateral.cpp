#include <chrono>
#include <functional>
#include <memory>
#include <cmath>
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

using namespace std::chrono_literals;

class EquilateralNode : public rclcpp::Node {
public:
    EquilateralNode() : Node("equilateral"), side_count_(0), state_(DRIVING) {
        // Declarar parámetros
        this->declare_parameter("angular_speed", 0.5);
        this->declare_parameter("linear_speed", 1.0);
        this->declare_parameter("side_length", 2.0);

        // Obtener valores iniciales
        double linear_speed = this->get_parameter("linear_speed").as_double();
        double angular_speed = this->get_parameter("angular_speed").as_double();
        double side_length = this->get_parameter("side_length").as_double();

        // Publicamos directamente en el tópico de la tortuga
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("turtle1/cmd_vel", 10);

        // Calcular tiempos con mayor precisión
        drive_duration_ = std::chrono::duration<double>(side_length / linear_speed);
        turn_duration_ = std::chrono::duration<double>((2.0 * M_PI / 3.0) / angular_speed);

        // Timer a 50Hz (20ms) para mejorar la precisión del timing
        timer_ = this->create_wall_timer(20ms, std::bind(&EquilateralNode::control_loop, this));
        start_time_ = this->now();
    }

private:
    enum State { DRIVING, TURNING, FINISHED };

    void control_loop() {
        auto now = this->now();
        auto elapsed = now - start_time_;
        geometry_msgs::msg::Twist msg;

        if (side_count_ < 3) {
            if (state_ == DRIVING) {
                if (elapsed.seconds() < drive_duration_.count()) {
                    msg.linear.x = this->get_parameter("linear_speed").as_double();
                } else {
                    // Parada momentánea antes de girar
                    publish_stop();
                    state_ = TURNING;
                    start_time_ = this->now(); 
                }
            } else if (state_ == TURNING) {
                if (elapsed.seconds() < turn_duration_.count()) {
                    msg.angular.z = this->get_parameter("angular_speed").as_double();
                } else {
                    // Parada momentánea antes de avanzar
                    publish_stop();
                    state_ = DRIVING;
                    start_time_ = this->now();
                    side_count_++;
                }
            }
            publisher_->publish(msg);
        } else {
            state_ = FINISHED;
            publish_stop();
            timer_->cancel();
            RCLCPP_INFO(this->get_logger(), "Triángulo completado.");
        }
    }

    void publish_stop() {
        geometry_msgs::msg::Twist stop_msg;
        publisher_->publish(stop_msg);
    }

    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Time start_time_;
    std::chrono::duration<double> drive_duration_, turn_duration_;
    int side_count_;
    State state_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<EquilateralNode>());
    rclcpp::shutdown();
    return 0;
}
