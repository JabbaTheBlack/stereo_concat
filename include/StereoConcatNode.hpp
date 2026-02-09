#ifndef STEREO_CONCAT_NODE_HPP_
#define STEREO_CONCAT_NODE_HPP_

#include <mutex>
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

class StereoConcatNode : public rclcpp::Node
{
public:
  StereoConcatNode();

private:
  using CompressedImage = sensor_msgs::msg::CompressedImage;
  using Image = sensor_msgs::msg::Image;
  using ApproxTimePolicy = message_filters::sync_policies::ApproximateTime<CompressedImage, CompressedImage>;

  void ConcatImageCallback(
    const CompressedImage::ConstSharedPtr left_image,
    const CompressedImage::ConstSharedPtr right_image);

  void TimerCallback();

  message_filters::Subscriber<CompressedImage> left_image_sub_;
  message_filters::Subscriber<CompressedImage> right_image_sub_;
  std::shared_ptr<message_filters::Synchronizer<ApproxTimePolicy>> sync_;
  
  rclcpp::Publisher<Image>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;

  std::mutex image_mutex_;
  cv::Mat latest_stitched_image_;
  std_msgs::msg::Header latest_header_;
};

} // namespace stereo_concat

#endif  // STEREO_CONCAT_NODE_HPP_