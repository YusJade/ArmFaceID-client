#pragma once
#include <qlabel.h>
#include <qpixmap.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include <QGridLayout>
// #include <QToolBar>
#include <qpixmap.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
// Qt 库中的图像处理、窗口和布局类。
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <cstddef>

#include <opencv2/core/mat.hpp>
// OpenCV 中的 cv::Mat，用于表示图像数据。
#include "camera.h"
#include "face_analyzer.h"
#include "observer/core.h"
#include "qt_gui/notification.h"

#include "face.pb.h"
// 自定义的 camera.h 和 face_analyzer.h 头文件。
// observer/core.h 可能与观察者模式有关，用于事件通知。

namespace arm_face_id {

class QtGUI  // 定义QtGUI类
    : public QMainWindow,
      public treasure_chest::pattern::Observer<cv::Mat>,
      public treasure_chest::pattern::Observer<FaceAnalyzer::AnalyzeMsg>,
      public treasure_chest::pattern::Observer<FaceAnalyzer::EventBase>,
      public treasure_chest::pattern::Observer<UserInfo> {
  Q_OBJECT
 public:
  QtGUI() = default;
  void OnNotify(const cv::Mat &message) override;
  void OnNotify(const FaceAnalyzer::EventBase &message) override;
  void OnNotify(const FaceAnalyzer::AnalyzeMsg &message) override;
  void OnNotify(const UserInfo &message) override;
  void InitWindow();

  void CheckNeedNotification(const FaceAnalyzer::AnalyzeMsg &message);

 signals:
  [[deprecated]] void notify(NotifyType type, NotifyPosition pos, QString title,
                             QString content, int nLive = 3000);

 private:
  const int notify_interval = 30;
  int cur_notify_cnter = 0;

 private:
  QWidget *main_widget_ = nullptr;
  // QLabel *camera_frame_label_ = nullptr;
  QPixmap faces_notification_pixmap_;
  // QPixmap antispoofing_notification_pixmap_;

  int notification_delay_cnter = 0;

  bool eventFilter(QObject *watched, QEvent *event) override;
  void ControlButton();
  bool event(QEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void updateRegion(QMouseEvent *event);
  void resizeRegion(int marginTop, int marginBottom, int marginLeft,
                    int marginRight);
  void mouseReleaseEvent(QMouseEvent *event);
  void leaveEvent(QEvent *event);
  void maximizeWidget();
  void restoreWidget();

 private:
  // 鼠标跟踪
  bool is_maximized_ = false;
  bool b_is_pressed_ = false;
  bool b_is_resizing_ = false;
  bool b_is_double_pressed_ = false;
  QPointF press_point_;
  QPointF move_point_;

  ResizableLabel::ResizeDirection direction_ = ResizableLabel::NONE;
  // 窗口|窗口按钮
  QWidget *centralwidget_ = nullptr;
  QGridLayout *main_grid_layout_ = nullptr;
  QWidget *contwidget_ = nullptr;
  QGridLayout *cont_grid_layout_ = nullptr;
  QGroupBox *gb_pointbutton_ = nullptr;
  QPushButton *close_button_ = nullptr;
  QPushButton *minimize_button_ = nullptr;
  QLabel *lb_bg_ = nullptr;
  QHBoxLayout *btn_hbox_layout_ = nullptr;
  // tool 栏
  QGroupBox *gb_tool_ = nullptr;
  QVBoxLayout *tool_vbox_layout_ = nullptr;
  QLabel *lb_logo_ = nullptr;
  QPushButton *btn1_, *btn2_, *btn3_;
  QPropertyAnimation *anim1_, *anim2_, *anim3_;
  // cam 栏
  QGroupBox *gb_camera_ = nullptr;
  QVBoxLayout *cam_vbox_layout_ = nullptr;
  ResizableLabel *lb_camera_ = nullptr;
  QGroupBox *gb_info_ = nullptr;
  QPixmap info_notification_pixmap_;
  // info 栏
  QVBoxLayout *info_vbox_layout_ = nullptr;
  QLabel *lb_headshot_ = nullptr;
  QLabel *lb_username_ = nullptr;
  QLineEdit *le_username_ = nullptr;
  QLabel *lb_email_ = nullptr;
  QLineEdit *le_email_ = nullptr;
  QLabel *lb_lastused_ = nullptr;
  QLineEdit *le_lastused_ = nullptr;
  QPushButton *pb_verify1_ = nullptr;
  QPushButton *pb_verify2_ = nullptr;
  Notification *nf1_, *nf2_;

  QString btn_style_small_ = R"(
    QPushButton {
        background: transparent;
        border: 1px solid #0ffff3;
        color: #0ffff3;
        font-weight: bold;
        text-align: center;
        outline: none;
    }
    QPushButton:hover {
        color: #8421FF;
        background-color: #0ffff3;
    }
    QPushButton:pressed {
        padding-left: 6px;
        padding-top: 6px;
    }
)";
  QString btn_style_big_ = R"(
    QPushButton {
        background: transparent;
        border: 1px solid #0ffff3;
        color: #0ffff3;
        font-weight: bold;
        padding: 10px 20px;
        text-align: center;
        outline: none;
    }
    QPushButton:hover {
        color: #8421FF;
        background-color: #0ffff3;
    }
    QPushButton:pressed {
        padding-left: 6px;
        padding-top: 6px;
    }
)";
};
}  // namespace arm_face_id