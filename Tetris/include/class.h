#pragma once

#include "Tetris.h"

// 计时器
class Timer {
  public:
    Timer() {
        interval = 0;
        t_start = 0;
        open_timer = false;
    }
    Timer(DWORD _interval): interval(_interval) {
        t_start = 0;
        open_timer = false;
    }

    // 设置间隔
    void set(DWORD _interval) { interval = _interval; }
    // 开启和关闭计时器
    void open() {
        open_timer = true;
        t_start = clock();
    }
    void close() { open_timer = false; }
    // 获得状态
    bool status() { return open_timer; }
    // 确认是否经过了间隔时间
    bool t_time() {
        if(open_timer && clock() - t_start > interval) {
            t_start = clock();
            return true;
        }
        return false;
    }

  private:
    DWORD t_start;
    DWORD interval;
    bool open_timer;
};

// 音乐封装类
class Music {
  public:
    // 获取音乐路径
    Music(const char _arr[]): command("") { strcat_s(command, _arr); }

    void init();    // 初始化
    void open();    // 打开
    void play();    // 播放
    void pause();   // 暂停
    void resume();  // 继续
    void stop();    // 停止
    void close();   // 关闭

    void length(char[]);  // 音乐长度
    void status(char[]);  // 音乐播放状态

  private:
    char command[BUF_SIZE];
};

// 矩形类，用于碰撞检测
class Rect {
  public:
    Rect(): width(RECT_SIZE), height(RECT_SIZE) {}
    Rect(int _left, int _top): left(_left), top(_top), width(RECT_SIZE), height(RECT_SIZE) {
        right = left + width;
        bottom = top + height;
    }
    Rect(int _left, int _top, int _width, int _height): left(_left), top(_top), width(_width), height(_height) {
        right = left + width;
        bottom = top + height;
    }

    // 返回碰撞的index，否则返回-1
    bool collision(const Rect*);
    int collision(int, Rect**);
    // 判断点是否在其中
    bool point_in(int, int);

    // 显示矩形
    void show();
    // 返回是否移动成功
    bool move(Direct);
    // 返回原位
    void back(Direct);

    // 绕定点顺时针旋转90度
    bool rotate_clock(int, int);
    // 逆时针旋转90度
    void rotate_anticlock(int, int);

    int left, right, top, bottom, width, height;
    static Timer move_timer;
};

class Rect_Size {
  public:
    // 设置存储方块
    void set(Type _type, const int _id_list[]) {
        // 通过id获取方块指针
        type = _type;
        for(int i = 0; i < 4; i++) {
            rect[i] = rect_list[_id_list[i]];
        }
    }

    // 移动我的形状
    bool move(Direct);
    // 顺时针旋转我的形状
    void rotate();
    // 获得旋转中心
    void rotate_center(int&, int&);

    static void show();
    // 静态创建形状
    static void create();
    // 销毁
    static void distory(Rect*);
    static void test_line();  // 消去一行

    static bool get_live() { return alive; }

    // 创建存储类
    static void create_rect() {
        if(rect_list == nullptr) {
            rect_list = new Rect*[MAX_NUM];
            alive = true;
            // 开启计时器
            Rect::move_timer.open();
        }
    }
    static Rect_Size* share_rect() {
        if(my_rect == nullptr) {
            my_rect = new Rect_Size;
        }
        return my_rect;
    }

    // 清空
    static void delete_rect() {
        if(rect_list != nullptr) {
            for(int i = 0; i < count; i++) {
                delete rect_list[i];
                rect_list[i] = nullptr;
            }
            delete[] rect_list;
            rect_list = nullptr;
            delete my_rect;
            my_rect = nullptr;
            count = 0;
        }
    }

  private:
    Type type;
    // 方块成员指针列表
    Rect* rect[4];

    // 存放所有方块
    static bool alive;
    static int count;
    static Rect_Size* my_rect;
    static Type show_type;
    static Rect** rect_list;
};
