// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN  // 从 Windows 头文件中排除极少使用的内容
// Windows 头文件
#include <conio.h>
#include <graphics.h>  // 引用图形库头文件

// C 运行时头文件
#include <malloc.h>
#include <memory.h>
#include <mmsystem.h>  //包含多媒体设备接口头文件
#include <stdlib.h>
#include <tchar.h>

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

#pragma comment(lib, "winmm.lib")  // 加载静态库
