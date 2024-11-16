#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QWidget>
#include <mutex>
#include <vector>

//消息类型
enum NotifyType {
  Notify_Type_None = 0,    // 普通
  Notify_Type_Success,     // 成功
  Notify_Type_Error,       // 错误
  Notify_Type_Warning,     // 警告
  Notify_Type_Information  // 信息
};

//消息位置
enum NotifyPosition {
  Pos_Top_Right = 0,  // 右上角开始提醒，从上至下
  Pos_Top_Left,       // 左上角开始提醒，从上至下
  Pos_Bottom_Left,    // 左下角开始提醒，从下至上
  Pos_Bottom_Right    // 右下角开始提醒，从下至上
};

class NotificationLabel;
class NotificationItem;
class Notification : public QObject {
  Q_OBJECT
 public:
  explicit Notification(QObject* parent = nullptr);
  ~Notification() override;

  /**
   * @brief Push 显示提示框
   * @param type 提示类型
   * @param pos 提示的方向(左上、左下、右上、右下)
   * @param title 提示的标题
   * @param content 提示的内容
   * @param nLive 提示框的存活时间，默认3000ms，若该值等于0，提示框不消失。
   */
  void Push(NotifyType type, NotifyPosition pos, QString title, QString content,
            int nLive = 3000);

 private:
  const int nMargin = 15;
  QSize m_size;
  std::mutex m_vecMtx;
  std::vector<NotificationItem*> m_vecItem;

 private slots:
  void itemRemoved(NotificationItem* pItem);
};

class NotificationItem : public QWidget {
  Q_OBJECT
 public:
  explicit NotificationItem(QWidget* parent = nullptr,
                            NotifyType type = Notify_Type_None,
                            NotifyPosition pos = Pos_Top_Right,
                            QString title = QString("标题"),
                            QString content = QString("这是一个提示"),
                            int nLife = 3000);
  ~NotificationItem() override;
  void Show();
  NotifyPosition GetPosType() const;
  bool IsAppearEnd() const;

 private:
  void Appear();
  void Disappear();

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  const int nFixedWidth = 300;
  const int nMinHeight = 50;
  const int nTopPadding = 14;
  const int nLeftPadding = 23;
  const int nRightPadding = 26;
  const int nMargin = 20;
  NotifyPosition m_enPos;
  QTimer m_liftTimer;
  int m_nLifeTime;
  bool m_bAppearEnd;

 signals:
  void itemRemoved(NotificationItem*);
};

class NotificationLabel : public QWidget {
  Q_OBJECT
 public:
  explicit NotificationLabel(QWidget* parent = nullptr, int nFixedWidth = 300,
                             QString content = "");
  ~NotificationLabel() override;
  void Adjust();

 protected:
  void paintEvent(QPaintEvent*) override;

 private:
  QStringList m_strList;
  QString m_strText;
  int m_nHeight;
  int m_nMargin = 5;                          // 上下间距
  QColor m_conetentColor = QColor(0x606266);  // 内容的字体颜色
};

#endif  // NOTIFICATION_H