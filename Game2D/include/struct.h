#pragma once

#include "Config.h"

// 封装图片结构
struct MImage {
    static std::map<std::string, IMAGE*> imageMap;  // 存放所有图片

    static IMAGE* createImage(LPCSTR name) {
        IMAGE* image = new IMAGE;
        loadimage(image, name);
        return image;
    }
    static void deleteImage() {
        for(auto it = imageMap.begin(); it != imageMap.end(); ++it) {
            if(it->second != nullptr) {
                delete it->second;
                imageMap[it->first] = nullptr;
            }
        }
    }
};

// 音乐封装类
struct Music {
    // 使用 LPCSTR 作为键值有重大隐患，可能会有离开作用域导致键值丢失的情况出现
    static std::map<std::string, Music*> musicMap;  // 存放所有音乐

    // 获取音乐路径
    Music(LPCSTR name): command("") { strcat_s(command, name); }
    static Music* createMusic(std::string name) { return new Music(name.c_str()); }
    static void deleteMusic() {
        for(auto it = musicMap.begin(); it != musicMap.end(); ++it) {
            delete it->second;
            musicMap[it->first] = nullptr;
        }
    }

    // 返回开头
    void toStart() {
        char command_name[BUF_SIZE] = "seek ";
        strcat_s(command_name, command);
        strcat_s(command_name, " to start");

        mciSendString(command_name, 0, 0, 0);
    }
    void open() {
        char command_name[BUF_SIZE] = "open ";
        strcat_s(command_name, command);
        mciSendString(command_name, 0, 0, 0);
    }
    void play(bool repeat = false) {
        char buf[BUF_SIZE];
        status(buf);
        // 如果音乐没有在播放，就播放音乐
        if(buf[0] != 'p') {
            char command_name[BUF_SIZE] = "play ";
            strcat_s(command_name, command);
            if(repeat) {
                strcat_s(command_name, " repeat");
            }
            mciSendString(command_name, 0, 0, 0);
        }
    }
    void pause() {
        char command_name[BUF_SIZE] = "pause ";
        strcat_s(command_name, command);
        mciSendString(command_name, 0, 0, 0);
    }
    void resume() {
        char command_name[BUF_SIZE] = "resume ";
        strcat_s(command_name, command);
        mciSendString(command_name, 0, 0, 0);
    }
    void stop() {
        char command_name[BUF_SIZE] = "stop ";
        strcat_s(command_name, command);
        mciSendString(command_name, 0, 0, 0);
    }
    void close() {
        char command_name[BUF_SIZE] = "close ";
        strcat_s(command_name, command);
        mciSendString(command_name, 0, 0, 0);
    }
    // 设置音量
    void setVolume(int volume) {
        char command_name[BUF_SIZE] = "setaudio ";
        strcat_s(command_name, command);
        strcat_s(command_name, " volume to");
        char volume_buf[BUF_SIZE];
        sprintf_s(volume_buf, " %d", volume);
        strcat_s(command_name, volume_buf);
        // 设置音量
        mciSendString(command_name, 0, 0, 0);
    }
    // 音乐长度
    void length(char buf[]) {
        char command_name[BUF_SIZE] = "status ";
        strcat_s(command_name, command);
        strcat_s(command_name, " length");

        // 记录状态
        mciSendString(command_name, buf, BUF_SIZE, 0);
    }
    // 获取播放状态
    void status(char buf[]) {
        char command_name[BUF_SIZE] = "status ";
        strcat_s(command_name, command);
        strcat_s(command_name, " mode");

        // 记录状态
        mciSendString(command_name, buf, BUF_SIZE, 0);
    }

    char command[BUF_SIZE];
};

// 定时器封装
struct Timer {
    static Timer* createTimer(const int ID, const DWORD interval) {
        Timer* timer = new Timer(ID, interval);
        timerMap[ID] = timer;
        return timer;
    }
    static void deleteTimer() {
        for(auto it = timerMap.begin(); it != timerMap.end(); ++it) {
            delete it->second;
            timerMap[it->first] = nullptr;
            timerMap.erase(it);
        }
    }
    static void deleteTimer(const int ID) {
        if(timerMap[ID] != nullptr) {
            delete timerMap[ID];
            timerMap[ID] = nullptr;
            timerMap.erase(ID);
        }
    }
    // 开启定时器，同时返回指针
    static Timer* start(const int ID) {
        Timer* timer = timerMap[ID];
        if(!timer->on) {
            timer->t = clock();
            timer->on = true;
        }
        return timer;
    }

    void start() {
        t = clock();
        on = true;
    }
    void stop() { on = false; }
    bool onTime() {
        if(on) {
            DWORD t2 = clock();
            if(t2 - t > interval) {
                t = t2;
                return true;
            }
        }
        return false;
    }
    DWORD now() {
        // 返回持续时间
        if(on) {
            return clock() - t;
        }
        return 0;
    }

    // 保存所有的定时器
    static std::map<int, Timer*> timerMap;

    Timer(const int ID, const DWORD interval): t(0), ID(ID), interval(interval), on(false) {}

    DWORD t;
    const DWORD interval;
    const int ID;
    bool on;
};

// 数据
struct Data {
    static Data* readSrc(std::fstream& fp);     // 特殊资源映射
    static Data* readObject(std::fstream& fp);  // 读取物体，返回数据指针
    static void readFramework(std::fstream& fp, std::vector<Data*>& dataList);
    static void readNote(std::fstream& fp);  // 读取注释

    // 读取数据，返回数据指针
    static void read(LPCSTR name, std::map<std::string, Data*>& dataMap);
    static void read(LPCSTR name, std::map<std::string, Data*>& dataMap, Mode mode);
    static std::string freeStr();  // 产生随机字符串

    std::map<std::string, double> singles;       // 单变量
    std::map<std::string, POINT> points;         // 点变量
    std::map<std::string, std::string> details;  // 描述
};

struct Event {
    static std::map<int, Event*> eventMap;  // 所有事件映射，通过 id 访问

    static void read(LPCSTR name);              // 读取所有事件
    static Event* readEvent(std::fstream& fp);  // 读取 {} 中的事件
    static void deleteEvent();                  // 删除所有事件

    // 事件同样不需要保存 id，其 id 通过 eventMap 的键值隐式地使用
    std::vector<std::string>::iterator eIt;  // 保存指令迭代器，当为 commandList.end 则结束
    std::vector<std::string> commandList;    // 指令列表
    std::vector<int> intList;                // 数值表
};