#pragma once

#include <qpixmap.h>
#include <qwidget.h>

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>

#include <opencv2/core/mat.hpp>

#include "camera.h"
#include "face_analyzer.h"
#include "observer/core.h"

namespace arm_face_id {
class QtGUI
    : public QMainWindow,
      public treasure_chest::pattern::Observer<cv::Mat>,
      public treasure_chest::pattern::Observer<FaceAnalyzer::EventBase> {
 public:
  QtGUI() = default;

  void InitWindow();

  void OnNotify(const cv::Mat& message) override;
  void OnNotify(const FaceAnalyzer::EventBase& message) override;

 private:
  QWidget* main_widget_ = nullptr;
  QGridLayout* main_grid_layout_ = nullptr;
  QLabel* camera_frame_label_ = nullptr;
  QPixmap info_notification_pixmap_;
};
}  // namespace arm_face_id