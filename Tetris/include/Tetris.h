#pragma once

#include <conio.h>
#include <graphics.h>
#include <mmsystem.h>  //包含多媒体设备接口头文件

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#pragma comment(lib, "winmm.lib")  // 加载静态库

using namespace std;

// 窗口属性
enum WIN_DOW {
    WHOLE_WIDTH = 720,
    WIDTH = 480,
    HEIGHT = 720,
    RECT_SIZE = 30,

    SHOW_X = 600,
    SHOW_Y = 120,
    SHOW_WIDTH = 160,
    SHOW_HEIGHT = 160,

    // 选项卡
    OPTION_WIDTH = 189,
    OPTION_HEIGHT = 43
};

// 定义不同形状
enum SIZE_TYPE { SIZE_01_10_11_12, SIZE_00_01_10_11, SIZE_00_01_02_03, SIZE_00_01_11_12, SIZE_00_10_20_21 };

enum DIRECT { MOVE_LEFT, MOVE_RIGHT, MOVE_DOWN, MOVE_UP };

#define BUF_SIZE 255
#define MAX_NUM 1000
#define MOVE_SEP 1000
#define HIT_SEP 200

typedef int Type;
typedef int Direct;

// 播放音乐的方法
void play_music(const char[]);
