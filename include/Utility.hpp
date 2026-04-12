#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <rclcpp/rclcpp.hpp>
#include <string>

namespace stereo_concat {
    struct Params {
        unsigned int queue_size;
        std::string left_topic;
        std::string right_topic;
        std::string concat_topic;

        void load_parameters() {
            rclcpp::Node::UniquePtr node = std::make_unique<rclcpp::Node>("param_loader");
            node->declare_parameter<int>("queue_size", 10);
            node->declare_parameter<std::string>("left_topic", "/left/image_rect_color/compressed");
            node->declare_parameter<std::string>("right_topic", "/right/image_rect_color/compressed");
            node->declare_parameter<std::string>("concat_topic", "/stereo_concat/image_rect_color");

            queue_size = node->get_parameter("queue_size").as_int();
            left_topic = node->get_parameter("left_topic").as_string();
            right_topic = node->get_parameter("right_topic").as_string();
            concat_topic = node->get_parameter("concat_topic").as_string();
        }
    };
}

#endif