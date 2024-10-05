#include "GameDoor.h"

GameDoor::GameDoor(QPoint pos, QRect rect): GameObject(pos, rect) {
    m_timer.setInterval(50);
    m_attributes["act"] = 0;
    m_attributes["open"] = false;
}

void GameDoor::animate() {
}

void GameDoor::draw(QMainWindow* window) {
    // 门的显示要利用 doorImg
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect.translated(0, m_attributes["act"] * 33));

    // 开门动画
    if(m_attributes["open"] && m_timer.timeout()) m_attributes["act"]++;

    // 门完全开启就销毁，然后终止计时器
    if(m_attributes["act"] == 4) {
        m_attributes["status"] = GameObject::dead;
        m_timer.stop();
    }
}

void GameDoor::collision() {
    // 返回原地，触发开门
    GameHero& hero = GameHero::shareHero();
    hero.moveBack();

    // 如果没有开门，才会进行变化
    if(!m_attributes["open"]) {
        // 提供属性变化，如果属性不足以变化或者这道门是机关门 locked，就直接返回
        for(int i = 0; i < m_attrList.size(); i++) {
            std::string name = m_attrList[i];
            if(m_attributes["locked"] || hero[name] + m_attributes[name] < 0) return;
            hero[name] += m_attributes[name];
        }

        // 设置状态为开门，同时开启开门计时器
        m_attributes["open"] = true;
        m_timer.start();

        // 开门音效
        GameEvent::s_openSound.init();
        GameEvent::s_openSound.play(false);
    }
}
