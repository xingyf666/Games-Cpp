#ifndef GAMETIMER_H
#define GAMETIMER_H

#include <ctime>

class GameTimer {
  public:
    GameTimer();

    void start();                    // 开启计时
    void stop();                     // 终止计时
    void setInterval(int interval);  // 设置区间
    void setSingle(bool single);     // 设置是否只触发一次
    bool timeout(double ratio);      // 检查是否达到一定比例
    bool timeout();                  // 检查是否超时，如果超时则重置
    bool active();                   // 检查是否开启

  protected:
    int m_start;
    int m_interval;
    bool m_single;
    bool m_active;
};

#endif  // GAMETIMER_H
