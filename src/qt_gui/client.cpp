#include "qt_gui/client.h"

#include <qimage.h>
#include <qobject.h>
#include <qpixmap.h>

#include "utils/base.h"

using namespace arm_face_id;

void Client::CheckNeedNotification(const FaceAnalyzer::AnalyzeMsg& message) {
  if (message.res.empty()) return;
  auto top_res = message.res.front();
  if (top_res.integrity_res.level <= seeta::QualityLevel::MEDIUM) {
    notifybar_disappear_tick = 0;
    emit notify(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
                "提示", "请让脸部完整出现在区域内");
  } else if (top_res.pose_res.level <= seeta::QualityLevel::MEDIUM) {
    notifybar_disappear_tick = 0;
    emit notify(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
                "提示", "请让正对摄像头");
  }
}

void Client::OnNotify(const cv::Mat& message) {
  // 将信息提示帧与画面帧叠加
  QPixmap frame = QPixmap::fromImage(utils::mat_to_qimage(message));
  QPainter painter(&frame);
  painter.drawPixmap(0, 0, hud_pixmap_);
  // painter.drawPixmap(0, 0, antispoofing_notification_pixmap_);
  lb_camera_->setPixmap(frame);

  ++hud_keepalive_tick;
  if (hud_keepalive_tick == hud_cleanup_tick) {
    hud_pixmap_.fill(Qt::transparent);
    //   antispoofing_notification_pixmap_.fill(Qt::transparent);
  }
}

void Client::OnNotify(const FaceAnalyzer::AnalyzeMsg& message) {
  notifybar_disappear_tick++;
  if (notifybar_disappear_tick >= notifybar_cooldown_tick) {
    CheckNeedNotification(message);
  }
  QFont default_font("Arial");
  default_font.setPixelSize(12);
  hud_keepalive_tick = 0;
  hud_pixmap_.fill(Qt::transparent);
  int num = -1;
  for (auto res : message.res) {
    num++;
    SeetaRect rect = res.face.pos;
    // 人脸框标注
    hud_pixmap_.fill(Qt::transparent);
    QPainter painter(&hud_pixmap_);
    painter.setFont(default_font);
    painter.setPen(Qt::green);

    painter.drawRect(rect.x, rect.y, rect.width, rect.height);
    painter.drawText(rect.x + 5, rect.y + 15, "编号:" + QString::number(num));
    painter.drawText(rect.x + 5, rect.y + rect.height - 5,
                     "分数:" + QString::number(res.face.score, 'g', 5));

    // 活体标注
    // painter.save();

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

void Client::OnNotify(const UserInfo& message) {
  if (message.user_id() < 0) return;
  if (notifybar_disappear_tick > notifybar_cooldown_tick) {
    emit notify(NotifyType::Notify_Type_Success, NotifyPosition::Pos_Top_Left,
                "成功",
                QString::fromStdString("欢迎你，" + message.user_name()));
    notifybar_disappear_tick = 0;
  }
  le_email_->setText(QString::fromStdString(message.email()));
  QByteArray pic_array(message.profile_picture().data(),
                       message.profile_picture().size());
  QImage pic;
  QDataStream stream(pic_array);
  stream >> pic;
  lb_headshot_->setPixmap(QPixmap::fromImage(pic));
  le_username_->setText(QString::fromStdString(message.user_name()));
  le_lastused_->setText(
      QString::fromStdString(message.last_recognized_datetime()));
}