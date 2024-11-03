#include "face_analyzer.h"

#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "seeta/CFaceInfo.h"
#include "seeta/Common/CStruct.h"
#include "seeta/FaceAntiSpoofing.h"

using namespace arm_face_id;

FaceAnalyzer::FaceAnalyzer(const FaceAnalyzer::Settings &settings,
                           treasure_chest::pattern::SyncQueue<cv::Mat> &queue)
    : treasure_chest::pattern::Consumer<cv::Mat>(queue),
      detector_(settings.detector_setting),
      landmarker_(settings.landmarker_setting),
      antispoofing_(settings.antispoofing_setting) {
  spdlog::info("人脸分析器已构造 :>");
}

void FaceAnalyzer::Process() {
  while (true) {
    cv::Mat frame = GetTask();
    SeetaImageData simg{frame.cols, frame.rows, frame.channels(), frame.data};

    // 人脸检测
    SeetaFaceInfoArray faces = detector_.detect(simg);
    Notify<EventBase>(DetectorEvent{simg, DETECTOR, faces});
    if (faces.size == 0) continue;
    spdlog::info("人脸分析器：检测到 {} 张人脸。", faces.size);
    // 活体检测

    // 人脸质量评估

    // 调用服务端的人脸识别服务
  }
}