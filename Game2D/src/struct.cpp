
#include "struct.h"

std::map<int, Timer*> Timer::timerMap;
std::map<std::string, Music*> Music::musicMap;
std::map<std::string, IMAGE*> MImage::imageMap;

// 读取资源文件
void Data::read(LPCSTR name, std::map<std::string, Data*>& dataMap) {
    std::fstream fp(name, std::ios::in | std::ios::out);
    std::string buf;
    // 存放返回的数据
    Data* data = nullptr;

    // 为了防止文件最后有换行，导致已经没有内容但还未到达文件尾，继续读取出错，应当添加结束符
    // 当还没到达文件尾，就继续读取
    while(fp.peek() != EOF) {
        fp >> buf;
        // 注释要用 [ 注释内容 ]
        if(buf == "[") {
            readNote(fp);
        } else if(buf == "end") {
            break;
        }
        // 资源文件
        else {
            // 此时 buf 是资源类型
            data = readSrc(fp);
            // 根据资源类型进行区分
            dataMap[buf] = data;
        }
    }
    // 关闭文件
    fp.close();
}

void Data::read(LPCSTR name, std::map<std::string, Data*>& dataMap, Mode mode) {
    std::fstream fp(name, std::ios::in | std::ios::out);
    std::string buf;
    // 存放返回的数据
    Data* data = nullptr;
    int count = 0;  // 记录读取的内容数

    // 当还没到达文件尾，就继续读取
    while(fp.peek() != EOF) {
        fp >> buf;
        // 注释要用 [ 注释内容 ]
        if(buf == "[") {
            readNote(fp);
        } else if(buf == "end") {
            break;
        }
        // 框架模式，用于重复大量出现的仅有位置不同的相同物体数据的读取
        else if(buf == "framework") {
            std::vector<Data*> v;
            readFramework(fp, v);
            for(auto it = v.begin(); it != v.end(); ++it) {
                (*it)->singles["id"] = ++count;
                // 随机字符串，直到该位置没有元素
                std::string fName;
                do {
                    fName = freeStr();
                } while(dataMap[fName] != nullptr);
                dataMap[fName] = (*it);
            }
        }
        // 开始读取物体数据
        else {
            // 此时 buf 是 {
            data = readObject(fp);
            // 在 {} 中要有名字 name
            // 自由模式，不指定映射的 Key
            // 另外，为了方便排序（map 插入时没有排序），我们应当手动添加 id
            data->singles["id"] = ++count;
            if(mode == ByName) {
                dataMap[data->details["name"]] = data;
            } else if(mode == ByFree) {
                // 随机字符串，直到该位置没有元素
                std::string fName;
                do {
                    fName = freeStr();
                } while(dataMap[fName] != nullptr);
                dataMap[fName] = data;
            }
        }
    }
    // 关闭文件
    fp.close();
}

Data* Data::readSrc(std::fstream& fp) {
    // 四种数据
    Data* data = new Data;

    std::string buf, STRData;
    // 第一个字符串是 "{" ，先读完
    fp >> buf;

    // 读取 {} 中的数据
    while(fp.peek() != EOF) {
        fp >> buf;
        // 注释要用 [ 注释内容 ]
        if(buf == "[") {
            readNote(fp);
        }
        // 读取完成
        else if(buf == "}") {
            return data;
        }
        // 此时读取的是资源名
        else {
            fp >> STRData;
            data->details[buf] = STRData;
        }
    }
    // 如果什么都没有读到，就删除
    delete data;
    return nullptr;
}

Data* Data::readObject(std::fstream& fp) {
    std::string buf;

    // 可读取的数据的临时存放
    double DData;
    POINT POINTData;
    std::string STRData;

    // 四种数据
    Data* data = new Data;

    // 读取 {} 中的数据
    while(fp.peek() != EOF) {
        fp >> buf;
        // 注释要用 [ 注释内容 ]
        if(buf == "[") {
            readNote(fp);
        }
        // 开始读取
        else if(buf == "Single") {
            fp >> buf >> DData;
            data->singles[buf] = DData;
        } else if(buf == "Detail") {
            fp >> buf >> STRData;
            data->details[buf] = STRData;
        } else if(buf == "Point") {
            fp >> buf >> POINTData.x >> POINTData.y;
            data->points[buf] = POINTData;
        }
        // 读取完成
        else if(buf == "}") {
            return data;
        }
    }
    // 如果什么也没读到就删除
    delete data;
    return nullptr;
}

void Data::readFramework(std::fstream& fp, std::vector<Data*>& dataList) {
    // 可读取的数据的临时存放，只读取位置信息
    int row = 0;              // 存放行位置
    int left = 0, right = 0;  // 左右范围
    std::string name, mode, dir;

    // 应当在开头读取名称，有格式： framework name  ... -1
    // -1 表示结束
    fp >> name;

    // 读取 {} 中的数据，不应当有注释
    while(fp.peek() != EOF) {
        // 读取行坐标，需要注意，row 其实是 y 方向的， col 是 x 方向的
        fp >> row;
        if(row == -1) {
            return;
        }
        // 读取方向和模式
        fp >> dir >> mode;
        // 开始读取列坐标
        // 列坐标按照特殊格式读取
        while(fp.peek() != EOF) {
            fp >> left;
            if(left == -1) {
                break;
            } else {
                if(mode == "link") {
                    // 连接模式
                    fp >> right;
                }
                // 点模式，设置左右相等
                else if(mode == "point") {
                    right = left;
                }
                // 注意左右都是可以到达的
                for(int i = left; i < right + 1; i++) {
                    Data* data = new Data;
                    data->details["name"] = name;
                    // 根据按行还是按列进行区分
                    if(dir == "row") {
                        data->points["pos"] = {i, row};
                    } else if(dir == "col") {
                        data->points["pos"] = {row, i};
                    }
                    dataList.push_back(data);
                }
            }
        }
    }
}

void Data::readNote(std::fstream& fp) {
    std::string buf;
    // 读取注释，不存储
    while(fp.peek() != EOF) {
        fp >> buf;
        if(buf == "]") {
            break;
        }
    }
}

std::string Data::freeStr() {
    int t = rand() % 10000;
    char buf[BUF_SIZE];
    sprintf_s(buf, "%d", t);
    return buf;
}

// ===================================================

void Event::read(LPCSTR name) {
    std::fstream fp(name, std::ios::in | std::ios::out);
    std::string buf;
    int count = 0;  // 记录读取的内容数

    // 当还没到达文件尾，就继续读取
    while(fp.peek() != EOF) {
        fp >> buf;
        // 注释要用 [ 注释内容 ]
        if(buf == "[") {
            // 直接使用 data 的注释读取函数
            Data::readNote(fp);
        } else if(buf == "end") {
            break;
        } else {
            eventMap[++count] = readEvent(fp);
        }
    }
    // 关闭文件
    fp.close();
}

Event* Event::readEvent(std::fstream& fp) {
    std::string command;  // 命令
    int value;            // 值
    Event* event = new Event;

    while(fp.peek() != EOF) {
        fp >> command;
        // 注释要用 [ 注释内容 ]
        if(command == "[") {
            // 直接使用 data 的注释读取函数
            Data::readNote(fp);
        }
        // 读取完成
        else if(command == "}") {
            return event;
        }
        // 否则就是命令
        else {
            // 纯命令参数直接推入 pause
            event->commandList.push_back(command);
            // 有一个参数
            if(command == "delay" || command == "delete" || command == "lock" || command == "unlock") {
                // 延时/id
                fp >> value;
                event->intList.push_back(value);
            }
            // 读取参数
            else if(command == "talk" || command == "tip" || command == "map") {
                // 对话内容/地图名
                fp >> command;
                event->commandList.push_back(command);
            }
            // 有两个参数
            else if(command == "origin") {
                // 位置
                fp >> value;
                event->intList.push_back(value);
                fp >> value;
                event->intList.push_back(value);
            }
            // 有三个参数
            else if(command == "move") {
                // id 位置
                fp >> value;
                event->intList.push_back(value);
                fp >> value;
                event->intList.push_back(value);
                fp >> value;
                event->intList.push_back(value);
            } else if(command == "create") {
                // 创建物体名
                fp >> command;
                event->commandList.push_back(command);
                // 创建位置（全局位置）
                fp >> value;
                event->intList.push_back(value);
                fp >> value;
                event->intList.push_back(value);
            } else if(command == "buff" || command == "debuff") {
                // 物体 id
                fp >> value;
                event->intList.push_back(value);
                // 属性
                fp >> command;
                event->commandList.push_back(command);
                // 加成
                fp >> value;
                event->intList.push_back(value);
            }
        }
    }
    delete event;
    return nullptr;
}

void Event::deleteEvent() {
    for(auto it = eventMap.begin(); it != eventMap.end(); ++it) {
        if(it->second != nullptr) {
            delete it->second;
            eventMap[it->first] = nullptr;
        }
    }
}