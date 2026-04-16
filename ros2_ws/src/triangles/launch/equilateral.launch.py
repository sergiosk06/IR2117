from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # Nodo de Turtlesim
        Node(
            package='turtlesim',
            executable='turtlesim_node',
            name='sim'
        ),
        # Tu nodo equilateral
        Node(
		    package='triangles',
		    executable='equilateral',
		    name='equilateral_driver',
		    parameters=[{
			'side_length': 2.5,
			'angular_speed': 2.0,
			'linear_speed': 1.0
		    }],
		    remappings=[
			('/cmd_vel', '/turtle1/cmd_vel'),
		    ]
		)
    ])
