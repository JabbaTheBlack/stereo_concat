import os.path

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    ld = LaunchDescription()

    package_path = get_package_share_directory('stereo_concat')
    config_file = os.path.join(package_path, 'config', 'stereo_concat_params.yaml')

    stereo_concat_node = Node(
        package='stereo_concat',
        executable='stereo_concat',
        name='stereo_concat_node',
        parameters=[config_file],
        output='screen'
    )


    ld.add_action(stereo_concat_node)
    return ld