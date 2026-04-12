#include <cstdio>
#include "StereoConcatNode.hpp"

int main(int argc, char ** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<stereo_concat::StereoConcatNode>());
  rclcpp::shutdown();
  return 0;
}
