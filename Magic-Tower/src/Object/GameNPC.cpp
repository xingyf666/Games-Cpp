#include "GameNPC.h"

GameNPC::GameNPC(QPoint pos, QRect rect): GameObject(pos, rect) {
    m_attributes["act"] = 0;
}

void GameNPC::animate() {
    // 一般人物有两张不同图像来产生动作
    m_attributes["act"] = (m_attributes["act"] + 1) % 2;
}

void GameNPC::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect.translated(0, m_attributes["act"] * 33));
}

void GameNPC::collision() {
    GameHero& hero = GameHero::shareHero();
    hero.moveBack();

    // NPC 会添加事件
    if(m_event != nullptr) {
        GameEvent::s_event = m_event;
        m_event->start();
    }
}
