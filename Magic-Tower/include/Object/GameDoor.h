#ifndef GAMEDOOR_H
#define GAMEDOOR_H

#include "GameObject.h"

class GameDoor : public GameObject {
  public:
    GameDoor(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞

  protected:
    GameTimer m_timer;  // 开门计时器
};

#endif  // GAMEDOOR_H
