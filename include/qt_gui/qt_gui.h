#pragma once
#include <qlabel.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <QAction>
#include <QCoreApplication>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>
#include <cstddef>

#include <opencv2/core/mat.hpp>

#include "camera.h"
#include "face_analyzer.h"
#include "observer/core.h"
#include "qt_gui/notification.h"

namespace arm_face_id {

// 定义label类（动态幕布）
class ResizableLabel : public QLabel {
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
// 定义QtGUI类
class QtGUI
    : public QMainWindow,
      public treasure_chest::pattern::Observer<cv::Mat>,
      public treasure_chest::pattern::Observer<FaceAnalyzer::EventBase> {
 public:
  QtGUI() = default;
  void InitWindow();
  void OnNotify(const cv::Mat &message) override;
  void OnNotify(const FaceAnalyzer::EventBase &message) override;

 private:
  QWidget *centralwidget = nullptr;
  QGridLayout *main_grid_layout_ = nullptr;

  // 用于显示信息通知的图像。
  QPixmap info_notification_pixmap_;

  // res 资源
  QLabel *lb_bg = nullptr;
  QLabel *lb_ico = nullptr;

  // menu 栏
  // QMenuBar *menubar;
  // QMenu *menu_0, *menu_1;
  // QAction *menu_0_action_0, *menu_0_action_1, *menu_1_action_0;

  // cam 栏
  QVBoxLayout *cam_vbox_layout = nullptr;
  QGroupBox *gb_camera = nullptr;
  ResizableLabel *lb_camera = nullptr;
  QGroupBox *gb_info = nullptr;

  // info 栏
  QGridLayout *info_grid_layout = nullptr;
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

  // 关闭|最小化按钮
  QPushButton *pb_close = nullptr;
  QPushButton *pb_hide = nullptr;
};
}  // namespace arm_face_id