
#include "object.h"

std::map<std::string, Data*> Object::dataMap;
std::map<int, Event*> Event::eventMap;

// 返回所在矩形，考虑修正 fix
RECT Object::getRect() {
    POINT size = getPoint("size");
    double wFix = getSingle("wFix");
    double hFix = getSingle("hFix");
    int dx = 0, dy = 0;
    dx = size.x * (1 - wFix) / 2;
    dy = size.y * (1 - hFix) / 2;
    return {coord.x + dx, coord.y + dy, coord.x + size.x - dx, coord.y + size.y - dy};
}

void Object::readData(LPCSTR name) {
    Data::read(name, dataMap, ByName);
}

void Object::deleteData() {
    for(auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        delete it->second;
        dataMap[it->first] = nullptr;
    }
}

Object::~Object() {
    if(data != nullptr) {
        delete data;
        data = nullptr;
    }
}