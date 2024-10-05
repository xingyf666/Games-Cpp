#ifndef GAMEGROUND_H
#define GAMEGROUND_H

#include "GameObject.h"

class GameGround : public GameObject {
  public:
    GameGround(QPoint pos, GameEvent* event);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞
};

#endif  // GAMEGROUND_H
