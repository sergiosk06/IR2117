from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(package='turtlesim', executable='turtlesim_node', name='sim'),
        Node(
            package='triangles',
            executable='scalene',
            name='scalene_driver',
            parameters=[{
                'a_side_length': 3.0,
                'b_side_length': 4.0,
                'c_side_length': 6.0,
                'linear_speed': 2.0,
                'angular_speed': 2.0
            }]
        )
    ])
