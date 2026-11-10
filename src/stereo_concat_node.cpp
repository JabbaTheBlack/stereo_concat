// Message filter subscriber and image transport publisher includes
#include "stereo_concat.hpp"




void stereo_concat::StereoConcatNode::syncCallback(const sensor_msgs::msg::Image::ConstSharedPtr& image1, const sensor_msgs::msg::Image::ConstSharedPtr& image2) {

    //Logger
    RCLCPP_INFO(this->get_logger(), "Matching images received: %s and %s", image1->header.frame_id.c_str(), image2->header.frame_id.c_str());
    RCLCPP_INFO(this->get_logger(), "Publishing synchronized image.");

    SyncImagePublisher->publish(*image1);
}
