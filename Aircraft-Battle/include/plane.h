#pragma once

#include "class.h"
#include "header.h"


// 父类，移动类
class Plane {
  public:
    Plane(int _width, int _height, int _speed): width(_width), height(_height), speed(_speed), move_timer(MOVE_SEP) {
        radius = (int)sqrt(width * width + height * height) / 2;
        // 开启移动计时器
        move_timer.open();
    }

    // 返回矩形区域
    Rect get_rect() {
        Rect rect(x, y, width, height);
        return rect;
    }

  protected:
    int radius;
    int width, height;
    int speed;
    int x, y;
    IMAGE img[2];      // 存放图片
    Timer move_timer;  // 移动计时器
};

// 弹药类
class Bullet : public Plane {
  public:
    Bullet(int _x, int _y, Type _type = normal_bullet): Plane(bullet_width, bullet_height, bullet_speed), type(_type) {
        x = _x, y = _y;

        if(type == normal_bullet) {
            loadimage(&img[0], TEST_DATA_PATH "/images/bullet0.png");
            loadimage(&img[1], TEST_DATA_PATH "/images/bullet1.png");
        } else {
            loadimage(&img[0], TEST_DATA_PATH "/images/bullet2.png");
            loadimage(&img[1], TEST_DATA_PATH "/images/bullet3.png");
        }
    }

    static void init();                  // 初始化类
    static void create(int, int, Type);  // 创建子弹
    static void test_live();             // 检测是否应该存活
    static void distory(int);            // 删除指定子弹
    static void move();                  // 移动全体子弹
    static void show();                  // 显示全体子弹
    static void delete_bullet();         // 清空子弹
    static Rect* get_rects();            // 获得所有子弹的矩形的数组
    static int get_count();              // 获得子弹数
    static void distory();               // 销毁全部

  private:
    Type type;  // 子弹类型

    // 记录子弹数和子弹
    static int count;
    static Bullet** bullet;
};

// 单例模式
class MyPlane : public Plane {
  public:
    void init_plane();
    void init_pos();   // 初始化飞机位置
    void move();       // 移动
    void fire();       // 开火
    void show();       // 显示
    bool test_live();  // 检查是否存活

    void change_bullet();  // 改变弹药
    void get_life();       // 获得生命
    void get_boom();       // 获得炸弹

    void reduce_life();
    void use_boom();

    void get_score(int);

    // 获取/创建唯一本地变量
    static MyPlane* share_plane() {
        if(my_plane == nullptr) {
            my_plane = new MyPlane;
        }
        return my_plane;
    }

    // 删除飞机
    static void delete_plane() {
        if(my_plane != nullptr) {
            // 删除指针，然后指向空指针
            delete my_plane;
            my_plane = nullptr;
        }
    }

  private:
    IMAGE life_img[2];  // 生命图像
    IMAGE boom_img[2];  // 炸弹图像

    Timer fire_timer;        // 开火计时器
    Timer super_timer;       // 超级子弹时间
    Timer invincible_timer;  // 无敌时间
    Timer protect_timer;     // 按键保护

    int hp;
    int my_score;        // 积分
    int boom_num;        // 炸弹数量
    bool my_invincible;  // 是否无敌
    Type bullet_type;    // 子弹类型

    static MyPlane* my_plane;  // 我的飞机

    // 私有化构造/析构函数
    MyPlane(): Plane(m_width, m_height, m_speed) {
        init_pos();
        init_plane();

        // 初始化计时器
        fire_timer.set(FIRE_SEP);
        super_timer.set(SUPER_TIME);
        protect_timer.set(PROTECT_TIME);
        invincible_timer.set(INVINCIBLE_TIME);

        // 开启计时器
        fire_timer.open();

        loadimage(&img[0], TEST_DATA_PATH "/images/me0.png");
        loadimage(&img[1], TEST_DATA_PATH "/images/me1.png");

        loadimage(&life_img[0], TEST_DATA_PATH "/images/life0.png");
        loadimage(&life_img[1], TEST_DATA_PATH "/images/life1.png");

        loadimage(&boom_img[0], TEST_DATA_PATH "/images/bomb0.png");
        loadimage(&boom_img[1], TEST_DATA_PATH "/images/bomb1.png");
    }
    ~MyPlane() {}

    // 私有化赋值
    void operator=(const MyPlane* plane) {}
};

// 敌人类
class Enemy : public Plane {
  public:
    Enemy(Type _type): Plane(0, 0, 0), type(_type) {
        // 根据不同类型获取不同属性
        switch(type) {
            case e_small:
                width = e_small_width;
                height = e_small_height;
                speed = e_small_speed;
                hp = e_small_hp;

                loadimage(&img[0], TEST_DATA_PATH "/images/smallenemy0.png");
                loadimage(&img[1], TEST_DATA_PATH "/images/smallenemy1.png");
                break;
            case e_mid:
                width = e_mid_width;
                height = e_mid_height;
                speed = e_mid_speed;
                hp = e_mid_hp;

                loadimage(&img[0], TEST_DATA_PATH "/images/midenemy0.png");
                loadimage(&img[1], TEST_DATA_PATH "/images/midenemy1.png");
                break;
            case e_big:
                width = e_big_width;
                height = e_big_height;
                speed = e_big_speed;
                hp = e_big_hp;

                loadimage(&img[0], TEST_DATA_PATH "/images/bigenemy0.png");
                loadimage(&img[1], TEST_DATA_PATH "/images/bigenemy1.png");
                break;
        }

        // 随机生成
        x = rand() % (WIDTH - width);
        y = -1 * height;
    }

    static void init();          // 初始化
    static void create();        // 创建敌人
    static void distory(int);    // 销毁敌人
    static void test_live();     // 检测是否存活
    static void move();          // 移动
    static void show();          // 显示
    static void delete_enemy();  // 清空所有敌人
    static void collision();     // 碰撞检测
    static void distory();       // 销毁全部

    void attacked();  // 被攻击
    int get_score();  // 获取该敌人的分数

  private:
    int hp;
    Type type;

    static Timer create_timer;     // 创建计时器
    static Timer collision_timer;  // 碰撞计时器
    static int count;              // 计数
    static Enemy** enemy;          // 存放敌人
};

// 补给（单例模式）
class Supply : public Plane {
  public:
    void move();       // 移动
    void show();       // 显示
    void test_live();  // 销毁

    static void create();  // 创建supply
    static void action();  // supply动作

    // 删除补给
    static void delete_supply() {
        if(my_supply != nullptr) {
            // 删除指针，然后指向空指针
            delete my_supply;
            my_supply = nullptr;
        }
    }

  private:
    Type type;

    static Timer supply_timer;  // 补给计时器
    static Supply* my_supply;   // 我的补给

    // 私有化构造/析构函数
    Supply(): Plane(supply_width, supply_height, supply_speed) {
        int id = rand() % 3;
        switch(id) {
            case 0:
                type = super_bullet;
                loadimage(&img[0], TEST_DATA_PATH "/images/bullet_supply0.png");
                loadimage(&img[1], TEST_DATA_PATH "/images/bullet_supply1.png");
                break;
            case 1:
                type = recover_life;
                loadimage(&img[0], TEST_DATA_PATH "/images/life_supply0.png");
                loadimage(&img[1], TEST_DATA_PATH "/images/life_supply1.png");
                break;
            case 2:
                type = boom_boom;
                loadimage(&img[0], TEST_DATA_PATH "/images/bomb_supply0.png");
                loadimage(&img[1], TEST_DATA_PATH "/images/bomb_supply1.png");
                break;
        }

        supply_timer.set(SUPPLY_SEP);

        // 随机生成
        x = rand() % (WIDTH - width);
        y = -1 * height;
    }
    ~Supply() {}

    // 私有化赋值
    void operator=(const Supply* supply) {}
};
