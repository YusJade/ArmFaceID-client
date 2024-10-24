#include "face_analyzer.h"

#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "CFaceInfo.h"
#include "Common/CStruct.h"
#include "FaceAntiSpoofing.h"
#include "QualityOfIntegrity.h"
#include "QualityStructure.h"

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
    Notify<EventBase>(DetectorEvent{faces, simg, DETECTOR});
    if (faces.size == 0) continue;
    spdlog::info("人脸分析器：检测到 {} 张人脸。", faces.size);
    // 活体检测

    // 人脸质量评估
    // 完整度评估
    for (int i = 0; i < faces.size; ++i) {
      SeetaRect face_rect = faces.data[i].pos;
      SeetaPointF points[5];
      landmarker_.mark(simg, face_rect, points);
      seeta::QualityResult integrity_result =
          integrity_assessor_.check(simg, face_rect, points, 5);
      Notify<QualityAssessorEvent>(
          QualityAssessorEvent{integrity_result, simg, QUALITY_ASSESSOR});
      if (integrity_result.level == seeta::QualityLevel::LOW) {
        spdlog::info("人脸分析器：人脸不完整。");
      }else {
        spdlog::info("人脸分析器：人脸完整。");
      }
    }

    // 调用服务端的人脸识别服务
  }
}