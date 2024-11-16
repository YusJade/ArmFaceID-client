#include "qt_gui/notification.h"

#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScreen>
#include <QScrollBar>
#include <QStyleOption>

static int nAppearTime = 200;     // 出现的时间200ms
static int nDisappearTime = 200;  // 消失的时间200ms

Notification::Notification(QObject *parent) : QObject(parent) {
  QWidget *pWidget = qobject_cast<QWidget *>(parent);
  if (pWidget == nullptr)
    throw std::runtime_error("parent of notification error!");
  m_size = pWidget->size();
  m_vecItem.reserve(30);
}

Notification::~Notification() {}

void Notification::Push(NotifyType type, NotifyPosition pos, QString title,
                        QString content, int nLive) {
  std::lock_guard<std::mutex> lck(m_vecMtx);
  NotificationItem *pItem = new NotificationItem(
      qobject_cast<QWidget *>(parent()), type, pos, title, content, nLive);
  connect(pItem, &NotificationItem::itemRemoved, this,
          &Notification::itemRemoved);
  int currentHeight = 0;
  int currentX = 0;

  if (pos == NotifyPosition::Pos_Top_Right) {
    currentX = m_size.width();
    currentHeight = nMargin;
  } else if (pos == NotifyPosition::Pos_Top_Left) {
    currentX = -pItem->width();
    currentHeight = nMargin;
  } else if (pos == NotifyPosition::Pos_Bottom_Left) {
    currentX = -pItem->width();
    currentHeight = m_size.height() - nMargin - pItem->height();
  } else {
    currentX = m_size.width();
    currentHeight = m_size.height() - nMargin - pItem->height();
  }

  for_each(m_vecItem.begin(), m_vecItem.end(), [&](NotificationItem *item) {
    if (item->GetPosType() == pos) {
      if (pos == NotifyPosition::Pos_Top_Right) {
        currentHeight += (item->height() + nMargin);
      } else if (pos == NotifyPosition::Pos_Top_Left) {
        currentHeight += (item->height() + nMargin);
      } else if (pos == NotifyPosition::Pos_Bottom_Left) {
        currentHeight -= (item->height() + nMargin);
      } else {
        currentHeight -= (item->height() + nMargin);
      }
    }
  });
  pItem->move(currentX, currentHeight);
  m_vecItem.emplace_back(pItem);
  pItem->Show();
}

void Notification::itemRemoved(NotificationItem *pRemoved) {
  std::unique_lock<std::mutex> lck(m_vecMtx);
  int currentY = 0;
  bool bFirst = true;
  NotifyPosition pos = pRemoved->GetPosType();
  for (auto itr = m_vecItem.begin(); itr != m_vecItem.end();) {
    if (*itr == pRemoved) {
      m_vecItem.erase(itr);
      break;
    } else
      ++itr;
  }

  for_each(
      m_vecItem.begin(), m_vecItem.end(),
      [&, pos, bFirst, currentY](NotificationItem *item) mutable {
        if (item->GetPosType() == pos) {
          if (bFirst) {
            if (pos == NotifyPosition::Pos_Top_Right) {
              currentY = nMargin;
            } else if (pos == NotifyPosition::Pos_Top_Left) {
              currentY = nMargin;
            } else if (pos == NotifyPosition::Pos_Bottom_Left) {
              currentY = m_size.height() - nMargin - item->height();
            } else {
              currentY = m_size.height() - nMargin - item->height();
            }
            bFirst = false;
          } else {
            if (item->IsAppearEnd()) {
              if (pos == NotifyPosition::Pos_Top_Right) {
                currentY += (item->height() + nMargin);
              } else if (pos == NotifyPosition::Pos_Top_Left) {
                currentY += (item->height() + nMargin);
              } else if (pos == NotifyPosition::Pos_Bottom_Left) {
                currentY -= (item->height() + nMargin);
              } else {
                currentY -= (item->height() + nMargin);
              }
            }
          }
          if (item->IsAppearEnd()) {
            QPropertyAnimation *pAnimation1 =
                new QPropertyAnimation(item, "geometry", this);
            pAnimation1->setDuration(nDisappearTime);
            pAnimation1->setStartValue(QRect(item->pos().x(), item->pos().y(),
                                             item->width(), item->height()));
            pAnimation1->setEndValue(QRect(item->pos().x(), currentY,
                                           item->width(), item->height()));

            pAnimation1->start(
                QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);
          }
        }
      });
}

///
NotificationItem::NotificationItem(QWidget *parent, NotifyType type,
                                   NotifyPosition pos, QString title,
                                   QString content, int nLife)
    : QWidget(parent), m_enPos(pos), m_bAppearEnd(false) {
  setObjectName(QStringLiteral("notification_item"));
  QLabel *pTitle = new QLabel(title, this);
  pTitle->setObjectName(QStringLiteral("label_title"));
  NotificationLabel *pContent =
      new NotificationLabel(this, nFixedWidth - 10, content);
  QFont font;
  font.setPointSize(11);
  font.setFamily(QStringLiteral("Microsoft Yahei"));
  pContent->setFont(font);
  QPushButton *pClose = new QPushButton(this);
  pClose->setFixedSize(16, 16);
  pClose->setObjectName(QStringLiteral("btn_close"));
  pClose->setCursor(QCursor(Qt::PointingHandCursor));

  setStyleSheet(QStringLiteral(
      "QWidget#notification_item{border:none;border-radius:8px;background-"
      "color:white;}"
      "QLabel#label_title{border:none;background-color:white;font-family:"
      "Microsoft Yahei;font-size:20px;font-weight:700;color:#303133;}"
      "QPushButton#btn_close{border:none;background-color:white;background-"
      "position:center;border-image:url(assets/tips/notification_close.png);}"
      "QPushButton:hover#btn_close{border-image:url(assets/tips/"
      "notification_close_hover.png);}"));

  // 标题设置
  pTitle->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  QFontMetrics fontWidth(pTitle->font());
  QString elideNote = fontWidth.elidedText(pTitle->text(), Qt::ElideRight, 120);
  pTitle->setText(elideNote);
  pTitle->setToolTip(title);
  pTitle->adjustSize();

  // 内容设置
  pContent->Adjust();

  // 布局
  if (type != NotifyType::Notify_Type_None) {
    QLabel *pIcon = new QLabel(this);
    pIcon->setStyleSheet(
        QStringLiteral("QLabel{border:none;background-color:white;}"));
    if (type == NotifyType::Notify_Type_Success) {
      pIcon->setPixmap(
          QPixmap(QString::fromUtf8("assets/tips/type_success.png")));
    } else if (type == NotifyType::Notify_Type_Error) {
      pIcon->setPixmap(
          QPixmap(QString::fromUtf8("assets/tips/type_error.png")));
    } else if (type == NotifyType::Notify_Type_Warning) {
      pIcon->setPixmap(
          QPixmap(QString::fromUtf8("assets/tips/type_warning.png")));
    } else {
      pIcon->setPixmap(
          QPixmap(QString::fromUtf8("assets/tips/type_information.png")));
    }
    pIcon->adjustSize();
    setFixedSize(
        nFixedWidth + nLeftPadding + nRightPadding + pIcon->width() + 10,
        pContent->height() + pTitle->height() + 2 * nTopPadding + 5);
    pIcon->move(nLeftPadding,
                nTopPadding - std::abs(pIcon->height() - pTitle->height()) / 2);
    pTitle->move(pIcon->x() + pIcon->width() + 10, nTopPadding);
  } else {
    setFixedSize(nFixedWidth + nLeftPadding + nRightPadding,
                 pContent->height() + pTitle->height() + 2 * nTopPadding + 5);
    pTitle->move(nLeftPadding, nTopPadding);
  }

  pContent->move(pTitle->x(), pTitle->y() + pTitle->height() + 5);
  pClose->move(width() - pClose->width() / 2 - nRightPadding, nTopPadding);

  QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
  effect->setBlurRadius(20);   // 阴影圆角的大小
  effect->setColor(Qt::gray);  //阴影的颜色
  effect->setOffset(0, 0);     //阴影的偏移量
  setGraphicsEffect(effect);

  connect(pClose, &QPushButton::clicked, this, [&]() {
    m_liftTimer.stop();
    Disappear();
  });

  connect(&m_liftTimer, &QTimer::timeout, this, [&]() {
    m_liftTimer.stop();
    Disappear();
  });
  m_nLifeTime = nLife;
  hide();
}

NotificationItem::~NotificationItem() {}

void NotificationItem::Show() {
  show();
  Appear();
}

NotifyPosition NotificationItem::GetPosType() const { return m_enPos; }

bool NotificationItem::IsAppearEnd() const { return m_bAppearEnd; }

void NotificationItem::Appear() {
  QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
  animation->setDuration(nAppearTime);
  animation->setStartValue(QRect(pos().x(), pos().y(), width(), height()));
  if (m_enPos == NotifyPosition::Pos_Top_Right) {
    animation->setEndValue(
        QRect(pos().x() - width() - nMargin, pos().y(), width(), height()));
  } else if (m_enPos == NotifyPosition::Pos_Top_Left) {
    animation->setEndValue(
        QRect(pos().x() + width() + nMargin, pos().y(), width(), height()));
  } else if (m_enPos == NotifyPosition::Pos_Bottom_Left) {
    animation->setEndValue(
        QRect(pos().x() + width() + nMargin, pos().y(), width(), height()));
  } else {
    animation->setEndValue(
        QRect(pos().x() - width() - nMargin, pos().y(), width(), height()));
  }
  animation->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);
  connect(animation, &QPropertyAnimation::finished, this, [&]() {
    m_bAppearEnd = true;
    if (m_nLifeTime > 0) m_liftTimer.start(m_nLifeTime);
  });
}

void NotificationItem::Disappear() {
  QGraphicsOpacityEffect *pOpacity = new QGraphicsOpacityEffect(this);
  pOpacity->setOpacity(1);
  setGraphicsEffect(pOpacity);
  QPropertyAnimation *pOpacityAnimation2 =
      new QPropertyAnimation(pOpacity, "opacity");
  pOpacityAnimation2->setDuration(nDisappearTime);
  pOpacityAnimation2->setStartValue(1);
  pOpacityAnimation2->setEndValue(0);
  pOpacityAnimation2->start(
      QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);
  connect(pOpacityAnimation2, &QPropertyAnimation::finished, this, [&]() {
    emit itemRemoved(this);
    deleteLater();
  });
}

void NotificationItem::paintEvent(QPaintEvent *event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
  QWidget::paintEvent(event);
}

//
NotificationLabel::NotificationLabel(QWidget *parent, int nFixedWidth,
                                     QString content)
    : QWidget(parent) {
  setObjectName(QStringLiteral("motification_label"));
  setFixedWidth(nFixedWidth);
  m_strText = content;
  m_nHeight = 0;
  setStyleSheet(QStringLiteral(
      "QWidget#motification_label{background-color:white;border:none;}"));
}

NotificationLabel::~NotificationLabel() {}

void NotificationLabel::Adjust() {
  m_strList.clear();
  QFontMetrics fm(font());
  int tpHeight = fm.height();
  int size = m_strText.length();
  QString strTp;
  for (int i = 0; i < size; i++) {
    strTp.append(m_strText.at(i));
    int tpWidth = fm.horizontalAdvance(strTp);
    if (tpWidth > width()) {
      i--;
      strTp.chop(1);
      m_strList.push_back(strTp);
      strTp.clear();
      m_nHeight += tpHeight;
    } else {
      if (i == size - 1) {
        m_strList.push_back(strTp);
        strTp.clear();
        m_nHeight += tpHeight;
      }
    }
  }
  setFixedHeight(m_nHeight + tpHeight / 2);
  repaint();
}

void NotificationLabel::paintEvent(QPaintEvent *event) {
  QStyleOption opt;
  opt.initFrom(this);
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
  QFontMetrics fm(font());
  int tpHeight = fm.height();
  int height = tpHeight;
  p.setPen(m_conetentColor);
  for (int i = 0; i < m_strList.count(); i++) {
    p.drawText(QPoint(0, height), m_strList[i]);
    height += (tpHeight + m_nMargin);
  }
  QWidget::paintEvent(event);
}