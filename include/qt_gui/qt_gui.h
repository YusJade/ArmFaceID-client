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
class ResizableLabel : public QLabel {  // 定义label类（动态幕布）
 public:
  ResizableLabel(QWidget *parent = nullptr) : QLabel(parent) {}

 protected:
  void resizeEvent(QResizeEvent *event) override {
    QLabel::resizeEvent(event);
    QSize newSize = event->size();
    QPixmap pixmap_cam(newSize);
    pixmap_cam.fill(Qt::transparent);
    QPainter painter_cam(&pixmap_cam);
    painter_cam.setBrush(Qt::black);
    painter_cam.drawRect(0, 0, pixmap_cam.width(), pixmap_cam.height());
    this->setPixmap(pixmap_cam);
  }
};
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
  [[deprecated]]
  void notify(NotifyType type, NotifyPosition pos, QString title,
              QString content, int nLive = 3000);

 private:
  const int notify_interval = 30;
  int cur_notify_cnter = 0;

 private:
  QWidget *main_widget_ = nullptr;
  QLabel *camera_frame_label_ = nullptr;
  QPixmap faces_notification_pixmap_;
  QPixmap antispoofing_notification_pixmap_;

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
  bool isMaximized = false;
  bool m_bIsPressed = false;
  bool m_bIsResizing = false;
  bool m_bIsDoublePressed = false;
  QPointF m_pressPoint;
  QPointF m_movePoint;
  enum ResizeDirection {
    NONE = 0,
    BOTTOMRIGHT,
    TOPRIGHT,
    TOPLEFT,
    BOTTOMLEFT,
    DOWN,
    LEFT,
    RIGHT,
    UP
  };
  ResizeDirection m_direction = NONE;
  // 窗口|窗口按钮
  QWidget *centralwidget = nullptr;
  QGridLayout *main_grid_layout_ = nullptr;
  QWidget *contwidget = nullptr;
  QGridLayout *cont_grid_layout = nullptr;
  QGroupBox *gb_pointbutton = nullptr;
  QPushButton *closeButton = nullptr;
  QPushButton *minimizeButton = nullptr;
  QLabel *lb_bg = nullptr;
  QHBoxLayout *btn_hbox_layout = nullptr;
  // tool 栏
  QGroupBox *gb_tool = nullptr;
  QVBoxLayout *tool_vbox_layout = nullptr;
  QLabel *lb_logo = nullptr;
  QPushButton *btn1, *btn2, *btn3;
  QPropertyAnimation *anim1, *anim2, *anim3;
  // cam 栏
  QGroupBox *gb_camera = nullptr;
  QVBoxLayout *cam_vbox_layout = nullptr;
  ResizableLabel *lb_camera = nullptr;
  QGroupBox *gb_info = nullptr;
  QPixmap info_notification_pixmap_;
  // info 栏
  QVBoxLayout *info_vbox_layout = nullptr;
  QLabel *lb_headshot = nullptr;
  QLabel *lb_username = nullptr;
  QLineEdit *le_username = nullptr;
  QLabel *lb_email = nullptr;
  QLineEdit *le_email = nullptr;
  QLabel *lb_lastused = nullptr;
  QLineEdit *le_lastused = nullptr;
  QPushButton *pb_verify1 = nullptr;
  QPushButton *pb_verify2 = nullptr;
  Notification *nf1, *nf2;

  QString btn_style_small = R"(
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
  QString btn_style_big = R"(
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