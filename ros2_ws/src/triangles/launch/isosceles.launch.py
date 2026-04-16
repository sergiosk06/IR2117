from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(package='turtlesim', executable='turtlesim_node', name='sim'),
        Node(
            package='triangles',
            executable='isosceles',
            name='isosceles_driver',
            parameters=[{
                'base_length': 1.5,
                'lateral_length': 2.5,
                'linear_speed': 2.0,
                'angular_speed': 1.0
            }]
        )
    ])
