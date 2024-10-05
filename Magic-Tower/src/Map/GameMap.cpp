#include "GameMap.h"

#include "Object/GameDoor.h"
#include "Object/GameEnemy.h"
#include "Object/GameGround.h"
#include "Object/GameNPC.h"
#include "Object/GameStair.h"
#include "Object/GameStore.h"
#include "Object/GameSuper.h"
#include "Object/GameTool.h"
#include "Object/GameWall.h"


QList<GameMap*> GameMap::s_map;
GameMusic GameMap::s_bgm;

void GameMap::initMap() {
    QString fname = QDir::cleanPath(TEST_DATA_PATH "/basic/map.json");
    std::fstream fp(fname.toStdString(), std::ios::in);
    nlohmann::json data;
    fp >> data;
    fp.close();

    // 循环播放音乐
    s_bgm.load(QDir::cleanPath(TEST_DATA_PATH "/sound/magicTower.mp3"));
    s_bgm.play(true);

    // 读取所有地图
    for(int i = 0; i < int(data.size()); i++) {
        QString str;
        str.setNum(i);
        str = "floor_" + str;
        s_map.push_back(new GameMap(i, data[str.toStdString()]));
    }
}

GameMap& GameMap::shareMap(int floor) {
    return *s_map[floor];
}

void GameMap::deleteMap() {
    for(int i = 0; i < s_map.size(); i++) delete s_map[i];
}

GameMap::GameMap(int floor, nlohmann::json json): m_floor(floor) {
    // 根据 json 数据进行初始化
    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < 13; j++) {
            // 获得此位置的事件
            GameEvent* event = nullptr;
            QString id = QString::asprintf("%d_%d", i, j);
            if(!json["event"][id.toStdString()].is_null()) event = new GameEvent(json["event"][id.toStdString()]);

            // 创建物体，如果非空则尝试绑定事件
            createObj(i, j, json["map"][i][j]);
            if(m_obj[i][j] != nullptr) m_obj[i][j]->bind(event);
            // 如果这里事件不是空的，就创建一块平地
            else if(event != nullptr)
                m_obj[i][j] = new GameGround(QPoint(i, j), event);
        }
    }
}

GameObject* GameMap::createObj(int i, int j, int id) {
    // 根据 id 获得对应的索引字符串
    QString t;
    t.setNum(id);
    t = "id_" + t;

    // 通过 s_sourceData 获得 id 对应的类型
    nlohmann::json data = GameObject::s_sourceData[t.toStdString()];
    std::string type = data["class"];

    // 获得对应源图像中的矩形
    int si = data["src"]["i"];
    int sj = data["src"]["j"];
    QRect rect(sj * 33, si * 33, 32, 32);
    QPoint pos(i, j);

    GameObject* obj = nullptr;
    if(type == "ground")
        obj = nullptr;
    else if(type == "wall")
        obj = new GameWall(pos, rect);
    else if(type == "door")
        obj = new GameDoor(pos, rect);
    else if(type == "enemy")
        obj = new GameEnemy(pos, rect);
    else if(type == "npc")
        obj = new GameNPC(pos, rect);
    else if(type == "stair")
        obj = new GameStair(pos, rect);
    else if(type == "tool")
        obj = new GameTool(pos, rect);
    else if(type == "superTool")
        obj = new GameSuper(pos, rect);
    else if(type == "store")
        obj = new GameStore(pos, rect);

    // 保存创建的指针
    m_obj[i][j] = obj;

    nlohmann::json attributes = data["attributes"];
    // 对于怪物，直接获得属性
    if(type == "enemy") {
        (*obj)["HP"] = attributes["HP"];
        (*obj)["attack"] = attributes["attack"];
        (*obj)["defense"] = attributes["defense"];
        (*obj)["money"] = attributes["money"];
    }
    // 如果属性非空，获得属性
    else if(!attributes.is_null()) {
        for(int i = 0; i < int(attributes.size()); i++) {
            std::string name = attributes[i]["name"];
            obj->push(name);
            (*obj)[name] = attributes[i]["val"];
        }
    }

    return obj;
}

GameObject* GameMap::shareObj(int i, int j) {
    return m_obj[i][j];
}

void GameMap::setObj(int i, int j, GameObject* obj) {
    m_obj[i][j] = obj;
}

void GameMap::deleteObj(int i, int j) {
    // 注意即使有删除方法也不能在碰撞时删除，因为碰撞函数在物体类内执行，没法删除
    if(m_obj[i][j] != nullptr) delete m_obj[i][j];
    m_obj[i][j] = nullptr;
}

void GameMap::draw(QMainWindow* window) {
    // 显示的同时进行清理
    for(int i = 0; i < 13; i++) {
        for(int j = 0; j < 13; j++) {
            GameObject* obj = m_obj[i][j];
            if(m_obj[i][j] != nullptr && (*m_obj[i][j])["status"] == GameObject::active)
                obj->draw(window);
            else {
                delete m_obj[i][j];
                m_obj[i][j] = nullptr;
            }
        }
    }
}

void GameMap::test() {
    GameHero& hero = GameHero::shareHero();
    int i = hero["i"];
    int j = hero["j"];
    if(m_obj[i][j] != nullptr) m_obj[i][j]->collision();
}

void GameMap::animate() {
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++)
            if(m_obj[i][j] != nullptr) m_obj[i][j]->animate();
}

GameMap::~GameMap() {
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++)
            if(m_obj[i][j] != nullptr) delete m_obj[i][j];
    for(auto it = m_event.begin(); it != m_event.end(); ++it) delete *it;
}
