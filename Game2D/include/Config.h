#pragma once

#include "framework.h"

#define MAX_LOADSTRING 100
#define BUF_SIZE 255

#define MOVESEP 50

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(x) ((x) > 0 ? 1 : (((x) == 0) ? 0 : -1))

enum { DGravity = 4, DUp = 0, DDown = 3, DLeft = 1, DRight = 2, DStay = 5, Gravity = 3 };

enum { StdWidth = 32, StdHeight = 32, WindowWidth = 720, WindowHeight = 480, GAME_HEAD, GAME_START, GAME_PAUSE, GAME_LOAD, GAME_SET, GAME_ABOUT };

enum { ById, ByName, ByFree };

typedef int Direction;
typedef int Mode;

// #define OPEN_DEBUG

// 调试输出
namespace DebugOutPut {
    inline void openConsole() {
        AllocConsole();
    }
    inline void closeConsole() {
        FreeConsole();
    }
    inline void output(const char buf[]) {
        HANDLE hd = GetStdHandle(STD_OUTPUT_HANDLE);
        WriteConsole(hd, buf, strlen(buf), NULL, NULL);
        CloseHandle(hd);
    }
};  // namespace DebugOutPut

// 设置文字
inline void setText(LPCSTR fName, COLORREF color, LONG fHeight, LONG fWeight) {
    settextcolor(color);
    LOGFONT f;
    gettextstyle(&f);

    f.lfHeight = fHeight;
    f.lfWeight = fWeight;

    _tcscpy_s(f.lfFaceName, fName);
    settextstyle(&f);
}

// 绘图函数，补充透明度
inline void drawAlpha(IMAGE* image, int x, int y, int width, int height, int pic_x, int pic_y, double AA = 1) {
    // 变量初始化
    DWORD* dst = GetImageBuffer();  // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
    DWORD* draw = GetImageBuffer();
    DWORD* src = GetImageBuffer(image);    // 获取picture的显存指针
    int imageWidth = image->getwidth();    // 获取图片宽度
    int imageHeight = image->getheight();  // 获取图片宽度
    int dstX = 0;                          // 在 绘图区域 显存里像素的角标
    int srcX = 0;                          // 在 image 显存里像素的角标

    // 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
    for(int iy = 0; iy < height; iy++) {
        for(int ix = 0; ix < width; ix++) {
            // 防止越界
            if(ix + pic_x >= 0 && ix + pic_x < imageWidth && iy + pic_y >= 0 && iy + pic_y < imageHeight && ix + x >= 0 && ix + x < WindowWidth && iy + y >= 0 && iy + y < WindowHeight) {
                // 获取像素角标
                int srcX = (ix + pic_x) + (iy + pic_y) * imageWidth;
                dstX = (ix + x) + (iy + y) * WindowWidth;

                int sa = ((src[srcX] & 0xff000000) >> 24) * AA;  // 0xAArrggbb;AA是透明度
                int sr = ((src[srcX] & 0xff0000) >> 16);         // 获取 RGB 里的 R
                int sg = ((src[srcX] & 0xff00) >> 8);            // G
                int sb = src[srcX] & 0xff;                       // B

                // 设置对应的绘图区域像素信息
                int dr = ((dst[dstX] & 0xff0000) >> 16);
                int dg = ((dst[dstX] & 0xff00) >> 8);
                int db = dst[dstX] & 0xff;
                draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)   // 公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
                             | ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)  // αp=sa/255 , FP=sg , BP=dg
                             | (sb * sa / 255 + db * (255 - sa) / 255);        // αp=sa/255 , FP=sb , BP=db
            }
        }
    }
}
