#include "GameTool.h"

GameTool::GameTool(QPoint pos, QRect rect): GameObject(pos, rect) {
}

void GameTool::animate() {
}

void GameTool::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect);
}

void GameTool::collision() {
    // 提供属性变化
    GameHero& hero = GameHero::shareHero();
    for(int i = 0; i < m_attrList.size(); i++) {
        std::string name = m_attrList[i];
        hero[name] += m_attributes[name];
    }

    // 获得道具音效
    GameEvent::s_gainSound.init();
    GameEvent::s_gainSound.play(false);

    // 销毁此道具
    m_attributes["status"] = GameObject::dead;
}
