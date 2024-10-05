#include "GameHero.h"

GameHero* GameHero::s_hero = nullptr;
QImage GameHero::s_heroImg;
QImage GameHero::s_fightImg;

void GameHero::initHero() {
    // 导入数据，然后创建英雄
    s_fightImg.load(QDir::cleanPath(TEST_DATA_PATH "/image/fight.png"));
    s_heroImg.load(QDir::cleanPath(TEST_DATA_PATH "/image/hero.png"));

    QString fname = QDir::cleanPath(TEST_DATA_PATH "/basic/hero.json");
    std::fstream fp(fname.toStdString(), std::ios::in);
    nlohmann::json data;
    fp >> data;
    fp.close();

    GameHero& hero = shareHero();
    hero["HP"] = data["attributes"]["HP"];
    hero["attack"] = data["attributes"]["attack"];
    hero["defense"] = data["attributes"]["defense"];
    hero["money"] = data["attributes"]["money"];
    hero["yellow"] = data["attributes"]["yellow"];
    hero["blue"] = data["attributes"]["blue"];
    hero["red"] = data["attributes"]["red"];
    hero["i"] = data["pos"]["i"];
    hero["j"] = data["pos"]["j"];
    hero["status"] = GameHero::moving;

    // 到达的楼层信息
    hero["floor"] = data["pos"]["f"];
    hero["maxFloor"] = data["pos"]["mf"];

    // 位置和源图像信息
    int w = data["pos"]["srcX"];
    int h = data["pos"]["srcY"];
    QRect rect(w * 32, h * 32, 32, 32);
    hero.m_back = QPoint(hero["i"], hero["j"]);
    hero.m_rect = rect;
}

GameHero& GameHero::shareHero() {
    if(s_hero == nullptr) s_hero = new GameHero;
    return *s_hero;
}

void GameHero::deleteHero() {
    if(s_hero != nullptr) delete s_hero;
}

void GameHero::move() {
    // 处于 moving 状态时才能移动
    int direct = GameEvent::s_key;
    if(m_attributes["status"] == GameHero::moving) {
        // 记录当前位置
        m_back = QPoint(m_attributes["i"], m_attributes["j"]);

        // 移动的过程中切换图像，这里 move 函数不会改变矩形的尺寸
        if(direct == Qt::Key_Up) {
            m_attributes["i"] -= 1;
            m_rect.moveTop(3 * 32);
            m_rect.translate(32, 0);
        } else if(direct == Qt::Key_Down) {
            m_attributes["i"] += 1;
            m_rect.moveTop(0 * 32);
            m_rect.translate(32, 0);
        } else if(direct == Qt::Key_Left) {
            m_attributes["j"] -= 1;
            m_rect.moveTop(1 * 32);
            m_rect.translate(32, 0);
        } else if(direct == Qt::Key_Right) {
            m_attributes["j"] += 1;
            m_rect.moveTop(2 * 32);
            m_rect.translate(32, 0);
        }

        // 防止越界
        if(m_rect.x() >= 4 * 32) m_rect.moveLeft(0);
    }
}

void GameHero::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);

    if(m_attributes["status"] == fighting)
        painter.drawImage(rect, s_fightImg, QRect(0, 0, 32, 32));
    else
        painter.drawImage(rect, s_heroImg, m_rect);
}

void GameHero::moveBack() {
    m_attributes["i"] = m_back.x();
    m_attributes["j"] = m_back.y();
}

int GameHero::fightResult(int hp, int attack, int defense) {
    // 攻击力不够，直接返回
    if(m_attributes["attack"] <= defense) return -1;
    // 防御力高于怪物攻击力，无损失
    else if(attack <= m_attributes["defense"])
        return 0;
    else {
        int tHP = m_attributes["HP"];
        while(tHP > 0) {
            hp -= m_attributes["attack"] - defense;

            // 如果此攻击杀死怪物，就不会反击
            if(hp <= 0) break;
            tHP -= attack - m_attributes["defense"];
        }

        // 计算最终结果
        if(tHP > 0) return m_attributes["HP"] - tHP;
        // 血量归零，则不可攻击
        else
            return -1;
    }
    return 0;
}

int GameHero::operator[](std::string name) const {
    return m_attributes[name];
}

int& GameHero::operator[](std::string name) {
    return m_attributes[name];
}
