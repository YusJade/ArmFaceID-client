#pragma once

#include <opencv2/core/mat.hpp>

#include "face_analyzer.h"
#include "observer/core.h"
#include "qt_gui/client_ui.h"

namespace arm_face_id {
using treasure_chest::pattern::Observer;
/**
 * @brief 封装业务逻辑
 *
 */
class Client : public ClientUI,
               public Observer<cv::Mat>,
               public Observer<FaceAnalyzer::AnalyzeMsg>,
               public Observer<UserInfo> {
 private:
  void OnNotify(const cv::Mat &message) override;
  void OnNotify(const FaceAnalyzer::AnalyzeMsg &message) override;
  void OnNotify(const UserInfo &message) override;
  void InitWindow();

  void CheckNeedNotification(const FaceAnalyzer::AnalyzeMsg &analyze_res);

  const int notifybar_cooldown_tick = 30;
  const int hud_cleanup_tick = 20;
  int notifybar_disappear_tick = 0;
  int hud_keepalive_tick = 0;
};
}  // namespace arm_face_id