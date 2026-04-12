import os
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
from launch.actions import ExecuteProcess

def generate_launch_description():
    ld = LaunchDescription()

    package_path = get_package_share_directory('stereo_concat')
    config = os.path.join(package_path, 'config', 'params.yaml')
    bag = os.path.join(package_path, 'bags', 'onboarding.mcap')

    stereo_concat_node = Node(
        package='stereo_concat',
        executable='stereo_concat',
        output='screen',
        parameters=[config]
    )

    play_bag_cmd = ExecuteProcess(
        cmd=['ros2', 'bag', 'play', bag],
        output='screen'
    )

    ld.add_action(stereo_concat_node)
    ld.add_action(play_bag_cmd)
    return ld