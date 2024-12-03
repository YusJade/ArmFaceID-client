#include <qfont.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qpushbutton.h>
#include <qwidget.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextEdit>

#include "qt_gui/notification.h"
#include "qt_gui/resizable_label.h"

namespace arm_face_id {

/**
 * @brief 客户端界面UI，对UI单独做一层封装，分离业务逻辑，便于维护。
 *
 */
class ClientUI : public QMainWindow {
  Q_OBJECT
 public:
  ClientUI();

 signals:
  void notify(NotifyType type, NotifyPosition pos, QString title,
              QString content, int nLive = 3000);

 private:
  bool eventFilter(QObject *watched, QEvent *event) override;
  bool event(QEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void updateRegion(QMouseEvent *event);

  void resizeRegion(int marginTop, int marginBottom, int marginLeft,
                    int marginRight);
  void ControlButton();
  void maximizeWidget();
  void restoreWidget();

 protected:
  // 叠加于相机的画面帧上，标记人脸分析结果。
  QPixmap hud_pixmap_;
  QFont info_font_;

  // 鼠标跟踪
  bool is_maximized_ = false;
  bool b_is_pressed_ = false;
  bool b_is_resizing_ = false;
  bool b_is_double_pressed_ = false;
  QPointF press_point_;
  QPointF move_point_;
  ResizableLabel::ResizeDirection direction_ = ResizableLabel::NONE;
  // 窗口|窗口按钮
  QWidget *centralwidget_ = nullptr;
  QGridLayout *main_grid_layout_ = nullptr;
  QWidget *contwidget_ = nullptr;
  QGridLayout *cont_grid_layout_ = nullptr;
  QGroupBox *gb_pointbutton_ = nullptr;
  QPushButton *close_button_ = nullptr;
  QPushButton *minimize_button_ = nullptr;
  QLabel *lb_bg_ = nullptr;
  QHBoxLayout *btn_hbox_layout_ = nullptr;
  // tool 栏
  QGroupBox *gb_tool_ = nullptr;
  QVBoxLayout *tool_vbox_layout_ = nullptr;
  QLabel *lb_logo_ = nullptr;
  QPushButton *btn1_, *btn2_, *btn3_;
  QPropertyAnimation *anim1_, *anim2_, *anim3_;
  // cam 栏
  QGroupBox *gb_camera_ = nullptr;
  QVBoxLayout *cam_vbox_layout_ = nullptr;
  ResizableLabel *lb_camera_ = nullptr;
  QGroupBox *gb_info_ = nullptr;
  QPixmap info_notification_pixmap_;
  // info 栏
  QVBoxLayout *info_vbox_layout_ = nullptr;
  QLabel *lb_headshot_ = nullptr;
  QLabel *lb_username_ = nullptr;
  QLineEdit *le_username_ = nullptr;
  QLabel *lb_email_ = nullptr;
  QLineEdit *le_email_ = nullptr;
  QLabel *lb_lastused_ = nullptr;
  QLineEdit *le_lastused_ = nullptr;
  QPushButton *pb_verify1_ = nullptr;
  QPushButton *pb_verify2_ = nullptr;
  Notification *notificationbar_ = nullptr;
};
}  // namespace arm_face_id