#pragma once

#include <opencv2/core/mat.hpp>
#include <seeta/FaceAntiSpoofing.h>
#include <seeta/FaceDetector.h>
#include <seeta/FaceLandmarker.h>
#include <seeta/QualityOfClarity.h>
#include <seeta/QualityOfIntegrity.h>
#include <seeta/QualityOfPose.h>

#include "CFaceInfo.h"
#include "Common/CStruct.h"
#include "Common/Struct.h"
#include "QualityStructure.h"
#include "treasure_chest/observer/core.h"
#include "treasure_chest/producer_consumer/core.h"

namespace arm_face_id {
/**
 * @brief
 * 人脸分析器，集成本地的人脸检测、活体检测、质量评估功能，
 * 通过 RPC 通信获得服务器的人脸识别服务。
 */
class FaceAnalyzer : public treasure_chest::pattern::Subject,
                     public treasure_chest::pattern::Consumer<cv::Mat> {
 public:
  struct Settings {
    seeta::ModelSetting detector_setting;
    seeta::ModelSetting landmarker_setting;
    seeta::ModelSetting antispoofing_setting;
    // seeta::ModelSetting quality_assessor_setting;
  };

 public:
  enum EventType { BASE, DETECTOR, LANDMARKER, ANTISPOOFING, QUALITY_ASSESSOR };
  struct EventBase {
    const SeetaImageData &simg;
    const EventType type;
  };

  struct DetectorEvent : EventBase {
    const SeetaFaceInfoArray &faces;
  };

  struct LandmarkerEvent : EventBase {};

  struct AntiSpoofingEvent : EventBase {
    const seeta::FaceAntiSpoofing::Status status;
  };

  struct QualityAssessorEvent : EventBase {
    const seeta::QualityResult res;
  };

  FaceAnalyzer() = delete;
  FaceAnalyzer(const Settings &,
               treasure_chest::pattern::SyncQueue<cv::Mat> &queue);

  /**
   * @brief
   * 阻塞当前线程，FaceAnalyzer 会开始等待任务，对画面帧进行人脸检测、
   * 活体检测、质量评估，再视需要与否，调用服务端服务进行人脸识别，并
   * 通过观察者模式在以上过程中通知其他模块当前任务的状态。
   */
  void Process();

 private:
  seeta::FaceDetector detector_;          // 人脸检测器
  seeta::FaceLandmarker landmarker_;      // 人脸关键点定位器
  seeta::FaceAntiSpoofing antispoofing_;  // 活体检测器

  seeta::QualityOfClarity clarity_assessor_;      // 人脸清晰度评估器
  seeta::QualityOfIntegrity integrity_assessor_;  // 人脸完整度评估器
  seeta::QualityOfPose pose_assessor_;            // 人脸姿态评估器
};

}  // namespace arm_face_id
