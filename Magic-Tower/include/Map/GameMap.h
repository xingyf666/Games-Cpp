#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "Object/GameObject.h"

class GameMap {
  public:
    GameMap(int floor, nlohmann::json json);
    ~GameMap();

    GameObject* createObj(int i, int j, int id);  // 根据类型创建不同的物体
    GameObject* shareObj(int i, int j);           // 获得指定位置的指针
    void setObj(int i, int j, GameObject*);       // 设置指定位置的指针
    void deleteObj(int i, int j);                 // 删除指定位置的物体

    void draw(QMainWindow*);  // 绘制地图
    void test();              // 检查是否有物体与英雄碰撞
    void animate();           // 动画刷新

  protected:
    int m_floor;                            // 记录这是第几层
    GameObject* m_obj[13][13];              // 地图列表
    QMap<std::string, GameEvent*> m_event;  // 记录事件

  public:
    static void initMap();          // 初始化地图
    static GameMap& shareMap(int);  // 获得地图
    static void deleteMap();        // 删除地图

    static QList<GameMap*> s_map;  // 记录地图
    static GameMusic s_bgm;        // 背景音乐
};

#endif  // GAMEMAP_H
