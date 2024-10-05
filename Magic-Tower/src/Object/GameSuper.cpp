#include "GameSuper.h"

GameSuper::GameSuper(QPoint pos, QRect rect): GameObject(pos, rect) {
}

void GameSuper::animate() {
}

void GameSuper::draw(QMainWindow* window) {
    QPainter painter(window);
    QRect rect(m_attributes["j"] * 40, m_attributes["i"] * 40, 40, 40);
    painter.drawImage(rect, s_objImg, m_rect);
}

void GameSuper::collision() {
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

    // 裁剪对应位置的图片进行拉伸，转换为 icon 保存
    QImage subimage = s_objImg.copy(m_rect).scaled(40, 40);
    QIcon icon(QPixmap::fromImage(subimage));

    QListWidgetItem* imageItem = new QListWidgetItem;
    imageItem->setIcon(icon);
    imageItem->setSizeHint(QSize(40, 40));
    imageItem->setBackground(QBrush(QColor(58, 58, 58)));
    s_toolList->addItem(imageItem);
}
