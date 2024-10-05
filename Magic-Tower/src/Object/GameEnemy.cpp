#include "GameEnemy.h"

GameEnemy::GameEnemy(QPoint pos, QRect rect): GameObject(pos, rect) {
    m_timer.setInterval(200);
    m_attributes["act"] = 0;
}

void GameEnemy::animate() {
    // 一般人物有两张不同图像来产生动作
    m_attributes["act"] = (m_attributes["act"] + 1) % 2;
}

void GameEnemy::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect.translated(0, m_attributes["act"] * 33));
}

void GameEnemy::collision() {
    // 计算战斗结果
    GameHero& hero = GameHero::shareHero();
    int result = hero.fightResult(m_attributes["HP"], m_attributes["attack"], m_attributes["defense"]);

    // 如果打不过，不能战斗，返回原位
    if(result == -1) {
        hero.moveBack();
        return;
    }

    // 如果计时器关闭，则开启
    if(!m_timer.active()) {
        m_timer.start();
        hero["status"] = GameHero::fixed;
    }

    // 到一半时间则刷新英雄显示，实现闪烁
    if(m_timer.timeout(0.5))
        hero["status"] = GameHero::fighting;
    else
        hero["status"] = GameHero::fixed;

    // 到时间战斗一次
    if(m_timer.timeout()) {
        GameEvent::s_fightSound.play(false);

        // 血量减少等于攻击减防御
        int dHP = m_attributes["attack"] - hero["defense"];
        m_attributes["HP"] -= hero["attack"] - m_attributes["defense"];

        // 如果怪物先死了，自己不用掉血
        if(m_attributes["HP"] <= 0) {
            // 移除怪物，解除锁定，关闭计时器
            m_attributes["status"] = GameObject::dead;
            hero["status"] = GameHero::moving;
            m_timer.stop();

            // 属性变动
            hero["money"] += m_attributes["money"];

            // 如果有事件，此时触发事件
            if(m_event != nullptr) {
                GameEvent::s_event = m_event;
                m_event->start();
            }
            return;
        }

        // 怪物没死，自己掉血
        if(dHP > 0) hero["HP"] -= dHP;
    }
}
