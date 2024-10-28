#include "face_analyzer.h"

#include <qimage.h>
#include <qregion.h>

#include <QIODevice>

#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "CFaceInfo.h"
#include "Common/CStruct.h"
#include "FaceAntiSpoofing.h"
#include "QualityOfIntegrity.h"
#include "QualityStructure.h"
#include "utils/base.h"

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
  bool need_recognize = true;
  while (true) {
    cv::Mat frame = GetTask();
    SeetaImageData simg{frame.cols, frame.rows, frame.channels(), frame.data};

    // 人脸检测
    SeetaFaceInfoArray faces = detector_.detect(simg);
    if (faces.size > 0) {
      Notify<EventBase>(DetectorEvent{faces, simg, DETECTOR});
    }
    if (faces.size == 0) {
      need_recognize = true;
      continue;
    }
    spdlog::info("人脸分析器：检测到 {} 张人脸。", faces.size);

    // 活体检测
    AntiSpoofingEvent event{simg, ANTISPOOFING};
    for (int i = 0; i < faces.size; ++i) {
      SeetaRect face_rect = faces.data[i].pos;
      SeetaPointF points[5];
      landmarker_.mark(simg, face_rect, points);
      seeta::FaceAntiSpoofing::Status status =
          antispoofing_.Predict(simg, face_rect, points);
      switch (status) {
        case seeta::FaceAntiSpoofing::REAL:
          spdlog::info("活体检测：检测到真实人脸。");
          break;
        case seeta::FaceAntiSpoofing::SPOOF:
          spdlog::info("活体检测：检测到攻击人脸。");
          break;
        case seeta::FaceAntiSpoofing::FUZZY:
          spdlog::info("活体检测：无法判断。");
          break;
        case seeta::FaceAntiSpoofing::DETECTING:
          spdlog::info("活体检测：正在检测。");
          break;
      }
      event.infos.push_back(AntiSpoofingInfo{status, faces.data[i]});
    }
    Notify<EventBase>(event);
    // 人脸质量评估

    for (int i = 0; i < faces.size; ++i) {
      SeetaRect face_rect = faces.data[i].pos;
      SeetaPointF points[5];
      landmarker_.mark(simg, face_rect, points);
      // 完整度评估
      seeta::QualityResult integrity_result =
          integrity_assessor_.check(simg, face_rect, points, 5);
      Notify<EventBase>(
          QualityAssessorEvent{integrity_result, simg, QUALITY_ASSESSOR});
      if (integrity_result.level == seeta::QualityLevel::LOW ||
          integrity_result.score < 0.8) {
        spdlog::info("人脸分析器：人脸不完整。");
      } else {
        spdlog::info("人脸分析器：人脸完整。");
      }
      // 姿态评估
      seeta::QualityResult pose_result =
          pose_assessor_.check(simg, face_rect, points, 5);
      Notify<QualityAssessorEvent>(
          QualityAssessorEvent{pose_result, simg, QUALITY_ASSESSOR});
      if (pose_result.level == seeta::QualityLevel::LOW) {
        spdlog::info("人脸分析器：正脸。");
      } else {
        spdlog::info("人脸分析器：非正脸。");
      }
    }

    // 调用服务端的人脸识别服务
    if (rpc_ && need_recognize) {
      QImage qimage = utils::mat_to_qimage(frame);
      QByteArray byte_arr;
      QDataStream stream(&byte_arr, QIODevice::WriteOnly);
      stream << qimage;
      std::string img_bytes(byte_arr.data(), byte_arr.size());
      rpc_->RecognizeFace(img_bytes);
      need_recognize = false;
    }
  }
}
