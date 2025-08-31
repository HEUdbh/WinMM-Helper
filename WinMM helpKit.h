#pragma once

#include "resource.h"

// 内存地址偏移常量
#define PERSON_X_BASE_OFFSET        0xAC54C    // WinMM.exe基地址偏移
#define PERSON_X_LEVEL1_OFFSET      0x60       // 第一级指针偏移
#define PERSON_X_LEVEL2_OFFSET      0x58       // 第二级指针偏移

// 人物Y坐标偏移常量
#define PERSON_Y_BASE_OFFSET        0xAC54C    // WinMM.exe基地址偏移（与X坐标相同）
#define PERSON_Y_LEVEL1_OFFSET      0x60       // 第一级指针偏移
#define PERSON_Y_LEVEL2_OFFSET      0x5C       // 第二级指针偏移

// 怪物1坐标偏移常量
#define MONSTER1_X_BASE_OFFSET      0xAC54C    // WinMM.exe基地址偏移（与人物相同）
#define MONSTER1_X_LEVEL1_OFFSET    0x64       // 第一级指针偏移
#define MONSTER1_X_LEVEL2_OFFSET    0x58       // 第二级指针偏移

#define MONSTER1_Y_BASE_OFFSET      0xAC54C    // WinMM.exe基地址偏移（与人物相同）
#define MONSTER1_Y_LEVEL1_OFFSET    0x64       // 第一级指针偏移
#define MONSTER1_Y_LEVEL2_OFFSET    0x5C       // 第二级指针偏移

// 怪物2坐标偏移常量
#define MONSTER2_X_BASE_OFFSET      0xAC54C    // WinMM.exe基地址偏移（与人物相同）
#define MONSTER2_X_LEVEL1_OFFSET    0x68       // 第一级指针偏移
#define MONSTER2_X_LEVEL2_OFFSET    0x58       // 第二级指针偏移

#define MONSTER2_Y_BASE_OFFSET      0xAC54C    // WinMM.exe基地址偏移（与人物相同）
#define MONSTER2_Y_LEVEL1_OFFSET    0x68       // 第一级指针偏移
#define MONSTER2_Y_LEVEL2_OFFSET    0x5C       // 第二级指针偏移

// 蝎子坐标偏移常量
#define SCORPION_X_BASE_OFFSET      0xAC54C    // WinMM.exe基地址偏移（与人物相同）
#define SCORPION_X_LEVEL1_OFFSET    0x6C       // 第一级指针偏移
#define SCORPION_X_LEVEL2_OFFSET    0x60       // 第二级指针偏移

#define SCORPION_Y_BASE_OFFSET      0xAC54C    // WinMM.exe基地址偏移（与人物相同）
#define SCORPION_Y_LEVEL1_OFFSET    0x6C       // 第一级指针偏移
#define SCORPION_Y_LEVEL2_OFFSET    0x64       // 第二级指针偏移

// 锁定功能相关常量
#define LOCK_CHECK_INTERVAL         100        // 锁定检测间隔（毫秒）

// 控件数组索引常量 - 防止面板间控件冲突
#define HELP_PANEL_BASE_INDEX       0          // 说明面板基础索引
#define PERSON_PANEL_BASE_INDEX     4          // 人物面板基础索引
#define MONSTER1_PANEL_BASE_INDEX   8          // 怪物1面板基础索引
#define MONSTER2_PANEL_BASE_INDEX   12         // 怪物2面板基础索引
#define SCORPION_PANEL_BASE_INDEX   16         // 蝎子面板基础索引
