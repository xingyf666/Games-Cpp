#include "GameGround.h"

GameGround::GameGround(QPoint pos, GameEvent* event): GameObject(pos, QRect(0, 0, 32, 32)) {
    m_event = event;
}

void GameGround::animate() {
}

void GameGround::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect);
}

void GameGround::collision() {
    if(m_event != nullptr) {
        GameEvent::s_event = m_event;
        m_event->start();
    }

    // 碰撞后立即销毁
    m_attributes["status"] = GameObject::dead;
}
