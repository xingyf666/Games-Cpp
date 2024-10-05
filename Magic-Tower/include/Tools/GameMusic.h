#ifndef GAMEMUSIC_H
#define GAMEMUSIC_H

#include <windows.h>

// 注意此库需要放置在 windows.h 之后
#include <mmsystem.h>

#include <QString>


#pragma comment(lib, "winmm.lib")  // 加载静态库

class GameMusic {
  public:
    GameMusic();
    GameMusic(QString name);

    void load(QString name);  // 加载音乐文件
    void init();              // 初始化
    void open();              // 打开
    void play(bool);          // 播放，可选是否循环
    void pause();             // 暂停
    void resume();            // 继续
    void stop();              // 停止
    void close();             // 关闭
    void setVolume(int);      // 设置音量

  private:
    QString m_filename;
};

#endif  // GAMEMUSIC_H
