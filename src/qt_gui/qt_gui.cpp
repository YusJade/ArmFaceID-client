#include "qt_gui/qt_gui.h"

#include <qcoreapplication.h>
#include <qgridlayout.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qstringview.h>
#include <qwidget.h>

#include <string>

#include "face_analyzer.h"
#include "qt_gui/notification.h"
#include "utils/base.h"

using namespace arm_face_id;

constexpr int flush_notification_cnt = 10;

void QtGUI::InitWindow() {
  centralwidget = new QWidget;
  setCentralWidget(centralwidget);
  main_grid_layout_ = new QGridLayout(centralwidget);

  // 设置大小策略
  QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
  centralwidget->setSizePolicy(sizePolicy);
  centralwidget->setMinimumSize(400, 200);
  centralwidget->setStyleSheet(QString::fromUtf8("background-color:#272b4c"));
  // // 设置背景图片
  // lb_bg = new QLabel(centralwidget);
  // main_grid_layout_->addWidget(lb_bg, 0, 0, 1, 2);
  // lb_bg->setSizePolicy(sizePolicy);
  // lb_bg->setPixmap(QPixmap(QString::fromUtf8("assets/bg.png")));
  // lb_bg->setScaledContents(true);
  // lb_bg->setMinimumSize(380, 180);

  // menu栏
  // menubar = new QMenuBar(centralwidget);
  // menubar->setObjectName("menubar");
  // // menubar->setGeometry(QRect(0, 0, 863, 33));
  // menu_0 = new QMenu(menubar);
  // menu_0->setObjectName("menu");
  // menu_1 = new QMenu(menubar);
  // menu_1->setObjectName("menutool");
  // // centralwidget->setMenuBar(menubar);
  // // statusbar = new QStatusBar(centralwidget);
  // // statusbar->setObjectName("statusbar");
  // // centralwidget->setStatusBar(statusbar);
  // menubar->addAction(menu_0->menuAction());
  // menu_0->addAction(menu_0_action_0);
  // menu_0->addAction(menu_0_action_1);
  // menubar->addAction(menu_1->menuAction());
  // menu_1->addAction(menu_1_action_0);

  // cam 栏目
  gb_camera = new QGroupBox(lb_bg);
  main_grid_layout_->addWidget(gb_camera, 0, 0);
  main_grid_layout_->setColumnStretch(0, 2);
  cam_vbox_layout = new QVBoxLayout(gb_camera);
  // cam 幕布
  lb_camera = new QLabel;
  cam_vbox_layout->addWidget(lb_camera);
  QPixmap pixmap_cam(lb_camera->size());
  QPainter painter_cam(&pixmap_cam);
  painter_cam.setBrush(Qt::blue);
  painter_cam.drawRect(0, 0, pixmap_cam.width(), pixmap_cam.height());
  lb_camera->setPixmap(pixmap_cam);
  faces_notification_pixmap_ = QPixmap(lb_camera->size());
  faces_notification_pixmap_.fill(Qt::transparent);
  antispoofing_notification_pixmap_ = QPixmap(lb_camera->size());
  antispoofing_notification_pixmap_.fill(Qt::transparent);

  // info 栏目
  gb_info = new QGroupBox(lb_bg);
  main_grid_layout_->addWidget(gb_info, 0, 1);
  main_grid_layout_->setColumnStretch(1, 1);
  gb_info->setStyleSheet(QString::fromUtf8("color:white"));
  info_grid_layout = new QGridLayout(gb_info);
  // 显示用户个性头像
  lb_headshot = new QLabel(gb_info);
  info_grid_layout->addWidget(lb_headshot, 0, 1, 2, 2);
  info_grid_layout->setColumnStretch(1, 2);
  info_grid_layout->setRowStretch(0, 2);
  QPixmap pixmap_hs(lb_headshot->size());
  QPainter painter_hs(&pixmap_hs);
  painter_hs.setBrush(Qt::red);
  painter_hs.drawRect(0, 0, pixmap_hs.width(), pixmap_hs.height());
  lb_headshot->setPixmap(pixmap_hs);
  // 显示用户昵称
  lb_username = new QLabel(gb_info);
  info_grid_layout->addWidget(lb_username, 3, 1, 1, 1);
  le_username = new QLineEdit(gb_info);
  le_username->setReadOnly(true);
  info_grid_layout->addWidget(le_username, 3, 2, 1, 1);
  // 显示电子邮箱
  lb_email = new QLabel(gb_info);
  info_grid_layout->addWidget(lb_email, 4, 1, 1, 1);
  le_email = new QLineEdit(gb_info);
  le_email->setReadOnly(true);
  info_grid_layout->addWidget(le_email, 4, 2, 1, 1);
  // 显示上一次使用时间
  lb_lastused = new QLabel(gb_info);
  info_grid_layout->addWidget(lb_lastused, 5, 1, 1, 1);
  le_lastused = new QLineEdit(gb_info);
  le_lastused->setReadOnly(true);
  info_grid_layout->addWidget(le_lastused, 5, 2, 1, 1);
  // 测试用按钮，点击弹出 人脸质量检测 结果
  pb_verify1 = new QPushButton("FunctionVerify1", gb_info);
  info_grid_layout->addWidget(pb_verify1, 6, 1, 1, 2);
  // 测试用按钮，点击弹出 活体检测 结果
  pb_verify2 = new QPushButton("FunctionVerify2", gb_info);
  info_grid_layout->addWidget(pb_verify2, 7, 1, 1, 2);

  // // 关闭按钮
  // pb_close = new QPushButton(centralwidget);
  // pb_close->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  // pb_close->setStyleSheet(QString::fromUtf8(
  //     "QPushButton{\n"
  //     "    background:#CE0000;\n"
  //     "    box-shadow: 1px 1px 3px rgba(0,0,0,0.3);\n"
  //     "	border-radius: 8px;}\n"
  //     "QPushButton:hover{background:#FF1493;}\n"
  //     "QPushButton:pressed{border: 1px solid #3C3C3C!important;}"));

  // // 最小化按钮
  // pb_hide = new QPushButton(centralwidget);
  // pb_hide->setStyleSheet(QString::fromUtf8(
  //     "QPushButton{\n"
  //     "    background:#96d8ff;\n"
  //     "    box-shadow: 1px 1px 3px rgba(0,0,0,0.3);\n"
  //     "	border-radius: 8px;}\n"
  //     "QPushButton:hover{background:#7FFFD4;}\n"
  //     "QPushButton:pressed{border: 1px solid #3C3C3C!important;}"));

  // 赋名
  gb_info->setTitle(
      QCoreApplication::translate("MainWindow", "Information", nullptr));
  lb_username->setText(
      QCoreApplication::translate("MainWindow", "Username", nullptr));
  lb_email->setText(
      QCoreApplication::translate("MainWindow", "Email", nullptr));
  lb_lastused->setText(
      QCoreApplication::translate("MainWindow", "LastUsed", nullptr));

  // menu_0->setTitle(QCoreApplication::translate("MainWindow", "menu0",
  // nullptr)); menu_1->setTitle(QCoreApplication::translate("MainWindow",
  // "menu1", nullptr)); menu_0_action_0->setText(
  //     QCoreApplication::translate("MainWindow", "Start", nullptr));
  // menu_0_action_1->setText(
  //     QCoreApplication::translate("MainWindow", "Stop", nullptr));
  // menu_1_action_0->setText(
  //     QCoreApplication::translate("MainWindow", "...", nullptr));

  // 测试按钮
  nf1 = new Notification(centralwidget);
  connect(pb_verify1, &QPushButton::clicked, this, [&]() {
    nf1->Push(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
              "Warning", "The face-quality-detection failed!", 2000);
  });
  pb_verify1->move(200, 50);
  nf2 = new Notification(centralwidget);
  connect(pb_verify2, &QPushButton::clicked, this, [&]() {
    nf2->Push(NotifyType::Notify_Type_Error, NotifyPosition::Pos_Top_Left,
              "Error", "The liveness-detection failed!", 1000);
  });
  pb_verify2->move(200, 50);
}

void QtGUI::OnNotify(const cv::Mat& message) {
  // 将信息提示帧与画面帧叠加
  QPixmap frame = QPixmap::fromImage(utils::mat_to_qimage(message));
  QPainter painter(&frame);
  painter.drawPixmap(0, 0, faces_notification_pixmap_);
  painter.drawPixmap(0, 0, antispoofing_notification_pixmap_);
  lb_camera->setPixmap(frame);

  ++notification_delay_cnter;
  if (notification_delay_cnter == flush_notification_cnt) {
    faces_notification_pixmap_.fill(Qt::transparent);
    antispoofing_notification_pixmap_.fill(Qt::transparent);
  }
}

void QtGUI::OnNotify(const FaceAnalyzer::EventBase& message) {
  notification_delay_cnter = 0;
  // 通过 EventBase 的 type 转换为具体的信息类型
  if (message.type == FaceAnalyzer::DETECTOR) {
    const auto& event =
        static_cast<const FaceAnalyzer::DetectorEvent&>(message);
    // 绘制人脸检测框
    // QPixmap pixmap = camera_frame_label_->pixmap();
    // 绘制并缓存信息提示帧，待与画面帧叠加，避免丢失信息提示帧
    faces_notification_pixmap_.fill(Qt::transparent);
    QPainter painter(&faces_notification_pixmap_);
    painter.setPen(Qt::green);
    for (int i = 0; i < event.faces.size; i++) {
      auto& face = event.faces.data[i];
      auto& face_rect = event.faces.data[i].pos;
      painter.drawRect(face_rect.x, face_rect.y, face_rect.width,
                       face_rect.height);

      painter.drawText(face_rect.x + 5, face_rect.y + 15,
                       "编号：" + QString::number(i));
      painter.drawText(face_rect.x + 5, face_rect.y + face_rect.height - 5,
                       "分数：" + QString::number(face.score));
    }
    // camera_frame_label_->setPixmap(pixmap);
  } else if (message.type == FaceAnalyzer::ANTISPOOFING) {
    const auto& event =
        static_cast<const FaceAnalyzer::AntiSpoofingEvent&>(message);

    QPainter painter(&antispoofing_notification_pixmap_);
    antispoofing_notification_pixmap_.fill(Qt::transparent);
    for (auto info : event.infos) {
      if (info.status == seeta::FaceAntiSpoofing::REAL) continue;
      SeetaRect face_rect = info.face_info.pos;
      painter.setPen(QPen(Qt::red, 2));
      painter.drawRect(face_rect.x, face_rect.y, face_rect.width,
                       face_rect.height);

      painter.drawText(face_rect.x + face_rect.width / 2 - 30, face_rect.y + 15,
                       "攻击人脸 ▼");
    }
  }
}