#include "qt_gui/resizable_label.h"

#include <QResizeEvent>

void ResizableLabel::resizeEvent(QResizeEvent *event) {
  QLabel::resizeEvent(event);
  QSize newSize = event->size();
  QPixmap pixmap_cam(newSize);
  pixmap_cam.fill(Qt::transparent);
  QPainter painter_cam(&pixmap_cam);
  painter_cam.setBrush(Qt::black);
  painter_cam.drawRect(0, 0, pixmap_cam.width(), pixmap_cam.height());
  this->setPixmap(pixmap_cam);
}