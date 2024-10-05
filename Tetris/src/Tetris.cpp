// Tetris.cpp : 定义应用程序的入口点。
//

#include "Tetris.h"

#include "class.h"
#include "framework.h"

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

// 绘制网格
void draw_bk() {
    setlinecolor(RGB(203, 203, 203));
    for(int i = 0; i < WIDTH / RECT_SIZE; i++) {
        line(RECT_SIZE * i, 0, RECT_SIZE * i, HEIGHT);
    }
    for(int i = 0; i < HEIGHT / RECT_SIZE; i++) {
        line(0, RECT_SIZE * i, WIDTH, RECT_SIZE * i);
    }
    setlinecolor(BLACK);
    line(WIDTH, 0, WIDTH, HEIGHT);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    srand((int)time(0));
    initgraph(WHOLE_WIDTH, HEIGHT);

    // 游戏进程
    bool game = true;
    bool game_process = true;
    bool hit_state = false;  // 按键按下

    // 初始化形状
    Rect_Size::create_rect();

    // 创建我的形状指针
    Rect_Size* my_rect = Rect_Size::share_rect();
    Rect_Size::create();  // 新建形状

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
    Timer protect_timer(HIT_SEP);

    setbkcolor(WHITE);

    BeginBatchDraw();
    while(game) {
        // 加载背景音乐
        play_music(TEST_DATA_PATH "/music/stand.mp3");

        // 死亡则停止
        if(!Rect_Size::get_live()) {
            game_process = false;
        }

        // 到时间则关闭
        if(protect_timer.t_time()) {
            protect_timer.close();
        }

        hit_state = false;
        cleardevice();
        draw_bk();

        // 按键检测，保护器关闭时检测
        if(!protect_timer.status()) {
            if(game_process) {
                if(GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')) {
                    hit_state = true;
                    my_rect->rotate();
                }
                if(GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')) {
                    hit_state = true;
                    my_rect->move(MOVE_DOWN);
                }
                if(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) {
                    hit_state = true;
                    my_rect->move(MOVE_LEFT);
                }
                if(GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) {
                    hit_state = true;
                    my_rect->move(MOVE_RIGHT);
                }
                if(GetAsyncKeyState(VK_SPACE)) {
                    hit_state = true;
                    while(my_rect->move(MOVE_DOWN)) {
                        // 向下直到碰撞
                    }
                }
            }

            // 暂停
            if(GetAsyncKeyState('P')) {
                hit_state = true;
                game_process = !game_process;
            }
        }

        // 有按键则开启保护
        if(hit_state) {
            protect_timer.open();
        }

        if(Rect::move_timer.t_time() && game_process) {
            my_rect->move(MOVE_DOWN);
        }

        Rect_Size::show();

        int o_x, o_y;
        my_rect->rotate_center(o_x, o_y);

        // 设置字体
        settextcolor(BLACK);
        setbkmode(TRANSPARENT);
        LOGFONT f;
        gettextstyle(&f);                     // 获取文字格式
        f.lfQuality = ANTIALIASED_QUALITY;    // 抗锯齿
        f.lfHeight = 30;                      // 字高
        f.lfWeight = 10;                      // 字重
        _tcscpy_s(f.lfFaceName, "Consolas");  // 字体
        settextstyle(&f);

        // char str[BUF_SIZE];
        // sprintf_s(str, "旋转中心：(%d, %d)", o_x / RECT_SIZE, o_y / RECT_SIZE);
        // outtextxy(10, 10, str);

        if(!game_process) {
            // 选项
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
                        // 死亡不播放继续音乐
                        if(Rect_Size::get_live() || chosen_id != 0) {
                            play_music(TEST_DATA_PATH "/music/button.wav");
                        }
                    }
                    break;
                case WM_LBUTTONDOWN: {
                    switch(chosen_id) {
                        case 0:
                            game_process = true;
                            protect_timer.close();
                            break;
                        case 1:
                            game_process = true;
                            protect_timer.close();
                            // 清除类
                            Rect_Size::delete_rect();
                            // 初始化形状
                            Rect_Size::create_rect();

                            // 创建我的形状指针
                            my_rect = Rect_Size::share_rect();
                            Rect_Size::create();  // 新建形状

                            break;
                        case 2:
                            // 清除类
                            Rect_Size::delete_rect();

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
                if(i == 0 && !Rect_Size::get_live()) {
                    continue;
                }
                putimage(option_x, option_y + (OPTION_HEIGHT + 25) * (i - 1), &option_img[6], NOTSRCERASE);
                putimage(option_x, option_y + (OPTION_HEIGHT + 25) * (i - 1), &option_img[put_index[i]], SRCINVERT);
            }

            if(!Rect_Size::get_live()) {
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

                // 画一个框
                setfillcolor(RGB(229, 233, 231));
                setlinecolor(BLACK);
                fillroundrect((WIDTH - text_w - 40) / 2, HEIGHT / 4 - 20, (WIDTH + text_w + 40) / 2, HEIGHT / 4 + text_h + 20, 10, 10);

                outtextxy((WIDTH - text_w) / 2, HEIGHT / 4, game_over_text);
            }
        }

        FlushBatchDraw();
    }

    EndBatchDraw();
    closegraph();

    // 清除类
    Rect_Size::delete_rect();

    return 0;
}
