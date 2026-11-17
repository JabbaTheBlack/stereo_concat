// Message filter subscriber and image transport publisher includes
#include <algorithm>
#include <chrono>
#include "stereo_concat_node_handler.hpp"

stereo_concat::StereoConcatNode::StereoConcatNode() : Node("stereo_concat_node") {

    read_parameters(*this, params_);

    leftImageSubscriber.subscribe(this, params_.left_image);
    rightImageSubscriber.subscribe(this, params_.right_image);

    sync_ = std::make_shared<message_filters::Synchronizer<SyncPolicy>>(SyncPolicy(10), leftImageSubscriber, rightImageSubscriber);

    sync_->registerCallback(std::bind(&StereoConcatNode::syncCallback, this, std::placeholders::_1, std::placeholders::_2));
    
    SyncImagePublisher = this->create_publisher<sensor_msgs::msg::Image>("sync_image_topic", 10);

    const auto period =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::duration<double>(1.0 / params_.hz));
    timer_ = this->create_wall_timer(
        period,
        std::bind(&StereoConcatNode::timerCallback, this)
    );
}

void stereo_concat::StereoConcatNode::syncCallback(const sensor_msgs::msg::CompressedImage::ConstSharedPtr& image1, const sensor_msgs::msg::CompressedImage::ConstSharedPtr& image2) {
    RCLCPP_INFO(this->get_logger(), "Matching images received: %s and %s", image1->header.frame_id.c_str(), image2->header.frame_id.c_str());

    cv_bridge::CvImagePtr left_cv;
    cv_bridge::CvImagePtr right_cv;
    try {
        left_cv = cv_bridge::toCvCopy(image1, "bgr8");
        right_cv = cv_bridge::toCvCopy(image2, "bgr8");
    } catch (const cv_bridge::Exception & exception) {
        RCLCPP_WARN(this->get_logger(), "Could not decode compressed images: %s", exception.what());
        return;
    }

    cv::Mat left = left_cv->image;
    cv::Mat right = right_cv->image;

    if (left.empty() || right.empty()) {
        RCLCPP_WARN(this->get_logger(), "Could not decode one or both compressed images.");
        return;
    }

    int output_height = std::max(left.rows, right.rows);
    cv::Mat left_resized;
    cv::Mat right_resized;

    cv::resize(left, left_resized,
        cv::Size(left.cols * output_height / left.rows, output_height));
    cv::resize(right, right_resized,
        cv::Size(right.cols * output_height / right.rows, output_height));

    cv::Mat concat_image;
    cv::hconcat(left_resized, right_resized, concat_image);

    concat_image_msg_latest_ = cv_bridge::CvImage(
            left_cv->header,
            sensor_msgs::image_encodings::BGR8,
            concat_image
    ).toImageMsg();
}

void stereo_concat::StereoConcatNode::timerCallback() {
    if (concat_image_msg_latest_) {
        SyncImagePublisher->publish(*concat_image_msg_latest_);
    }
}

