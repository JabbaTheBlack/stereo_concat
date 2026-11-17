// Message filter subscriber and image transport publisher includes
#include "stereo_concat_node_handler.hpp"
#include "utilities.hpp"

stereo_concat::StereoConcatNode::StereoConcatNode() : Node("stereo_concat_node") {

    read_parameters(*this, params_);

    leftImageSubscriber.subscribe(this, params_.left_image);
    rightImageSubscriber.subscribe(this, params_.right_image);

    //int hz = this->get_parameter("hz").as_int();


    sync_ = std::make_shared<message_filters::Synchronizer<SyncPolicy>>(SyncPolicy(10), leftImageSubscriber, rightImageSubscriber);

    sync_->registerCallback(std::bind(&StereoConcatNode::syncCallback, this, std::placeholders::_1, std::placeholders::_2));
    
    SyncImagePublisher = this->create_publisher<sensor_msgs::msg::CompressedImage>("sync_image_topic", 10);
}

void stereo_concat::StereoConcatNode::syncCallback(const sensor_msgs::msg::CompressedImage::ConstSharedPtr& image1, const sensor_msgs::msg::CompressedImage::ConstSharedPtr& image2) {

    //Logger
    RCLCPP_INFO(this->get_logger(), "Matching images received: %s and %s", image1->header.frame_id.c_str(), image2->header.frame_id.c_str());
    RCLCPP_INFO(this->get_logger(), "Publishing synchronized image.");

    SyncImagePublisher->publish(*image1);
}
