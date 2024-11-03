#include "qt_gui/qt_gui.h"

#include <qbrush.h>
#include <qfont.h>
#include <qgridlayout.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qstringview.h>
#include <qwidget.h>

#include <QTextItem>
#include <string>

#include "Common/CStruct.h"
#include "FaceAntiSpoofing.h"
#include "QualityStructure.h"

// 包含了必要的 Qt 组件

#include "face_analyzer.h"
#include "qt_gui/qt_gui.h"
#include "utils/base.h"
// 包含了自定义头文件，准备构建 GUI。

using namespace arm_face_id;
// 引入 arm_face_id
// 命名空间，以便在后续代码中可以直接使用该命名空间下的类和函数。

constexpr int flush_notification_cnt = 10;
constexpr int KMARGIN_RIGHT = 12;

void QtGUI::InitWindow() {
  centralwidget = new QWidget;
  setCentralWidget(centralwidget);
  // 创建一个 QWidget 作为主窗口，并将其设置为中心控件。
  main_grid_layout_ = new QGridLayout(centralwidget);
  //  camera_frame_label_ = new QLabel("camera");
  //  camera_frame_label_->setFixedSize(640, 400);
  // 创建一个标签，用于显示摄像头画面，并设置其固定大小。
  //**************************新UI设计******************************//
  lb_bg = new QLabel(centralwidget);
  lb_bg->setObjectName("lb_bg");
  lb_bg->setGeometry(QRect(-20, -10, 951, 631));
  lb_bg->setAutoFillBackground(false);
  lb_bg->setPixmap(QPixmap(QString::fromUtf8("assets/bg.png")));
  lb_bg->setScaledContents(true);
  lb_ico = new QLabel(centralwidget);
  lb_ico->setObjectName("lb_ico");
  lb_ico->setGeometry(QRect(30, 30, 71, 61));
  lb_ico->setPixmap(QPixmap(QString::fromUtf8("assets/fr2.ico")));
  tw_main = new QTabWidget(centralwidget);
  tw_main->setObjectName("tw_main");
  tw_main->setGeometry(QRect(100, 40, 771, 541));
  QFont font;
  font.setFamilies(
      {QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221")});
  font.setPointSize(14);
  tw_main->setFont(font);
  tw_main->setAutoFillBackground(false);
  tw_main->setStyleSheet(
      QString::fromUtf8("QTabWidget::pane{\n"
                        "min-width:70px;\n"
                        "min-height:25px;\n"
                        "border-top: 2px solid;}\n"
                        "QTabBar::tab {\n"
                        "min-width:70px;\n"
                        "min-height:25px;\n"
                        "color: white;\n"
                        "font:16px \"Microsoft YaHei\";\n"
                        "border: 0px solid;}\n"
                        "QTabBar::tab:selected{\n"
                        "min-width:70px;\n"
                        "min-height:45px;\n"
                        "color: #4796f0;\n"
                        "font:18px \"Microsoft YaHei\";\n"
                        "border: 0px solid;\n"
                        "border-bottom: 3px solid;\n"
                        "border-color: #4796f0;}"));
  tw_main->setTabsClosable(false);
  tw_main->setMovable(false);
  tw_main->setTabBarAutoHide(false);
  tab_func = new QWidget();
  tab_func->setObjectName("tab_func");
  bg_camera = new QGroupBox(tab_func);
  bg_camera->setObjectName("bg_camera");
  // bg_camera->setGeometry(QRect(10, 10, 521, 471));
  bg_camera->setStyleSheet(QString::fromUtf8("color:white"));

  // add layout for bg
  QVBoxLayout* gb_camera_layout = new QVBoxLayout(bg_camera);
  gb_camera_layout->setContentsMargins(10, 10, 10, 10);

  camera_frame_label_ = new QLabel("camera");
  camera_frame_label_->setFixedSize(640, 400);

  faces_notification_pixmap_ = QPixmap(camera_frame_label_->size());
  faces_notification_pixmap_.fill(Qt::transparent);
  antispoofing_notification_pixmap_ = QPixmap(camera_frame_label_->size());
  antispoofing_notification_pixmap_.fill(Qt::transparent);

  QPixmap pixmap(camera_frame_label_->size());
  QPainter painter(&pixmap);
  painter.setBrush(Qt::blue);
  painter.drawRect(0, 0, pixmap.width(), pixmap.height());
  lb_camera->setPixmap(pixmap);
  gb_camera_layout->addWidget(lb_camera);

  lb_camera = new QLabel;
  lb_camera->setObjectName("lb_camera1");
  lb_camera->setGeometry(QRect(10, 20, 491, 431));
  // 创建一个与标签同样大小的 QPixmap，并填充为透明色，后续将用于叠加信息提示。
  QPixmap pixmap1(lb_camera->size());
  QPainter painter1(&pixmap);
  painter.setBrush(Qt::yellow);
  painter.drawRect(0, 0, pixmap.width(), pixmap.height());
  lb_camera->setPixmap(pixmap);
  gb_camera_layout->addWidget(lb_camera);

  gb_info = new QGroupBox(tab_func);
  gb_info->setObjectName("gb_info");
  gb_info->setGeometry(QRect(540, 30, 211, 431));
  gb_info->setStyleSheet(QString::fromUtf8("color:white"));
  gridLayout = new QGridLayout(gb_info);
  gridLayout->setObjectName("gridLayout");
  lb_email = new QLabel(gb_info);
  lb_email->setObjectName("lb_email");

  gridLayout->addWidget(lb_email, 4, 1, 1, 1);

  le_lastused = new QLineEdit(gb_info);
  le_lastused->setObjectName("le_lastused");
  le_lastused->setReadOnly(true);

  gridLayout->addWidget(le_lastused, 5, 2, 1, 1);

  lb_facequality = new QLabel(gb_info);
  lb_facequality->setObjectName("lb_facequality");

  gridLayout->addWidget(lb_facequality, 1, 1, 1, 1);

  lb_username = new QLabel(gb_info);
  lb_username->setObjectName("lb_username");

  gridLayout->addWidget(lb_username, 3, 1, 1, 1);

  le_facequality = new QLineEdit(gb_info);
  le_facequality->setObjectName("le_facequality");
  le_facequality->setReadOnly(true);

  gridLayout->addWidget(le_facequality, 1, 2, 1, 1);

  le_username = new QLineEdit(gb_info);
  le_username->setObjectName("le_username");
  le_username->setReadOnly(true);

  gridLayout->addWidget(le_username, 3, 2, 1, 1);

  lb_lastused = new QLabel(gb_info);
  lb_lastused->setObjectName("lb_lastused");

  gridLayout->addWidget(lb_lastused, 5, 1, 1, 1);

  le_email = new QLineEdit(gb_info);
  le_email->setObjectName("le_email");
  le_email->setReadOnly(true);

  gridLayout->addWidget(le_email, 4, 2, 1, 1);

  lb_liveness = new QLabel(gb_info);
  lb_liveness->setObjectName("lb_liveness");

  gridLayout->addWidget(lb_liveness, 2, 1, 1, 1);

  le_liveness = new QLineEdit(gb_info);
  le_liveness->setObjectName("le_liveness");
  le_liveness->setReadOnly(true);

  gridLayout->addWidget(le_liveness, 2, 2, 1, 1);

  tw_main->addTab(tab_func, QString());
  tab_log = new QWidget();
  tab_log->setObjectName("tab_log");
  tw_main->addTab(tab_log, QString());
  tab_about = new QWidget();
  tab_about->setObjectName("tab_about");
  lb_about = new QLabel(tab_about);
  lb_about->setObjectName("lb_about");
  lb_about->setGeometry(QRect(30, 70, 711, 411));
  lb_about->setStyleSheet(QString::fromUtf8("color:white"));
  lb_about->setTextFormat(Qt::TextFormat::AutoText);
  lb_about->setAlignment(Qt::AlignmentFlag::AlignLeading |
                         Qt::AlignmentFlag::AlignLeft |
                         Qt::AlignmentFlag::AlignTop);
  lb_about->setWordWrap(true);
  tw_main->addTab(tab_about, QString());
  pb_close = new QPushButton(centralwidget);
  pb_close->setObjectName("pb_close");
  pb_close->setGeometry(QRect(860, 50, 20, 21));
  pb_close->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  pb_close->setStyleSheet(QString::fromUtf8(
      "QPushButton{\n"
      "    background:#CE0000;\n"
      "    color:white;\n"
      "    box-shadow: 1px 1px 3px rgba(0,0,0,0.3);\n"
      "	border-radius: 8px;}\n"
      "QPushButton:hover{background:#FF1493;}\n"
      "QPushButton:pressed{border: 1px solid #3C3C3C!important;}"));
  pb_hide = new QPushButton(centralwidget);
  pb_hide->setObjectName("pb_hide");
  pb_hide->setGeometry(QRect(831, 50, 20, 21));
  pb_hide->setStyleSheet(QString::fromUtf8(
      "QPushButton{\n"
      "    background:#96d8ff;\n"
      "    color:white;\n"
      "    box-shadow: 1px 1px 3px rgba(0,0,0,0.3);\n"
      "	border-radius: 8px;}\n"
      "QPushButton:hover{background:#7FFFD4;}\n"
      "QPushButton:pressed{border: 1px solid #3C3C3C!important;}"));
  setCentralWidget(centralwidget);
  //        retranslateUi(MainWindow);
  tw_main->setCurrentIndex(0);
  //        QMetaObject::connectSlotsByName(MainWindow);

  //***************************************************************//
  info_notification_pixmap_ = QPixmap(lb_camera->size());
  info_notification_pixmap_.fill(Qt::transparent);

  // 创建一个黑色背景的 QPixmap，并将其设置为标签的显示内容。
  //   main_grid_layout_->addWidget(toolbar);
  //// 将 toolbar_frame_label_ 添加到布局中，放在网格的第一行第一列。
  //    main_grid_layout_->addWidget(info_text_edit, 1, 1);
  //// 将 info_frame_label_ 添加到布局中，放在网格的第二行第二列。'
  main_grid_layout_->addWidget(lb_camera, 1, 0);
  // 将 camera_frame_label_ 添加到布局中，放在网格的第二行第一列。

  resize(936, 611);
}

void QtGUI::OnNotify(const cv::Mat& message) {
  // 将信息提示帧与画面帧叠加
  QPixmap frame = QPixmap::fromImage(utils::mat_to_qimage(message));
  QPainter painter(&frame);
  painter.drawPixmap(0, 0, faces_notification_pixmap_);
  // painter.drawPixmap(0, 0, antispoofing_notification_pixmap_);
  camera_frame_label_->setPixmap(frame);

  ++notification_delay_cnter;
  if (notification_delay_cnter == flush_notification_cnt) {
    faces_notification_pixmap_.fill(Qt::transparent);
    //   antispoofing_notification_pixmap_.fill(Qt::transparent);
  }
}

void QtGUI::OnNotify(const FaceAnalyzer::EventBase& message) {
  notification_delay_cnter = 0;
  // 通过 EventBase 的 type 转换为具体的信息类型
  if (message.type == FaceAnalyzer::DETECTOR) {
    const auto& event =
        static_cast<const FaceAnalyzer::DetectorEvent&>(message);
    // 检查事件类型是否为人脸检测，如果是，则将事件转换为具体的DetectorEvent类型。
    //  绘制人脸检测框
    //  QPixmap pixmap = camera_frame_label_->pixmap();
    //  绘制并缓存信息提示帧，待与画面帧叠加，避免丢失信息提示帧
    faces_notification_pixmap_.fill(Qt::transparent);
    QPainter painter(&faces_notification_pixmap_);
    painter.setPen(Qt::green);
    for (int i = 0; i < event.faces.size; i++) {
      auto& face = event.faces.data[i];
      auto& rect = event.faces.data[i].pos;
      painter.drawRect(rect.x, rect.y, rect.width, rect.height);

      painter.drawText(rect.x + 5, rect.y + 15, "编号：" + QString::number(i));
      painter.drawText(rect.x + 5, rect.y + rect.height - 5,
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
      SeetaRect rect = info.face_info.pos;
      painter.setPen(QPen(Qt::red, 2));
      painter.drawRect(rect.x, rect.y, rect.width, rect.height);

      painter.drawText(rect.x + rect.width / 2 - 30, rect.y + 15, "攻击人脸 ▼");
    }
  }
}

void QtGUI::OnNotify(const FaceAnalyzer::AnalyzeMsg& message) {
  notification_delay_cnter = 0;
  faces_notification_pixmap_.fill(Qt::transparent);
  int num = -1;
  for (auto res : message.res) {
    num++;
    SeetaRect rect = res.face.pos;
    // 人脸框标注
    faces_notification_pixmap_.fill(Qt::transparent);
    QPainter painter(&faces_notification_pixmap_);
    painter.setPen(Qt::green);
    painter.drawRect(rect.x, rect.y, rect.width, rect.height);
    painter.drawText(rect.x + 5, rect.y + 15, "编号:" + QString::number(num));
    painter.drawText(rect.x + 5, rect.y + rect.height - 5,
                     "分数:" + QString::number(res.face.score, 'g', 5));

    // 活体标注
    // painter.save();
    painter.setFont(QFont("consola"));
    QRect text_rect(rect.x + rect.width - 50, rect.y, 50, 20);
    // painter.translate(rect.x + rect.width - 3, rect.y + 18);
    // painter.rotate(90);
    switch (res.status) {
      case seeta::FaceAntiSpoofing::REAL:
        painter.setPen(QPen(Qt::green));
        painter.drawText(text_rect, Qt::AlignRight, "REAL");
        break;
      case seeta::FaceAntiSpoofing::SPOOF:
        painter.setPen(QPen(Qt::red));
        painter.drawText(text_rect, Qt::AlignRight, "SPOOF");
        break;
      case seeta::FaceAntiSpoofing::FUZZY:
        painter.setPen(QPen(Qt::yellow));
        painter.drawText(text_rect, Qt::AlignRight, "FUZZY");
        break;
      case seeta::FaceAntiSpoofing::DETECTING:
        painter.setPen(QPen(Qt::yellow));
        painter.drawText(text_rect, Qt::AlignRight, "DETECTING");
        break;
    }

    float quality_width_rate = 0.4;
    // 清晰度标注
    int rect_right = rect.x + rect.width;
    int quality_width = rect.width * quality_width_rate;
    int cur_floor = 6;
    int rect_height_per = 0.1 * rect.height;
    text_rect.setRect(rect_right - quality_width,
                      rect.y + rect_height_per * cur_floor++, quality_width,
                      15);
    switch (res.clarity_res.level) {
      case seeta::QualityLevel::LOW:
        painter.setPen(QPen(Qt::red));
        painter.drawText(text_rect, Qt::AlignRight, "清晰度:L");
        break;
      case seeta::QualityLevel::MEDIUM:
        painter.setPen(QPen(Qt::yellow));
        painter.drawText(text_rect, Qt::AlignRight, "清晰度:M");
        break;
      case seeta::QualityLevel::HIGH:
        painter.setPen(QPen(Qt::green));
        painter.drawText(text_rect, Qt::AlignRight, "清晰度:H");
        break;
    }
    // 完整度标注
    text_rect.setRect(rect_right - quality_width,
                      rect.y + rect_height_per * cur_floor++, quality_width,
                      15);
    switch (res.integrity_res.level) {
      case seeta::QualityLevel::LOW:
        painter.setPen(QPen(Qt::red));
        painter.drawText(text_rect, Qt::AlignRight, "完整度:L");
        break;
      case seeta::QualityLevel::MEDIUM:
        painter.setPen(QPen(Qt::yellow));
        painter.drawText(text_rect, Qt::AlignRight, "完整度:M");
        break;
      case seeta::QualityLevel::HIGH:
        painter.setPen(QPen(Qt::green));
        painter.drawText(text_rect, Qt::AlignRight, "完整度:H");
        break;
    }

    // 分辨率标注
    text_rect.setRect(rect_right - quality_width,
                      rect.y + rect_height_per * cur_floor++, quality_width,
                      15);
    switch (res.resolution_res.level) {
      case seeta::QualityLevel::LOW:
        painter.setPen(QPen(Qt::red));
        painter.drawText(text_rect, Qt::AlignRight, "分辨率:L");
        break;
      case seeta::QualityLevel::MEDIUM:
        painter.setPen(QPen(Qt::yellow));
        painter.drawText(text_rect, Qt::AlignRight, "分辨率:M");
        break;
      case seeta::QualityLevel::HIGH:
        painter.setPen(QPen(Qt::green));
        painter.drawText(text_rect, Qt::AlignRight, "分辨率:H");
        break;
    }

    // 姿态标注
    text_rect.setRect(rect_right - quality_width,
                      rect.y + rect_height_per * cur_floor++, quality_width,
                      15);
    switch (res.pose_res.level) {
      case seeta::QualityLevel::LOW:
        painter.setPen(QPen(Qt::red));
        painter.drawText(text_rect, Qt::AlignRight, "  姿态:L");
        break;
      case seeta::QualityLevel::MEDIUM:
        painter.setPen(QPen(Qt::yellow));
        painter.drawText(text_rect, Qt::AlignRight, "  姿态:M");
        break;
      case seeta::QualityLevel::HIGH:
        painter.setPen(QPen(Qt::green));
        painter.drawText(text_rect, Qt::AlignRight, "  姿态:H");
        break;
    }
  }
}
