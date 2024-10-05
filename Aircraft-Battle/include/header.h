#pragma once

#include <SDKDDKVer.h>
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
enum WIN_DOW { WIDTH = 480, HEIGHT = 700, OPTION_WIDTH = 189, OPTION_HEIGHT = 43 };

// 我的飞机属性
enum M_PLANE {
    m_width = 52,
    m_height = 60,

    m_speed = 8,
    m_speed_down = 3,

    m_hp = 3,
    m_boom = 3,

    m_life_width = 30,
    m_life_height = 30,

    m_boom_width = 63,
    m_boom_height = 56,
};

// 敌机属性
enum ENEMY {
    // 分类

    // 小型机
    e_small,
    e_small_width = 30,
    e_small_height = 20,
    e_small_speed = 4,
    e_small_hp = 1,

    // 中型机
    e_mid,
    e_mid_width = 49,
    e_mid_height = 58,
    e_mid_speed = 2,
    e_mid_hp = 10,

    // 大型机
    e_big,
    e_big_width = 174,
    e_big_height = 254,
    e_big_speed = 1,
    e_big_hp = 25
};

// 弹药属性
enum BULLET {
    bullet_width = 4,
    bullet_height = 10,
    bullet_speed = 10,
    supply_width = 39,
    supply_height = 47,
    supply_speed = 3,
    // 弹药类型
    normal_bullet,
    super_bullet,
    recover_life,
    boom_boom
};

// 敌机/子弹最大数量
#define MAX_NUM 100
// 缓冲区
#define BUF_SIZE 255
// 移动刷新间隔
#define MOVE_SEP 20
// 子弹发射间隔
#define FIRE_SEP 150
// 敌人创建间隔
#define ENEMY_SEP 500
// 碰撞检测间隔
#define COLLISION_SEP 1
// 补给间隔
#define SUPPLY_SEP 25000
// 超级子弹持续时间
#define SUPER_TIME 10000
// 无敌时间
#define INVINCIBLE_TIME 3000
// 按键保护时间（防止重复按键）
#define PROTECT_TIME 2000

// 定义类型
typedef int Type;

void play_music(const char[]);           // 音乐播放
void blood_line(int, int, int, double);  // 画血条
