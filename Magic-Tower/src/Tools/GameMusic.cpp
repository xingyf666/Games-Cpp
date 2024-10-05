#include "GameMusic.h"

GameMusic::GameMusic(): m_filename("") {
}

GameMusic::GameMusic(QString name): m_filename(name) {
}

// 打开音乐文件
void GameMusic::load(QString name) {
    m_filename = name;
}

// 回到开头
void GameMusic::init() {
    QString command = "seek " + m_filename + " to start";
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}

// 打开音乐（支持多种音乐格式）
void GameMusic::open() {
    QString command = "open " + m_filename;
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}

// 音量
void GameMusic::setVolume(int volume) {
    QString v;
    v.setNum(volume);

    QString command = "setaudio " + m_filename + " volume to " + v;
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}

// 播放
void GameMusic::play(bool repeat) {
    QString command = "play " + m_filename;
    if(repeat) command += " repeat";
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}

// 暂停
void GameMusic::pause() {
    QString command = "pause " + m_filename;
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}

// 继续
void GameMusic::resume() {
    QString command = "resume " + m_filename;
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}

// 结束
void GameMusic::stop() {
    QString command = "stop " + m_filename;
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}

// 关闭
void GameMusic::close() {
    QString command = "close " + m_filename;
    mciSendString(command.toStdWString().c_str(), 0, 0, 0);
}
