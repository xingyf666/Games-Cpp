
#include "class.h"

#include "Tetris.h"

// 回到开头
void Music::init() {
    char command_name[BUF_SIZE] = "seek ";
    strcat_s(command_name, command);
    strcat_s(command_name, " to start");

    mciSendString(command_name, 0, 0, 0);
}

// 打开音乐（支持多种音乐格式）
void Music::open() {
    char command_name[BUF_SIZE] = "open ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 播放
void Music::play() {
    char command_name[BUF_SIZE] = "play ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 暂停
void Music::pause() {
    char command_name[BUF_SIZE] = "pause ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 继续
void Music::resume() {
    char command_name[BUF_SIZE] = "resume ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 结束
void Music::stop() {
    char command_name[BUF_SIZE] = "stop ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 关闭
void Music::close() {
    char command_name[BUF_SIZE] = "close ";
    strcat_s(command_name, command);
    mciSendString(command_name, 0, 0, 0);
}

// 获取设备状态
void Music::status(char buf[]) {
    char command_name[BUF_SIZE] = "status ";
    strcat_s(command_name, command);
    strcat_s(command_name, " mode");

    // 记录状态
    mciSendString(command_name, buf, BUF_SIZE, 0);
}

// 获取文件长度
void Music::length(char buf[]) {
    char command_name[BUF_SIZE] = "status ";
    strcat_s(command_name, command);
    strcat_s(command_name, " length");

    // 记录状态
    mciSendString(command_name, buf, BUF_SIZE, 0);
}

// 矩形方法
Timer Rect::move_timer(MOVE_SEP);

bool Rect::collision(const Rect* rect) {
    // 除自己以外，完全重叠则碰撞
    if(rect != this && left == rect->left && right == rect->right && top == rect->top && bottom == rect->bottom) {
        return true;
    }
    return false;
}

int Rect::collision(int len, Rect** rect) {
    for(int i = 0; i < len; i++) {
        if(collision(rect[i])) {
            return i;
        }
    }
    return -1;
}

bool Rect::point_in(int point_x, int point_y) {
    if(point_x >= left && point_x <= right) {
        if(point_y >= top && point_y <= bottom) {
            return true;
        }
    }
    return false;
}

// 绕定点顺时针旋转90度
bool Rect::rotate_clock(int o_x, int o_y) {
    // 需要注意坐标的方向，y轴是向下的，因此正常坐标系下的顺时针在这里是逆时针
    // 相对于定点的坐标
    int d_x = left - o_x;
    int d_y = top - o_y;

    right = o_x - d_y;
    top = o_y + d_x;

    left = right - width;
    bottom = top + height;

    if(left < 0 || bottom > HEIGHT || right > WIDTH) {
        return false;
    }
    return true;
}

void Rect::rotate_anticlock(int o_x, int o_y) {
    // 相对于定点的坐标
    int d_x = left - o_x;
    int d_y = top - o_y;

    left = o_x + d_y;
    bottom = o_y - d_x;

    right = left + width;
    top = bottom - height;
}

void Rect::show() {
    setfillcolor(BLUE);
    setlinecolor(WHITE);
    fillrectangle(left, top, right, bottom);
}

bool Rect::move(Direct dir) {
    switch(dir) {
        case MOVE_LEFT:
            left -= width;
            right -= width;

            if(left < 0 || right > WIDTH) {
                return false;
            }
            break;
        case MOVE_RIGHT:
            left += width;
            right += width;

            if(right > WIDTH || left < 0) {
                return false;
            }
            break;
        case MOVE_DOWN:
            top += height;
            bottom += height;

            if(bottom > HEIGHT) {
                return false;
            }
            break;
        case MOVE_UP:
            break;
    }

    return true;
}

void Rect::back(Direct dir) {
    switch(dir) {
        case MOVE_LEFT:
            left += width;
            right += width;
            break;
        case MOVE_RIGHT:
            left -= width;
            right -= width;
            break;
        case MOVE_DOWN:
            top -= height;
            bottom -= height;
            break;
        case MOVE_UP:
            break;
    }
}

// 方块方法
bool Rect_Size::alive = true;
int Rect_Size::count = 0;
Rect** Rect_Size::rect_list = nullptr;
Rect_Size* Rect_Size::my_rect = nullptr;
Type Rect_Size::show_type = SIZE_01_10_11_12;

void Rect_Size::show() {
    for(int i = 0; i < count; i++) {
        rect_list[i]->show();
    }
    // 展示框
    setlinecolor(BLACK);
    rectangle(SHOW_X - SHOW_WIDTH / 2, SHOW_Y - SHOW_HEIGHT / 2, SHOW_X + SHOW_WIDTH / 2, SHOW_Y + SHOW_HEIGHT / 2);

    // 展示下一个形状
    setfillcolor(BLUE);
    setlinecolor(WHITE);

    switch(show_type) {
        case SIZE_01_10_11_12:
            fillrectangle(SHOW_X - RECT_SIZE / 2, SHOW_Y - RECT_SIZE, SHOW_X + RECT_SIZE / 2, SHOW_Y);
            fillrectangle(SHOW_X - 3 * RECT_SIZE / 2, SHOW_Y, SHOW_X - RECT_SIZE / 2, SHOW_Y + RECT_SIZE);
            fillrectangle(SHOW_X - RECT_SIZE / 2, SHOW_Y, SHOW_X + RECT_SIZE / 2, SHOW_Y + RECT_SIZE);
            fillrectangle(SHOW_X + RECT_SIZE / 2, SHOW_Y, SHOW_X + 3 * RECT_SIZE / 2, SHOW_Y + RECT_SIZE);
            break;
        case SIZE_00_01_10_11:
            fillrectangle(SHOW_X - RECT_SIZE, SHOW_Y - RECT_SIZE, SHOW_X, SHOW_Y);
            fillrectangle(SHOW_X, SHOW_Y - RECT_SIZE, SHOW_X + RECT_SIZE, SHOW_Y);
            fillrectangle(SHOW_X - RECT_SIZE, SHOW_Y, SHOW_X, SHOW_Y + RECT_SIZE);
            fillrectangle(SHOW_X, SHOW_Y, SHOW_X + RECT_SIZE, SHOW_Y + RECT_SIZE);
            break;
        case SIZE_00_01_02_03:
            fillrectangle(SHOW_X - 2 * RECT_SIZE, SHOW_Y - RECT_SIZE / 2, SHOW_X - RECT_SIZE, SHOW_Y + RECT_SIZE / 2);
            fillrectangle(SHOW_X - RECT_SIZE, SHOW_Y - RECT_SIZE / 2, SHOW_X, SHOW_Y + RECT_SIZE / 2);
            fillrectangle(SHOW_X, SHOW_Y - RECT_SIZE / 2, SHOW_X + RECT_SIZE, SHOW_Y + RECT_SIZE / 2);
            fillrectangle(SHOW_X + RECT_SIZE, SHOW_Y - RECT_SIZE / 2, SHOW_X + 2 * RECT_SIZE, SHOW_Y + RECT_SIZE / 2);
            break;
        case SIZE_00_01_11_12:
            setfillcolor(BLUE);
            setlinecolor(WHITE);

            fillrectangle(SHOW_X - 3 * RECT_SIZE / 2, SHOW_Y - RECT_SIZE, SHOW_X - RECT_SIZE / 2, SHOW_Y);
            fillrectangle(SHOW_X - RECT_SIZE / 2, SHOW_Y - RECT_SIZE, SHOW_X + RECT_SIZE / 2, SHOW_Y);
            fillrectangle(SHOW_X - RECT_SIZE / 2, SHOW_Y, SHOW_X + RECT_SIZE / 2, SHOW_Y + RECT_SIZE);
            fillrectangle(SHOW_X + RECT_SIZE / 2, SHOW_Y, SHOW_X + 3 * RECT_SIZE / 2, SHOW_Y + RECT_SIZE);
            break;
        case SIZE_00_10_20_21:
            fillrectangle(SHOW_X - RECT_SIZE, SHOW_Y - 3 * RECT_SIZE / 2, SHOW_X, SHOW_Y - RECT_SIZE / 2);
            fillrectangle(SHOW_X - RECT_SIZE, SHOW_Y - RECT_SIZE / 2, SHOW_X, SHOW_Y + RECT_SIZE / 2);
            fillrectangle(SHOW_X - RECT_SIZE, SHOW_Y + RECT_SIZE / 2, SHOW_X, SHOW_Y + 3 * RECT_SIZE / 2);
            fillrectangle(SHOW_X, SHOW_Y + RECT_SIZE / 2, SHOW_X + RECT_SIZE, SHOW_Y + 3 * RECT_SIZE / 2);
            break;
    }
}

bool Rect_Size::move(Direct dir) {
    // 检测是否移动成功
    bool check = true;
    for(int i = 0; i < 4; i++) {
        // 移动不成功
        if(!rect[i]->move(dir)) {
            check = false;
        }
    }
    for(int i = 0; i < 4; i++) {
        // 移动不成功
        if(rect[i]->collision(count, rect_list) != -1) {
            check = false;
        }
    }
    // 不成功则返回，然后如果触底创建新的形状，同时检测是否可以消行
    if(!check) {
        for(int i = 0; i < 4; i++) {
            // 当最高点超出，则死亡
            if(rect[i]->top < 0 && dir == MOVE_DOWN) {
                alive = false;
            }
            rect[i]->back(dir);
        }
        if(dir == MOVE_DOWN) {
            // 在创建之前test，防止消去行可能导致的多次向下位移
            Rect_Size::test_line();
            Rect_Size::create();
        }
    }
    return check;
}

void Rect_Size::create() {
    if(count + 4 < MAX_NUM) {
        int x = (rand() % 12) * RECT_SIZE;
        int y = -3 * RECT_SIZE;

        switch(show_type) {
            case SIZE_01_10_11_12:
                rect_list[count] = new Rect(x + RECT_SIZE, y);
                rect_list[count + 1] = new Rect(x, y + RECT_SIZE);
                rect_list[count + 2] = new Rect(x + RECT_SIZE, y + RECT_SIZE);
                rect_list[count + 3] = new Rect(x + RECT_SIZE * 2, y + RECT_SIZE);
                break;
            case SIZE_00_01_10_11:
                rect_list[count] = new Rect(x, y);
                rect_list[count + 1] = new Rect(x + RECT_SIZE, y);
                rect_list[count + 2] = new Rect(x, y + RECT_SIZE);
                rect_list[count + 3] = new Rect(x + RECT_SIZE, y + RECT_SIZE);
                break;
            case SIZE_00_01_02_03:
                rect_list[count] = new Rect(x, y);
                rect_list[count + 1] = new Rect(x + RECT_SIZE, y);
                rect_list[count + 2] = new Rect(x + RECT_SIZE * 2, y);
                rect_list[count + 3] = new Rect(x + RECT_SIZE * 3, y);
                break;
            case SIZE_00_01_11_12:
                rect_list[count] = new Rect(x, y);
                rect_list[count + 1] = new Rect(x + RECT_SIZE, y);
                rect_list[count + 2] = new Rect(x + RECT_SIZE, y + RECT_SIZE);
                rect_list[count + 3] = new Rect(x + RECT_SIZE * 2, y + RECT_SIZE);
                break;
            case SIZE_00_10_20_21:
                rect_list[count] = new Rect(x, y);
                rect_list[count + 1] = new Rect(x, y + RECT_SIZE);
                rect_list[count + 2] = new Rect(x, y + RECT_SIZE * 2);
                rect_list[count + 3] = new Rect(x + RECT_SIZE, y + RECT_SIZE * 2);
                break;
        }

        // 将新的形状赋给my_rect
        int id_list[4] = {count, count + 1, count + 2, count + 3};
        my_rect->set(show_type, id_list);

        // 产生新的类型
        int id = rand() % 5;
        show_type = SIZE_01_10_11_12 + id;

        count += 4;
    }
}

void Rect_Size::distory(Rect* rect_ptr) {
    int id = 0;
    // 找到指针位置
    for(int i = 0; i < count; i++) {
        if(rect_ptr == rect_list[i]) {
            id = i;
            break;
        }
    }

    // 删除并移动指针
    delete rect_ptr;
    for(int i = id; i < count - 1; i++) {
        rect_list[i] = rect_list[i + 1];
    }
    rect_list[count - 1] = nullptr;
    count--;
}

void Rect_Size::test_line() {
    Rect* bk_map[HEIGHT / RECT_SIZE][WIDTH / RECT_SIZE];
    // 初始化为nullptr
    for(int i = 0; i < HEIGHT / RECT_SIZE; i++) {
        for(int j = 0; j < WIDTH / RECT_SIZE; j++) {
            bk_map[i][j] = nullptr;
        }
    }
    // 记录在对应行列位置的方块指针
    for(int i = 0; i < count; i++) {
        // x行,y列
        int bk_x = rect_list[i]->top / RECT_SIZE;
        int bk_y = rect_list[i]->left / RECT_SIZE;
        bk_map[bk_x][bk_y] = rect_list[i];
    }
    // 检测行
    for(int i = 0; i < HEIGHT / RECT_SIZE; i++) {
        bool test = true;
        // 如果一行全不为-1，则消去
        for(int j = 0; j < WIDTH / RECT_SIZE; j++) {
            if(bk_map[i][j] == nullptr) {
                test = false;
            }
        }
        if(test) {
            // 消去整行
            for(int j = 0; j < WIDTH / RECT_SIZE; j++) {
                distory(bk_map[i][j]);
            }
            // 将i行上方的方块向下移动，注意要倒着移动，防止重叠
            for(int p = i - 1; p >= 0; p--) {
                for(int q = 0; q < WIDTH / RECT_SIZE; q++) {
                    // 当不为空时移动
                    Rect* mov_rect = bk_map[p][q];
                    if(mov_rect != nullptr) {
                        mov_rect->move(MOVE_DOWN);
                    }
                }
            }
            // 播放音乐
            play_music("./music/clear.wav");
        }
    }
}

void Rect_Size::rotate_center(int& o_x, int& o_y) {
    int sum_x = 0, sum_y = 0;
    for(int i = 0; i < 4; i++) {
        sum_x += rect[i]->right + rect[i]->left;
        sum_y += rect[i]->top + rect[i]->bottom;
    }
    // 求取中心点
    o_x = ((sum_x / 8) / 30) * 30;
    o_y = ((sum_y / 8) / 30) * 30;
}

void Rect_Size::rotate() {
    int o_x, o_y;
    bool check = true;

    rotate_center(o_x, o_y);

    for(int i = 0; i < 4; i++) {
        if(!rect[i]->rotate_clock(o_x, o_y)) {
            check = false;
        }
    }
    for(int i = 0; i < 4; i++) {
        // 旋转不成功
        if(rect[i]->collision(count, rect_list) != -1) {
            check = false;
        }
    }
    if(!check && move(MOVE_RIGHT)) {
        check = true;
    }
    if(!check) {
        for(int i = 0; i < 4; i++) {
            rect[i]->rotate_anticlock(o_x, o_y);
        }
    }
}
