#pragma once

#include "object.h"

// 静态物体不会移动，只是装饰物
class GStatic : public Object {
  public:
    // 默认情况下是会显示的
    GStatic(std::string name, POINT pos, Data* data = nullptr): Object(name, pos, data) {}

    void draw(POINT origin) {
        POINT size = getPoint("size");
        // 如果离开画面范围，就不用绘制
        if(coord.x - origin.x < WindowWidth && origin.x - coord.x < size.x && coord.y - origin.y < WindowHeight && origin.y - coord.y < size.y) {
            POINT picPos = getPoint("imagePos");
            drawAlpha(MImage::imageMap["GStatic"], coord.x - origin.x, coord.y - origin.y, (int)size.x, (int)size.y, (int)picPos.x, (int)picPos.y);
        }
    }
};

// 按钮是特殊的静态物体
class GButton : public GStatic {
  public:
    GButton(std::string name, POINT pos, Data* data = nullptr): GStatic(name, pos, data), clicked(false) {}

    void draw(POINT origin) {
        POINT picPos = (clicked ? getPoint("imagePos1") : getPoint("imagePos2"));
        POINT size = getPoint("size");
        drawAlpha(MImage::imageMap["GButton"], coord.x - origin.x, coord.y - origin.y, (int)size.x, (int)size.y, (int)picPos.x, (int)picPos.y);

        // 从附加信息中获取文本
        std::string text = data->details["text"];
        if(text != "") {
            // 显示文本
            setText("Consolas", WHITE, 15, 5);
            LPCSTR p = text.c_str();
            RECT r = getRect();
            drawtext(p, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }
    bool getClicked() const { return clicked; }
    void setClicked(bool c) { clicked = c; }

  protected:
    bool clicked;
};