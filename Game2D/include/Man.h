#pragma once

#include "GDynamic.h"

// 关于主角的相关类

class Man : public GDynamic {
  public:
    // 创建和删除
    static Man* createMan(Data* data);
    static bool deleteMan();

    // 移动方向
    void jump();
    void draw(POINT origin);

    // 能力
    void sword();                   // 剑刺
    void shield();                  // 护盾
    RECT swordRange(Direction& d);  // 返回剑刺的范围和方向

    void dead();
    void thinking(POINT&, POINT&);

    int HP, attack;

  private:
    static Man* man;

    int jumpHeight;
    bool shielding;  // 是否有护盾
    POINT alivePos;  // 复活位置
    int swordStep;
    Timer* timer;  // 用于一些技能的计时（护盾）

    // 这里涉及到一个关键 bug ：不要偷懒在构造函数执行前就直接调用封装在类中的 getPoint 等函数
    Man(): GDynamic("man", dataMap["man"]->points["pos"], {0, 0}), alivePos({0, 0}), HP(0), attack(0), jumpHeight(0), shielding(false), swordStep(0), timer(nullptr) {}
    ~Man() {}
};