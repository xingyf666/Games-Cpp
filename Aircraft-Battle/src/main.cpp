// Aircraft battle.cpp : 定义应用程序的入口点。
//

#include "class.h"
#include "framework.h"
#include "header.h"
#include "plane.h"


using namespace std;

// 播放音乐的方法
void play_music(const char _arr[]) {
    Music music(_arr);
    music.open();

    char buf[BUF_SIZE];
    music.status(buf);

    if(buf[0] == 's') {
        music.init();
        music.play();
    }
}

// 画血条
void blood_line(int x, int y, int len, double ratio) {
    setlinestyle(PS_SOLID, 3);
    setlinecolor(BLACK);
    line(x, y, x + len, y);
    if(ratio > 0.4) {
        setlinecolor(GREEN);
    } else {
        setlinecolor(RED);
    }
    line(x, y, (int)(x + len * ratio), y);
}

// 主程序入口
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    srand((int)time(0));
    initgraph(WIDTH, HEIGHT);

    // 加载背景图片
    IMAGE bk_img;
    loadimage(&bk_img, TEST_DATA_PATH "/images/background.png");

    // 游戏选项图片
    IMAGE option_img[7];
    loadimage(&option_img[0], TEST_DATA_PATH "/images/continue_nor.png");
    loadimage(&option_img[1], TEST_DATA_PATH "/images/again_nor.png");
    loadimage(&option_img[2], TEST_DATA_PATH "/images/gameover_nor.png");

    loadimage(&option_img[3], TEST_DATA_PATH "/images/continue_nor1.png");
    loadimage(&option_img[4], TEST_DATA_PATH "/images/again_nor1.png");
    loadimage(&option_img[5], TEST_DATA_PATH "/images/gameover_nor1.png");

    loadimage(&option_img[6], TEST_DATA_PATH "/images/nor.png");  // 掩码

    // 输出索引的数组
    int put_index[3] = {0, 1, 2};
    int chosen_id = -1;  // 选中的图片

    // 中间选项的位置
    int option_x = (WIDTH - OPTION_WIDTH) / 2;
    int option_y = (HEIGHT - OPTION_HEIGHT) / 2;

    // 选项的矩形
    Rect continue_rect(option_x, option_y - OPTION_HEIGHT - 25, OPTION_WIDTH, OPTION_HEIGHT);
    Rect again_rect(option_x, option_y, OPTION_WIDTH, OPTION_HEIGHT);
    Rect gameover_rect(option_x, option_y + OPTION_HEIGHT + 25, OPTION_WIDTH, OPTION_HEIGHT);

    // 按键保护计时器
    Timer protect_timer(PROTECT_TIME);

    MyPlane* my_plane = MyPlane::share_plane();  // 我的飞机

    // 初始化类
    Bullet::init();
    Enemy::init();

    // 游戏进程
    bool game_process = true;
    bool game = true;
    bool my_alive = true;

    BeginBatchDraw();
    while(game) {
        // 加载背景音乐
        play_music(TEST_DATA_PATH "/music/KOTOKO.mp3");

        putimage(0, 0, &bk_img);
        // 死亡则停止
        if(!my_alive) {
            game_process = false;
        }

        // 保护器到时间就关闭
        if(protect_timer.t_time()) {
            protect_timer.close();
        }

        if(!protect_timer.status() && GetAsyncKeyState('P')) {
            protect_timer.open();
            game_process = !game_process;
        }

        if(game_process) {
            // 敌机操作
            Enemy::test_live();
            Enemy::create();
            Enemy::move();
            Enemy::collision();
            Enemy::show();

            // 补给
            Supply::create();
            Supply::action();

            // 我的飞机操作
            my_plane->move();
            my_plane->show();
            my_plane->fire();
            my_plane->use_boom();
            my_alive = my_plane->test_live();

            // 子弹操作
            Bullet::move();
            Bullet::test_live();
            Bullet::show();
        } else {
            Enemy::show();
            if(my_alive) {
                my_plane->show();
                Bullet::show();
            }

            int x = 0, y = 0;
            int record_id = chosen_id;

            ExMessage msg;
            peekmessage(&msg, EM_MOUSE);
            switch(msg.message) {
                case WM_MOUSEMOVE:
                    x = msg.x, y = msg.y;
                    chosen_id = -1;
                    put_index[0] = 0;
                    put_index[1] = 1;
                    put_index[2] = 2;

                    if(continue_rect.point_in(x, y)) {
                        put_index[0] = 3;
                        chosen_id = 0;
                    } else if(again_rect.point_in(x, y)) {
                        put_index[1] = 4;
                        chosen_id = 1;
                    } else if(gameover_rect.point_in(x, y)) {
                        put_index[2] = 5;
                        chosen_id = 2;
                    }
                    // 当前后id不同且不为-1才播放
                    if(chosen_id != record_id && chosen_id != -1) {
                        // 死亡不播放第一个音乐
                        if(my_alive || chosen_id != 0) {
                            play_music(TEST_DATA_PATH "/music/button.wav");
                        }
                    }
                    break;
                case WM_LBUTTONDOWN: {
                    switch(chosen_id) {
                        case 0:
                            game_process = true;
                            protect_timer.set(0);
                            break;
                        case 1:
                            my_alive = true;
                            game_process = true;
                            protect_timer.set(0);
                            // 清除所有子弹和敌人
                            Bullet::distory();
                            Enemy::distory();
                            Supply::delete_supply();

                            MyPlane::share_plane()->init_plane();  // 我的飞机初始化
                            break;
                        case 2:
                            // 清除类
                            MyPlane::delete_plane();
                            Bullet::delete_bullet();
                            Enemy::delete_enemy();
                            Supply::delete_supply();

                            // 退出窗口
                            HWND hWnd = GetHWnd();
                            SendMessage(hWnd, WM_CLOSE, NULL, NULL);
                            break;
                    }
                    break;
                }
                default:
                    break;
            }

            // 显示游戏选项
            for(int i = 0; i < 3; i++) {
                if(i == 0 && !my_alive) {
                    continue;
                }
                putimage(option_x, option_y + (OPTION_HEIGHT + 25) * (i - 1), &option_img[6], NOTSRCERASE);
                putimage(option_x, option_y + (OPTION_HEIGHT + 25) * (i - 1), &option_img[put_index[i]], SRCINVERT);
            }

            if(!my_alive) {
                // 设置字体
                LOGFONT f;
                gettextstyle(&f);                     // 获取文字格式
                f.lfQuality = ANTIALIASED_QUALITY;    // 抗锯齿
                f.lfHeight = 50;                      // 字高
                f.lfWeight = 10;                      // 字重
                _tcscpy_s(f.lfFaceName, "Consolas");  // 字体
                settextstyle(&f);

                char game_over_text[15] = "游戏结束";
                int text_w = textwidth(game_over_text);
                int text_h = textheight(game_over_text);
                outtextxy((WIDTH - text_w) / 2, HEIGHT / 4, game_over_text);
            }
        }
        FlushBatchDraw();
    }

    EndBatchDraw();
    closegraph();

    // 清除类
    MyPlane::delete_plane();
    Bullet::delete_bullet();
    Enemy::delete_enemy();
    Supply::delete_supply();

    return 0;
}
