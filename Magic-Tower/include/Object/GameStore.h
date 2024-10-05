#ifndef GAMESTORE_H
#define GAMESTORE_H

#include "GameObject.h"

class GameStore : public GameObject {
  public:
    GameStore(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞
};

#endif  // GAMESTORE_H
