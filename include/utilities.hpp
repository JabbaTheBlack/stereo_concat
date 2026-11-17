#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include "rclcpp/rclcpp.hpp"

namespace stereo_concat {

    struct Params {
        std::string left_image;
        std::string right_image;
        int hz;
    };

    inline void read_parameters(rclcpp::Node &node, Params &params) {
        node.declare_parameter<std::string>("left_image", "/defl");
        node.declare_parameter<std::string>("right_image", "/defr");
        node.declare_parameter<int>("hz", 50);

        params.left_image = node.get_parameter("left_image").as_string();
        params.right_image = node.get_parameter("right_image").as_string();
        params.hz = node.get_parameter("hz").as_int();
    }
}

#endif