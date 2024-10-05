#ifndef GAMEENEMY_H
#define GAMEENEMY_H

#include "GameObject.h"

class GameEnemy : public GameObject {
  public:
    GameEnemy(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞

  protected:
    GameTimer m_timer;  // 战斗计时器
};

#endif  // GAMEENEMY_H
