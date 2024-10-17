#include <string>
#include <thread>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <opencv2/core/mat.hpp>

#include "camera.h"

ABSL_FLAG(int, camera_index, 0, "必需：本地摄像头 index");
ABSL_FLAG(bool, net_camera, true, "可选：是否启用网络摄像头");
ABSL_FLAG(std::string, camera_url, "http://localhost:3306/",
          "可选：网络摄像头 url");
ABSL_FLAG(std::string, model_dir, "./", "必需： SeetaFace6 的模型路径");
ABSL_FLAG(std::string, server_addr, "http://localhost:12345/",
          "必需：服务器地址");

int main(int argc, char *argv[]) {
  absl::SetProgramUsageMessage("可用的参数：");
  absl::ParseCommandLine(argc, argv);

  int camera_index = absl::GetFlag(FLAGS_camera_index);
  bool enable_net_camera = absl::GetFlag(FLAGS_net_camera);
  std::string camera_url = absl::GetFlag(FLAGS_camera_url);
  std::string server_addr = absl::GetFlag(FLAGS_server_addr);
  std::string model_dir = absl::GetFlag(FLAGS_model_dir);

  /*---- 初始化 gRPC 服务器  ----*/

  /*----------------------------*/

  /*---- 初始化 摄像头与 SeetaFace 模块 ----*/

  treasure_chest::pattern::SyncQueue<cv::Mat> frame_queue;

  arm_face_id::Camera::Settings settings;
  settings.enable_net_cam = enable_net_camera;
  settings.cam_index = camera_index;
  settings.cam_url = camera_url;
  arm_face_id::Camera camera(settings, frame_queue);

  camera.Open();
  std::thread camera_thread(&arm_face_id::Camera::Start, &camera);

  camera_thread.join();
  /*---------------------------------------*/

  /*---- 初始化 Qt 用户界面  ----*/

  /*----------------------------*/

  return 0;
}
