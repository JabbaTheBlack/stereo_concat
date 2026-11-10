#ifndef STEREO_CONCAT_HPP
#define STEREO_CONCAT_HPP

#include <string>
#include "sensor_msgs/msg/image.hpp"
#include "message_filters/subscriber.h"
#include "message_filters/sync_policies/approximate_time.h"
#include "message_filters/synchronizer.h"

namespace stereo_concat {
    
    class StereoConcatNode : public rclcpp::Node {
        private:
            //topic subscribers                     
            message_filters::Subscriber<sensor_msgs::msg::Image> leftImageSubscriber;
            message_filters::Subscriber<sensor_msgs::msg::Image> rightImageSubscriber;

            //topic publisher
            rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr SyncImagePublisher;

            typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::msg::Image, sensor_msgs::msg::Image> SyncPolicy;
    
            std::shared_ptr<message_filters::Synchronizer<SyncPolicy>> sync_;
        public:
            StereoConcatNode() : Node("StereoConcatNode"){
                // Initialize subscribers and publishers
                leftImageSubscriber.subscribe(this, "/left/image_rect_color/compressed");
                rightImageSubscriber.subscribe(this, "/right/image_rect_color/compressed");

                sync_ = std::make_shared<message_filters::Synchronizer<SyncPolicy>>(SyncPolicy(10), leftImageSubscriber, rightImageSubscriber);

                sync_->registerCallback(std::bind(&StereoConcatNode::syncCallback, this, std::placeholders::_1, std::placeholders::_2));
                
                SyncImagePublisher = this->create_publisher<sensor_msgs::msg::Image>("sync_image_topic", 10);
            }

            void syncCallback(const sensor_msgs::msg::Image::ConstSharedPtr& image1, const sensor_msgs::msg::Image::ConstSharedPtr& image2);

    };
}


#endif
