// Game2D.cpp : 定义应用程序的入口点。
//

#include "Game2D.h"

#include "framework.h"


// 全局变量:
CHAR szTitle[MAX_LOADSTRING];  // 标题栏文本
POINT WindowSize;              // 窗口信息

// 定时器
int moveTimeID = 1000;

// 声明函数
void WndCreate();           // 初始化
void WndDelete();           // 销毁内容
Map* loadMap(LPCSTR name);  // 加载地图
Map* loadUI(LPCSTR name);   // 加载 UI

// 不同界面的消息处理
int WndProc(HINSTANCE hInstance, HWND hWnd, int status);
int WndProcHead(HINSTANCE hInstance, HWND hWnd);  // 首页
int WndProcGame(HINSTANCE hInstance, HWND hWnd);  // 游戏

// 创建主角
Man* man = nullptr;
Map* map = nullptr;
Map* ui = nullptr;
// 外部数据
std::map<std::string, Data*> srcMap;

// 绘图偏移坐标系：随人物移动绘图原点发生变化
POINT origin = {0, 0};
// 移动偏移值
POINT moveDist = {0, 0};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    // 调试选项
#ifdef OPEN_DEBUG
    DebugOutPut::openConsole();
#endif  // OPEN_DEBUG

    // TODO: 初始化数据
    WndCreate();

    // 初始化全局字符串
    // LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    // 创建绘图窗口
    initgraph(WindowWidth, WindowHeight);

    HWND hWnd = GetHWnd();
    SetWindowText(hWnd, szTitle);

    // 开始绘图
    BeginBatchDraw();
    setbkcolor(RGB(0, 0, 0));
    setbkmode(TRANSPARENT);

    // 页面状态初始化为首页
    int status = GAME_HEAD;

    while(TRUE) {
        // 使用当前背景色刷新窗口
        // 由于使用了定时器，这一刷新会覆盖绘图
        status = WndProc(hInstance, hWnd, status);
        // 绘制到窗口
        FlushBatchDraw();
        // 睡眠可能导致无法按时响应鼠标消息
        // Sleep(ROUNDSEP);
    }

    EndBatchDraw();  // 结束绘图
    closegraph();    // 关闭绘图窗口
    WndDelete();     // 销毁数据

    return 0;
}

void WndCreate() {
    // 随机数种子
    srand((int)time(0));

    // 外部数据初始化，读取配置文件
    LPCSTR STRData;
    Data::read(TEST_DATA_PATH "/config/config.txt", srcMap);

    // 物体数据
    STRData = srcMap["object"]->details["object"].c_str();
    Object::readData(STRData);

    // 加载图片，它们分别和类名对应
    std::map<std::string, std::string> mImage = srcMap["image"]->details;
    for(auto it = mImage.begin(); it != mImage.end(); ++it) {
        STRData = it->second.c_str();
        MImage::imageMap[it->first] = MImage::createImage(STRData);
    }

    // 加载音乐，它们分别和标签对应
    std::map<std::string, std::string> mMusic = srcMap["music"]->details;
    for(auto it = mMusic.begin(); it != mMusic.end(); ++it) {
        Music::musicMap[it->first] = Music::createMusic(it->second);
    }

    // 创建定时器
    Timer::createTimer(moveTimeID, MOVESEP);
}

void WndDelete() {
    Man::deleteMan();
    Event::deleteEvent();
    Object::deleteData();
    Timer::deleteTimer();
    Music::deleteMusic();
    MImage::deleteImage();

    // 清除 ui 和 map
    if(ui != nullptr) {
        delete ui;
    }
    if(map != nullptr) {
        delete map;
    }
}

//
//  函数: void WndProc(HINSTANCE hInstance, HWND hWnd);
//
//  目标: 处理主窗口的消息。

int WndProc(HINSTANCE hInstance, HWND hWnd, int status) {
    switch(status) {
            // 设置框，加载框放在首页
        case GAME_ABOUT:  // 弹出一个关于框
            MessageBox(hWnd, "version 0.0.1", "About", MB_OK);
            status = GAME_HEAD;
        case GAME_SET:
        case GAME_LOAD:
        case GAME_HEAD:
            status = WndProcHead(hInstance, hWnd);
            break;
            // 游戏内容相关
        case GAME_PAUSE:
        case GAME_START:
            // 游戏进程，开启计时器
            status = WndProcGame(hInstance, hWnd);
            break;
    }
    return status;
}

Map* loadMap(LPCSTR name) {
    // 加载地图
    std::map<std::string, std::string> mMap = srcMap["map"]->details;
    map = Map::createMap(mMap[name].c_str());

    // 创建人物，先执行这一步，因为要修正位置
    man = Man::createMan(map->mData);
    // 记录最初位置
    moveDist = man->getPos();
    // 调整原点位置
    origin = {moveDist.x - WindowWidth / 2, moveDist.y - WindowHeight / 2};

    return map;
}

Map* loadUI(LPCSTR name) {
    // 加载界面
    std::map<std::string, std::string> mMap = srcMap["UI"]->details;
    ui = Map::createMap(mMap[name].c_str());
    return ui;
}

int WndProcHead(HINSTANCE hInstance, HWND hWnd) {
    static Map* ui = loadUI("head");
    int status = GAME_HEAD;

    // 播放音乐
    Music::musicMap["head"]->play(true);

    // 首页，处理鼠标消息
    ExMessage msg;
    if(peekmessage(&msg, EM_MOUSE)) {
        std::string str = "";
        switch(msg.message) {
            case WM_LBUTTONDOWN:
                ui->buttonClicked({msg.x, msg.y});
                break;
            case WM_LBUTTONUP:
                str = ui->buttonClicked({msg.x, msg.y});
                if(str == "start") {
                    // 进入游戏，音乐停止
                    Music::musicMap["head"]->stop();
                    status = GAME_START;
                } else if(str == "load") {
                    status = GAME_LOAD;
                } else if(str == "setting") {
                    // 显示设置窗口，按照 id 进行设置
                    ui->staticMap[5]->setShow(true);
                    status = GAME_SET;
                } else if(str == "about") {
                    status = GAME_ABOUT;
                }
                break;
        }
    }

    // 背景刷新
    cleardevice();

    // 绘制背景
    drawAlpha(MImage::imageMap["Background"], 0, 0, WindowWidth, WindowHeight, 0, 0);
    // 绘制标题
    RECT r = {0, 0, WindowWidth, 200};
    setText("Acquaintance", WHITE, 100, 20);
    drawtext("Game2D", &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    // 绘制按钮
    ui->draw(origin);

    return status;
}

int WndProcGame(HINSTANCE hInstance, HWND hWnd) {
    static Map* map = loadMap("map");

    // 游戏进程，开启计时器
    Timer* moveTimer = Timer::start(moveTimeID);

    // 播放音乐
    Music::musicMap["game"]->play(true);

    // 游戏内操作制造 50 毫秒延时
    if(moveTimer->onTime()) {
        // 记录移动前的位置
        POINT oPos = man->getPos();

        // 跳跃
        if(GetAsyncKeyState(VK_SPACE)) {
            man->jump();
        }
        // 剑刺
        if(GetAsyncKeyState('J')) {
            man->sword();
        }
        // 暂时禁用护盾
        /*
        // 护盾
        if (GetAsyncKeyState('K'))
        {
            man->shield();
        }*/
        // 移动过程
        if(GetAsyncKeyState('A')) {
            // 碰撞检测
            man->setDir(DLeft);
            man->move();
            map->collision(man, DLeft);
        }
        if(GetAsyncKeyState('D')) {
            // 碰撞检测
            man->setDir(DRight);
            man->move();
            map->collision(man, DRight);
        }
        // 重力作用
        man->gravity();
        map->collision(man, DGravity);

        // 地图本身的移动
        map->move(man);

        // 调整窗口位置
        // 记录移动到的位置
        POINT nPos = man->getPos();
        moveDist.x += nPos.x - oPos.x;
        moveDist.y += nPos.y - oPos.y;

        RECT mr = man->getRect();
        POINT mSize = {mr.right - mr.left, mr.bottom - mr.top};

        // 横向移动
        // 向右
        if(nPos.x > oPos.x && (nPos.x - origin.x) + mSize.x > WindowWidth * 0.85) {
            origin.x = moveDist.x + mSize.x - WindowWidth * 0.85;
        } else if(nPos.x < oPos.x && (nPos.x - origin.x) < WindowWidth * 0.15) {
            origin.x = moveDist.x - WindowWidth * 0.15;
        }
        // 纵向移动
        if(nPos.y > oPos.y && (nPos.y - origin.y) + mSize.y > WindowHeight * 0.85) {
            origin.y = moveDist.y + mSize.y - WindowHeight * 0.85;
        } else if(nPos.y < oPos.y && (nPos.y - origin.y) < WindowHeight * 0.15) {
            origin.y = moveDist.y - WindowHeight * 0.15;
        }

        // 绘图部分
        // 背景刷新
        cleardevice();

        map->draw(origin);  // 画一个简单的地图
        man->draw(origin);  // 绘制人物
        // 调整原点和移动距离
        man->thinking(origin, moveDist);
    }

    return GAME_START;
}