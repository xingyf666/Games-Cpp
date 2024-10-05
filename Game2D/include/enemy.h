#pragma once

#include "GDynamic.h"

// 怪物相关

class Enemy : public GDynamic {
  public:
    // 创建怪物，删除操作在 Map 中进行
    static Enemy* createEnemy(std::string name, POINT pos, POINT moveRange);

    // 移动方向
    void draw(POINT origin);
    void hurt(int a) {
        HP -= a;
        if(HP <= 0) {
            alive = false;
        }
    }

    // 砍击
    void cut(int ID);
    bool ifSkill() { return skilling; }

    // 技能步数，攻击范围
    int HP, skillRange;
    bool skilling;
    POINT moveRange;
    Timer* timer;  // 技能计时

    Enemy(std::string name, POINT pos, POINT moveRange, Data* data = nullptr): GDynamic(name, pos, {0, 0}, data), HP(0), skillRange(0), skilling(false), timer(nullptr), moveRange(moveRange) {}
    ~Enemy() {
        if(timer != nullptr) {
            delete timer;
        }
    }
};