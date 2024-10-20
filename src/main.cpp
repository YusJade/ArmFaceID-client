#include <QApplication>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>
#include <opencv2/core/mat.hpp>

#include "Common/Struct.h"
#include "camera.h"
#include "face_analyzer.h"
#include "qt_gui/qt_gui.h"

ABSL_FLAG(int, camera_index, 0, "必需：本地摄像头 index");
ABSL_FLAG(bool, net_camera, true, "可选：是否启用网络摄像头");
ABSL_FLAG(std::string, camera_url, "http://localhost:3306/",
          "可选：网络摄像头 url");
ABSL_FLAG(std::string, model_dir, "./", "必需： SeetaFace6 的模型路径");
ABSL_FLAG(std::string, server_addr, "http://localhost:12345/",
          "必需：服务器地址");

int main(int argc, char *argv[]) {
  // 解析命令行参数，配置各个模块的设置
  absl::SetProgramUsageMessage("可用的参数：");
  absl::ParseCommandLine(argc, argv);

  int camera_index = absl::GetFlag(FLAGS_camera_index);
  bool enable_net_camera = absl::GetFlag(FLAGS_net_camera);
  std::string camera_url = absl::GetFlag(FLAGS_camera_url);
  std::string server_addr = absl::GetFlag(FLAGS_server_addr);
  std::string model_dir = absl::GetFlag(FLAGS_model_dir);

  // TODO: 初始化 gRPC 服务器

  /*----------------------------*/

  // 初始化 摄像头与 SeetaFace 模块
  treasure_chest::pattern::SyncQueue<cv::Mat> frame_queue;

  arm_face_id::Camera::Settings camera_settings;
  camera_settings.enable_net_cam = enable_net_camera;
  camera_settings.cam_index = camera_index;
  camera_settings.cam_url = camera_url;
  arm_face_id::Camera camera(camera_settings, frame_queue);

  camera.Open();
  std::thread camera_thread(&arm_face_id::Camera::Start, &camera);
  camera_thread.detach();
  /*---------------------------------------*/

  // TODO: 初始化人脸分析器
  arm_face_id::FaceAnalyzer::Settings analyzer_settings;
  analyzer_settings.detector_setting = seeta::ModelSetting(
      "sf3.0_models/face_detector.csta", seeta::ModelSetting::CPU);
  analyzer_settings.landmarker_setting = seeta::ModelSetting(
      "sf3.0_models/face_landmarker_pts5.csta", seeta::ModelSetting::CPU);
  analyzer_settings.antispoofing_setting = seeta::ModelSetting(
      std::vector<std::string>{"sf3.0_models/fas_first.csta",
                               "sf3.0_models/fas_second.csta"},
      seeta::ModelSetting::CPU);
  arm_face_id::FaceAnalyzer face_analyzer(analyzer_settings, frame_queue);

  std::thread analyzer_thread(&arm_face_id::FaceAnalyzer::Process,
                              &face_analyzer);
  analyzer_thread.detach();
  /*----------------------------*/

  /*---- 初始化 Qt 用户界面  ----*/
  QApplication app(argc, argv);

  std::shared_ptr<arm_face_id::QtGUI> qt_gui(new arm_face_id::QtGUI);
  qt_gui->InitWindow();
  qt_gui->show();
  camera.AddObserver<cv::Mat>(qt_gui);
  face_analyzer.AddObserver<arm_face_id::FaceAnalyzer::EventBase>(qt_gui);
  /*----------------------------*/

  return app.exec();
}
