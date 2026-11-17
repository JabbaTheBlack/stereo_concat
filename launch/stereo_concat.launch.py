import os
from datetime import datetime
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.conditions import IfCondition
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    ld = LaunchDescription()

    package_path = get_package_share_directory('stereo_concat')
    config = os.path.join(package_path, 'config', 'params.yaml')
    bags_path = os.environ.get(
        'STEREO_CONCAT_BAGS_DIR',
        os.path.join(package_path, 'bags')
    )
    bag_path = os.path.join(bags_path, 'onboarding.mcap')
    record_bag = LaunchConfiguration('record_bag')
    recording_name = datetime.now().strftime('stereo_concat_recording_%Y%m%d_%H%M%S')
    record_output = os.path.join(bags_path, recording_name)
    ld.add_action(
        DeclareLaunchArgument(
            'record_bag',
            default_value='false',
            description='Record all topics to a new bag'
        )
    )
    stereo_concat_node = Node(
        package='stereo_concat',
        executable='stereo_concat',
        name='stereo_concat',
        parameters=[config],
        output='screen'
    )

    ld.add_action(stereo_concat_node)
    ld.add_action(
        ExecuteProcess(
            cmd=['ros2', 'bag', 'play', bag_path],
            output='screen'
        )
    )
    ld.add_action(
        ExecuteProcess(
            cmd=[
                'ros2', 'bag', 'record',
                '/sync_image_topic',
                '--storage', 'mcap',
                '-o', record_output
            ],
            output='screen',
            condition=IfCondition(record_bag)
        )
    )
    return ld