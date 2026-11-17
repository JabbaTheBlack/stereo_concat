# stereo_concat
ROS2 C++ template package for synchronizing and concatenating stereo camera images.

Build and launch the package, including bag playback:

```bash
./build_and_launch.sh
```

Place bags that should be played in the workspace `bags/` directory. The default
launch file plays `bags/onboarding.mcap`.

Recording is disabled by default. Enable it to record all topics into a new
MCAP bag:

```bash
./build_and_launch.sh record_bag:=true
```
