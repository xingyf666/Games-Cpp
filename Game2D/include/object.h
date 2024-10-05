#pragma once

#include "struct.h"

class Object {
  public:
    static std::map<std::string, Data*> dataMap;  // 存放所有物体对应的信息

    Object(std::string name, POINT pos, Data* data = nullptr): name(name), coord(pos), data(data), show(true), eId(0), AA(1) {}
    // 清除缓存数据和附加信息
    ~Object();

    virtual void draw(POINT origin) = 0;  // 绘图

    // 从 dataMap 返回数据
    double getSingle(std::string str) { return dataMap[name]->singles[str]; }
    POINT getPoint(std::string str) { return dataMap[name]->points[str]; }
    std::string getDetail(std::string str) { return dataMap[name]->details[str]; }

    // 绑定事件
    void bindEvent(int eventId) { eId = eventId; }

    POINT getPos() const { return coord; }  // 位置
    RECT getRect();                         // 所在矩形，考虑修正 fix
    std::string getName() const { return name; }
    Data* getData() const { return data; }
    bool ifShow() const { return show; }

    void setShow(bool s) { show = s; }
    double getAA() { return AA; }

    static void readData(LPCSTR name);  // 获取初始化信息
    static void deleteData();           // 清除相关信息

  protected:
    // 物体不需要 id ， id 通过 Map 中的映射隐式地使用
    std::string name;  // 用于标记物体类型
    POINT coord;
    Data* data;  // 可能携带的附加信息，例如返回值，文本等
    bool show;   // 是否显示
    int eId;     // 绑定的事件 id ，每次只能绑定一个
    double AA;   // 透明度
};

// 碰撞检测
namespace Collision {
    inline bool checkIn(POINT p, RECT r) {
        if((p.x > r.left) && (p.x < r.right) && (p.y > r.top) && (p.y < r.bottom)) {
            return true;
        }
        return false;
    }

    inline bool checkCollision(RECT r1, RECT r2) {
        // r1 中心位置
        POINT size = {r1.right - r1.left, r1.bottom - r1.top};
        POINT c = {(r1.right + r1.left) / 2, (r1.top + r1.bottom) / 2};
        RECT r = {r2.left - size.x / 2, r2.top - size.y / 2, r2.right + size.x / 2, r2.bottom + size.y / 2};
        if(checkIn(c, r)) {
            return true;
        }
        return false;
    }

    inline bool checkCollision(Object* obj1, Object* obj2) {
        RECT r1 = obj1->getRect(), r2 = obj2->getRect();
        return checkCollision(r1, r2);
    }

    inline std::vector<int> checkCollision(Object* obj, std::vector<Object*>& l) {
        std::vector<int> v;
        int index = 0;
        for(auto it = l.begin(); it != l.end(); ++it) {
            if(checkCollision(obj, *it)) {
                v.push_back(index);
            }
            index++;
        }
        return v;
    }
};  // namespace Collision
