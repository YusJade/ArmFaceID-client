#include "qt_gui/qt_gui.h"

#include <qapplication.h>
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

void QtGUI::InitWindow() {
  // 设置大小策略
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  // 主窗口
  centralwidget = new QWidget;
  setCentralWidget(centralwidget);
  main_grid_layout_ = new QGridLayout(centralwidget);
  main_grid_layout_->setContentsMargins(0, 0, 0, 0);
  centralwidget->setSizePolicy(sizePolicy);
  centralwidget->setMinimumSize(800, 400);
  // centralwidget->setStyleSheet("background-color:#272b4c");

  setWindowFlags(Qt::FramelessWindowHint);     //隐藏标题栏（无边框）
  setAttribute(Qt::WA_StyledBackground);       //启用样式背景绘制
  setAttribute(Qt::WA_TranslucentBackground);  //背景透明
  setAttribute(Qt::WA_Hover);

  // 背景标签
  lb_bg = new QLabel(centralwidget);
  lb_bg->setPixmap(QPixmap("assets/bg.png"));
  lb_bg->setScaledContents(true);
  // lb_bg->setContentsMargins(0, 0, 0, 0);
  lb_bg->setStyleSheet("background-color:red");
  main_grid_layout_->addWidget(lb_bg, 0, 0, 1, 1);

  // 创建子容器
  QWidget* bgwidget = new QWidget(centralwidget);
  QGridLayout* bg_grid_layout = new QGridLayout(bgwidget);
  bgwidget->setLayout(bg_grid_layout);
  main_grid_layout_->addWidget(bgwidget, 0, 0, 1, 1);
  bg_grid_layout->setContentsMargins(40, 80, 40, 20);
  bgwidget->raise();

  // cam 栏目
  gb_camera = new QGroupBox(lb_bg);
  bg_grid_layout->addWidget(gb_camera, 0, 0);
  bg_grid_layout->setColumnStretch(0, 2);
  cam_vbox_layout = new QVBoxLayout(gb_camera);
  lb_camera = new ResizableLabel;
  cam_vbox_layout->addWidget(lb_camera);
  QPixmap info_notification_pixmap_(lb_camera->size());
  info_notification_pixmap_.fill(Qt::transparent);

  // info 栏目
  gb_info = new QGroupBox(lb_bg);
  bg_grid_layout->addWidget(gb_info, 0, 1);
  bg_grid_layout->setColumnStretch(1, 1);
  gb_info->setStyleSheet("color:white");
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

  ControlButton();
  // // 关闭按钮
  // pb_close = new QPushButton(centralwidget);
  // pb_close->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
  // pb_close->setStyleSheet(
  //     "QPushButton{\n"
  //     "    background:#CE0000;\n"
  //     "    box-shadow: 1px 1px 3px rgba(0,0,0,0.3);\n"
  //     "	border-radius: 8px;}\n"
  //     "QPushButton:hover{background:#FF1493;}\n"
  //     "QPushButton:pressed{border: 1px solid #3C3C3C!important;}");

  // 备注
  gb_info->setTitle(
      QCoreApplication::translate("MainWindow", "Information", nullptr));
  lb_username->setText(
      QCoreApplication::translate("MainWindow", "Username", nullptr));
  lb_email->setText(
      QCoreApplication::translate("MainWindow", "Email", nullptr));
  lb_lastused->setText(
      QCoreApplication::translate("MainWindow", "LastUsed", nullptr));

  // 测试按钮
  nf1 = new Notification(bgwidget);
  connect(pb_verify1, &QPushButton::clicked, this, [&]() {
    nf1->Push(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
              "Warning", "The face-quality-detection failed!", 2000);
  });
  pb_verify1->move(200, 50);
  nf2 = new Notification(bgwidget);
  connect(pb_verify2, &QPushButton::clicked, this, [&]() {
    nf2->Push(NotifyType::Notify_Type_Error, NotifyPosition::Pos_Top_Left,
              "Error", "The liveness-detection failed!", 2000);
  });
  pb_verify2->move(200, 50);
}

bool QtGUI::event1(QEvent* event) {
  if (event->type() == QEvent::HoverMove) {
    QHoverEvent* hoverEvent = static_cast<QHoverEvent*>(event);
    QMouseEvent mouseEvent(QEvent::MouseMove, hoverEvent->position(),
                           Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    mouseMoveEvent(&mouseEvent);
  }
  return QWidget::event(event);
}
void QtGUI::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    m_bIsPressed = true;
    m_pressPoint = event->globalPosition();
  }
  return QWidget::mousePressEvent(event);
}
void QtGUI::mouseMoveEvent(QMouseEvent* event) {
  if (m_bIsPressed) {
    if (m_bIsResizing) {
      m_movePoint = event->globalPosition() - m_pressPoint;
      m_pressPoint += m_movePoint;
    } else {
      if (!m_bIsDoublePressed && windowState() == Qt::WindowMaximized) {
        // restoreWidget();
        // 使用QScreen获取屏幕尺寸
        QScreen* screen = QGuiApplication::primaryScreen();  // 获取主屏幕
        QRect screenGeometry = screen->geometry();  // 获取屏幕的几何尺寸

        QPointF point(width() * ((double)(event->globalPosition().x()) /
                                 screenGeometry.width()),
                      height() * ((double)(event->globalPosition().y()) /
                                  screenGeometry.height()));

        move((event->globalPosition() - point).toPoint());
        m_pressPoint = event->globalPosition();
      }

      QPointF point = event->globalPosition() - m_pressPoint;
      move((pos() + point).toPoint());

      m_pressPoint = event->globalPosition();
    }
  }
  if (windowState() != Qt::WindowMaximized) {
    updateRegion(event);
  }
  QWidget::mouseMoveEvent(event);
}
void QtGUI::updateRegion(QMouseEvent* event) {
  QRect mainRect = geometry();

  // 获取鼠标的全局位置
  QPointF globalPos = event->globalPosition();

  int marginTop = globalPos.y() - mainRect.y();
  int marginBottom = mainRect.y() + mainRect.height() - globalPos.y();
  int marginLeft = globalPos.x() - mainRect.x();
  int marginRight = mainRect.x() + mainRect.width() - globalPos.x();

  // 设置最小和最大边缘尺寸常量
  const int MARGIN_MIN_SIZE = 10;  // 最小边缘尺寸，适合调整
  const int MARGIN_MAX_SIZE = 20;  // 最大边缘尺寸，防止缩放过小或过大

  if (!m_bIsResizing) {
    // 判断鼠标是否在可缩放区域内，并设置相应的鼠标指针
    if ((marginRight >= MARGIN_MIN_SIZE && marginRight <= MARGIN_MAX_SIZE) &&
        (marginBottom <= MARGIN_MAX_SIZE && marginBottom >= MARGIN_MIN_SIZE)) {
      m_direction = BOTTOMRIGHT;
      setCursor(Qt::SizeFDiagCursor);
    } else if ((marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE) &&
               (marginRight >= MARGIN_MIN_SIZE &&
                marginRight <= MARGIN_MAX_SIZE)) {
      m_direction = TOPRIGHT;
      setCursor(Qt::SizeBDiagCursor);
    } else if ((marginLeft >= MARGIN_MIN_SIZE &&
                marginLeft <= MARGIN_MAX_SIZE) &&
               (marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE)) {
      m_direction = TOPLEFT;
      setCursor(Qt::SizeFDiagCursor);
    } else if ((marginLeft >= MARGIN_MIN_SIZE &&
                marginLeft <= MARGIN_MAX_SIZE) &&
               (marginBottom >= MARGIN_MIN_SIZE &&
                marginBottom <= MARGIN_MAX_SIZE)) {
      m_direction = BOTTOMLEFT;
      setCursor(Qt::SizeBDiagCursor);
    } else if (marginBottom <= MARGIN_MAX_SIZE &&
               marginBottom >= MARGIN_MIN_SIZE) {
      m_direction = DOWN;
      setCursor(Qt::SizeVerCursor);
    } else if (marginLeft <= MARGIN_MAX_SIZE - 1 &&
               marginLeft >= MARGIN_MIN_SIZE - 1) {
      m_direction = LEFT;
      setCursor(Qt::SizeHorCursor);
    } else if (marginRight <= MARGIN_MAX_SIZE &&
               marginRight >= MARGIN_MIN_SIZE) {
      m_direction = RIGHT;
      setCursor(Qt::SizeHorCursor);
    } else if (marginTop <= MARGIN_MAX_SIZE && marginTop >= MARGIN_MIN_SIZE) {
      m_direction = UP;
      setCursor(Qt::SizeVerCursor);
    } else {
      if (!m_bIsPressed) {
        setCursor(Qt::ArrowCursor);
      }
    }
  }
  if (NONE != m_direction) {
    m_bIsResizing = true;
    resizeRegion(marginTop, marginBottom, marginLeft, marginRight);
  }
}
void QtGUI::resizeRegion(int marginTop, int marginBottom, int marginLeft,
                         int marginRight) {
  if (m_bIsPressed) {
    switch (m_direction) {
      case BOTTOMRIGHT: {
        QRect rect = geometry();
        rect.setBottomRight((rect.bottomRight() + m_movePoint).toPoint());
        setGeometry(rect);
      } break;
      case TOPRIGHT: {
        if (marginLeft > minimumWidth() && marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTopRight((rect.topRight() + m_movePoint).toPoint());
          setGeometry(rect);
        }
      } break;
      case TOPLEFT: {
        if (marginRight > minimumWidth() && marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTopLeft((rect.topLeft() + m_movePoint).toPoint());
          setGeometry(rect);
        }
      } break;
      case BOTTOMLEFT: {
        if (marginRight > minimumWidth() && marginTop > minimumHeight()) {
          QRect rect = geometry();
          rect.setBottomLeft((rect.bottomLeft() + m_movePoint).toPoint());
          setGeometry(rect);
        }
      } break;
      case RIGHT: {
        QRect rect = geometry();
        rect.setWidth(rect.width() + m_movePoint.x());
        setGeometry(rect);
      } break;
      case DOWN: {
        QRect rect = geometry();
        rect.setHeight(rect.height() + m_movePoint.y());
        setGeometry(rect);
      } break;
      case LEFT: {
        if (marginRight > minimumWidth()) {
          QRect rect = geometry();
          rect.setLeft(rect.x() + m_movePoint.x());
          setGeometry(rect);
        }
      } break;
      case UP: {
        if (marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTop(rect.y() + m_movePoint.y());
          setGeometry(rect);
        }
      } break;
      default: {
      } break;
    }
  } else {
    m_bIsResizing = false;
    m_direction = NONE;
  }
}
void QtGUI::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    m_bIsPressed = false;
    m_bIsResizing = false;
    m_bIsDoublePressed = false;
  }
  QWidget::mouseReleaseEvent(event);
}
void QtGUI::leaveEvent(QEvent* event) {
  m_bIsPressed = false;
  m_bIsDoublePressed = false;
  m_bIsResizing = false;
  QWidget::leaveEvent(event);
}

void QtGUI::OnNotify(const cv::Mat& message) {
  QSize lb_size = lb_camera->size();
  QPixmap frame = QPixmap::fromImage(utils::mat_to_qimage(message));
  frame = frame.scaled(lb_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QPainter painter(&frame);
  painter.drawPixmap(0, 0, info_notification_pixmap_);
  lb_camera->setPixmap(frame);
}

void QtGUI::OnNotify(const FaceAnalyzer::EventBase& message) {
  if (message.type == FaceAnalyzer::DETECTOR) {
    const auto& event =
        static_cast<const FaceAnalyzer::DetectorEvent&>(message);
    info_notification_pixmap_.fill(Qt::transparent);
    QPainter painter(&info_notification_pixmap_);
    painter.setPen(Qt::yellow);
    for (int i = 0; i < event.faces.size; i++) {
      auto& face = event.faces.data[i];
      auto& face_rect = event.faces.data[i].pos;
      painter.drawRect(face_rect.x, face_rect.y, face_rect.width,
                       face_rect.height);
      painter.drawText(face_rect.x + 5, face_rect.y + 15,
                       "code: " + QString::number(i));
      painter.drawText(face_rect.x + 5, face_rect.y + face_rect.height - 5,
                       "score: " + QString::number(face.score));
    }
  }
}