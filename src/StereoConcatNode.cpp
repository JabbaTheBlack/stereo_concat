#include <mutex>
#include <memory>
#include <functional>
#include "StereoConcatNode.hpp"

#include "rclcpp/rclcpp.hpp"

#include "sensor_msgs/msg/compressed_image.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/header.hpp"

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>

#include <cv_bridge/cv_bridge.hpp>
#include <opencv2/opencv.hpp>

namespace stereo_concat
{
  StereoConcatNode::StereoConcatNode()
  : Node("stereo_concat")
  {
    RCLCPP_INFO(this->get_logger(), "StereoConcatNode has been started.");

    std::string left_image_topic = this->declare_parameter<std::string>("left_image_topic", "/left/image_rect_color/compressed");
    std::string right_image_topic = this->declare_parameter<std::string>("right_image_topic", "/right/image_rect_color/compressed");
    std::string output_image_topic = this->declare_parameter<std::string>("output_image_topic", "/stereo_concat/image");
    double publish_rate_hz = this->declare_parameter<double>("publish_rate_hz", 50.0);

    left_image_sub_.subscribe(this, left_image_topic);
    right_image_sub_.subscribe(this, right_image_topic);

    sync_ = std::make_shared<message_filters::Synchronizer<ApproxTimePolicy>>(ApproxTimePolicy(10), left_image_sub_, right_image_sub_);

    sync_->registerCallback(std::bind(&StereoConcatNode::ConcatImageCallback, this, std::placeholders::_1, std::placeholders::_2));

    publisher_ = this->create_publisher<sensor_msgs::msg::Image>(output_image_topic, 10);

    auto timer_period = std::chrono::duration<double>(1.0 / publish_rate_hz);
    timer_ = this->create_wall_timer(
      std::chrono::duration_cast<std::chrono::milliseconds>(timer_period),
      std::bind(&StereoConcatNode::TimerCallback, this)
    );
  }

  void StereoConcatNode::ConcatImageCallback(
    const sensor_msgs::msg::CompressedImage::ConstSharedPtr left_image,
    const sensor_msgs::msg::CompressedImage::ConstSharedPtr right_image)
  {
    cv::Mat left_mat = cv::imdecode(cv::Mat(left_image->data), cv::IMREAD_COLOR);
    cv::Mat right_mat = cv::imdecode(cv::Mat(right_image->data), cv::IMREAD_COLOR);

    if (left_mat.empty() || right_mat.empty()) {
      RCLCPP_WARN(this->get_logger(), "Failed to decode one of the images, skipping.");
      return;
    }

    cv::Mat stitched_image;
    try {
      cv::hconcat(left_mat, right_mat, stitched_image);
    } catch (const cv::Exception& e) {
      RCLCPP_ERROR(this->get_logger(), "Error concatenating images: %s", e.what());
      return;
    }

    {
      std::scoped_lock lock(image_mutex_);
      latest_stitched_image_ = stitched_image.clone();
      latest_header_ = left_image->header;
      latest_header_.frame_id = "";
    }
  }

  void StereoConcatNode::TimerCallback()
  {
    cv::Mat stitched_image;
    std_msgs::msg::Header header;
    {
      std::scoped_lock lock(image_mutex_);
      if (latest_stitched_image_.empty()) [[unlikely]] {
        RCLCPP_DEBUG(this->get_logger(), "No images received yet, skipping publish.");
        return;
      }
      stitched_image = latest_stitched_image_.clone();
      header = latest_header_;
    }

    sensor_msgs::msg::Image::SharedPtr output_msg = cv_bridge::CvImage(header, "bgr8", stitched_image).toImageMsg();
    publisher_->publish(*output_msg);
  }
} // namespace stereo_concat