#pragma once

#include <QLabel>
#include <QPainter>
#include <QWidget>

class ResizableLabel : public QLabel {  // 定义label类（动态幕布）
 public:
  ResizableLabel(QWidget *parent = nullptr) : QLabel(parent) {}

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

 protected:
  void resizeEvent(QResizeEvent *event);
};