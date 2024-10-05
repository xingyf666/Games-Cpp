#include "GameWindow.h"

#include "ui_GameWindow.h"


GameWindow::GameWindow(QWidget* parent): QMainWindow(parent), ui(new Ui::GameWindow), m_second(0), m_minute(0), m_hour(0) {
    this->setWindowIcon(QIcon(":/MagicTower.ico"));

    ui->setupUi(this);
    ui->frame->hide();

    ui->toolWidget->setStyleSheet("background-color:rgb(58, 58, 58);");  // 设置背景色
    ui->toolWidget->setViewMode(QListWidget::IconMode);                  // 显示模式
    ui->toolWidget->setIconSize(QSize(40, 40));                          // 设置图片大小
    ui->toolWidget->setSpacing(0);                                       // 间距
    ui->toolWidget->setDragEnabled(false);                               // 禁止拖动控件

    // 设置强制焦点；当应用程序窗口切换为活动状态时更改焦点为此窗口，用于捕捉空格按键
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus(Qt::ActiveWindowFocusReason);

    // 初始化数据
    GameEvent::initEvent();
    GameObject::initObject();
    GameMap::initMap();
    GameHero::initHero();

    // 控制标签
    GameEvent::s_frame = ui->frame;
    GameEvent::s_label = ui->keyLabel;
    GameEvent::s_edit = ui->textEdit;
    GameEvent::s_bkFrame = ui->bkFrame;
    GameObject::s_toolList = ui->toolWidget;

    // 窗口计时器
    m_animateTimer.start(300);
    m_hitTimer.start(50);
    m_gameTimer.start(1000);
    m_eventTimer.start(0);
    m_timer.start(40);

    // 绑定刷新显示
    QObject::connect(&m_timer, SIGNAL(timeout(void)), this, SLOT(update(void)));
    QObject::connect(&m_animateTimer, SIGNAL(timeout(void)), this, SLOT(updateAnimate(void)));
    QObject::connect(&m_gameTimer, SIGNAL(timeout(void)), this, SLOT(updateTime(void)));
    QObject::connect(&m_hitTimer, SIGNAL(timeout(void)), this, SLOT(updateHit(void)));
    QObject::connect(&m_eventTimer, SIGNAL(timeout(void)), this, SLOT(updateEvent(void)));
}

GameWindow::~GameWindow() {
    delete ui;
    GameHero::deleteHero();
    GameMap::deleteMap();
}

void GameWindow::updateAnimate() {
    // 动画刷新
    GameHero& hero = GameHero::shareHero();
    GameMap& map = GameMap::shareMap(hero["floor"]);
    map.animate();
}

void GameWindow::updateTime() {
    // 时间计时，游戏时长
    m_second++;

    if(m_second == 60) {
        m_second = 0;
        m_minute++;
        if(m_minute == 60) {
            m_minute = 0;
            m_hour++;
        }
    }

    // 刷新时间显示
    char buf[255];
    sprintf_s(buf, "%02d:%02d:%02d", m_hour, m_minute, m_second);
    QString str = buf;
    ui->timeLabel->setText(str);
}

void GameWindow::updateHit() {
    // 如果不处于移动状态或者有按键时进行检测
    GameHero& hero = GameHero::shareHero();
    if(hero["status"] != GameHero::moving || GameEvent::s_key != -1) {
        // 移动英雄
        hero.move();

        // 如果移动后的位置有物体，就会发生碰撞
        GameMap& map = GameMap::shareMap(hero["floor"]);
        map.test();

        // 清除按键
        GameEvent::s_key = -1;
    }
}

void GameWindow::updateEvent() {
    GameHero& hero = GameHero::shareHero();
    ui->HP->setNum(hero["HP"]);
    ui->Attack->setNum(hero["attack"]);
    ui->Defense->setNum(hero["defense"]);
    ui->Money->setNum(hero["money"]);
    ui->Yellow->setNum(hero["yellow"]);
    ui->Blue->setNum(hero["blue"]);
    ui->Red->setNum(hero["red"]);
    ui->floorLabel->setNum(hero["floor"] + 1);

    // 执行事件
    GameEvent::doEvent();
}

void GameWindow::paintEvent(QPaintEvent*) {
    QPainter painter(this);

    // 绘制背景色
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor{58, 58, 58});
    painter.drawRect(this->rect());

    // 绘制 13 * 13 的地面，地面在源文件的 0,0 位置
    for(int i = 0; i < 13; i++)
        for(int j = 0; j < 13; j++) painter.drawImage(QRect(i * 40, j * 40, 40, 40), GameObject::s_objImg, QRect(0, 0, 32, 32));

    // 绘制场景
    GameHero& hero = GameHero::shareHero();
    GameMap& map = GameMap::shareMap(hero["floor"]);
    map.draw(this);
    hero.draw(this);
}

void GameWindow::keyPressEvent(QKeyEvent* event) {
    // 只记录，等到 hit 定时器触发再处理
    GameEvent::s_key = event->key();
}
