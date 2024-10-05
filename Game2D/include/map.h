#pragma once

#include "GStatic.h"
#include "Man.h"
#include "enemy.h"


class Map {
  public:
    static Map* createMap(LPCSTR name);

    // id 对应每一个物体，每个物体的 id 就是它的下标。当然，为了方便创建物体，我们保存最大 id
    std::map<int, Object*> staticMap;  // 当前地图的静态物体
    std::map<int, Enemy*> enemyMap;    // 敌人

    int MAXID;    // 最大 id ，是当前地图中物体的最大 id
    Data* mData;  // 人物数据

    Map(LPCSTR name);
    ~Map();

    void draw(POINT origin);
    bool collision(GDynamic* gd, Direction d);
    void move(Man* man);                  // 移动地图中的物体，同时进行碰撞检测，它应当在 man 移动后进行
    std::string buttonClicked(POINT mp);  // 返回按下的结果
};