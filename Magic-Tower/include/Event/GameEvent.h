#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QMap>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QTextEdit>
#include <QTimer>
#include <fstream>

#include "GameMusic.h"
#include "GameTimer.h"
#include "json.hpp"


class GameEvent {
  public:
    bool m_active;  // 事件是否活跃
    bool m_buy;     // 是否处于购买状态

  protected:
    // 创建操作的结构
    struct GameStruct {
        int f, i, j, id;
        int di, dj;
    };

    GameTimer m_timer;                         // 定时器
    QMap<std::string, int> m_ind;              // 记录每个列表的执行位置
    QList<std::string> m_stepList;             // 按顺序记录命令名称
    QList<std::string> m_talkList;             // 记录对话内容
    QList<std::string> m_soundList;            // 播放音效
    QList<GameStruct> m_moveList;              // 移动操作
    QList<GameStruct> m_createList;            // 创建操作
    QList<GameStruct> m_deleteList;            // 删除操作
    QList<GameStruct> m_transportList;         // 传送操作
    QList<QList<GameStruct>> m_openList;       // 开门操作
    QList<QList<GameStruct>> m_testList;       // 检查指定位置
    QList<QMap<std::string, int>> m_buffList;  // 记录属性增益
    QList<int> m_bkList;                       // 背景色列表
    QList<int> m_delayList;                    // 延时列表

  public:
    GameEvent(nlohmann::json data);

    void init();    // 初始化命令
    void start();   // 开始命令
    void quit();    // 退出命令
    void step();    // 执行一步命令
    bool buy();     // 处理购买
    bool finish();  // 返回是否完成事件

  public:
    static void initEvent();
    static void doEvent();

    // 用于控制窗口控件
    static QTextEdit* s_edit;
    static QFrame* s_frame;
    static QFrame* s_bkFrame;
    static QLabel* s_label;
    static int s_key;

    static GameEvent* s_event;      // 将要执行的事件
    static GameMusic s_gainSound;   // 获得道具的音效
    static GameMusic s_fightSound;  // 战斗音效
    static GameMusic s_eventSound;  // 事件音效
    static GameMusic s_openSound;   // 开门音效
    static GameMusic s_buffSound;   // 负面音效
    static GameMusic s_failSound;   // 购买失败音效
};

#endif  // GAMEEVENT_H
