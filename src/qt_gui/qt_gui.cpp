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
#include <qpen.h>
#include <qpixmap.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qwidget.h>

#include <QFontDatabase>
#include <string>

#include "QualityStructure.h"
#include "face_analyzer.h"
#include "qt_gui/notification.h"
#include "utils/base.h"
using namespace arm_face_id;
// 引入 arm_face_id
// 命名空间，以便在后续代码中可以直接使用该命名空间下的类和函数。

constexpr int flush_notification_cnt = 10;
constexpr int KMARGIN_RIGHT = 12;

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
  centralwidget_ = new QWidget;
  setCentralWidget(centralwidget_);
  main_grid_layout_ = new QGridLayout(centralwidget_);
  main_grid_layout_->setContentsMargins(0, 0, 0, 0);
  centralwidget_->setSizePolicy(sizePolicy);
  centralwidget_->setMinimumSize(800, 400);

  // 背景标签
  lb_bg_ = new QLabel(centralwidget_);
  lb_bg_->setPixmap(QPixmap("assets/bg5.png"));
  lb_bg_->setScaledContents(true);
  main_grid_layout_->addWidget(lb_bg_, 0, 0, 1, 1);

  // 创建子容器
  contwidget_ = new QWidget(centralwidget_);
  cont_grid_layout_ = new QGridLayout(contwidget_);
  contwidget_->setLayout(cont_grid_layout_);
  main_grid_layout_->addWidget(contwidget_, 0, 0, 1, 1);
  cont_grid_layout_->setContentsMargins(15, 15, 25, 15);
  contwidget_->raise();

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
  gb_tool_ = new QGroupBox(lb_bg_);
  cont_grid_layout_->addWidget(gb_tool_, 0, 0, 2, 1);
  tool_vbox_layout_ = new QVBoxLayout(gb_tool_);
  lb_logo_ = new QLabel(gb_tool_);
  lb_logo_->setPixmap(QPixmap("assets/logo3.ico"));
  lb_logo_->setFixedSize(50, 50);
  lb_logo_->setScaledContents(true);
  gb_tool_->setStyleSheet(
      "QGroupBox { border: none;margin: 0px; max-width: "
      "60px;margin-right:5px;}");
  tool_vbox_layout_->addWidget(lb_logo_);
  btn1_ = new QPushButton("Go", gb_tool_);
  btn1_->setFixedSize(50, 50);
  btn2_ = new QPushButton("Stop", gb_tool_);
  btn2_->setFixedSize(50, 50);
  btn3_ = new QPushButton("...", gb_tool_);
  btn3_->setFixedSize(50, 50);
  btn1_->setStyleSheet(btn_style_small_);
  btn2_->setStyleSheet(btn_style_small_);
  btn3_->setStyleSheet(btn_style_small_);
  btn1_->setVisible(false);
  btn2_->setVisible(false);
  btn3_->setVisible(false);
  tool_vbox_layout_->addWidget(btn1_);
  tool_vbox_layout_->addWidget(btn2_);
  tool_vbox_layout_->addWidget(btn3_);
  tool_vbox_layout_->setSpacing(10);
  tool_vbox_layout_->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  anim1_ = new QPropertyAnimation(btn1_, "pos");
  anim2_ = new QPropertyAnimation(btn2_, "pos");
  anim3_ = new QPropertyAnimation(btn3_, "pos");
  anim1_->setDuration(200);
  anim2_->setDuration(300);
  anim3_->setDuration(400);
  anim1_->setStartValue(QPoint(10, 60));
  anim1_->setEndValue(QPoint(10, 80));
  anim2_->setStartValue(QPoint(10, 65));
  anim2_->setEndValue(QPoint(10, 140));
  anim3_->setStartValue(QPoint(10, 70));
  anim3_->setEndValue(QPoint(10, 200));
  lb_logo_->installEventFilter(this);
  gb_tool_->installEventFilter(this);

  // cam 栏目
  gb_camera_ = new QGroupBox(lb_bg_);
  gb_camera_->setStyleSheet(
      "QGroupBox {min-width:400px; "
      "min-height:200px;margin-top:15px;background-color:#121630;}");
  cont_grid_layout_->addWidget(gb_camera_, 0, 1, 2, 1);
  cam_vbox_layout_ = new QVBoxLayout(gb_camera_);
  lb_camera_ = new ResizableLabel;
  cam_vbox_layout_->addWidget(lb_camera_);

  camera_frame_label_ = lb_camera_;
  faces_notification_pixmap_ = QPixmap(camera_frame_label_->size());
  faces_notification_pixmap_.fill(Qt::transparent);

  // info 栏目
  QSpacerItem* info_spacer =
      new QSpacerItem(10, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);
  gb_info_ = new QGroupBox(lb_bg_);
  cont_grid_layout_->addWidget(gb_info_, 1, 2, 1, 1);
  gb_info_->setStyleSheet(
      "QGroupBox {"
      "   border:transparent;"
      "   min-width: 90px;"
      "}"
      "QGroupBox QWidget {"
      "   color: #0ffff3;"
      "}");
  info_vbox_layout_ = new QVBoxLayout(gb_info_);
  info_vbox_layout_->setSpacing(0);
  QString lineEditStyle = "QLineEdit {background-color:#1A1F3D;color:white;}";

  // 显示用户个性头像
  lb_headshot_ = new ResizableLabel(gb_info_);
  info_vbox_layout_->addWidget(lb_headshot_, 3);
  lb_headshot_->setStyleSheet("background-color:black;min-height:100px;");
  QPixmap pixmap_hs(lb_headshot_->size());
  QPainter painter_hs(&pixmap_hs);

  lb_username_ = new QLabel("◇  Username", gb_info_);  // 显示用户昵称
  info_vbox_layout_->addWidget(lb_username_, 1);
  le_username_ = new QLineEdit(gb_info_);
  le_username_->setReadOnly(true);
  le_username_->setStyleSheet(lineEditStyle);
  info_vbox_layout_->addWidget(le_username_, 1);

  lb_email_ = new QLabel("◇  Email", gb_info_);  // 显示电子邮箱
  info_vbox_layout_->addWidget(lb_email_, 1);
  le_email_ = new QLineEdit(gb_info_);
  le_email_->setReadOnly(true);
  le_email_->setStyleSheet(lineEditStyle);
  info_vbox_layout_->addWidget(le_email_, 1);

  lb_lastused_ = new QLabel("◇  LastUsed", gb_info_);  // 显示上一次使用时间
  info_vbox_layout_->addWidget(lb_lastused_, 1);
  le_lastused_ = new QLineEdit(gb_info_);
  le_lastused_->setReadOnly(true);
  le_lastused_->setStyleSheet(lineEditStyle);
  info_vbox_layout_->addWidget(le_lastused_, 1);
  info_vbox_layout_->addItem(info_spacer);
  pb_verify1_ =
      new QPushButton("TEST A", gb_info_);  // 测试用按钮，点击弹出‘警告’
  pb_verify1_->setStyleSheet(btn_style_big_);
  info_vbox_layout_->addWidget(pb_verify1_, 2);
  nf1_ = new Notification(contwidget_);  // 测试按钮a
  connect(pb_verify1_, &QPushButton::clicked, this, [&]() {
    nf1_->Push(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
               "Warning", "ABCDE Failed!", 2000);
  });
  pb_verify1_->move(200, 50);
  info_vbox_layout_->addItem(info_spacer);
  pb_verify2_ =
      new QPushButton("TEST B", gb_info_);  // 测试用按钮，点击弹出‘错误’
  pb_verify2_->setStyleSheet(btn_style_big_);
  info_vbox_layout_->addWidget(pb_verify2_, 2);

  nf2_ = new Notification(contwidget_);  // 测试按钮b
  connect(pb_verify2_, &QPushButton::clicked, this, [&]() {
    nf2_->Push(NotifyType::Notify_Type_Error, NotifyPosition::Pos_Top_Left,
               "Error", "ABCDE Failed!", 2000);
  });
  pb_verify2_->move(200, 50);

  info_vbox_layout_->addItem(info_spacer);

  // 设置伸缩因子
  cont_grid_layout_->setColumnStretch(0, 2);
  cont_grid_layout_->setColumnStretch(1, 9);
  cont_grid_layout_->setColumnStretch(2, 3);
  cont_grid_layout_->setRowStretch(0, 1);
  cont_grid_layout_->setRowStretch(1, 10);

  connect(
      this, &QtGUI::notify, this,
      [&](NotifyType type, NotifyPosition pos, QString title, QString content,
          int nLive) { nf1_->Push(type, pos, title, content); });
}
// 工具栏悬停监控
bool QtGUI::eventFilter(QObject* watched, QEvent* event) {
  if (watched == lb_logo_ && event->type() == QEvent::Enter) {
    btn1_->setVisible(true);
    btn2_->setVisible(true);
    btn3_->setVisible(true);
    anim1_->setDirection(QAbstractAnimation::Forward);
    anim2_->setDirection(QAbstractAnimation::Forward);
    anim3_->setDirection(QAbstractAnimation::Forward);
    anim1_->start();
    anim2_->start();
    anim3_->start();
  } else if (watched == gb_tool_ && event->type() == QEvent::Leave) {
    QPropertyAnimation* animations[] = {anim1_, anim2_, anim3_};
    for (auto anim : animations) {
      anim->setDirection(QAbstractAnimation::Backward);
      anim->start();
    }
    QTimer::singleShot(300, [this]() {
      btn1_->setVisible(false);
      btn2_->setVisible(false);
      btn3_->setVisible(false);
    });
  }
  return QWidget::eventFilter(watched, event);
}

// 自定义窗口按钮
void QtGUI::ControlButton() {
  close_button_ = new QPushButton("✕");
  close_button_->setFixedSize(40, 50);
  close_button_->setStyleSheet(btn_style_small_);
  connect(close_button_, &QPushButton::clicked, this, &QWidget::close);
  minimize_button_ = new QPushButton("-");
  minimize_button_->setFixedSize(20, 50);
  minimize_button_->setStyleSheet(btn_style_small_);
  connect(minimize_button_, &QPushButton::clicked, this,
          &QWidget::showMinimized);
  QPushButton* maximizeButton = new QPushButton("□");
  maximizeButton->setFixedSize(30, 50);
  maximizeButton->setStyleSheet(btn_style_small_);
  connect(maximizeButton, &QPushButton::clicked, this,
          [this, maximizeButton]() {
            if (is_maximized_) {
              this->showNormal();
              maximizeButton->setText("□");
              maximizeButton->setStyleSheet(btn_style_small_);
              lb_bg_->setStyleSheet("background-color: transparent;");
            } else {
              this->showMaximized();
              maximizeButton->setText("⛶");
              maximizeButton->setStyleSheet(btn_style_small_);
              lb_bg_->setStyleSheet("background-color: #090C21;");
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
  gb_pointbutton_->setStyleSheet(
      "QGroupBox {border: none; margin-right:20px; margin-top:15px;}");
  cont_grid_layout_->addWidget(gb_pointbutton_, 0, 2, 1, 1);
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
    b_is_pressed_ = true;
    press_point_ = event->globalPosition();
  }
  return QWidget::mousePressEvent(event);
}

void QtGUI::mouseMoveEvent(QMouseEvent* event) {
  if (b_is_pressed_) {
    if (b_is_resizing_) {
      move_point_ = event->globalPosition() - press_point_;
      press_point_ += move_point_;
    } else {
      if (!b_is_double_pressed_ && windowState() == Qt::WindowMaximized) {
        QScreen* screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        QPointF point(width() * ((double)(event->globalPosition().x()) /
                                 screenGeometry.width()),
                      height() * ((double)(event->globalPosition().y()) /
                                  screenGeometry.height()));
        move((event->globalPosition() - point).toPoint());
        press_point_ = event->globalPosition();
      }
      QPointF point = event->globalPosition() - press_point_;
      move((pos() + point).toPoint());
      press_point_ = event->globalPosition();
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
  if (!b_is_resizing_) {
    if ((marginRight >= MARGIN_MIN_SIZE && marginRight <= MARGIN_MAX_SIZE) &&
        (marginBottom <= MARGIN_MAX_SIZE && marginBottom >= MARGIN_MIN_SIZE)) {
      direction_ = BOTTOMRIGHT;
      setCursor(Qt::SizeFDiagCursor);
    } else if ((marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE) &&
               (marginRight >= MARGIN_MIN_SIZE &&
                marginRight <= MARGIN_MAX_SIZE)) {
      direction_ = TOPRIGHT;
      setCursor(Qt::SizeBDiagCursor);
    } else if ((marginLeft >= MARGIN_MIN_SIZE &&
                marginLeft <= MARGIN_MAX_SIZE) &&
               (marginTop >= MARGIN_MIN_SIZE && marginTop <= MARGIN_MAX_SIZE)) {
      direction_ = TOPLEFT;
      setCursor(Qt::SizeFDiagCursor);
    } else if ((marginLeft >= MARGIN_MIN_SIZE &&
                marginLeft <= MARGIN_MAX_SIZE) &&
               (marginBottom >= MARGIN_MIN_SIZE &&
                marginBottom <= MARGIN_MAX_SIZE)) {
      direction_ = BOTTOMLEFT;
      setCursor(Qt::SizeBDiagCursor);
    } else if (marginBottom <= MARGIN_MAX_SIZE &&
               marginBottom >= MARGIN_MIN_SIZE) {
      direction_ = DOWN;
      setCursor(Qt::SizeVerCursor);
    } else if (marginLeft <= MARGIN_MAX_SIZE - 1 &&
               marginLeft >= MARGIN_MIN_SIZE - 1) {
      direction_ = LEFT;
      setCursor(Qt::SizeHorCursor);
    } else if (marginRight <= MARGIN_MAX_SIZE &&
               marginRight >= MARGIN_MIN_SIZE) {
      direction_ = RIGHT;
      setCursor(Qt::SizeHorCursor);
    } else if (marginTop <= MARGIN_MAX_SIZE && marginTop >= MARGIN_MIN_SIZE) {
      direction_ = UP;
      setCursor(Qt::SizeVerCursor);
    } else {
      if (!b_is_pressed_) {
        setCursor(Qt::ArrowCursor);
      }
    }
  }
  if (NONE != direction_) {
    b_is_resizing_ = true;
    resizeRegion(marginTop, marginBottom, marginLeft, marginRight);
  }
}

void QtGUI::resizeRegion(int marginTop, int marginBottom, int marginLeft,
                         int marginRight) {
  if (b_is_pressed_) {
    switch (direction_) {
      case BOTTOMRIGHT: {
        QRect rect = geometry();
        rect.setBottomRight((rect.bottomRight() + move_point_).toPoint());
        setGeometry(rect);
      } break;
      case TOPRIGHT: {
        if (marginLeft > minimumWidth() && marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTopRight((rect.topRight() + move_point_).toPoint());
          setGeometry(rect);
        }
      } break;
      case TOPLEFT: {
        if (marginRight > minimumWidth() && marginBottom > minimumHeight()) {
          QRect rect = geometry();
          rect.setTopLeft((rect.topLeft() + move_point_).toPoint());
          setGeometry(rect);
        }
      } break;
      case BOTTOMLEFT: {
        if (marginRight > minimumWidth() && marginTop > minimumHeight()) {
          QRect rect = geometry();
          rect.setBottomLeft((rect.bottomLeft() + move_point_).toPoint());
          setGeometry(rect);
        }
      } break;
      case RIGHT: {
        QRect rect = geometry();
        rect.setWidth(rect.width() + move_point_.x());
        setGeometry(rect);
      } break;
      case DOWN: {
        QRect rect = geometry();
        rect.setHeight(rect.height() + move_point_.y());
        setGeometry(rect);
      } break;
      case LEFT: {
        if (marginRight > minimumWidth()) {
          QRect rect = geometry();
          rect.setLeft(rect.x() + move_point_.x());
          setGeometry(rect);
        }
      } break;
      case UP: {
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
    direction_ = NONE;
  }
}

void QtGUI::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::LeftButton) {
    b_is_pressed_ = false;
    b_is_resizing_ = false;
    b_is_double_pressed_ = false;
  }
  QWidget::mouseReleaseEvent(event);
}
void QtGUI::leaveEvent(QEvent* event) {
  b_is_pressed_ = false;
  b_is_double_pressed_ = false;
  b_is_resizing_ = false;
  QWidget::leaveEvent(event);
}

void QtGUI::CheckNeedNotification(const FaceAnalyzer::AnalyzeMsg& message) {
  if (message.res.empty()) return;
  auto top_res = message.res.front();
  if (top_res.integrity_res.level <= seeta::QualityLevel::MEDIUM) {
    cur_notify_cnter = 0;
    emit notify(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
                "提示", "请让脸部完整出现在区域内");
  } else if (top_res.pose_res.level <= seeta::QualityLevel::MEDIUM) {
    cur_notify_cnter = 0;
    emit notify(NotifyType::Notify_Type_Warning, NotifyPosition::Pos_Top_Left,
                "提示", "请让正对摄像头");
  }
}

void QtGUI::OnNotify(const UserInfo& message) {
  if (message.user_id() < 0) return;
  if (cur_notify_cnter > notify_interval) {
    emit notify(NotifyType::Notify_Type_Success, NotifyPosition::Pos_Top_Left,
                "成功",
                QString::fromStdString("欢迎你，" + message.user_name()));
    cur_notify_cnter = 0;
  }
  le_email_->setText(QString::fromStdString(message.email()));
  QByteArray pic_array(message.profile_picture().data(),
                       message.profile_picture().size());
  QPixmap pic;
  pic.loadFromData(pic_array);
  lb_headshot_->setPixmap(pic);
  le_username_->setText(QString::fromStdString(message.user_name()));
  le_lastused_->setText(
      QString::fromStdString(message.last_recognized_datetime()));
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

void QtGUI::OnNotify(const FaceAnalyzer::AnalyzeMsg& message) {
  cur_notify_cnter++;
  if (cur_notify_cnter >= notify_interval) {
    CheckNeedNotification(message);
  }
  QFont default_font("Arial");
  default_font.setPixelSize(12);
  notification_delay_cnter = 0;
  faces_notification_pixmap_.fill(Qt::transparent);
  int num = -1;
  for (auto res : message.res) {
    num++;
    SeetaRect rect = res.face.pos;
    // 人脸框标注
    faces_notification_pixmap_.fill(Qt::transparent);
    QPainter painter(&faces_notification_pixmap_);
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
