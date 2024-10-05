
#include "class.h"

// 回到开头
void Music::init() {
    char command_name[BUF_SIZE] = "seek ";
    strcat_s(command_name, command);
    strcat_s(command_name, " to start");

    mciSendString(command_name, 0, 0, 0);
}

// 打开音乐（支持多种音乐格式）
void Music::open() {
    char command_name[BUF_SIZE] = "open ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 播放
void Music::play() {
    char command_name[BUF_SIZE] = "play ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 暂停
void Music::pause() {
    char command_name[BUF_SIZE] = "pause ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 继续
void Music::resume() {
    char command_name[BUF_SIZE] = "resume ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 结束
void Music::stop() {
    char command_name[BUF_SIZE] = "stop ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 关闭
void Music::close() {
    char command_name[BUF_SIZE] = "close ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 获取设备状态
void Music::status(char buf[]) {
    char command_name[BUF_SIZE] = "status ";
    strcat_s(command_name, command);
    strcat_s(command_name, " mode");

    // 记录状态
    mciSendString(command_name, buf, BUF_SIZE, 0);
}

// 获取文件长度
void Music::length(char buf[]) {
    char command_name[BUF_SIZE] = "status ";
    strcat_s(command_name, command);
    strcat_s(command_name, " length");

    // 记录状态
    mciSendString(command_name, buf, BUF_SIZE, 0);
}

// 矩形方法
bool Rect::collision(const Rect& rect) {
    if((right >= rect.left && right <= rect.right) || (left >= rect.left && left <= rect.right)) {
        if((top >= rect.top && top <= rect.bottom) || (bottom >= rect.top && bottom <= rect.bottom)) {
            return true;
        }
    }
    if((rect.right >= left && rect.right <= right) || (rect.left >= left && rect.left <= right)) {
        if((rect.top >= top && rect.top <= bottom) || (rect.bottom >= top && rect.bottom <= bottom)) {
            return true;
        }
    }
    return false;
}

int Rect::collision(int len, const Rect rect[]) {
    for(int i = 0; i < len; i++) {
        if(collision(rect[i])) {
            return i;
        }
    }
    return -1;
}

bool Rect::point_in(int point_x, int point_y) {
    if(point_x >= left && point_x <= right) {
        if(point_y >= top && point_y <= bottom) {
            return true;
        }
    }
    return false;
}
