#include "qt_gui/client_ui.h"

#include <qapplication.h>
#include <qevent.h>
#include <qobject.h>
#include <qstringview.h>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QScreen>

#include "qt_gui/notification.h"
#include "qt_gui/resizable_label.h"

using namespace arm_face_id;

ClientUI::ClientUI() {
  QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_StyledBackground);
  setAttribute(Qt::WA_TranslucentBackground);
  setAttribute(Qt::WA_Hover);

  int fontId = QFontDatabase::addApplicationFont(
      QStringLiteral(":/MonacoNerdFont-Regular.ttf"));
  QStringList font_families = QFontDatabase::applicationFontFamilies(fontId);

  info_font_.setFamily(QStringLiteral(":/MonacoNerdFont-Regular.ttf"));
  info_font_.setPixelSize(20);

  qDebug() << "fontfamilies:" << font_families;
  if (font_families.size() > 0) {
    QFont font(font_families[0], 16);
    font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
    QApplication::setFont(font);
  }

  // 主窗口
  centralwidget_ = new QWidget;
  setCentralWidget(centralwidget_);
  main_grid_layout_ = new QGridLayout(centralwidget_);
  main_grid_layout_->setContentsMargins(0, 0, 0, 0);
  centralwidget_->setSizePolicy(sizePolicy);
  centralwidget_->setMinimumSize(800, 400);

  // 背景标签
  lb_bg_ = new QLabel(centralwidget_);
  lb_bg_->setPixmap(QPixmap(":/background.png"));
  lb_bg_->setScaledContents(true);
  main_grid_layout_->addWidget(lb_bg_, 0, 0, 1, 1);

  // 创建子容器
  contwidget_ = new QWidget(centralwidget_);
  cont_grid_layout_ = new QGridLayout(contwidget_);
  contwidget_->setLayout(cont_grid_layout_);
  main_grid_layout_->addWidget(contwidget_, 0, 0, 1, 1);
  cont_grid_layout_->setContentsMargins(15, 15, 25, 15);
  contwidget_->raise();

  ControlButton();
  // tool 栏目
  gb_tool_ = new QGroupBox(lb_bg_);
  cont_grid_layout_->addWidget(gb_tool_, 1, 0, 2, 1);
  tool_vbox_layout_ = new QVBoxLayout(gb_tool_);
  lb_logo_ = new QLabel(gb_tool_);
  lb_logo_->setPixmap(QPixmap("assets/logo3.ico"));
  lb_logo_->setFixedSize(50, 50);
  lb_logo_->setScaledContents(true);
  // TODO: style
  // gb_tool_->setStyleSheet(
  //     "QGroupBox { border: none;margin: 0px; max-width: "
  //     "60px;margin-right:5px;}");
  tool_vbox_layout_->addWidget(lb_logo_);
  btn1_ = new QPushButton("Go", gb_tool_);
  btn1_->setFixedSize(50, 50);
  btn2_ = new QPushButton("Stop", gb_tool_);
  btn2_->setFixedSize(50, 50);
  btn3_ = new QPushButton("...", gb_tool_);
  btn3_->setFixedSize(50, 50);
  // TODO: style
  // btn1_->setStyleSheet(btn_style_small_);
  // btn2_->setStyleSheet(btn_style_small_);
  // btn3_->setStyleSheet(btn_style_small_);
  // btn1_->setVisible(false);
  // btn2_->setVisible(false);
  // btn3_->setVisible(false);
  tool_vbox_layout_->addWidget(btn1_);
  tool_vbox_layout_->addWidget(btn2_);
  tool_vbox_layout_->addWidget(btn3_);
  tool_vbox_layout_->setSpacing(10);
  tool_vbox_layout_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  // anim1_ = new QPropertyAnimation(btn1_, "pos");
  // anim2_ = new QPropertyAnimation(btn2_, "pos");
  // anim3_ = new QPropertyAnimation(btn3_, "pos");
  // anim1_->setDuration(200);
  // anim2_->setDuration(300);
  // anim3_->setDuration(400);
  // anim1_->setStartValue(QPoint(10, 60));
  // anim1_->setEndValue(QPoint(10, 80));
  // anim2_->setStartValue(QPoint(10, 65));
  // anim2_->setEndValue(QPoint(10, 140));
  // anim3_->setStartValue(QPoint(10, 70));
  // anim3_->setEndValue(QPoint(10, 200));
  lb_logo_->installEventFilter(this);
  gb_tool_->installEventFilter(this);

  // cam 栏目
  gb_camera_ = new QGroupBox(lb_bg_);
  // TODO: style
  // gb_camera_->setStyleSheet(
  //     "QGroupBox {min-width:400px; "
  //     "min-height:200px;margin-top:15px;background-color:#121630;}");
  cont_grid_layout_->addWidget(gb_camera_, 1, 1, 2, 2);
  cam_vbox_layout_ = new QVBoxLayout(gb_camera_);
  lb_camera_ = new ResizableLabel;
  cam_vbox_layout_->addWidget(lb_camera_);

  hud_pixmap_ = QPixmap(lb_camera_->size());
  hud_pixmap_.fill(Qt::transparent);

  // info 栏目
  QSpacerItem* info_spacer =
      new QSpacerItem(10, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);
  gb_info_ = new QGroupBox(lb_bg_);
  cont_grid_layout_->addWidget(gb_info_, 1, 3, 1, 1);
  //     "QGroupBox {"
  //     "   border:transparent;"
  //     "   min-width: 90px;"
  //     "}"
  //     "QGroupBox QWidget {"
  //     "   color: #0ffff3;"
  //     "}");
  info_vbox_layout_ = new QVBoxLayout(gb_info_);
  info_vbox_layout_->setSpacing(0);
  QString lineEditStyle = "QLineEdit {background-color:#1A1F3D;color:white;}";

  // 显示用户个性头像
  lb_headshot_ = new ResizableLabel(gb_info_);
  info_vbox_layout_->addWidget(lb_headshot_, 3);

  lb_username_ = new QLabel("username", gb_info_);  // 显示用户昵称
  lb_username_->setFont(info_font_);
  info_vbox_layout_->addStretch();
  info_vbox_layout_->addWidget(lb_username_, 1);
  le_username_ = new QLineEdit(gb_info_);
  le_username_->setReadOnly(true);

  info_vbox_layout_->addWidget(le_username_, 1);

  lb_email_ = new QLabel("email", gb_info_);  // 显示电子邮箱
  lb_email_->setFont(info_font_);
  info_vbox_layout_->addStretch();
  info_vbox_layout_->addWidget(lb_email_, 1);
  le_email_ = new QLineEdit(gb_info_);
  le_email_->setReadOnly(true);

  info_vbox_layout_->addWidget(le_email_, 1);

  lb_lastused_ = new QLabel("last recognized", gb_info_);  // 显示上一次使用时间
  lb_lastused_->setFont(info_font_);
  info_vbox_layout_->addStretch();
  info_vbox_layout_->addWidget(lb_lastused_, 1);
  le_lastused_ = new QLineEdit(gb_info_);
  le_lastused_->setReadOnly(true);

  info_vbox_layout_->addWidget(le_lastused_, 1);
  info_vbox_layout_->addItem(info_spacer);

  // 设置伸缩因子
  cont_grid_layout_->setColumnStretch(0, 2);
  cont_grid_layout_->setColumnStretch(1, 9);
  cont_grid_layout_->setColumnStretch(2, 3);
  cont_grid_layout_->setRowStretch(0, 1);
  cont_grid_layout_->setRowStretch(1, 10);

  notificationbar_ = new Notification(this);
  connect(
      this, &ClientUI::notify, this,
      [&](NotifyType type, NotifyPosition pos, QString title, QString content,
          int nLive) { notificationbar_->Push(type, pos, title, content); });
}

// 工具栏悬停监控
bool ClientUI::eventFilter(QObject* watched, QEvent* event) {
  // if (watched == lb_logo_ && event->type() == QEvent::Enter) {
  //   btn1_->setVisible(true);
  //   btn2_->setVisible(true);
  //   btn3_->setVisible(true);
  //   anim1_->setDirection(QAbstractAnimation::Forward);
  //   anim2_->setDirection(QAbstractAnimation::Forward);
  //   anim3_->setDirection(QAbstractAnimation::Forward);
  //   anim1_->start();
  //   anim2_->start();
  //   anim3_->start();
  // } else if (watched == gb_tool_ && event->type() == QEvent::Leave) {
  //   QPropertyAnimation* animations[] = {anim1_, anim2_, anim3_};
  //   for (auto anim : animations) {
  //     anim->setDirection(QAbstractAnimation::Backward);
  //     anim->start();
  //   }
  //   QTimer::singleShot(300, [this]() {
  //     btn1_->setVisible(false);
  //     btn2_->setVisible(false);
  //     btn3_->setVisible(false);
  //   });
  // }
  return QWidget::eventFilter(watched, event);
}

// 自定义窗口按钮
void ClientUI::ControlButton() {
  close_button_ = new QPushButton("✕");
  close_button_->setFixedSize(30, 30);
  // TODO: style
  // close_button_->setStyleSheet(btn_style_small_);
  connect(close_button_, &QPushButton::clicked, this, &QWidget::close);
  minimize_button_ = new QPushButton("-");
  minimize_button_->setFixedSize(30, 30);
  // TODO: style
  // minimize_button_->setStyleSheet(btn_style_small_);
  connect(minimize_button_, &QPushButton::clicked, this,
          &QWidget::showMinimized);
  QPushButton* maximizeButton = new QPushButton("□");
  maximizeButton->setFixedSize(30, 30);
  // TODO: style
  // maximizeButton->setStyleSheet(btn_style_small_);
  connect(maximizeButton, &QPushButton::clicked, this,
          [this, maximizeButton]() {
            if (is_maximized_) {
              this->showNormal();
              maximizeButton->setText("□");
              // TODO: style
              // maximizeButton->setStyleSheet(btn_style_small_);

              // lb_bg_->setStyleSheet("background-color: transparent;");
            } else {
              this->showMaximized();
              maximizeButton->setText("⛶");
              // maximizeButton->setStyleSheet(btn_style_small_);
              // TODO: style
              // lb_bg_->setStyleSheet("background-color: #090C21;");
            }
            is_maximized_ = !is_maximized_;
          });
  gb_pointbutton_ = new QGroupBox(lb_bg_);
  btn_hbox_layout_ = new QHBoxLayout(gb_pointbutton_);
  btn_hbox_layout_->addWidget(minimize_button_);
  btn_hbox_layout_->addWidget(maximizeButton);
  btn_hbox_layout_->addWidget(close_button_);
  btn_hbox_layout_->setAlignment(Qt::AlignRight | Qt::AlignTop);
  btn_hbox_layout_->setContentsMargins(0, 0, 0, 0);
  // TODO: style
  // gb_pointbutton_->setStyleSheet(
  //     "QGroupBox {border: none; margin-right:20px; margin-top:15px;}");
  cont_grid_layout_->addWidget(gb_pointbutton_, 0, 0, 1, 4);
}

bool ClientUI::event(QEvent* event) {
  if (event->type() == QEvent::HoverMove) {
    QHoverEvent* hoverEvent = static_cast<QHoverEvent*>(event);
    QMouseEvent mouseEvent(QEvent::MouseMove, hoverEvent->pos(), Qt::NoButton,
                           Qt::NoButton, Qt::NoModifier);
    mouseMoveEvent(&mouseEvent);
  }
  return QWidget::event(event);
}

void ClientUI::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    b_is_pressed_ = true;
    press_point_ = event->globalPos();
  }
  return QWidget::mousePressEvent(event);
}

void ClientUI::mouseMoveEvent(QMouseEvent* event) {
  if (b_is_pressed_) {
    if (b_is_resizing_) {
      move_point_ = event->globalPos() - press_point_;
      press_point_ += move_point_;
    } else {
      if (!b_is_double_pressed_ && windowState() == Qt::WindowMaximized) {
        QScreen* screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        QPointF point(width() * ((double)(event->globalPos().x()) /
                                 screenGeometry.width()),
                      height() * ((double)(event->globalPos().y()) /
                                  screenGeometry.height()));
        move((event->globalPos() - point).toPoint());
        press_point_ = event->globalPos();
      }
      QPointF point = event->globalPos() - press_point_;
      move((pos() + point).toPoint());
      press_point_ = event->globalPos();
    }
  }
  if (windowState() != Qt::WindowMaximized) {
    updateRegion(event);
  }
  QWidget::mouseMoveEvent(event);
}

void ClientUI::updateRegion(QMouseEvent* event) {
  QRect mainRect = geometry();
  QPointF globalPos = event->globalPos();
  int marginTop = globalPos.y() - mainRect.y();
  int marginBottom = mainRect.y() + mainRect.height() - globalPos.y();
  int marginLeft = globalPos.x() - mainRect.x();
  int marginRight = mainRect.x() + mainRect.width() - globalPos.x();
  const int MARGIN_MIN_SIZE = 10;
  const int MARGIN_MAX_SIZE = 20;
  if (!b_is_resizing_) {
    if ((marginRight >= MARGIN_MIN_SIZE && marginRight <= MARGIN_MAX_SIZE) &&
        (marginBottom <= MARGIN_MAX_SIZE && marginBottom >= MARGIN_MIN_SIZE)) {
      direction_ = ResizableLabel::BOTTOMRIGHT;
      setCursor(Qt::SizeFDiagCursor);
    } else if ((marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE) &&
               (marginRight >= MARGIN_MIN_SIZE &&
                marginRight <= MARGIN_MAX_SIZE)) {
      direction_ = ResizableLabel::TOPRIGHT;
      setCursor(Qt::SizeBDiagCursor);
    } else if ((marginLeft >= MARGIN_MIN_SIZE &&
                marginLeft <= MARGIN_MAX_SIZE) &&
               (marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE)) {
      direction_ = ResizableLabel::TOPLEFT;
      setCursor(Qt::SizeFDiagCursor);
    } else if ((marginLeft >= MARGIN_MIN_SIZE &&
                marginLeft <= MARGIN_MAX_SIZE) &&
               (marginBottom >= MARGIN_MIN_SIZE &&
                marginBottom <= MARGIN_MAX_SIZE)) {
      direction_ = ResizableLabel::BOTTOMLEFT;
      setCursor(Qt::SizeBDiagCursor);
    } else if (marginBottom <= MARGIN_MAX_SIZE &&
               marginBottom >= MARGIN_MIN_SIZE) {
      direction_ = ResizableLabel::DOWN;
      setCursor(Qt::SizeVerCursor);
    } else if (marginLeft <= MARGIN_MAX_SIZE - 1 &&
               marginLeft >= MARGIN_MIN_SIZE - 1) {
      direction_ = ResizableLabel::LEFT;
      setCursor(Qt::SizeHorCursor);
    } else if (marginRight <= MARGIN_MAX_SIZE &&
               marginRight >= MARGIN_MIN_SIZE) {
      direction_ = ResizableLabel::RIGHT;
      setCursor(Qt::SizeHorCursor);
    } else if (marginTop <= MARGIN_MAX_SIZE && marginTop >= MARGIN_MIN_SIZE) {
      direction_ = ResizableLabel::UP;
      setCursor(Qt::SizeVerCursor);
    } else {
      if (!b_is_pressed_) {
        setCursor(Qt::ArrowCursor);
      }
    }
  }
  if (ResizableLabel::NONE != direction_) {
    b_is_resizing_ = true;
    resizeRegion(marginTop, marginBottom, marginLeft, marginRight);
  }
}

void ClientUI::resizeRegion(int marginTop, int marginBottom, int marginLeft,
                            int marginRight) {
  if (b_is_pressed_) {
    switch (direction_) {
      case ResizableLabel::BOTTOMRIGHT: {
        QRect rect = geometry();
        rect.setBottomRight((rect.bottomRight() + move_point_).toPoint());
        setGeometry(rect);
      } break;
      case ResizableLabel::TOPRIGHT: {
        if (marginLeft > minimumWidth() && marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTopRight((rect.topRight() + move_point_).toPoint());
          setGeometry(rect);
        }
      } break;
      case ResizableLabel::TOPLEFT: {
        if (marginRight > minimumWidth() && marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTopLeft((rect.topLeft() + move_point_).toPoint());
          setGeometry(rect);
        }
      } break;
      case ResizableLabel::BOTTOMLEFT: {
        if (marginRight > minimumWidth() && marginTop > minimumHeight()) {
          QRect rect = geometry();
          rect.setBottomLeft((rect.bottomLeft() + move_point_).toPoint());
          setGeometry(rect);
        }
      } break;
      case ResizableLabel::RIGHT: {
        QRect rect = geometry();
        rect.setWidth(rect.width() + move_point_.x());
        setGeometry(rect);
      } break;
      case ResizableLabel::DOWN: {
        QRect rect = geometry();
        rect.setHeight(rect.height() + move_point_.y());
        setGeometry(rect);
      } break;
      case ResizableLabel::LEFT: {
        if (marginRight > minimumWidth()) {
          QRect rect = geometry();
          rect.setLeft(rect.x() + move_point_.x());
          setGeometry(rect);
        }
      } break;
      case ResizableLabel::UP: {
        if (marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTop(rect.y() + move_point_.y());
          setGeometry(rect);
        }
      } break;
      default: {
      } break;
    }
  } else {
    b_is_resizing_ = false;
    direction_ = ResizableLabel::NONE;
  }
}

void ClientUI::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    b_is_pressed_ = false;
    b_is_resizing_ = false;
    b_is_double_pressed_ = false;
  }
  QWidget::mouseReleaseEvent(event);
}
void ClientUI::leaveEvent(QEvent* event) {
  b_is_pressed_ = false;
  b_is_double_pressed_ = false;
  b_is_resizing_ = false;
  QWidget::leaveEvent(event);
}
