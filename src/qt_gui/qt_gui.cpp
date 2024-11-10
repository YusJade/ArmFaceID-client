#include "qt_gui/qt_gui.h"

#include <qapplication.h>
#include <qboxlayout.h>
#include <qcoreapplication.h>
#include <qgridlayout.h>
#include <qgroupbox.h>
#include <qicon.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlayoutitem.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qstringview.h>
#include <qwidget.h>

#include <QFontDatabase>
#include <string>

#include "face_analyzer.h"
#include "qt_gui/notification.h"
#include "utils/base.h"
using namespace arm_face_id;

void QtGUI::InitWindow() {
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_StyledBackground);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_Hover);

  int fontId = QFontDatabase::addApplicationFont(
      QStringLiteral("assets/Cyberpunk-Regular.ttf"));
  QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
  qDebug() << "fontfamilies:" << fontFamilies;
  if (fontFamilies.size() > 0) {
    QFont font(fontFamilies[0], 16);
    font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
    QApplication::setFont(font);
  }

  // 主窗口
  centralwidget = new QWidget;
  setCentralWidget(centralwidget);
  main_grid_layout_ = new QGridLayout(centralwidget);
  main_grid_layout_->setContentsMargins(0, 0, 0, 0);
  centralwidget->setSizePolicy(sizePolicy);
  centralwidget->setMinimumSize(800, 400);

  // 背景标签
  lb_bg = new QLabel(centralwidget);
  lb_bg->setPixmap(QPixmap("assets/bg5.png"));
  lb_bg->setScaledContents(true);
  main_grid_layout_->addWidget(lb_bg, 0, 0, 1, 1);

  // 创建子容器
  contwidget = new QWidget(centralwidget);
  cont_grid_layout = new QGridLayout(contwidget);
  contwidget->setLayout(cont_grid_layout);
  main_grid_layout_->addWidget(contwidget, 0, 0, 1, 1);
  cont_grid_layout->setContentsMargins(15, 15, 25, 15);
  contwidget->raise();

  // 一些样式
  //   QString btn_style = R"(
  //     QPushButton {
  //         background: transparent;
  //         border: 1px solid #0ffff3;
  //         color: #0ffff3;
  //         font-weight: bold;
  //         text-align: center;
  //         outline: none;
  //     }
  //     QPushButton:hover {
  //         color: #8421FF;
  //         background-color: #0ffff3;
  //     }
  //     QPushButton:pressed {
  //         padding-left: 6px;
  //         padding-top: 6px;
  //     }
  // )";

  ControlButton();

  // tool 栏目
  gb_tool = new QGroupBox(lb_bg);
  cont_grid_layout->addWidget(gb_tool, 0, 0, 2, 1);
  tool_vbox_layout = new QVBoxLayout(gb_tool);
  lb_logo = new QLabel(gb_tool);
  lb_logo->setPixmap(QPixmap("assets/logo3.ico"));
  lb_logo->setFixedSize(50, 50);
  lb_logo->setScaledContents(true);
  gb_tool->setStyleSheet(
      "QGroupBox { border: none;margin: 0px; max-width: "
      "60px;margin-right:5px;}");
  tool_vbox_layout->addWidget(lb_logo);
  btn1 = new QPushButton("Go", gb_tool);
  btn1->setFixedSize(50, 50);
  btn2 = new QPushButton("Stop", gb_tool);
  btn2->setFixedSize(50, 50);
  btn3 = new QPushButton("...", gb_tool);
  btn3->setFixedSize(50, 50);
  btn1->setStyleSheet(btn_style_small);
  btn2->setStyleSheet(btn_style_small);
  btn3->setStyleSheet(btn_style_small);
  btn1->setVisible(false);
  btn2->setVisible(false);
  btn3->setVisible(false);
  tool_vbox_layout->addWidget(btn1);
  tool_vbox_layout->addWidget(btn2);
  tool_vbox_layout->addWidget(btn3);
  tool_vbox_layout->setSpacing(10);
  tool_vbox_layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  anim1 = new QPropertyAnimation(btn1, "pos");
  anim2 = new QPropertyAnimation(btn2, "pos");
  anim3 = new QPropertyAnimation(btn3, "pos");
  anim1->setDuration(200);
  anim2->setDuration(300);
  anim3->setDuration(400);
  anim1->setStartValue(QPoint(10, 60));
  anim1->setEndValue(QPoint(10, 80));
  anim2->setStartValue(QPoint(10, 65));
  anim2->setEndValue(QPoint(10, 140));
  anim3->setStartValue(QPoint(10, 70));
  anim3->setEndValue(QPoint(10, 200));
  lb_logo->installEventFilter(this);
  gb_tool->installEventFilter(this);

  // cam 栏目
  gb_camera = new QGroupBox(lb_bg);
  gb_camera->setStyleSheet(
      "QGroupBox {min-width:400px; "
      "min-height:200px;margin-top:15px;background-color:#121630;}");
  cont_grid_layout->addWidget(gb_camera, 0, 1, 2, 1);
  cam_vbox_layout = new QVBoxLayout(gb_camera);
  lb_camera = new ResizableLabel;
  cam_vbox_layout->addWidget(lb_camera);
  QPixmap info_notification_pixmap_(lb_camera->size());
  info_notification_pixmap_.fill(Qt::transparent);

  // info 栏目
  QSpacerItem* info_spacer =
      new QSpacerItem(10, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);
  gb_info = new QGroupBox(lb_bg);
  cont_grid_layout->addWidget(gb_info, 1, 2, 1, 1);
  gb_info->setStyleSheet(
      "QGroupBox {"
      "   border:transparent;"
      "   min-width: 90px;"
      "}"
      "QGroupBox QWidget {"
      "   color: #0ffff3;"
      "}");
  info_vbox_layout = new QVBoxLayout(gb_info);
  info_vbox_layout->setSpacing(0);
  QString lineEditStyle = "QLineEdit {background-color:#1A1F3D;color:white;}";

  // 显示用户个性头像
  lb_headshot = new ResizableLabel(gb_info);
  info_vbox_layout->addWidget(lb_headshot, 3);
  lb_headshot->setStyleSheet("background-color:black;min-height:100px;");
  QPixmap pixmap_hs(lb_headshot->size());
  QPainter painter_hs(&pixmap_hs);

  lb_username = new QLabel("◇  Username", gb_info);  // 显示用户昵称
  info_vbox_layout->addWidget(lb_username, 1);
  le_username = new QLineEdit(gb_info);
  le_username->setReadOnly(true);
  le_username->setStyleSheet(lineEditStyle);
  info_vbox_layout->addWidget(le_username, 1);

  lb_email = new QLabel("◇  Email", gb_info);  // 显示电子邮箱
  info_vbox_layout->addWidget(lb_email, 1);
  le_email = new QLineEdit(gb_info);
  le_email->setReadOnly(true);
  le_email->setStyleSheet(lineEditStyle);
  info_vbox_layout->addWidget(le_email, 1);

  lb_lastused = new QLabel("◇  LastUsed", gb_info);  // 显示上一次使用时间
  info_vbox_layout->addWidget(lb_lastused, 1);
  le_lastused = new QLineEdit(gb_info);
  le_lastused->setReadOnly(true);
  le_lastused->setStyleSheet(lineEditStyle);
  info_vbox_layout->addWidget(le_lastused, 1);
  info_vbox_layout->addItem(info_spacer);
  pb_verify1 =
      new QPushButton("TEST A", gb_info);  // 测试用按钮，点击弹出‘警告’
  pb_verify1->setStyleSheet(btn_style_big);
  info_vbox_layout->addWidget(pb_verify1, 2);
  nf1 = new Notification(contwidget);  // 测试按钮a
  connect(pb_verify1, &QPushButton::clicked, this, [&]() {
    nf1->Push(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
              "Warning", "ABCDE Failed!", 2000);
  });
  pb_verify1->move(200, 50);
  info_vbox_layout->addItem(info_spacer);
  pb_verify2 =
      new QPushButton("TEST B", gb_info);  // 测试用按钮，点击弹出‘错误’
  pb_verify2->setStyleSheet(btn_style_big);
  info_vbox_layout->addWidget(pb_verify2, 2);

  nf2 = new Notification(contwidget);  // 测试按钮b
  connect(pb_verify2, &QPushButton::clicked, this, [&]() {
    nf2->Push(NotifyType::Notify_Type_Error, NotifyPosition::Pos_Top_Left,
              "Error", "ABCDE Failed!", 2000);
  });
  pb_verify2->move(200, 50);

  info_vbox_layout->addItem(info_spacer);

  // 设置伸缩因子
  cont_grid_layout->setColumnStretch(0, 2);
  cont_grid_layout->setColumnStretch(1, 9);
  cont_grid_layout->setColumnStretch(2, 3);
  cont_grid_layout->setRowStretch(0, 1);
  cont_grid_layout->setRowStretch(1, 10);
}

// 工具栏悬停监控
bool QtGUI::eventFilter(QObject* watched, QEvent* event) {
  if (watched == lb_logo && event->type() == QEvent::Enter) {
    btn1->setVisible(true);
    btn2->setVisible(true);
    btn3->setVisible(true);
    anim1->setDirection(QAbstractAnimation::Forward);
    anim2->setDirection(QAbstractAnimation::Forward);
    anim3->setDirection(QAbstractAnimation::Forward);
    anim1->start();
    anim2->start();
    anim3->start();
  } else if (watched == gb_tool && event->type() == QEvent::Leave) {
    QPropertyAnimation* animations[] = {anim1, anim2, anim3};
    for (auto anim : animations) {
      anim->setDirection(QAbstractAnimation::Backward);
      anim->start();
    }
    QTimer::singleShot(300, [this]() {
      btn1->setVisible(false);
      btn2->setVisible(false);
      btn3->setVisible(false);
    });
  }
  return QWidget::eventFilter(watched, event);
}

// 自定义窗口按钮
void QtGUI::ControlButton() {
  closeButton = new QPushButton("✕");
  closeButton->setFixedSize(40, 50);
  closeButton->setStyleSheet(btn_style_small);
  connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
  minimizeButton = new QPushButton("-");
  minimizeButton->setFixedSize(20, 50);
  minimizeButton->setStyleSheet(btn_style_small);
  connect(minimizeButton, &QPushButton::clicked, this, &QWidget::showMinimized);
  QPushButton* maximizeButton = new QPushButton("□");
  maximizeButton->setFixedSize(30, 50);
  maximizeButton->setStyleSheet(btn_style_small);
  connect(maximizeButton, &QPushButton::clicked, this,
          [this, maximizeButton]() {
            if (isMaximized) {
              this->showNormal();
              maximizeButton->setText("□");
              maximizeButton->setStyleSheet(btn_style_small);
              lb_bg->setStyleSheet("background-color: transparent;");
            } else {
              this->showMaximized();
              maximizeButton->setText("⛶");
              maximizeButton->setStyleSheet(btn_style_small);
              lb_bg->setStyleSheet("background-color: #090C21;");
            }
            isMaximized = !isMaximized;
          });
  gb_pointbutton = new QGroupBox(lb_bg);
  btn_hbox_layout = new QHBoxLayout(gb_pointbutton);
  btn_hbox_layout->addWidget(minimizeButton);
  btn_hbox_layout->addWidget(maximizeButton);
  btn_hbox_layout->addWidget(closeButton);
  btn_hbox_layout->setAlignment(Qt::AlignRight | Qt::AlignTop);
  btn_hbox_layout->setContentsMargins(0, 0, 0, 0);
  gb_pointbutton->setStyleSheet(
      "QGroupBox {border: none; margin-right:20px; margin-top:15px;}");
  cont_grid_layout->addWidget(gb_pointbutton, 0, 2, 1, 1);
}

bool QtGUI::event(QEvent* event) {
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
        QScreen* screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
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
  QPointF globalPos = event->globalPosition();
  int marginTop = globalPos.y() - mainRect.y();
  int marginBottom = mainRect.y() + mainRect.height() - globalPos.y();
  int marginLeft = globalPos.x() - mainRect.x();
  int marginRight = mainRect.x() + mainRect.width() - globalPos.x();
  const int MARGIN_MIN_SIZE = 10;
  const int MARGIN_MAX_SIZE = 20;
  if (!m_bIsResizing) {
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