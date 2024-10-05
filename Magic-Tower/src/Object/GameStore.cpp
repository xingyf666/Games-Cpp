#include "GameStore.h"

GameStore::GameStore(QPoint pos, QRect rect): GameObject(pos, rect) {
    m_attributes["act"] = 0;

    // 商店会自动生成事件
}

void GameStore::animate() {
    // 一般人物有两张不同图像来产生动作
    m_attributes["act"] = (m_attributes["act"] + 1) % 2;
}

void GameStore::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect.translated(0, m_attributes["act"] * 33));
}

void GameStore::collision() {
    GameHero& hero = GameHero::shareHero();
    hero.moveBack();
}
