#pragma once
//#pragma once 是一种防止头文件被多次包含的方式。
#include <qpixmap.h>
#include <qwidget.h>
#include <QGridLayout>
//#include <QToolBar>
#include <QLabel>
#include <QTextEdit>
#include <QLineEdit>
#include <QGroupBox>
#include <QPushButton>
#include <QMainWindow>
//Qt 库中的图像处理、窗口和布局类。
#include <opencv2/core/mat.hpp>
//OpenCV 中的 cv::Mat，用于表示图像数据。
#include "camera.h"
#include "face_analyzer.h"
#include "observer/core.h"
//自定义的 camera.h 和 face_analyzer.h 头文件。
//observer/core.h 可能与观察者模式有关，用于事件通知。

namespace arm_face_id { //定义一个命名空间 arm_face_id，用于组织代码和避免命名冲突。
class QtGUI //QtGUI 继承自 QMainWindow，这是一个 Qt 的主窗口类。
    : public QMainWindow,
      public treasure_chest::pattern::Observer<cv::Mat>,
      public treasure_chest::pattern::Observer<FaceAnalyzer::EventBase> {//实现了两个观察者接口，用于响应 cv::Mat 和 FaceAnalyzer::EventBase 类型的事件。

 public:
  QtGUI() = default;//默认构造函数，未进行任何特殊初始化。
  void InitWindow();//InitWindow() 方法用于初始化窗口的设置，具体实现不在此处。
//  void retranslateUi(QMainWindow *MainWindow);
  void OnNotify(const cv::Mat& message) override;
  void OnNotify(const FaceAnalyzer::EventBase& message) override;//OnNotify 方法是观察者接口的实现，用于处理通知事件，分别针对图像数据和人脸分析事件。

 private:
  QWidget* centralwidget = nullptr;//主窗口的 QWidget。
  QGridLayout* main_grid_layout_ = nullptr;//用于布局的网格布局。
//  QLabel* camera_frame_label_ = nullptr;//显示摄像头帧的标签。
  QPixmap info_notification_pixmap_;//用于显示信息通知的图像。

//  QMainWindow *MainWindow = nullptr; //主窗口
  QLabel *lb_bg = nullptr;
  QLabel *lb_ico = nullptr;
  QTabWidget *tw_main = nullptr;
  QWidget *tab_func = nullptr;
  QGroupBox *bg_camera = nullptr;
  QLabel *lb_camera = nullptr;//显示摄像头帧的标签。
  QGroupBox *gb_info = nullptr;
  QGridLayout *gridLayout = nullptr;
  QLabel *lb_email = nullptr;
  QLineEdit *le_lastused = nullptr;
  QLabel *lb_facequality = nullptr;
  QLabel *lb_username = nullptr;
  QLineEdit *le_facequality = nullptr;
  QLineEdit *le_username = nullptr;
  QLabel *lb_lastused = nullptr;
  QLineEdit *le_email = nullptr;
  QLabel *lb_liveness = nullptr;
  QLineEdit *le_liveness = nullptr;
  QWidget *tab_log = nullptr;
  QWidget *tab_about = nullptr;
  QLabel *lb_about = nullptr;
  QPushButton *pb_close = nullptr;
  QPushButton *pb_hide = nullptr;
};
}  // namespace arm_face_id