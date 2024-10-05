#include "GameTimer.h"

GameTimer::GameTimer(): m_start(0), m_interval(0), m_single(false), m_active(false) {
}

void GameTimer::start() {
    m_start = clock();
    m_active = true;
}

void GameTimer::stop() {
    m_start = 0;
    m_active = false;
}

void GameTimer::setInterval(int interval) {
    m_interval = interval;
}

void GameTimer::setSingle(bool single) {
    m_single = single;
}

bool GameTimer::timeout(double ratio) {
    if(!m_active || clock() - m_start >= m_interval * ratio) return true;
    return false;
}

bool GameTimer::timeout() {
    // 计时器关闭默认超时
    if(!m_active || clock() - m_start >= m_interval) {
        m_start = clock();
        // 如果设定只触发一次，就直接停止
        if(m_single) stop();
        return true;
    }
    return false;
}

bool GameTimer::active() {
    return m_active;
}
