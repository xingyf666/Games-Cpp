#ifndef GAMEWALL_H
#define GAMEWALL_H

#include "GameObject.h"

class GameWall : public GameObject {
  public:
    GameWall(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞
};

#endif  // GAMEWALL_H
