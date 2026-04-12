#ifndef STEREO_CONCAT_NODE_HPP
#define STEREO_CONCAT_NODE_HPP

#include <string>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/compressed_image.h>
#include <cv_bridge/cv_bridge.hpp>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include "Utility.hpp"

namespace stereo_concat {
    class StereoConcatNode : public rclcpp::Node {
    private:
        Params params_;

        using SyncPolicy = message_filters::sync_policies::ApproximateTime<sensor_msgs::msg::CompressedImage, sensor_msgs::msg::CompressedImage>;

        message_filters::Subscriber<sensor_msgs::msg::CompressedImage> left_image_sub_;
        message_filters::Subscriber<sensor_msgs::msg::CompressedImage> right_image_sub_;

        std::shared_ptr<message_filters::Synchronizer<SyncPolicy>> sync_;

        sensor_msgs::msg::Image::SharedPtr concat_image_msg_latest_;
        rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr concat_image_pub_;

        void syncedImageCallback(
            const sensor_msgs::msg::CompressedImage::ConstSharedPtr left_msg,
            const sensor_msgs::msg::CompressedImage::ConstSharedPtr right_msg
        );

        rclcpp::TimerBase::SharedPtr timer_;
        void timerCallback();

    public:
        StereoConcatNode();
    };
}

#endif