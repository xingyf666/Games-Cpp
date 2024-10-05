
#include "map.h"

Map* Map::createMap(LPCSTR name) {
    return new Map(name);
}

Map::Map(LPCSTR name) {
    std::map<std::string, Data*> dataMap;
    // 读取数据
    Data::read(name, dataMap, ByFree);
    MAXID = 0;  // 从这里开始标记 id
    mData = nullptr;

    // 由于 id 是有顺序的，因此指定映射也有顺序
    for(auto it = dataMap.begin(); it != dataMap.end(); ++it) {
        Data* data = it->second;

        // id 就是按照读取物体的顺序得到的
        int id = data->singles["id"];
        MAXID = MAX(id, MAXID);

        // 获取资源名和位置
        std::string nName = data->details["name"];
        // 获取事件 id
        int eId = data->singles["event"];

        // Object 总是有位置
        POINT r = data->points["pos"];

        // 通过 Object 数据获取所属类名
        std::string className = Object::dataMap[nName]->details["class"];
        if(className == "Man") {
            mData = data;
            continue;
        } else if(className == "GStatic") {
            // 这里对 pos 进行修正
            r.x *= StdWidth;
            r.y *= StdHeight;
            GStatic* pg = new GStatic(nName, r);

            // 如果有设定不显示，就改变显示状态
            if(data->details["show"] == "false") {
                pg->setShow(false);
            }
            staticMap[id] = pg;
        }
        // 按钮会附带返回值 result 和按钮文本 text ，还可能包括事件
        else if(className == "GButton") {
            Data* nData = new Data;

            // 点按结果和显示文本
            nData->details["result"] = data->details["result"];
            nData->details["text"] = data->details["text"];

            staticMap[id] = new GButton(nName, r, nData);
        }
        // 怪物的读取
        else if(className == "Enemy") {
            // 这里对 pos 进行修正
            r.x *= StdWidth;
            r.y *= StdHeight;

            // 移动方向
            std::string dir = data->details["dir"];
            // 移动范围
            POINT moveRange = data->points["range"];

            // 移动范围修正
            moveRange.x *= StdWidth;
            moveRange.y *= StdWidth;

            Enemy* enemy = Enemy::createEnemy(nName, r, moveRange);
            if(dir == "left") {
                enemy->setDir(DLeft);
            } else if(dir == "right") {
                enemy->setDir(DRight);
            }
            enemyMap[id] = enemy;
        }
    }
}

void Map::draw(POINT origin) {
    for(auto it = staticMap.begin(); it != staticMap.end(); ++it) {
        if(it->second->ifShow()) {
            it->second->draw(origin);
        }
    }
    for(auto it = enemyMap.begin(); it != enemyMap.end(); ++it) {
        if(it->second->ifShow()) {
            it->second->draw(origin);
        }
    }
}

bool Map::collision(GDynamic* gd, Direction d) {
    // 如果没有移动就直接返回
    if(d == DStay) {
        return false;
    }

    bool ifCollision = false;
    RECT distance = {0, 0, 0, 0};
    RECT mr = gd->getRect();
    POINT pos = gd->getPos();
    POINT speed = gd->getSpeed();

    // 创建并发送消息给人物和物体，这里不需要区分 id，因为碰撞事件是立即处理的
    // 主角事件包括 事件 id 物体 id 物体矩形 碰撞方向
    // 先不考虑获得物品的情况，现在碰到物体就会返回
    // 如果碰撞则发送事件

    for(auto it = staticMap.begin(); it != staticMap.end(); ++it) {
        if(Collision::checkCollision(gd, it->second)) {
            ifCollision = true;
            RECT objR = it->second->getRect();
            distance.left = objR.right - mr.left;
            distance.right = mr.right - objR.left;
            distance.top = objR.bottom - mr.top;
            distance.bottom = mr.bottom - objR.top;
            break;
        }
    }
    // 没有碰撞则立即返回
    if(!ifCollision) {
        return false;
    }
    // 碰撞处理
    if(d == DLeft) {
        pos.x += distance.left;
    } else if(d == DRight) {
        pos.x -= distance.right;
    } else if(d == DUp) {
        pos.y += distance.top;
    } else if(d == DDown) {
        pos.y -= distance.bottom;
    } else if(d == DGravity) {
        // 向下，清空竖直速度，这里设为 1 ，防止再次跳跃
        if(speed.y + (int)Gravity > 0) {
            pos.y -= distance.bottom;
            speed.y = 0;
        } else {
            pos.y += distance.top;
            speed.y = 1;
        }
    }
    gd->setPos(pos);
    gd->setSpeed(speed);

    return true;
}

std::string Map::buttonClicked(POINT mp) {
    std::string res = "";
    for(auto it = staticMap.begin(); it != staticMap.end(); ++it) {
        std::string name = it->second->getName();
        std::string cName = Object::dataMap[name]->details["class"];

        GButton* gbutton = (GButton*)(it->second);
        if(cName == "GButton" && Collision::checkIn(mp, gbutton->getRect())) {
            // 鼠标点击消息，立即处理
            if(gbutton->getClicked()) {
                // 说明已经点击过，这次是释放按钮，因此返回结果
                Data* oData = gbutton->getData();
                res = oData->details["result"];
                gbutton->setClicked(false);
            } else {
                gbutton->setClicked(true);
            }
        } else {
            gbutton->setClicked(false);
        }
    }
    return res;
}

Map::~Map() {
    // 删除人物数据
    delete mData;
    mData = nullptr;

    for(auto it = staticMap.begin(); it != staticMap.end(); ++it) {
        if(it->second != nullptr) {
            delete it->second;
            staticMap[it->first] = nullptr;
        }
    }
    for(auto it = enemyMap.begin(); it != enemyMap.end(); ++it) {
        if(it->second != nullptr) {
            delete it->second;
            enemyMap[it->first] = nullptr;
        }
    }
}

void Map::move(Man* man) {
    // 敌人的移动情况
    for(auto it = enemyMap.begin(); it != enemyMap.end();) {
        Enemy* enemy = it->second;

        // 当敌人快消失，就删除
        if(enemy->getAA() <= 0.1) {
            delete it->second;
            it = enemyMap.erase(it);
            continue;
        }

        int dir = enemy->getDir();
        POINT ev = enemy->getSpeed();

        // 移动及其碰撞检测
        enemy->move();
        // 发生碰撞就改变速度
        if(collision(enemy, dir)) {
            ev.x = -ev.x;
            enemy->setSpeed(ev);
        }

        // 作用范围
        int range = enemy->skillRange;
        POINT moveRange = enemy->moveRange;

        RECT er = enemy->getRect();
        RECT mr = man->getRect();

        // 检查是否超出移动范围，超出范围就改变速度
        if(er.left < moveRange.x || er.right > moveRange.y) {
            // 如果正在发动技能，就锁定移动，返回位置
            if(enemy->ifSkill()) {
                enemy->setPos({er.left - ev.x, er.top});
            } else {
                // 反向速度
                ev.x = -ev.x;
                enemy->setSpeed(ev);
            }
        }

        // 重力作用要放在后面
        enemy->gravity();
        collision(enemy, DGravity);

        // 这里的修正是发现 man 的范围
        if(ev.x < 0) {
            er.left -= 5 * 32;
            er.right -= 16;
        } else {
            er.left += 16;
            er.right += 5 * 32;
        }

        // 检测与 man 的碰撞
        if(man->ifAlive() && Collision::checkCollision(er, mr)) {
            // 传入物体 ID
            enemy->cut(it->first);

            // 这里的修正是杀死 man 的范围
            if((dir == DLeft && mr.left < er.right && mr.right > er.right - 48) || (dir == DRight && mr.right > er.left && mr.left < er.left + 48)) {
                man->dead();
            }
        }

        Direction d;
        RECT swordR = man->swordRange(d);
        // 检测与 剑刺 的碰撞
        if(man->ifAlive() && Collision::checkCollision(er, swordR)) {
            // 传入物体 ID
            enemy->hurt(man->attack);
            // 击退效果
            POINT nPos = enemy->getPos();
            nPos.y -= 5;
            if(d == DLeft) {
                nPos.x -= 5;
            } else {
                nPos.x += 5;
            }
            enemy->setPos(nPos);
        }

        // 因为删除操作会移动迭代器，因此放在最后
        ++it;
    }
}