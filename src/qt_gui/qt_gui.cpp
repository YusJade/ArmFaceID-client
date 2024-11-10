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
#include "face_analyzer.h"
#include "utils/base.h"

using namespace arm_face_id;

constexpr int flush_notification_cnt = 10;
constexpr int KMARGIN_RIGHT = 12;

void QtGUI::InitWindow() {
  main_widget_ = new QWidget;
  setCentralWidget(main_widget_);

  main_grid_layout_ = new QGridLayout(main_widget_);

  camera_frame_label_ = new QLabel("camera");
  camera_frame_label_->setFixedSize(640, 400);

  faces_notification_pixmap_ = QPixmap(camera_frame_label_->size());
  faces_notification_pixmap_.fill(Qt::transparent);
  antispoofing_notification_pixmap_ = QPixmap(camera_frame_label_->size());
  antispoofing_notification_pixmap_.fill(Qt::transparent);

  QPixmap pixmap(camera_frame_label_->size());
  QPainter painter(&pixmap);
  painter.setBrush(Qt::black);
  painter.drawRect(0, 0, pixmap.width(), pixmap.height());
  camera_frame_label_->setPixmap(pixmap);

  main_grid_layout_->addWidget(camera_frame_label_, 0, 0);
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
    // 绘制人脸检测框
    // QPixmap pixmap = camera_frame_label_->pixmap();
    // 绘制并缓存信息提示帧，待与画面帧叠加，避免丢失信息提示帧
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
