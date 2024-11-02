#pragma once
#include <qpixmap.h>
#include <qwidget.h>

#include <QGridLayout>
// #include <QToolBar>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
// Qt 库中的图像处理、窗口和布局类。
#include <opencv2/core/mat.hpp>
// OpenCV 中的 cv::Mat，用于表示图像数据。
#include "camera.h"
#include "face_analyzer.h"
#include "observer/core.h"
// 自定义的 camera.h 和 face_analyzer.h 头文件。
// observer/core.h 可能与观察者模式有关，用于事件通知。

namespace arm_face_id {  // 定义一个命名空间
                         // arm_face_id，用于组织代码和避免命名冲突。
class QtGUI  // QtGUI 继承自 QMainWindow，这是一个 Qt 的主窗口类。
    : public QMainWindow,
      public treasure_chest::pattern::Observer<cv::Mat>,
      public treasure_chest::pattern::Observer<FaceAnalyzer::AnalyzeMsg>,
      public treasure_chest::pattern::Observer<FaceAnalyzer::EventBase> {
 public:
  QtGUI() = default;  // 默认构造函数，未进行任何特殊初始化。
  void
  InitWindow();  // InitWindow() 方法用于初始化窗口的设置，具体实现不在此处。
  //  void retranslateUi(QMainWindow *MainWindow);
  void OnNotify(const cv::Mat& message) override;
  void OnNotify(const FaceAnalyzer::EventBase& message) override;
  void OnNotify(const FaceAnalyzer::AnalyzeMsg& message) override;

 private:
  QWidget* main_widget_ = nullptr;
  QGridLayout* main_grid_layout_ = nullptr;
  QLabel* camera_frame_label_ = nullptr;
  QPixmap faces_notification_pixmap_;
  QPixmap antispoofing_notification_pixmap_;

  int notification_delay_cnter = 0;
};
}  // namespace arm_face_id