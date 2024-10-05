
#include "Man.h"

// 静态本地变量初始化

Man* Man::man = nullptr;

// 创建和删除
Man* Man::createMan(Data* data) {
    if(man == nullptr) {
        // 创建人物，初始化属性
        man = new Man;
        man->speed = data->points["speed"];
        man->HP = data->singles["HP"];
        man->attack = data->singles["attack"];
        man->jumpHeight = data->singles["jump"];
        man->coord = data->points["pos"];

        // 修正位置
        man->coord.x *= StdWidth;
        man->coord.y *= StdHeight;

        man->alivePos = man->coord;
        man->status = 20;
    }
    return man;
}

bool Man::deleteMan() {
    if(man == nullptr) {
        return false;
    }
    if(man->timer != nullptr) {
        delete man->timer;
    }
    delete man;
    return true;
}

void Man::jump() {
    if(speed.y == 0) {
        speed.y = -jumpHeight;
    }
}

void Man::draw(POINT origin) {
    Data* mData = dataMap["man"];
    POINT size = getPoint("size");
    POINT picPos = {(status % 10) / 3 * size.x, (status / 10) * size.y};
    drawAlpha(MImage::imageMap["Man"], coord.x - origin.x, coord.y - origin.y, (int)size.x, (int)size.y, (int)picPos.x, (int)picPos.y, AA);

    // 如果没有在向上爬，就绘制剑
    int y = status / 10;
    if(y == DLeft || y == DRight) {
        int dx;
        POINT swordPicPos;
        double maxStep = mData->singles["swordStep"];
        double stayStep = mData->singles["swordStay"];
        // 如果步数进入静止帧，就直接显示最后一帧
        if(swordStep > maxStep - stayStep) {
            dx = size.x / 2;
        } else {
            dx = size.x / (maxStep - stayStep) * swordStep / 2;
        }
        // 向右
        if(speed.x > 0) {
            swordPicPos = mData->points["swordPos2"];
        }
        // 向左
        else {
            swordPicPos = mData->points["swordPos1"];
            dx = -dx;
        }
        drawAlpha(MImage::imageMap["Man"], coord.x - origin.x + dx, coord.y - origin.y, (int)size.x, (int)size.y, (int)swordPicPos.x, (int)swordPicPos.y, AA);

        // 只有当步数不为 0 是才会改变位置
        if(swordStep != 0) {
            swordStep++;
            swordStep %= (int)maxStep;
        }
    }
    // 最后绘制盾
    if(alive && timer != nullptr) {
        if(timer->now() < mData->singles["shieldTime"]) {
            shielding = true;  // 此时有护盾
            POINT shieldPicPos = mData->points["shieldPos"];
            POINT shieldSize = mData->points["shieldSize"];
            POINT shieldPos = {coord.x - origin.x, coord.y - origin.y};
            shieldPos.x += (size.x - shieldSize.x) / 2;
            shieldPos.y += (size.y - shieldSize.y) / 2;
            drawAlpha(MImage::imageMap["Man"], shieldPos.x, shieldPos.y, (int)shieldSize.x, (int)shieldSize.y, (int)shieldPicPos.x, (int)shieldPicPos.y);
        } else {
            shielding = false;  // 没有护盾
        }
        if(timer->onTime()) {
            delete timer;
            timer = nullptr;
        }
    }
}

void Man::sword() {
    // 活着的时候，如果没有剑刺，就剑刺
    if(alive && swordStep == 0) {
        swordStep = 1;
    }
}

RECT Man::swordRange(Direction& d) {
    if(swordStep != 0) {
        RECT r = getRect();
        if(speed.x > 0) {
            r.left += 8;
            r.right += 8;
            d = DRight;
        } else {
            r.left -= 8;
            r.right -= 8;
            d = DLeft;
        }
        return r;
    }
    return {0, 0, 0, 0};
}

void Man::shield() {
    // 活着的时候，如果没有开启护盾，则开启护盾
    if(timer == nullptr) {
        Data* mData = dataMap["man"];
        timer = new Timer(mData->singles["shieldID"], mData->singles["shieldCool"]);
        timer->start();
    }
}

void Man::dead() {
    // 如果开启了护盾，则不会死亡
    if(!shielding) {
        // 死亡，同时锁定移动
        alive = false;
        stay = true;

        if(timer != nullptr) {
            delete timer;
        }
        // 设置复活时间
        timer = new Timer(0, 1000);
        timer->start();
    }
}

void Man::thinking(POINT& origin, POINT& moveDist) {
    // 死亡时逐渐消失
    if(!alive) {
        AA *= 0.8;
        // 死亡计时结束
        if(timer->onTime()) {
            delete timer;
            timer = nullptr;
            // 恢复移动
            alive = true;
            stay = false;
            // 提供护盾，防止死亡
            shield();
            // 恢复透明度
            AA = 1;
            // 在复活位置复活，返回原点移动向量
            moveDist.x += alivePos.x - coord.x;
            moveDist.y += alivePos.y - coord.y;

            coord = alivePos;
            origin = {coord.x - WindowWidth / 2, coord.y - WindowHeight / 2};
        }
    }
}