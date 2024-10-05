#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QKeyEvent>

#include "Map/GameMap.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class GameWindow;
}
QT_END_NAMESPACE

class GameWindow : public QMainWindow {
    Q_OBJECT

  public:
    GameWindow(QWidget* parent = nullptr);
    ~GameWindow();

    virtual void paintEvent(QPaintEvent*);
    virtual void keyPressEvent(QKeyEvent*);

  public slots:
    void updateTime();     // 刷新时间
    void updateHit();      // 进行移动
    void updateAnimate();  // 动画刷新
    void updateEvent();    // 事件刷新

  private:
    Ui::GameWindow* ui;

    int m_second;  // 记录秒
    int m_minute;  // 记录分
    int m_hour;    // 记录时

    QTimer m_animateTimer;  // 动画间隔计时器
    QTimer m_hitTimer;      // 按键间隔计时器
    QTimer m_eventTimer;    // 事件间隔计时器
    QTimer m_gameTimer;     // 游戏时间计时器
    QTimer m_timer;         // 画面刷新速度

    GameMusic m_bgm;  // 背景音乐
};
#endif  // GAMEWINDOW_H
