#pragma once

#include "header.h"

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
    Rect() {}
    Rect(int _left, int _top, int _width, int _height): left(_left), top(_top), width(_width), height(_height) {
        c_x = left + width / 2;
        c_y = top + height / 2;

        right = left + width;
        bottom = top + height;

        radius = (int)sqrt(width * width + height * height) / 2;
    }

    // 返回碰撞的index，否则返回-1
    int collision(int, const Rect[]);
    bool collision(const Rect&);

    // 返回点是否在矩形中
    bool point_in(int, int);

    // 用于修改矩形大小
    void operator*=(double times) {
        radius *= times;
        width *= times, height *= times;

        left = c_x - width / 2;
        right = c_x + width / 2;
        top = c_y - height / 2;
        bottom = c_y + height / 2;
    }

    // 输出矩形
    friend ostream& operator<<(ostream& out, const Rect& rect) {
        out << rect.left << " " << rect.right << endl;
        out << rect.top << " " << rect.bottom << endl;
        return out;
    }

    int left, right, top, bottom, width, height;
    int c_x, c_y;
    int radius;
};
