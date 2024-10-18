#pragma once

#include <qwidget.h>

#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>

#include <opencv2/core/mat.hpp>

#include "camera.h"

namespace arm_face_id {
class QtGUI : public QMainWindow,
              public treasure_chest::pattern::Observer<cv::Mat> {
 public:
  QtGUI() = default;

  void InitWindow();

  void OnNotify(const cv::Mat& message) override;

 private:
  QWidget* main_widget_ = nullptr;
  QGridLayout* main_grid_layout_ = nullptr;
  QLabel* camera_frame_label_ = nullptr;
};
}  // namespace arm_face_id