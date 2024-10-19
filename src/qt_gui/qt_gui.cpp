#include "qt_gui/qt_gui.h"

#include <qgridlayout.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>

#include "face_analyzer.h"
#include "utils/base.h"

using namespace arm_face_id;

void QtGUI::InitWindow() {
  main_widget_ = new QWidget;
  setCentralWidget(main_widget_);

  main_grid_layout_ = new QGridLayout(main_widget_);

  camera_frame_label_ = new QLabel("camera");
  camera_frame_label_->setFixedSize(640, 400);
  QPixmap pixmap(camera_frame_label_->size());
  QPainter painter(&pixmap);
  painter.setBrush(Qt::black);
  painter.drawRect(0, 0, pixmap.width(), pixmap.height());
  camera_frame_label_->setPixmap(pixmap);

  main_grid_layout_->addWidget(camera_frame_label_, 0, 0);
}

void QtGUI::OnNotify(const cv::Mat& message) {
  camera_frame_label_->setPixmap(
      QPixmap::fromImage(utils::mat_to_qimage(message)));
}

void QtGUI::OnNotify(const FaceAnalyzer::EventBase& message) {
  // 通过 EventBase 的 type 转换为具体的信息类型
  if (message.type == FaceAnalyzer::DETECTOR) {
    const auto& event =
        static_cast<const FaceAnalyzer::DetectorEvent&>(message);
    // 绘制人脸检测框
    QPixmap pixmap = camera_frame_label_->pixmap();
    QPainter painter(&pixmap);
    painter.setPen(Qt::yellow);
    for (int i = 0; i < event.faces.size; i++) {
      auto& face_rect = event.faces.data[i].pos;
      painter.drawRect(face_rect.x, face_rect.y, face_rect.width,
                       face_rect.height);
    }
    camera_frame_label_->setPixmap(pixmap);
  }
}