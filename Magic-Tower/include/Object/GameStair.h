#ifndef GAMESTAIR_H
#define GAMESTAIR_H

#include "GameObject.h"

class GameStair : public GameObject {
  public:
    GameStair(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞
};

#endif  // GAMESTAIR_H
