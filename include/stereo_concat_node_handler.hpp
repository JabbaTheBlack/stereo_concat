#ifndef STEREO_CONCAT_HPP
#define STEREO_CONCAT_HPP

#include "sensor_msgs/msg/compressed_image.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "message_filters/subscriber.h"
#include "message_filters/sync_policies/approximate_time.h"
#include "message_filters/synchronizer.h"

#include "utilities.hpp"

#include <cv_bridge/cv_bridge.hpp>

namespace stereo_concat {


    class StereoConcatNode : public rclcpp::Node {
        private:
            Params params_;

            //topic subscribers                     
            message_filters::Subscriber<sensor_msgs::msg::CompressedImage> leftImageSubscriber;
            message_filters::Subscriber<sensor_msgs::msg::CompressedImage> rightImageSubscriber;

            //topic publisher
            rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr SyncImagePublisher;

            typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::msg::CompressedImage, sensor_msgs::msg::CompressedImage> SyncPolicy;
    
            std::shared_ptr<message_filters::Synchronizer<SyncPolicy>> sync_;

            rclcpp::TimerBase::SharedPtr timer_;
            
            sensor_msgs::msg::Image::SharedPtr concat_image_msg_latest_;

        public:
            StereoConcatNode();

            void syncCallback(const sensor_msgs::msg::CompressedImage::ConstSharedPtr& image1, const sensor_msgs::msg::CompressedImage::ConstSharedPtr& image2);
            
            void timerCallback();
    };
}


#endif
