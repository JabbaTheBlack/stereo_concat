#include "StereoConcatNode.hpp"

namespace stereo_concat {
    StereoConcatNode::StereoConcatNode() : Node("stereo_concat_node") {
        params_.load_parameters();

        left_image_sub_.subscribe(this, params_.left_topic);
        right_image_sub_.subscribe(this, params_.right_topic);

        sync_ = std::make_shared<message_filters::Synchronizer<SyncPolicy>>(SyncPolicy(10), left_image_sub_, right_image_sub_);
        sync_->registerCallback(std::bind(
            &StereoConcatNode::syncedImageCallback,
            this,
            std::placeholders::_1,
            std::placeholders::_2
        ));

        concat_image_pub_ = this->create_publisher<sensor_msgs::msg::Image>(params_.concat_topic, params_.queue_size);

        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(20),
            std::bind(&StereoConcatNode::timerCallback, this)
        );
    }

    void StereoConcatNode::syncedImageCallback(
        const sensor_msgs::msg::CompressedImage::ConstSharedPtr left_msg,
        const sensor_msgs::msg::CompressedImage::ConstSharedPtr right_msg
    ) {
        cv_bridge::CvImagePtr left_cv_ptr;
        cv_bridge::CvImagePtr right_cv_ptr;

        try {
            left_cv_ptr = cv_bridge::toCvCopy(left_msg, sensor_msgs::image_encodings::BGR8);
            right_cv_ptr = cv_bridge::toCvCopy(right_msg, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return;
        }

        cv::Mat concat_image;
        cv::hconcat(left_cv_ptr->image, right_cv_ptr->image, concat_image);

        concat_image_msg_latest_ = cv_bridge::CvImage(
            left_cv_ptr->header,
            sensor_msgs::image_encodings::BGR8,
            concat_image
        ).toImageMsg();
    }

    void StereoConcatNode::timerCallback() {
        if (concat_image_msg_latest_) {
            concat_image_pub_->publish(*concat_image_msg_latest_);
        } else {
            RCLCPP_WARN_THROTTLE(
                this->get_logger(), 
                *this->get_clock(), 
                1000, 
                "Waiting for first synchronized image."
            );
        }
    }
}