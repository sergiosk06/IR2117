#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

using namespace std::chrono_literals;

// Variables globales
float minimo_izquierda = 99999.0;   // Mínimo del lado izquierdo 
float minimo_derecha = 99999.0;     // Mínimo del lado derecho
bool obstaculo_detectado = false;
bool girando = false;

//Variables para saber a que lado girar
bool girar_izquierda = false;
bool girar_derecha = false;    

void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
    // Calcular mínimo del lado derecho
    minimo_derecha = 99999.0;
    for (int i = 0; i < 10; i++) {
        if (msg->ranges[i] < minimo_derecha) {
            minimo_derecha = msg->ranges[i];
        }
    }
    
    // Calcular mínimo del lado izquierdo
    minimo_izquierda = 99999.0;
    for (int i = 350; i < 360; i++) {
        if (msg->ranges[i] < minimo_izquierda) {
            minimo_izquierda = msg->ranges[i];
        }
    }
    
    // Detectar si hay obstáculo a menos de 1 metro
    if (minimo_derecha < 1.0) {
        obstaculo_detectado = true;
    } else if (minimo_izquierda < 1.0) {
        obstaculo_detectado = true;
    } else {
        obstaculo_detectado = false;
    }
    
    // Mostrar los mínimos
    RCLCPP_INFO(rclcpp::get_logger("wandering"), "Mínimo derecha = %f, Mínimo izquierda = %f", minimo_derecha, minimo_izquierda);
}

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("wandering");
    
     // Suscripción a /scan con SensorDataQoS()
    auto subscription = node->create_subscription<sensor_msgs::msg::LaserScan>(
        "scan", rclcpp::SensorDataQoS(), scan_callback);
    
    // Publicador en /cmd_vel
    auto publisher = node->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);
    
    // Bucle a 10ms
    rclcpp::WallRate loop_rate(10ms);
    geometry_msgs::msg::Twist message;
    
    
    while (rclcpp::ok()) {
        
        if (!obstaculo_detectado && !girando) {
            // No hay obstáculo y no estamos girando, el robot avanza
            message.linear.x = 0.1;
            message.angular.z = 0.0;
            
        } 
        
        else if (obstaculo_detectado && !girando) {
            // Hay obstáculo y no estamos girando, decidir dirección de giro
            girando = true;
            
            // Girar hacia el lado con mayor distancia mínima 
            if (minimo_izquierda > minimo_derecha) {
                girar_izquierda = true;
                girar_derecha = false;
                RCLCPP_INFO(rclcpp::get_logger("wandering"), "Girando a la IZQUIERDA");
            } 
            
            else {
                girar_izquierda = false;
                girar_derecha = true;
                RCLCPP_INFO(rclcpp::get_logger("wandering"), "Girando a la DERECHA");
            }
            
            message.linear.x = 0.0;
            message.angular.z = 0.0;
            
        } 
        
        else if (girando && girar_izquierda) {
            // Girando a la izquierda
            message.linear.x = 0.0;
            message.angular.z = 0.2;
            
            if (!obstaculo_detectado) {
                girando = false;
                girar_izquierda = false;

            }
            
        } else if (girando && girar_derecha) {
            // Girando a la derecha
            message.linear.x = 0.0;
            message.angular.z = -0.2;
            
            if (!obstaculo_detectado) {
                girando = false;
                girar_derecha = false;

            }
            
        } 
        
        else {
            // Parar el robot
            message.linear.x = 0.0;
            message.angular.z = 0.0;
        }
        
        publisher->publish(message);
        rclcpp::spin_some(node);
        loop_rate.sleep();
    }
    
    rclcpp::shutdown();
    return 0;
}
