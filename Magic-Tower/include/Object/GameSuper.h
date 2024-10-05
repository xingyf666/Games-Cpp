#ifndef GAMESUPER_H
#define GAMESUPER_H

#include "GameObject.h"

class GameSuper : public GameObject {
  public:
    GameSuper(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞
};

#endif  // GAMESUPER_H
