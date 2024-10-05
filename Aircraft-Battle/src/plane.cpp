#include "plane.h"

// 子弹类
int Bullet::count = 0;
Bullet** Bullet::bullet = nullptr;

void Bullet::init() {
    if(bullet == nullptr) {
        bullet = new Bullet*[MAX_NUM];
        for(int i = 0; i < MAX_NUM; i++) {
            bullet[i] = nullptr;
        }
    }
}

void Bullet::create(int x, int y, Type _type) {
    if(count < MAX_NUM) {
        bullet[count] = new Bullet(x, y, _type);
        count++;
    }
}

void Bullet::show() {
    for(int i = 0; i < count; i++) {
        putimage(bullet[i]->x, bullet[i]->y, &(bullet[i]->img[0]), NOTSRCERASE);
        putimage(bullet[i]->x, bullet[i]->y, &(bullet[i]->img[1]), SRCINVERT);
    }
}

void Bullet::move() {
    for(int i = 0; i < count; i++) {
        if(bullet[i]->move_timer.t_time()) {
            bullet[i]->y -= bullet[i]->speed;
        }
    }
}

void Bullet::test_live() {
    for(int i = 0; i < count; i++) {
        if(bullet[i]->y < -1 * bullet[i]->height) {
            distory(i);
        }
    }
}

Rect* Bullet::get_rects() {
    Rect* rects = new Rect[count];
    for(int i = 0; i < count; i++) {
        rects[i] = bullet[i]->get_rect();
    }
    return rects;
}

int Bullet::get_count() {
    return count;
}

void Bullet::distory(int i) {
    delete bullet[i];
    for(int j = i; j < count - 1; j++) {
        bullet[j] = bullet[j + 1];
    }
    bullet[count - 1] = nullptr;
    count--;
}

void Bullet::distory() {
    for(int i = 0; i < count; i++) {
        delete bullet[i];
        bullet[i] = nullptr;
    }
    count = 0;
}

void Bullet::delete_bullet() {
    distory();
    delete bullet;
    bullet = nullptr;
}

/*我的飞机函数*/
MyPlane* MyPlane::my_plane = nullptr;

void MyPlane::init_plane() {
    init_pos();

    hp = m_hp;
    my_score = 0;
    boom_num = m_boom;
    bullet_type = normal_bullet;
    my_invincible = false;
}

void MyPlane::init_pos() {
    x = (WIDTH - width) / 2;
    y = HEIGHT - height * 2;
}

void MyPlane::show() {
    putimage(x, y, &img[0], NOTSRCERASE);
    putimage(x, y, &img[1], SRCINVERT);

    if(my_invincible) {
        setlinecolor(GREEN);
        setlinestyle(PS_DOT, 2);
        circle(x + width / 2, y + height / 2, radius);
    }

    // 显示生命
    int life_x = WIDTH - m_life_width - 10, life_y = HEIGHT - m_life_height - 10;
    for(int i = 0; i < hp; i++) {
        putimage(life_x, life_y, &life_img[0], NOTSRCERASE);
        putimage(life_x, life_y, &life_img[1], SRCINVERT);

        life_x -= m_life_width + 5;
    }

    // 显示炸弹数
    int boom_x = 5, boom_y = HEIGHT - m_boom_height - 5;
    putimage(boom_x, boom_y, &boom_img[0], NOTSRCERASE);
    putimage(boom_x, boom_y, &boom_img[1], SRCINVERT);

    // 设置字体样式
    settextcolor(WHITE);
    setbkmode(TRANSPARENT);  // 去掉文字背景

    LOGFONT f;
    gettextstyle(&f);                     // 获取文字格式
    f.lfQuality = ANTIALIASED_QUALITY;    // 抗锯齿
    f.lfHeight = 30;                      // 字高
    _tcscpy_s(f.lfFaceName, "Consolas");  // 字体
    settextstyle(&f);

    // 显示文字
    char str[15];
    sprintf_s(str, " X %d", boom_num);
    int str_h = textheight(str);
    outtextxy(boom_x + m_boom_width + 5, boom_y + (m_boom_height - str_h) / 2, str);

    // 显示分数
    int score_x = 5, score_y = 5;
    sprintf_s(str, "Score: %d", my_score);
    outtextxy(score_x, score_y, str);
}

bool MyPlane::test_live() {
    if(hp < 0) {
        play_music(TEST_DATA_PATH "/music/me_down.wav");
        return false;
    }
    return true;
}

void MyPlane::fire() {
    if(fire_timer.t_time()) {
        if(bullet_type == super_bullet) {
            // 超级子弹会有两发
            Bullet::create(x + (width - bullet_width) / 6, y - bullet_height, bullet_type);
            Bullet::create(x + 5 * (width - bullet_width) / 6, y - bullet_height, bullet_type);
        } else {
            Bullet::create(x + (width - bullet_width) / 2, y - bullet_height, bullet_type);
        }
    }
    // 超级子弹持续完毕
    if(super_timer.t_time()) {
        super_timer.close();
        change_bullet();
    }
}

void MyPlane::move() {
    if(move_timer.t_time()) {
        // 直接获取键是否按下，可以得到多个按键操作
        if(GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')) {
            y -= speed;
            if(y < 0) {
                y = 0;
            }
        }
        if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')) {
            y += speed;
            if(y + height > HEIGHT) {
                y = HEIGHT - height;
            }
        }
        if(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) {
            x -= speed;
            if(x < -1 * width / 2) {
                x = -1 * width / 2;
            }
        }
        if(GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) {
            x += speed;
            if(x + width / 2 > WIDTH) {
                x = WIDTH - width / 2;
            }
        }
    }
    // 无敌时间结束，取消计时器
    if(invincible_timer.t_time()) {
        // 速度回复
        speed = m_speed;
        my_invincible = false;
        invincible_timer.close();
    }
}

void MyPlane::change_bullet() {
    if(bullet_type == normal_bullet) {
        super_timer.open();
        bullet_type = super_bullet;
    } else {
        bullet_type = normal_bullet;
    }
}

void MyPlane::get_life() {
    hp++;
    hp %= 6;
}

void MyPlane::get_boom() {
    boom_num++;
    boom_num %= 6;
}

void MyPlane::reduce_life() {
    if(!my_invincible) {
        hp--;
        // 回到初始位置
        init_pos();
        // 掉血无敌，减速
        speed = m_speed_down;
        my_invincible = true;
        // 开启计时器
        invincible_timer.open();
    }
}

void MyPlane::use_boom() {
    // 保护器到时间就关闭
    if(protect_timer.t_time()) {
        protect_timer.close();
    }

    // 如果保护器关闭则进入
    if(boom_num > 0 && GetAsyncKeyState(VK_SPACE) && !protect_timer.status()) {
        // 开启
        protect_timer.open();

        play_music(TEST_DATA_PATH "/music/bomb.wav");
        Enemy::init();
        boom_num--;
    }
}

void MyPlane::get_score(int score) {
    my_score += score;
}

// 敌机
int Enemy::count = 0;
Enemy** Enemy::enemy = nullptr;
Timer Enemy::create_timer(ENEMY_SEP);
Timer Enemy::collision_timer(COLLISION_SEP);

void Enemy::init() {
    if(enemy == nullptr) {
        enemy = new Enemy*[MAX_NUM];
        for(int i = 0; i < MAX_NUM; i++) {
            enemy[i] = nullptr;
        }
        // 开启计时器
        create_timer.open();
        collision_timer.open();
    } else {
        for(int i = 0; i < count; i++) {
            // 得分
            MyPlane::share_plane()->get_score(enemy[i]->get_score());
            delete enemy[i];
            enemy[i] = nullptr;
        }
        count = 0;
    }
}

void Enemy::show() {
    for(int i = 0; i < count; i++) {
        putimage(enemy[i]->x, enemy[i]->y, &(enemy[i]->img[0]), NOTSRCERASE);
        putimage(enemy[i]->x, enemy[i]->y, &(enemy[i]->img[1]), SRCINVERT);

        if(enemy[i]->type == e_mid) {
            blood_line(enemy[i]->x, enemy[i]->y - 5, enemy[i]->width, 1.0 * enemy[i]->hp / e_mid_hp);
        }
        if(enemy[i]->type == e_big) {
            blood_line(enemy[i]->x, enemy[i]->y - 5, enemy[i]->width, 1.0 * enemy[i]->hp / e_big_hp);
        }
    }
}

void Enemy::move() {
    for(int i = 0; i < count; i++) {
        if(enemy[i]->move_timer.t_time()) {
            enemy[i]->y += enemy[i]->speed;
        }
    }
}

void Enemy::attacked() {
    hp--;
}

void Enemy::create() {
    if(count < MAX_NUM && create_timer.t_time()) {
        double id = rand() / double(RAND_MAX);
        if(id >= 0.95) {
            enemy[count] = new Enemy(e_big);
        } else if(id >= 0.85) {
            enemy[count] = new Enemy(e_mid);
        } else {
            enemy[count] = new Enemy(e_small);
        }
        count++;
    }
}

void Enemy::test_live() {
    for(int i = 0; i < count; i++) {
        if(enemy[i]->hp <= 0) {
            // 得分
            MyPlane::share_plane()->get_score(enemy[i]->get_score());
            distory(i);
        } else if(enemy[i]->y > HEIGHT) {
            distory(i);
        }
    }
}

void Enemy::collision() {
    if(collision_timer.t_time()) {
        // 得到所有需要检测碰撞的矩形
        int bullet_num = Bullet::get_count();
        Rect* bullet_rects = Bullet::get_rects();
        Rect plane_rect = MyPlane::share_plane()->get_rect();

        for(int i = 0; i < count; i++) {
            Rect rect = enemy[i]->get_rect();
            int id = rect.collision(bullet_num, bullet_rects);
            if(id != -1) {
                // 删除碰撞的子弹
                enemy[i]->attacked();
                Bullet::distory(id);
            } else if(rect.collision(plane_rect)) {
                // 删除碰撞的敌人，自己掉血，进入无敌
                MyPlane::share_plane()->reduce_life();
                // 得分
                MyPlane::share_plane()->get_score(enemy[i]->get_score());
                Enemy::distory(i);
            }
        }
        delete[] bullet_rects;
        bullet_rects = nullptr;
    }
}

int Enemy::get_score() {
    switch(type) {
        case e_small:
            return 100;
        case e_mid:
            return 250;
        case e_big:
            return 400;
    }
    return 0;
}

void Enemy::distory(int i) {
    switch(enemy[i]->type) {
        case e_small:
            play_music(TEST_DATA_PATH "/music/smallenemy_down.wav");
            break;
        case e_mid:
            play_music(TEST_DATA_PATH "/music/midenemy_down.wav");
            break;
        case e_big:
            play_music(TEST_DATA_PATH "/music/bigenemy_down.wav");
            break;
    }

    delete enemy[i];
    for(int j = i; j < count - 1; j++) {
        enemy[j] = enemy[j + 1];
    }
    enemy[count - 1] = nullptr;
    count--;
}

void Enemy::distory() {
    for(int i = 0; i < count; i++) {
        delete enemy[i];
        enemy[i] = nullptr;
    }
    count = 0;
}

void Enemy::delete_enemy() {
    distory();
    delete enemy;
    enemy = nullptr;
}

/*补给*/
Timer Supply::supply_timer(SUPPLY_SEP);
Supply* Supply::my_supply = nullptr;

void Supply::show() {
    putimage(x, y, &img[0], NOTSRCERASE);
    putimage(x, y, &img[1], SRCINVERT);
}

void Supply::move() {
    if(move_timer.t_time()) {
        y += speed;
    }
}

void Supply::test_live() {
    if(y > HEIGHT) {
        delete_supply();
        return;
    }
    Rect rect = MyPlane::share_plane()->get_rect();
    Rect supply_rect = get_rect();

    if(rect.collision(supply_rect)) {
        switch(type) {
            case super_bullet:
                // 碰撞则播放音乐，改变弹药，删除补给
                play_music(TEST_DATA_PATH "/music/get_bullet.wav");
                MyPlane::share_plane()->change_bullet();
                break;
            case recover_life:
                // 碰撞则播放音乐，增加生命，删除补给
                play_music(TEST_DATA_PATH "/music/life.wav");
                MyPlane::share_plane()->get_life();
                break;
            case boom_boom:
                // 碰撞则播放音乐，增加炸药，删除补给
                play_music(TEST_DATA_PATH "/music/get_bomb.wav");
                MyPlane::share_plane()->get_boom();
                break;
        }

        delete_supply();
        return;
    }
}

void Supply::create() {
    // 到时间则关闭
    if(supply_timer.t_time()) {
        supply_timer.close();
    }

    // 当保护器关闭则产生
    if(my_supply == nullptr && !supply_timer.status()) {
        // 开启计时器
        supply_timer.open();
        my_supply = new Supply;
    }
}

void Supply::action() {
    if(my_supply != nullptr) {
        my_supply->move();
        my_supply->show();
        my_supply->test_live();
    }
}
