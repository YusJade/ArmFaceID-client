#pragma once
#include <qlabel.h>
#include <qmainwindow.h>
#include <qpixmap.h>
#include <qpoint.h>
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
  bool event1(QEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void updateRegion(QMouseEvent *event);
  void resizeRegion(int marginTop, int marginBottom, int marginLeft,
                    int marginRight);
  void mouseReleaseEvent(QMouseEvent *event);
  void leaveEvent(QEvent *event);
  void OnNotify(const cv::Mat &message) override;
  void OnNotify(const FaceAnalyzer::EventBase &message) override;

 private:
  //主布局
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

  QWidget *centralwidget = nullptr;
  QGridLayout *main_grid_layout_ = nullptr;
  // res 资源
  QLabel *lb_bg = nullptr;
  QLabel *lb_ico = nullptr;
  // cam 栏
  QVBoxLayout *cam_vbox_layout = nullptr;
  QGroupBox *gb_camera = nullptr;
  ResizableLabel *lb_camera = nullptr;
  QGroupBox *gb_info = nullptr;
  QPixmap info_notification_pixmap_;
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

  void ControlButton() {
    // 创建关闭按钮
    QPushButton *closeButton = new QPushButton("X");
    closeButton->setFixedSize(30, 30);
    closeButton->setStyleSheet(
        "background-color: red; border: none; color: white;");
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    // 创建最小化按钮
    QPushButton *minimizeButton = new QPushButton("-");
    minimizeButton->setFixedSize(30, 30);
    minimizeButton->setStyleSheet(
        "background-color: yellow; border: none; color: black;");
    connect(minimizeButton, &QPushButton::clicked, this,
            &QWidget::showMinimized);
    // 创建最大化按钮
    QPushButton *maximizeButton = new QPushButton("□");
    maximizeButton->setFixedSize(30, 30);
    maximizeButton->setStyleSheet(
        "background-color: green; border: none; color: white;");
    // 连接最大化按钮的点击信号到自定义的槽
    connect(maximizeButton, &QPushButton::clicked, this,
            [this, maximizeButton]() {
              if (isMaximized) {
                this->showNormal();
                maximizeButton->setText("□");
                maximizeButton->setStyleSheet(
                    "background-color: green; border: none; color: white;");
              } else {
                this->showMaximized();
                maximizeButton->setText("⛶");
                maximizeButton->setStyleSheet(
                    "background-color: blue; border: none; color: white;");
              }
              isMaximized = !isMaximized;
            });
    // 创建水平布局，并添加按钮
    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(minimizeButton);
    hLayout->addWidget(maximizeButton);
    hLayout->addWidget(closeButton);

    // 创建一个包含按钮的 QWidget 并设置为标题栏

    QWidget *titleWidget = new QWidget();
    titleWidget->setLayout(hLayout);
    titleWidget->setStyleSheet("background-color: blue;");

    // 在窗口的右上角添加标题栏
    main_grid_layout_->addWidget(titleWidget, 0, 1, 1, 1,
                                 Qt::AlignTop | Qt::AlignRight);
  }
};
}  // namespace arm_face_id