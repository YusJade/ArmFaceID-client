#include "face_analyzer.h"

#include <qimage.h>
#include <qregion.h>

#include <QBuffer>
#include <QIODevice>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <spdlog/spdlog.h>

#include "CFaceInfo.h"
#include "Common/CStruct.h"
#include "FaceAntiSpoofing.h"
#include "QualityOfIntegrity.h"
#include "QualityStructure.h"
#include "utils/base.h"
#include "utils/log.h"

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
  bool need_recognize = false;
  while (true) {
    cv::Mat frame = GetTask();
    SeetaImageData simg{frame.cols, frame.rows, frame.channels(), frame.data};
    SeetaFaceInfoArray faces = detector_.detect(simg);
    if (faces.size == 0) {
      continue;
    }
    spdlog::info("人脸分析器：检测到 {} 张人脸。", faces.size);

    // 人脸质量评估
    std::vector<AnalyzeResult> analyze_res;
    for (int i = 0; i < faces.size; ++i) {
      SeetaRect face_rect = faces.data[i].pos;
      SeetaPointF points[5];
      landmarker_.number();
      landmarker_.mark(simg, face_rect, points);
      seeta::FaceAntiSpoofing::Status status =
          antispoofing_.Predict(simg, face_rect, points);
      // switch (status) {
      //   case seeta::FaceAntiSpoofing::REAL:
      //     spdlog::info("活体检测：检测到真实人脸。");
      //     break;
      //   case seeta::FaceAntiSpoofing::SPOOF:
      //     spdlog::info("活体检测：检测到攻击人脸。");
      //     break;
      //   case seeta::FaceAntiSpoofing::FUZZY:
      //     spdlog::info("活体检测：无法判断。");
      //     break;
      //   case seeta::FaceAntiSpoofing::DETECTING:
      //     spdlog::info("活体检测：正在检测。");
      //     break;
      // }

      // 完整度评估
      bool is_integrity = true;
      seeta::QualityResult integrity_result =
          integrity_assessor_.check(simg, face_rect, points, 5);
      // Notify<EventBase>(
      //     QualityAssessorEvent{integrity_result, simg, QUALITY_ASSESSOR});
      // if (integrity_result.level == seeta::QualityLevel::LOW ||
      //     integrity_result.score < 0.8) {
      //   log::info("人脸分析器：人脸不完整。");
      //   is_integrity = false;
      // } else {
      //   log::info("人脸分析器：人脸完整。");
      // }

      // 姿态评估
      seeta::QualityResult pose_result =
          pose_assessor_.check(simg, face_rect, points, 5);
      // Notify<EventBase>(
      //     QualityAssessorEvent{pose_result, simg, QUALITY_ASSESSOR});
      // if (pose_result.level == seeta::QualityLevel::LOW || !is_integrity) {
      //   spdlog::info("人脸分析器：非正脸。");
      // } else {
      //   spdlog::info("人脸分析器：正脸。");
      // }

      // 分辨率评估
      seeta::QualityResult resolution_result =
          resolution_assessor_.check(simg, face_rect, points, 5);
      // Notify<EventBase>(
      //     QualityAssessorEvent{resolution_result, simg, QUALITY_ASSESSOR});
      // if (resolution_result.level == seeta::QualityLevel::LOW) {
      //   spdlog::info("人脸分析器：分辨率低。");
      // } else if (resolution_result.level == seeta::QualityLevel::MEDIUM) {
      //   spdlog::info("人脸分析器：分辨率中等。");
      // } else {
      //   spdlog::info("人脸分析器：分辨率高。");
      // }

      // 清晰度评估
      seeta::QualityResult clarity_result =
          clarity_assessor_.check(simg, face_rect, points, 5);
      // Notify<EventBase>(
      //     QualityAssessorEvent{clarity_result, simg, QUALITY_ASSESSOR});
      // if (clarity_result.level == seeta::QualityLevel::LOW) {
      //   spdlog::info("人脸分析器：清晰度低。");
      // } else if (clarity_result.level == seeta::QualityLevel::MEDIUM) {
      //   spdlog::info("人脸分析器：清晰度中等。");
      // } else {
      //   spdlog::info("人脸分析器：清晰度高。");
      // }

      analyze_res.push_back(AnalyzeResult{faces.data[i], status,
                                          integrity_result, pose_result,
                                          clarity_result, resolution_result});
    }
    Notify(AnalyzeMsg{analyze_res, simg});

    need_recognize =
        analyze_res[0].status == seeta::FaceAntiSpoofing::REAL &&
        analyze_res[0].clarity_res.level == seeta::QualityLevel::HIGH &&
        analyze_res[0].integrity_res.level == seeta::QualityLevel::HIGH &&
        analyze_res[0].pose_res.level == seeta::QualityLevel::HIGH &&
        analyze_res[0].resolution_res.level == seeta::QualityLevel::HIGH;

    // 调用服务端的人脸识别服务
    if (rpc_ && need_recognize) {
      QImage qimage = utils::mat_to_qimage(frame);
      QByteArray byte_arr;
      QBuffer buffer(&byte_arr);
      buffer.open(QIODevice::WriteOnly);
      qimage.save(&buffer, "PNG");
      // QDataStream stream(&byte_arr, QIODevice::WriteOnly);
      // stream << qimage.bits();
      std::string img_bytes(byte_arr.data(), byte_arr.size());
      rpc_->RecognizeFace(img_bytes);
      // rpc_->RecognizeFace(byte_arr.toBase64().toStdString());
      need_recognize = false;
    }
  }
}
