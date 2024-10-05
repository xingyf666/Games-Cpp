#include "GameWall.h"

GameWall::GameWall(QPoint pos, QRect rect): GameObject(pos, rect) {
}

void GameWall::animate() {
}

void GameWall::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect);
}

void GameWall::collision() {
    GameHero& hero = GameHero::shareHero();
    hero.moveBack();
}
