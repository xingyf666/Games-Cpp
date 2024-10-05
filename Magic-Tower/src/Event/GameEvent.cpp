#include "GameEvent.h"

#include "GameWindow.h"


int GameEvent::s_key = -1;
QTextEdit* GameEvent::s_edit = nullptr;
QFrame* GameEvent::s_frame = nullptr;
QFrame* GameEvent::s_bkFrame = nullptr;
QLabel* GameEvent::s_label = nullptr;
GameEvent* GameEvent::s_event = nullptr;
GameMusic GameEvent::s_gainSound;
GameMusic GameEvent::s_openSound;
GameMusic GameEvent::s_eventSound;
GameMusic GameEvent::s_fightSound;
GameMusic GameEvent::s_buffSound;
GameMusic GameEvent::s_failSound;

void GameEvent::initEvent() {
    // 导入音效
    s_gainSound.load(QDir::cleanPath(TEST_DATA_PATH "/sound/gain.wav"));
    s_openSound.load(QDir::cleanPath(TEST_DATA_PATH "/sound/open.wav"));
    s_eventSound.load(QDir::cleanPath(TEST_DATA_PATH "/sound/event.wav"));
    s_fightSound.load(QDir::cleanPath(TEST_DATA_PATH "/sound/fight.wav"));
    s_buffSound.load(QDir::cleanPath(TEST_DATA_PATH "/sound/debuff.mp3"));
    s_failSound.load(QDir::cleanPath(TEST_DATA_PATH "/sound/fail.wav"));
}

void GameEvent::doEvent() {
    // 如果当按键为空格，Y,N 等就发起事件
    if(s_event != nullptr && (s_key == Qt::Key_Space || s_key == Qt::Key_Y || s_key == Qt::Key_N)) s_event->start();

    // 依次执行每一步
    if(s_event != nullptr) s_event->step();
}

GameEvent::GameEvent(nlohmann::json data): m_active(false), m_buy(false) {
    // 初始化索引
    init();

    // 根据 data 中保存的命令类型进行不同的数据读取
    for(int i = 0; i < int(data.size()); i++) {
        // 记录命令
        nlohmann::json event = data[i];
        std::string command = event["command"];
        m_stepList.push_back(command);

        // 记录数据
        if(command == "talk" || command == "question")
            m_talkList.push_back(event["detail"]);
        else if(command == "bk")
            m_bkList.push_back(event["val"]);
        else if(command == "sound")
            m_soundList.push_back(event["path"]);
        else if(command == "delay")
            m_delayList.push_back(event["time"]);
        else if(command == "create") {
            GameStruct gameStr;
            gameStr.f = event["pos"]["f"];
            gameStr.i = event["pos"]["i"];
            gameStr.j = event["pos"]["j"];
            gameStr.id = event["id"];
            m_createList.push_back(gameStr);
        } else if(command == "delete") {
            GameStruct gameStr;
            gameStr.f = event["pos"]["f"];
            gameStr.i = event["pos"]["i"];
            gameStr.j = event["pos"]["j"];
            m_deleteList.push_back(gameStr);
        } else if(command == "test") {
            m_testList.push_back(QList<GameStruct>());
            for(int j = 0; j < int(event["pos"].size()); j++) {
                GameStruct gameStr;
                gameStr.f = event["pos"][j]["f"];
                gameStr.i = event["pos"][j]["i"];
                gameStr.j = event["pos"][j]["j"];
                m_testList.back().push_back(gameStr);
            }
        } else if(command == "open") {
            m_openList.push_back(QList<GameStruct>());
            for(int j = 0; j < int(event["pos"].size()); j++) {
                GameStruct gameStr;
                gameStr.f = event["pos"][j]["f"];
                gameStr.i = event["pos"][j]["i"];
                gameStr.j = event["pos"][j]["j"];
                m_openList.back().push_back(gameStr);
            }
        } else if(command == "buff") {
            // 保存属性增益
            m_buffList.push_back(QMap<std::string, int>());
            for(int j = 0; j < int(event["attributes"].size()); j++) {
                std::string name = event["attributes"][j]["name"];
                int val = event["attributes"][j]["val"];
                m_buffList.back()[name] = val;
            }
        } else if(command == "transport") {
            GameStruct gameStr;
            gameStr.f = event["pos"]["f"];
            gameStr.i = event["pos"]["i"];
            gameStr.j = event["pos"]["j"];
            m_transportList.push_back(gameStr);
        } else if(command == "move") {
            GameStruct gameStr;
            gameStr.f = event["pos"]["f"];
            gameStr.i = event["pos"]["i"];
            gameStr.j = event["pos"]["j"];
            gameStr.di = event["di"];
            gameStr.dj = event["dj"];
            m_moveList.push_back(gameStr);
        }
    }
}

void GameEvent::init() {
    m_ind["step"] = 0;
    m_ind["talk"] = 0;
    m_ind["sound"] = 0;
    m_ind["move"] = 0;
    m_ind["create"] = 0;
    m_ind["delete"] = 0;
    m_ind["open"] = 0;
    m_ind["test"] = 0;
    m_ind["buff"] = 0;
    m_ind["bk"] = 0;
    m_ind["delay"] = 0;
    m_ind["transport"] = 0;
}

void GameEvent::start() {
    m_active = true;

    // 事件开启，锁定 hero，减小背景音量
    GameHero::shareHero()["status"] = GameHero::fixed;
    GameMap::s_bgm.setVolume(200);
}

void GameEvent::quit() {
    s_event = nullptr;
    GameHero::shareHero()["status"] = GameHero::moving;
    GameMap::s_bgm.setVolume(500);
    m_active = false;
    m_buy = false;
}

bool GameEvent::buy() {
    // 如果没有处于购买状态，直接返回
    if(!m_buy) return true;
    // 如果处于购买状态，检查按键消息

    // 只有为 N 时表示拒绝，立即退出
    if(s_key == Qt::Key_N) {
        quit();
        s_frame->hide();
        return false;
    }
    // 如果为 Y 或空格，就测试是否买得起
    else if(s_key == Qt::Key_Space || s_key == Qt::Key_Y) {
        // 获得 buff
        QMap<std::string, int>& bList = m_buffList[m_ind["buff"]];
        GameHero& hero = GameHero::shareHero();

        // 如果买不起，就退出
        for(auto key = bList.keyBegin(); key != bList.keyEnd(); ++key) {
            if(hero[*key] + bList[*key] < 0) {
                s_failSound.play(false);
                return false;
            }
        }
        // 到这里说明买得起，就让命令向后移动
        m_ind["talk"]++;
        m_ind["step"]++;

        // 退出购买状态
        m_buy = false;
    }
    return true;
}

bool GameEvent::finish() {
    // 命令一直循环，如果执行完成，就初始化，然后退出
    if(m_ind["step"] == m_stepList.size()) {
        init();
        quit();
        s_frame->hide();

        // 开启循环
        return true;
    }
    return false;
}

void GameEvent::step() {
    // 活跃/计时器超时时执行
    if(m_active && m_timer.timeout() && !finish() && buy()) {
        // 获得当前命令
        std::string command = m_stepList[m_ind["step"]];
        if(command == "bk") {
            // 填充背景色
            if(m_bkList[m_ind["bk"]] == 1)
                s_bkFrame->setStyleSheet("background-color:rgb(0,0,0);");
            else
                s_bkFrame->setStyleSheet("background-color:rgba(0,0,0,0);");
            m_ind["bk"]++;
        } else if(command == "talk") {
            // 展示对话框
            std::string detail = m_talkList[m_ind["talk"]];
            s_edit->setText(QString::fromStdString(detail));
            s_label->setText("按空格键继续.");
            s_frame->show();
            m_ind["talk"]++;

            // 展示对话框后暂停
            m_active = false;
        } else if(command == "question") {
            // 展示对话框
            std::string detail = m_talkList[m_ind["talk"]];
            s_edit->setText(QString::fromStdString(detail));
            s_label->setText("按 Y/N (Yes)/(No) 选择");
            s_frame->show();

            // 进入购买状态
            m_buy = true;

            // 展示对话框后暂停，然后就直接退出，使得命令一直停止
            m_active = false;
            return;
        } else {
            // 隐藏窗口
            s_frame->hide();
            if(command == "quit") quit();
            if(command == "delay") {
                m_timer.setInterval(m_delayList[m_ind["delay"]]);
                m_timer.setSingle(true);
                m_timer.start();
                m_ind["delay"]++;
            } else if(command == "sound") {
                QString name = QString::fromStdString(m_soundList[m_ind["sound"]]);
                GameMusic music(QDir::cleanPath(TEST_DATA_PATH + name));
                music.play(false);
                m_ind["sound"]++;
            } else if(command == "delete") {
                GameStruct gStr = m_deleteList[m_ind["delete"]];
                GameMap& map = GameMap::shareMap(gStr.f);
                map.deleteObj(gStr.i, gStr.j);
                m_ind["delete"]++;
            }
            // 注意由于创建会直接覆盖，因此最好先删除对应位置之前的物体
            else if(command == "create") {
                GameStruct gStr = m_createList[m_ind["create"]];
                GameMap& map = GameMap::shareMap(gStr.f);
                map.createObj(gStr.i, gStr.j, gStr.id);
                m_ind["create"]++;
            } else if(command == "test") {
                QList<GameStruct>& gList = m_testList[m_ind["test"]];

                // 检查列表指定的位置是否有物体，如果有则 pass 失败
                bool fail = false;
                for(int i = 0; i < gList.size(); i++) {
                    GameStruct gStr = gList[i];
                    GameMap& map = GameMap::shareMap(gStr.f);
                    if(map.shareObj(gStr.i, gStr.j) != nullptr) {
                        fail = true;
                        break;
                    }
                }
                m_ind["test"]++;

                // 如果失败，跳过命令直到遇到 end
                while(fail && m_stepList[m_ind["step"]] != "end") m_ind["step"]++;
            } else if(command == "open") {
                QList<GameStruct>& gList = m_openList[m_ind["open"]];

                // 我们这里先让 hero 原地动一下，防止 moveBack 返回
                GameHero& hero = GameHero::shareHero();
                hero["status"] = GameHero::moving;
                hero.move();
                hero["status"] = GameHero::fixed;

                // 打开指定位置的门
                for(int i = 0; i < gList.size(); i++) {
                    GameStruct gStr = gList[i];
                    GameObject* obj = GameMap::shareMap(gStr.f).shareObj(gStr.i, gStr.j);

                    // 先将 obj 解锁，然后碰撞来开启
                    (*obj)["locked"] = 0;
                    if(obj != nullptr) obj->collision();
                }
                m_ind["open"]++;
            } else if(command == "buff") {
                QMap<std::string, int>& bList = m_buffList[m_ind["buff"]];

                // 遍历 key 获得增益
                GameHero& hero = GameHero::shareHero();
                for(auto key = bList.keyBegin(); key != bList.keyEnd(); ++key) hero[*key] += bList[*key];

                // 播放音效
                s_buffSound.play(false);
                m_ind["buff"]++;
            } else if(command == "transport") {
                // 将 hero 传送到指定位置
                GameStruct gStr = m_transportList[m_ind["transport"]];
                GameHero& hero = GameHero::shareHero();
                hero["floor"] = gStr.f;
                hero["i"] = gStr.i;
                hero["j"] = gStr.j;

                m_ind["transport"]++;
            } else if(command == "move") {
                // 移动间隔
                m_timer.setInterval(200);
                m_timer.setSingle(true);
                m_timer.start();

                GameStruct gStr = m_moveList[m_ind["move"]];
                GameMap& map = GameMap::shareMap(gStr.f);
                GameObject* obj = map.shareObj(gStr.i, gStr.j);

                // 如果移动的位置有物体，就暂时不移动，防止覆盖操作
                if(map.shareObj(gStr.i + gStr.di, gStr.j + gStr.dj) != nullptr) return;

                // 现在进行移动操作
                map.setObj(gStr.i + gStr.di, gStr.j + gStr.dj, obj);
                map.setObj(gStr.i, gStr.j, nullptr);
                (*obj)["i"] = gStr.i + gStr.di;
                (*obj)["j"] = gStr.j + gStr.dj;
                m_ind["move"]++;
            }
        }
        m_ind["step"]++;
    }
}
