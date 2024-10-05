#ifndef GAMETOOL_H
#define GAMETOOL_H

#include "GameObject.h"

class GameTool : public GameObject {
  public:
    GameTool(QPoint pos, QRect rect);

    virtual void animate();           // 动画函数
    virtual void draw(QMainWindow*);  // 绘制重载
    virtual void collision();         // 与英雄碰撞
};

#endif  // GAMETOOL_H
