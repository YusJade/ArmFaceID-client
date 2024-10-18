#pragma once

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include "treasure_chest/observer/core.h"
#include "treasure_chest/producer_consumer/core.h"

namespace arm_face_id {
class Camera : public treasure_chest::pattern::Producer<cv::Mat>,
               public treasure_chest::pattern::Subject {
 public:
  // 摄像头相关配置
  struct Settings {
    // 网络摄像头 url
    std::string cam_url = "";
    // 本地摄像头设备 index
    int cam_index = 0;
    // 是否启用网络摄像头
    bool enable_net_cam = true;
  };

  Camera() = delete;
  Camera(const Settings &, treasure_chest::pattern::SyncQueue<cv::Mat> &);

  bool Open();
  void Start();

 private:
  Settings settings_;
  bool is_running_ = false;
  cv::VideoCapture cap_;
};
}  // namespace arm_face_id
