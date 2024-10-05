#include "GameStair.h"

GameStair::GameStair(QPoint pos, QRect rect): GameObject(pos, rect) {
}

void GameStair::animate() {
}

void GameStair::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect);
}

void GameStair::collision() {
    // 提供属性变化
    GameHero& hero = GameHero::shareHero();
    for(int i = 0; i < m_attrList.size(); i++) {
        std::string name = m_attrList[i];
        hero[name] += m_attributes[name];
    }
}
