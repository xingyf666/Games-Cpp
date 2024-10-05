#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <QListWidget>

#include "Hero/GameHero.h"


class GameObject {
  public:
    enum { active, dead };

  public:
    GameObject(QPoint pos, QRect rect);
    virtual ~GameObject();

    virtual void animate() = 0;           // 动画函数
    virtual void draw(QMainWindow*) = 0;  // 绘制函数
    virtual void collision() = 0;         // 与英雄碰撞

    // 获得属性
    int operator[](std::string name) const;
    int& operator[](std::string name);

    void push(std::string name);  // 添加属性名称
    void bind(GameEvent* event);  // 绑定事件

  protected:
    QMap<std::string, int> m_attributes;  // 记录属性
    QList<std::string> m_attrList;        // 保存额外属性的名称列表
    GameEvent* m_event;                   // 绑定的事件
    QRect m_rect;                         // 在源图像中的矩形

  public:
    static void initObject();            // 初始化资源
    static nlohmann::json s_sourceData;  // 存放资源
    static QImage s_objImg;              // 保存所有物体图像
    static QListWidget* s_toolList;      // 列表控件
};

#endif  // GAMEOBJECT_H
