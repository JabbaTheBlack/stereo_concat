#!/usr/bin/env bash

set -e

WORKSPACE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$WORKSPACE_DIR"

colcon build --packages-select stereo_concat
source install/setup.bash

ros2 launch stereo_concat stereo_concat.launch.py "$@"
