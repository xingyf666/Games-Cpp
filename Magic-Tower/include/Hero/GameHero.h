#ifndef GAMEHERO_H
#define GAMEHERO_H

#include "Event/GameEvent.h"

class GameHero {
  public:
    enum { moving, fixed, fighting };

  public:
    void draw(QMainWindow*);  // 绘制函数
    void move();              // 移动函数
    void moveBack();          // 返回上一步

    // 计算战斗结果
    int fightResult(int hp, int attack, int defense);

    // 获得属性
    int operator[](std::string name) const;
    int& operator[](std::string name);

  protected:
    QMap<std::string, int> m_attributes;  // 记录属性

    QPoint m_back;  // 记录上一步的位置
    QRect m_rect;   // 在源图像中的矩形

  public:
    static void initHero();        // 英雄数据初始化
    static GameHero& shareHero();  // 创建英雄
    static void deleteHero();      // 删除英雄

  protected:
    static GameHero* s_hero;   // 全局唯一英雄
    static QImage s_heroImg;   // 保存英雄图像
    static QImage s_fightImg;  // 保存战斗图像
};

#endif  // GAMEHERO_H
