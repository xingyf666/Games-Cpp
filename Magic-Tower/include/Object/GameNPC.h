#ifndef GAMENPC_H
#define GAMENPC_H

#include "GameObject.h"

class GameNPC : public GameObject {
  public:
    GameNPC(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞
};

#endif  // GAMENPC_H
