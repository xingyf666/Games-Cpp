#pragma once

#include "object.h"

class GDynamic : public Object {
  public:
    // 移动方向
    void move() {
        if(!stay) {
            coord.x += speed.x;
        }
        int x = status % 10, y = status / 10;
        x = (x + 1) % 9;
        status = x + y * 10;
    }

    // 原地不动
    void lock(bool ifLock) { stay = ifLock; }

    void gravity() {
        coord.y += speed.y;
        speed.y += Gravity;
    }

    bool ifAlive() { return alive; }

    void setDir(Direction d) {
        int x = status % 10, y = status / 10;
        switch(d) {
            case DLeft:
                speed.x = -std::abs(speed.x);
                y = DLeft;
                break;
            case DRight:
                speed.x = std::abs(speed.x);
                y = DRight;
                break;
            case DUp:
                break;
            case DDown:
                break;
        }
        status = x + y * 10;
    }
    int getDir() {
        if(speed.x > 0) {
            return DRight;
        } else if(speed.x < 0) {
            return DLeft;
        } else {
            return DStay;
        }
    }
    void setSpeed(POINT v) {
        speed = v;
        if(v.x > 0) {
            setDir(DRight);
        } else {
            setDir(DLeft);
        }
    }

    POINT getSpeed() const { return speed; }
    void setPos(POINT pos) { coord = pos; }

    POINT speed;  // 速度
    bool alive;   // 是否存活
    bool stay;    // 运动锁
    int status;   // 记录运动状态 十位表示上下左右，个位表示状态

    // 这里涉及到一个关键 bug ：不要偷懒在构造函数执行前就直接调用封装在类中的 getPoint 等函数
    GDynamic(std::string name, POINT pos, POINT speed, Data* data = nullptr): Object(name, pos, data), speed(speed), alive(true), stay(false), status(0) {}
};