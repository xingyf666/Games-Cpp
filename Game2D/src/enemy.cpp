
#include "enemy.h"

Enemy* Enemy::createEnemy(std::string name, POINT pos, POINT moveRange) {
    Enemy* enemy = new Enemy(name, pos, moveRange);

    enemy->speed = dataMap[name]->points["speed"];
    enemy->HP = dataMap[name]->singles["HP"];
    enemy->skillRange = dataMap[name]->singles["range"];

    return enemy;
}

void Enemy::draw(POINT origin) {
    if(!alive) {
        AA *= 0.8;
    }

    POINT size = getPoint("size");
    POINT picPos = {(status % 10) / 3 * size.x, (status / 10) * size.y};
    drawAlpha(MImage::imageMap["Enemy"], coord.x - origin.x, coord.y - origin.y, (int)size.x, (int)size.y, (int)picPos.x, (int)picPos.y, AA);

    // 显示武器
    if(skilling) {
        Data* eData = dataMap[name];
        POINT actPicPos;
        int dx = eData->singles["dx"];

        // 向右
        if(speed.x > 0) {
            actPicPos = eData->points["actPos2"];
        }
        // 向左
        else {
            actPicPos = eData->points["actPos1"];
            dx = -dx;
        }

        POINT sSize = eData->points["actSize"];
        drawAlpha(MImage::imageMap["Enemy"], coord.x - origin.x + dx, coord.y - origin.y, (int)sSize.x, (int)sSize.y, (int)actPicPos.x, (int)actPicPos.y, AA);

        // 定时器判断
        if(timer->onTime()) {
            skilling = false;
            delete timer;
            timer = nullptr;

            // 速度回复
            if(speed.x > 0) {
                speed.x = eData->points["speed"].x;
            } else {
                speed.x = -eData->points["speed"].x;
            }
        }
    }
}

void Enemy::cut(int ID) {
    // 如果没有砍击，就砍击
    if(!skilling) {
        skilling = true;

        Data* eData = dataMap[name];
        // 提供速度加成
        if(speed.x > 0) {
            speed.x = eData->points["speed"].x + eData->singles["speedBuf"];
        } else {
            speed.x = -eData->points["speed"].x - eData->singles["speedBuf"];
        }
        // 创建计时器，用物体本身的 ID 创建
        timer = new Timer(ID, eData->singles["bufTime"]);
        timer->start();
    }
}