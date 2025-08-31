// WinMM helpKit.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WinMM helpKit.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <stdio.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 进程扫描相关全局变量
DWORD targetProcessId = 0;                      // 目标进程ID
HANDLE targetProcessHandle = NULL;              // 目标进程句柄
DWORD_PTR targetBaseAddress = 0;                // 目标进程基地址

// 侧边栏相关全局变量
HWND hSidebarButtons[5];                        // 侧边栏按钮句柄数组
HWND hCurrentInputPanel = NULL;                 // 当前显示的输入面板
HWND hInputControls[20];                        // 输入控件句柄数组
int currentSelectedCategory = -1;               // 当前选中的分类(-1表示无选择)

// 进程检测相关全局变量
HWND hStatusLabel = NULL;                       // 状态标签句柄
HWND hProcessStatus = NULL;                     // 进程状态显示句柄
BOOL isProcessConnected = FALSE;                // 进程连接状态
UINT_PTR processCheckTimer = 0;                 // 进程检测定时器

// 内存操作相关全局变量
DWORD_PTR personXAddress = 0;                   // 人物X坐标的最终地址
DWORD_PTR personYAddress = 0;                   // 人物Y坐标的最终地址
DWORD_PTR monster1XAddress = 0;                 // 怪物1 X坐标的最终地址
DWORD_PTR monster1YAddress = 0;                 // 怪物1 Y坐标的最终地址
DWORD_PTR monster2XAddress = 0;                 // 怪物2 X坐标的最终地址
DWORD_PTR monster2YAddress = 0;                 // 怪物2 Y坐标的最终地址
DWORD_PTR scorpionXAddress = 0;                 // 蝎子 X坐标的最终地址
DWORD_PTR scorpionYAddress = 0;                 // 蝎子 Y坐标的最终地址
BOOL isMemoryAddressValid = FALSE;              // 内存地址是否有效
BOOL isPersonYAddressValid = FALSE;             // 人物Y坐标地址是否有效
BOOL isMonster1XAddressValid = FALSE;           // 怪物1 X坐标地址是否有效
BOOL isMonster1YAddressValid = FALSE;           // 怪物1 Y坐标地址是否有效
BOOL isMonster2XAddressValid = FALSE;           // 怪物2 X坐标地址是否有效
BOOL isMonster2YAddressValid = FALSE;           // 怪物2 Y坐标地址是否有效
BOOL isScorpionXAddressValid = FALSE;           // 蝎子 X坐标地址是否有效
BOOL isScorpionYAddressValid = FALSE;           // 蝎子 Y坐标地址是否有效

// 锁定功能相关全局变量
BOOL isMonster1Locked = FALSE;                  // 怪物1是否锁定
BOOL isMonster2Locked = FALSE;                  // 怪物2是否锁定
BOOL isScorpionLocked = FALSE;                  // 蝎子是否锁定
int monster1LockedX = 0;                        // 怪物1锁定的X坐标值
int monster1LockedY = 0;                        // 怪物1锁定的Y坐标值
int monster2LockedX = 0;                        // 怪物2锁定的X坐标值
int monster2LockedY = 0;                        // 怪物2锁定的Y坐标值
int scorpionLockedX = 0;                        // 蝎子锁定的X坐标值
int scorpionLockedY = 0;                        // 蝎子锁定的Y坐标值
HWND hMonster1LockCheckbox = NULL;              // 怪物1锁定复选框句柄
HWND hMonster2LockCheckbox = NULL;              // 怪物2锁定复选框句柄
HWND hScorpionLockCheckbox = NULL;              // 蝎子锁定复选框句柄
HWND hRefreshButton = NULL;                     // 刷新按钮句柄
UINT_PTR lockCheckTimer = 0;                    // 锁定检测定时器

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL                ScanTargetProcess();
void                CreateSidebar(HWND hWnd);
void                CreateInputPanel(HWND hWnd, int category);
void                CreateHelpPanel(HWND hWnd);
void                HideAllInputPanels();
void                ShowInputPanel(int category);
void                CreateProcessStatusControls(HWND hWnd);
void                UpdateProcessStatus();
void                StartProcessDetection(HWND hWnd);
void                StopProcessDetection();
void                ShowProcessWarning(HWND hWnd);
BOOL                ReadMemoryValue(DWORD_PTR address, void* buffer, SIZE_T size);
BOOL                WriteMemoryValue(DWORD_PTR address, void* buffer, SIZE_T size);
DWORD_PTR           ResolvePointerChain(DWORD_PTR baseAddress, int* offsets, int offsetCount);
void                UpdatePersonXCoordinate();
void                WritePersonXCoordinate(int value);
void                UpdatePersonYCoordinate();
void                WritePersonYCoordinate(int value);
void                UpdateMonster1XCoordinate();
void                WriteMonster1XCoordinate(int value);
void                UpdateMonster1YCoordinate();
void                WriteMonster1YCoordinate(int value);
void                UpdateMonster2XCoordinate();
void                WriteMonster2XCoordinate(int value);
void                UpdateMonster2YCoordinate();
void                WriteMonster2YCoordinate(int value);
void                UpdateScorpionXCoordinate();
void                WriteScorpionXCoordinate(int value);
void                UpdateScorpionYCoordinate();
void                WriteScorpionYCoordinate(int value);
void                CheckMemoryAddresses();
void                ShowMemoryWarning(HWND hWnd, const wchar_t* message);
void                CreateLockCheckboxes(HWND hWnd);
void                StartLockDetection(HWND hWnd);
void                StopLockDetection();
void                CheckAndRestoreLockedValues();
void                LockMonster1Coordinates();
void                LockMonster2Coordinates();
void                LockScorpionCoordinates();
void                UnlockMonster1Coordinates();
void                UnlockMonster2Coordinates();
void                UnlockScorpionCoordinates();
void                CreateRefreshButton(HWND hWnd);
void                PerformManualRefresh(HWND hWnd);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINMMHELPKIT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINMMHELPKIT));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINMMHELPKIT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINMMHELPKIT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   // 创建侧边栏
   CreateSidebar(hWnd);
   
       // 创建进程状态控件
    CreateProcessStatusControls(hWnd);
    
    // 创建锁定复选框
    CreateLockCheckboxes(hWnd);
    
    // 创建刷新按钮
    CreateRefreshButton(hWnd);
    
        // 启动进程检测
    StartProcessDetection(hWnd);
    
    // 默认显示说明面板
    ShowInputPanel(0);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            int wmEvent = HIWORD(wParam);
            
            // 处理输入框内容变化
            if (wmEvent == EN_CHANGE && wmId == IDC_PERSON_X_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemory = FALSE;
                if (!isUpdatingFromMemory)
                {
                    WritePersonXCoordinate(value);
                }
                break;
            }
            else if (wmEvent == EN_CHANGE && wmId == IDC_PERSON_Y_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemoryY = FALSE;
                if (!isUpdatingFromMemoryY)
                {
                    WritePersonYCoordinate(value);
                }
                break;
            }
            else if (wmEvent == EN_CHANGE && wmId == IDC_MONSTER1_X_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemoryMonster1X = FALSE;
                if (!isUpdatingFromMemoryMonster1X)
                {
                    WriteMonster1XCoordinate(value);
                }
                break;
            }
            else if (wmEvent == EN_CHANGE && wmId == IDC_MONSTER1_Y_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemoryMonster1Y = FALSE;
                if (!isUpdatingFromMemoryMonster1Y)
                {
                    WriteMonster1YCoordinate(value);
                }
                break;
            }
            else if (wmEvent == EN_CHANGE && wmId == IDC_MONSTER2_X_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemoryMonster2X = FALSE;
                if (!isUpdatingFromMemoryMonster2X)
                {
                    WriteMonster2XCoordinate(value);
                }
                break;
            }
            else if (wmEvent == EN_CHANGE && wmId == IDC_MONSTER2_Y_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemoryMonster2Y = FALSE;
                if (!isUpdatingFromMemoryMonster2Y)
                {
                    WriteMonster2YCoordinate(value);
                }
                break;
            }
            else if (wmEvent == EN_CHANGE && wmId == IDC_SCORPION_X_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemoryScorpionX = FALSE;
                if (!isUpdatingFromMemoryScorpionX)
                {
                    WriteScorpionXCoordinate(value);
                }
                break;
            }
            else if (wmEvent == EN_CHANGE && wmId == IDC_SCORPION_Y_EDIT)
            {
                // 获取输入框内容并写入内存
                wchar_t buffer[32];
                GetWindowTextW((HWND)lParam, buffer, 32);
                int value = _wtoi(buffer);
                
                // 防止递归更新
                static BOOL isUpdatingFromMemoryScorpionY = FALSE;
                if (!isUpdatingFromMemoryScorpionY)
                {
                    WriteScorpionYCoordinate(value);
                }
                break;
            }
            
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            case IDC_SIDEBAR_HELP:
                ShowInputPanel(0);
                break;
            case IDC_SIDEBAR_PERSON:
                ShowInputPanel(1);
                break;
            case IDC_SIDEBAR_MONSTER1:
                ShowInputPanel(2);
                break;
            case IDC_SIDEBAR_MONSTER2:
                ShowInputPanel(3);
                break;
            case IDC_SIDEBAR_SCORPION:
                ShowInputPanel(4);
                break;
            case IDC_MONSTER1_LOCK_CHECKBOX:
                if (isMonster1Locked)
                {
                    UnlockMonster1Coordinates();
                }
                else
                {
                    LockMonster1Coordinates();
                }
                break;
            case IDC_MONSTER2_LOCK_CHECKBOX:
                if (isMonster2Locked)
                {
                    UnlockMonster2Coordinates();
                }
                else
                {
                    LockMonster2Coordinates();
                }
                break;
            case IDC_SCORPION_LOCK_CHECKBOX:
                if (isScorpionLocked)
                {
                    UnlockScorpionCoordinates();
                }
                else
                {
                    LockScorpionCoordinates();
                }
                break;
            case IDC_REFRESH_BUTTON:
                PerformManualRefresh(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_TIMER:
        if (wParam == TIMER_PROCESS_CHECK)
        {
            UpdateProcessStatus();
        }
        else if (wParam == TIMER_LOCK_CHECK)
        {
            CheckAndRestoreLockedValues();
        }
        break;
    case WM_DESTROY:
        StopProcessDetection();
        StopLockDetection();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//
// 函数: ScanTargetProcess()
//
// 目标: 扫描指定的目标进程
//
// 说明: 基于植物大战僵尸进程扫描代码改写，用于扫描调试器目标进程
//
BOOL ScanTargetProcess()
{
    // 打印开始扫描信息
    OutputDebugStringW(L"开始扫描WinMM.exe进程...\n");
    wprintf(L"开始扫描WinMM.exe进程...\n");
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"创建进程快照失败! 错误代码: %lu", error);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return FALSE;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);

    BOOL found = FALSE;
    DWORD processCount = 0;
    
    if (Process32FirstW(hSnapshot, &pe32))
    {
        do
        {
            processCount++;
            
            // 扫描目标进程：WinMM.exe
            if (_wcsicmp(pe32.szExeFile, L"WinMM.exe") == 0 ||
                _wcsicmp(pe32.szExeFile, L"winmm.exe") == 0)
            {
                targetProcessId = pe32.th32ProcessID;
                found = TRUE;
                
                wchar_t foundMsg[256];
                swprintf_s(foundMsg, L"找到WinMM.exe进程! 名称: %s, 进程ID: %lu", 
                           pe32.szExeFile, targetProcessId);
                OutputDebugStringW(foundMsg);
                wprintf(L"%s\n", foundMsg);
                break;
            }
        } while (Process32NextW(hSnapshot, &pe32));
    }
    else
    {
        DWORD error = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"枚举进程失败! 错误代码: %lu", error);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }

    CloseHandle(hSnapshot);

    if (!found)
    {
        wchar_t notFoundMsg[256];
        swprintf_s(notFoundMsg, L"未找到WinMM.exe进程! 已扫描 %lu 个进程", processCount);
        OutputDebugStringW(notFoundMsg);
        wprintf(L"%s\n", notFoundMsg);
        
        // 列出所有进程名称以便调试
        OutputDebugStringW(L"当前运行的进程列表:\n");
        wprintf(L"当前运行的进程列表:\n");
        
        HANDLE hSnapshot2 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot2 != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32W pe32_2;
            pe32_2.dwSize = sizeof(PROCESSENTRY32W);
            
            if (Process32FirstW(hSnapshot2, &pe32_2))
            {
                int count = 0;
                do
                {
                    wchar_t processInfo[128];
                    swprintf_s(processInfo, L"进程: %s (ID: %lu)", pe32_2.szExeFile, pe32_2.th32ProcessID);
                    OutputDebugStringW(processInfo);
                    wprintf(L"%s\n", processInfo);
                    
                    // 限制输出进程数量，避免过多输出
                    if (++count >= 50) 
                    {
                        OutputDebugStringW(L"... (更多进程省略)\n");
                        wprintf(L"... (更多进程省略)\n");
                        break;
                    }
                } while (Process32NextW(hSnapshot2, &pe32_2));
            }
            CloseHandle(hSnapshot2);
        }
        
        isProcessConnected = FALSE;
        return FALSE;
    }

    // 尝试打开进程句柄
    targetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcessId);
    if (targetProcessHandle == NULL)
    {
        // 如果PROCESS_ALL_ACCESS失败，尝试较少的权限
        targetProcessHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, 
                                        FALSE, targetProcessId);
        if (targetProcessHandle == NULL)
        {
            DWORD error = GetLastError();
            wchar_t errorMsg[256];
            swprintf_s(errorMsg, L"打开进程句柄失败! 进程ID: %lu, 错误代码: %lu", targetProcessId, error);
            OutputDebugStringW(errorMsg);
            wprintf(L"%s\n", errorMsg);
            isProcessConnected = FALSE;
            return FALSE;
        }
        else
        {
            OutputDebugStringW(L"以受限权限打开进程句柄成功\n");
            wprintf(L"以受限权限打开进程句柄成功\n");
        }
    }

    // 获取进程基地址
    HMODULE hModule;
    DWORD cbNeeded;
    if (EnumProcessModules(targetProcessHandle, &hModule, sizeof(hModule), &cbNeeded))
    {
        if (cbNeeded > 0)
        {
            targetBaseAddress = (DWORD_PTR)hModule;
            
            // 获取模块信息
            MODULEINFO modInfo;
            if (GetModuleInformation(targetProcessHandle, hModule, &modInfo, sizeof(modInfo)))
            {
                wchar_t connectInfo[512];
                swprintf_s(connectInfo, 
                    L"WinMM.exe进程连接成功!\n进程ID: %lu\n基地址: 0x%p\n模块大小: 0x%X (%lu 字节)\n入口点: 0x%p", 
                    targetProcessId, 
                    (void*)targetBaseAddress, 
                    modInfo.SizeOfImage, 
                    modInfo.SizeOfImage,
                    modInfo.EntryPoint);
                OutputDebugStringW(connectInfo);
                wprintf(L"%s\n", connectInfo);
            }
            else
            {
                wchar_t connectInfo[256];
                swprintf_s(connectInfo, L"WinMM.exe进程连接成功!\n进程ID: %lu\n基地址: 0x%p", 
                           targetProcessId, (void*)targetBaseAddress);
                OutputDebugStringW(connectInfo);
                wprintf(L"%s\n", connectInfo);
            }
            
            isProcessConnected = TRUE;
            return TRUE;
        }
        else
        {
            wchar_t errorMsg[256];
            swprintf_s(errorMsg, L"获取进程模块失败! cbNeeded: %lu", cbNeeded);
            OutputDebugStringW(errorMsg);
            wprintf(L"%s\n", errorMsg);
            
            CloseHandle(targetProcessHandle);
            targetProcessHandle = NULL;
            isProcessConnected = FALSE;
            return FALSE;
        }
    }
    else
    {
        DWORD err = GetLastError();
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"枚举进程模块失败! 错误代码: %lu", err);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        
        CloseHandle(targetProcessHandle);
        targetProcessHandle = NULL;
        isProcessConnected = FALSE;
        return FALSE;
    }
}

//
// 函数: CreateSidebar(HWND hWnd)
//
// 目标: 创建侧边栏界面
//
void CreateSidebar(HWND hWnd)
{
    // 创建侧边栏按钮
    const wchar_t* buttonTexts[] = { L"📖 说明", L"人物", L"怪物1", L"怪物2", L"蝎子" };
    const int buttonIds[] = { IDC_SIDEBAR_HELP, IDC_SIDEBAR_PERSON, IDC_SIDEBAR_MONSTER1, IDC_SIDEBAR_MONSTER2, IDC_SIDEBAR_SCORPION };
    
    for (int i = 0; i < 5; i++)
    {
        hSidebarButtons[i] = CreateWindowW(
            L"BUTTON",
            buttonTexts[i],
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10,                     // x位置
            50 + i * 60,           // y位置 (间隔60像素)
            100,                    // 宽度
            50,                     // 高度
            hWnd,
            (HMENU)(UINT_PTR)buttonIds[i],
            hInst,
            NULL
        );
        
        if (hSidebarButtons[i] == NULL)
        {
            wchar_t errorMsg[128];
            swprintf_s(errorMsg, L"创建侧边栏按钮失败: %s", buttonTexts[i]);
            OutputDebugStringW(errorMsg);
        }
    }
    
    // 初始化所有输入控件句柄为NULL
    for (int i = 0; i < 20; i++)
    {
        hInputControls[i] = NULL;
    }
    
    OutputDebugStringW(L"侧边栏创建完成\n");
}

//
// 函数: CreateHelpPanel(HWND hWnd)
//
// 目标: 创建说明面板
//
void CreateHelpPanel(HWND hWnd)
{
    // 计算控件位置
    int baseX = 130;    // 侧边栏右侧
    int baseY = 50;     // 说明面板位置
    
    // 创建说明文本控件
    hInputControls[HELP_PANEL_BASE_INDEX] = CreateWindowW(
        L"STATIC",
        L"WinMM 坐标修改工具使用说明",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        baseX, baseY,
        400, 30,
        hWnd,
        (HMENU)(UINT_PTR)IDC_PERSON_X_LABEL,  // 复用ID
        hInst,
        NULL
    );
    
    // 创建详细说明文本
    const wchar_t* helpText = L"🎮 使用方法：\n"
        L"1. 点击侧边栏按钮选择要修改的目标\n"
        L"2. 在输入框中输入新的坐标值\n"
        L"3. 坐标会自动写入游戏内存\n"
        L"🔒 锁定功能：\n"
        L"• 勾选锁定复选框可锁定对应目标的坐标\n"
        L"• 锁定后坐标值不会因游戏变化而改变\n"
        L"• 进入下一关后锁定可能失效，点击刷新按钮恢复\n";
    
    hInputControls[HELP_PANEL_BASE_INDEX + 1] = CreateWindowW(
        L"STATIC",
        helpText,
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        baseX, baseY + 35,
        400, 300,
        hWnd,
        (HMENU)(UINT_PTR)IDC_PERSON_X_EDIT,  // 复用ID
        hInst,
        NULL
    );
    
    // 设置字体为更易读的字体
    HFONT hFontTitle = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                  DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"微软雅黑");
    HFONT hFontContent = CreateFontW(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                    DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"微软雅黑");
    
    if (hFontTitle != NULL)
    {
        SendMessage(hInputControls[HELP_PANEL_BASE_INDEX], WM_SETFONT, (WPARAM)hFontTitle, TRUE);
    }
    if (hFontContent != NULL)
    {
        SendMessage(hInputControls[HELP_PANEL_BASE_INDEX + 1], WM_SETFONT, (WPARAM)hFontContent, TRUE);
    }
    
    OutputDebugStringW(L"说明面板创建完成\n");
}

//
// 函数: CreateInputPanel(HWND hWnd, int category)
//
// 目标: 为指定分类创建输入面板
//
void CreateInputPanel(HWND hWnd, int category)
{
    if (category < 0 || category >= 5) return;
    
    // 计算控件位置
    int baseX = 130;    // 侧边栏右侧
    int baseY = 50 + category * 60;
    
    // 每个分类对应的控件ID基础值
    int labelXId, editXId, labelYId, editYId;
    
    switch (category)
    {
    case 0: // 说明
        CreateHelpPanel(hWnd);
        return;
    case 1: // 人物
        labelXId = IDC_PERSON_X_LABEL;
        editXId = IDC_PERSON_X_EDIT;
        labelYId = IDC_PERSON_Y_LABEL;
        editYId = IDC_PERSON_Y_EDIT;
        break;
    case 2: // 怪物1
        labelXId = IDC_MONSTER1_X_LABEL;
        editXId = IDC_MONSTER1_X_EDIT;
        labelYId = IDC_MONSTER1_Y_LABEL;
        editYId = IDC_MONSTER1_Y_EDIT;
        break;
    case 3: // 怪物2
        labelXId = IDC_MONSTER2_X_LABEL;
        editXId = IDC_MONSTER2_X_EDIT;
        labelYId = IDC_MONSTER2_Y_LABEL;
        editYId = IDC_MONSTER2_Y_EDIT;
        break;
    case 4: // 蝎子
        labelXId = IDC_SCORPION_X_LABEL;
        editXId = IDC_SCORPION_X_EDIT;
        labelYId = IDC_SCORPION_Y_LABEL;
        editYId = IDC_SCORPION_Y_EDIT;
        break;
    default:
        return;
    }
    
    // 确定当前分类的基础索引
    int baseIndex;
    switch (category)
    {
    case 1: // 人物
        baseIndex = PERSON_PANEL_BASE_INDEX;
        break;
    case 2: // 怪物1
        baseIndex = MONSTER1_PANEL_BASE_INDEX;
        break;
    case 3: // 怪物2
        baseIndex = MONSTER2_PANEL_BASE_INDEX;
        break;
    case 4: // 蝎子
        baseIndex = SCORPION_PANEL_BASE_INDEX;
        break;
    default:
        return;
    }
    
    // 创建X坐标标签和输入框
    hInputControls[baseIndex] = CreateWindowW(
        L"STATIC",
        L"X坐标:",
        WS_VISIBLE | WS_CHILD,
        baseX, baseY,
        60, 20,
        hWnd,
        (HMENU)(UINT_PTR)labelXId,
        hInst,
        NULL
    );
    
    hInputControls[baseIndex + 1] = CreateWindowW(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        baseX + 70, baseY,
        80, 20,
        hWnd,
        (HMENU)(UINT_PTR)editXId,
        hInst,
        NULL
    );
    
    // 创建Y坐标标签和输入框
    hInputControls[baseIndex + 2] = CreateWindowW(
        L"STATIC",
        L"Y坐标:",
        WS_VISIBLE | WS_CHILD,
        baseX, baseY + 25,
        60, 20,
        hWnd,
        (HMENU)(UINT_PTR)labelYId,
        hInst,
        NULL
    );
    
    hInputControls[baseIndex + 3] = CreateWindowW(
        L"EDIT",
        L"",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        baseX + 70, baseY + 25,
        80, 20,
        hWnd,
        (HMENU)(UINT_PTR)editYId,
        hInst,
        NULL
    );
    
    wchar_t debugMsg[128];
    swprintf_s(debugMsg, L"为分类 %d 创建输入面板完成\n", category);
    OutputDebugStringW(debugMsg);
}

//
// 函数: HideAllInputPanels()
//
// 目标: 隐藏所有输入面板
//
void HideAllInputPanels()
{
    for (int i = 0; i < 20; i++)
    {
        if (hInputControls[i] != NULL)
        {
            ShowWindow(hInputControls[i], SW_HIDE);
        }
    }
}

//
// 函数: ShowInputPanel(int category)
//
// 目标: 显示指定分类的输入面板
//
void ShowInputPanel(int category)
{
    if (category < 0 || category >= 5) return;
    
    // 如果点击的是当前已选中的分类，则隐藏面板
    if (currentSelectedCategory == category)
    {
        HideAllInputPanels();
        currentSelectedCategory = -1;
        OutputDebugStringW(L"隐藏输入面板\n");
        return;
    }
    
    // 隐藏所有面板
    HideAllInputPanels();
    
    // 确定当前分类的基础索引
    int baseIndex;
    switch (category)
    {
    case 0: // 说明
        baseIndex = HELP_PANEL_BASE_INDEX;
        break;
    case 1: // 人物
        baseIndex = PERSON_PANEL_BASE_INDEX;
        break;
    case 2: // 怪物1
        baseIndex = MONSTER1_PANEL_BASE_INDEX;
        break;
    case 3: // 怪物2
        baseIndex = MONSTER2_PANEL_BASE_INDEX;
        break;
    case 4: // 蝎子
        baseIndex = SCORPION_PANEL_BASE_INDEX;
        break;
    default:
        return;
    }
    
    // 如果当前分类的控件还未创建，则创建它们
    if (hInputControls[baseIndex] == NULL)
    {
        CreateInputPanel(GetParent(hSidebarButtons[0]), category);
    }
    
    // 显示指定分类的控件
    int controlCount = (category == 0) ? 2 : 4; // 说明面板只有2个控件，其他面板有4个
    for (int i = 0; i < controlCount; i++)
    {
        int controlIndex = baseIndex + i;
        if (hInputControls[controlIndex] != NULL)
        {
            ShowWindow(hInputControls[controlIndex], SW_SHOW);
        }
    }
    
    currentSelectedCategory = category;
    
    const wchar_t* categoryNames[] = { L"说明", L"人物", L"怪物1", L"怪物2", L"蝎子" };
    wchar_t debugMsg[128];
    swprintf_s(debugMsg, L"显示 %s 输入面板\n", categoryNames[category]);
    OutputDebugStringW(debugMsg);
}

//
// 函数: CreateProcessStatusControls(HWND hWnd)
//
// 目标: 创建进程状态显示控件
//
void CreateProcessStatusControls(HWND hWnd)
{
    // 创建状态标签
    hStatusLabel = CreateWindowW(
        L"STATIC",
        L"进程状态:",
        WS_VISIBLE | WS_CHILD,
        10, 310,        // 位置：左下角
        80, 20,
        hWnd,
        (HMENU)(UINT_PTR)IDC_STATUS_LABEL,
        hInst,
        NULL
    );
    
    // 创建进程状态显示
    hProcessStatus = CreateWindowW(
        L"STATIC",
        L"未连接",
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        100, 310,
        120, 20,
        hWnd,
        (HMENU)(UINT_PTR)IDC_PROCESS_STATUS,
        hInst,
        NULL
    );
    
    OutputDebugStringW(L"进程状态控件创建完成\n");
}

//
// 函数: UpdateProcessStatus()
//
// 目标: 更新进程连接状态
//
void UpdateProcessStatus()
{
    static BOOL lastConnectionState = FALSE;
    static HWND lastWarningWindow = NULL;
    
    // 检测进程状态
    BOOL currentState = ScanTargetProcess();
    
    // 更新状态显示
    if (hProcessStatus != NULL)
    {
        if (currentState)
        {
            SetWindowTextW(hProcessStatus, L"已连接 ✓");
            
            // 如果之前未连接，现在连接了，显示成功消息
            if (!lastConnectionState)
            {
                OutputDebugStringW(L"进程连接成功！\n");
                wprintf(L"进程连接成功！\n");
                
                // 重新检查内存地址
                CheckMemoryAddresses();
                
                // 如果有锁定状态，重新启动锁定检测
                if (isMonster1Locked || isMonster2Locked || isScorpionLocked)
                {
                    if (lockCheckTimer == 0)
                    {
                        StartLockDetection(GetParent(hProcessStatus));
                    }
                    wprintf(L"[锁定恢复] 进程重新连接，锁定功能已恢复\n");
                    OutputDebugStringW(L"[锁定恢复] 进程重新连接，锁定功能已恢复\n");
                }
            }
        }
        else
        {
            SetWindowTextW(hProcessStatus, L"未连接 ✗");
            
            // 如果进程丢失或从未连接，显示警告
            if (lastConnectionState || lastWarningWindow == NULL)
            {
                ShowProcessWarning(GetParent(hProcessStatus));
                lastWarningWindow = GetParent(hProcessStatus);
            }
        }
    }
    
    lastConnectionState = currentState;
}

//
// 函数: StartProcessDetection(HWND hWnd)
//
// 目标: 启动进程检测定时器
//
void StartProcessDetection(HWND hWnd)
{
    // 启动定时器，每3秒检测一次
    processCheckTimer = SetTimer(hWnd, TIMER_PROCESS_CHECK, 3000, NULL);
    
    if (processCheckTimer == 0)
    {
        OutputDebugStringW(L"启动进程检测定时器失败\n");
    }
    else
    {
        OutputDebugStringW(L"进程检测定时器启动成功，间隔3秒\n");
        
        // 立即执行一次检测
        UpdateProcessStatus();
        
        // 检查内存地址是否可读
        CheckMemoryAddresses();
        
        // 如果有锁定状态，启动锁定检测定时器
        if (isMonster1Locked || isMonster2Locked || isScorpionLocked)
        {
            if (lockCheckTimer == 0)
            {
                StartLockDetection(hWnd);
            }
        }
    }
}

//
// 函数: StopProcessDetection()
//
// 目标: 停止进程检测定时器
//
void StopProcessDetection()
{
    if (processCheckTimer != 0)
    {
        KillTimer(GetParent(hProcessStatus), TIMER_PROCESS_CHECK);
        processCheckTimer = 0;
        OutputDebugStringW(L"进程检测定时器已停止\n");
    }
    
    // 清理进程句柄
    if (targetProcessHandle != NULL)
    {
        CloseHandle(targetProcessHandle);
        targetProcessHandle = NULL;
    }
}

//
// 函数: ShowProcessWarning(HWND hWnd)
//
// 目标: 显示进程未找到警告
//
void ShowProcessWarning(HWND hWnd)
{
    wchar_t warningMessage[256];
    swprintf_s(warningMessage, 
        L"警告：未找到目标进程 WinMM.exe！\n\n"
        L"请确保：\n"
        L"1. WinMM.exe 程序正在运行\n"
        L"2. 程序未被防火墙阻止\n"
        L"3. 具有足够的系统权限\n\n"
        L"程序将继续尝试连接...");
    
    // 显示警告消息框（非阻塞）
    MessageBoxW(hWnd, warningMessage, L"进程连接警告", MB_OK | MB_ICONWARNING | MB_TOPMOST);
    
    // 输出调试信息
    OutputDebugStringW(L"显示进程未找到警告\n");
    wprintf(L"警告：未找到目标进程 WinMM.exe！\n");
}

//
// 函数: ReadMemoryValue(DWORD_PTR address, void* buffer, SIZE_T size)
//
// 目标: 从目标进程读取内存数据
//
BOOL ReadMemoryValue(DWORD_PTR address, void* buffer, SIZE_T size)
{
    if (targetProcessHandle == NULL || address == 0)
    {
        return FALSE;
    }
    
    SIZE_T bytesRead = 0;
    BOOL result = ReadProcessMemory(targetProcessHandle, (LPCVOID)address, buffer, size, &bytesRead);
    
    if (!result || bytesRead != size)
    {
        DWORD error = GetLastError();
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"读取内存失败! 地址: 0x%p, 错误: %lu", (void*)address, error);
        OutputDebugStringW(errorMsg);
        return FALSE;
    }
    
    return TRUE;
}

//
// 函数: WriteMemoryValue(DWORD_PTR address, void* buffer, SIZE_T size)
//
// 目标: 向目标进程写入内存数据
//
BOOL WriteMemoryValue(DWORD_PTR address, void* buffer, SIZE_T size)
{
    if (targetProcessHandle == NULL || address == 0)
    {
        return FALSE;
    }
    
    SIZE_T bytesWritten = 0;
    BOOL result = WriteProcessMemory(targetProcessHandle, (LPVOID)address, buffer, size, &bytesWritten);
    
    if (!result || bytesWritten != size)
    {
        DWORD error = GetLastError();
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"写入内存失败! 地址: 0x%p, 错误: %lu", (void*)address, error);
        OutputDebugStringW(errorMsg);
        return FALSE;
    }
    
    wchar_t successMsg[128];
    swprintf_s(successMsg, L"内存写入成功! 地址: 0x%p, 大小: %zu 字节", (void*)address, size);
    OutputDebugStringW(successMsg);
    return TRUE;
}

//
// 函数: ResolvePointerChain(DWORD_PTR baseAddress, int* offsets, int offsetCount)
//
// 目标: 解析多级指针链，获取最终地址
//
DWORD_PTR ResolvePointerChain(DWORD_PTR baseAddress, int* offsets, int offsetCount)
{
    if (targetProcessHandle == NULL || baseAddress == 0)
    {
        return 0;
    }
    
    DWORD_PTR currentAddress = baseAddress;
    
    wchar_t debugMsg[256];
    swprintf_s(debugMsg, L"开始解析指针链，基地址: 0x%p", (void*)baseAddress);
    OutputDebugStringW(debugMsg);
    wprintf(L"%s\n", debugMsg);
    
    // 如果是人物X坐标的指针链，显示额外信息
    if (baseAddress == targetBaseAddress + PERSON_X_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[人物X坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", PERSON_X_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    // 如果是人物Y坐标的指针链，显示额外信息
    else if (baseAddress == targetBaseAddress + PERSON_Y_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[人物Y坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", PERSON_Y_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    // 如果是怪物1 X坐标的指针链，显示额外信息
    else if (baseAddress == targetBaseAddress + MONSTER1_X_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[怪物1 X坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", MONSTER1_X_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    // 如果是怪物1 Y坐标的指针链，显示额外信息
    else if (baseAddress == targetBaseAddress + MONSTER1_Y_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[怪物1 Y坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", MONSTER1_Y_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    // 如果是怪物2 X坐标的指针链，显示额外信息
    else if (baseAddress == targetBaseAddress + MONSTER2_X_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[怪物2 X坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", MONSTER2_X_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    // 如果是怪物2 Y坐标的指针链，显示额外信息
    else if (baseAddress == targetBaseAddress + MONSTER2_Y_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[怪物2 Y坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", MONSTER2_Y_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    // 如果是蝎子 X坐标的指针链，显示额外信息
    else if (baseAddress == targetBaseAddress + SCORPION_X_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[蝎子 X坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", SCORPION_X_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    // 如果是蝎子 Y坐标的指针链，显示额外信息
    else if (baseAddress == targetBaseAddress + SCORPION_Y_BASE_OFFSET)
    {
        wchar_t extraInfo[256];
        swprintf_s(extraInfo, L"[蝎子 Y坐标] 开始解析指针链: WinMM.exe+%X (0x%p)", SCORPION_Y_BASE_OFFSET, (void*)baseAddress);
        OutputDebugStringW(extraInfo);
        wprintf(L"%s\n", extraInfo);
    }
    
    for (int i = 0; i < offsetCount; i++)
    {
        // 读取当前地址指向的值作为下一级地址
        DWORD_PTR nextAddress = 0;
        if (!ReadMemoryValue(currentAddress, &nextAddress, sizeof(DWORD_PTR)))
        {
            wchar_t errorMsg[128];
            swprintf_s(errorMsg, L"指针解析失败在级别 %d，地址: 0x%p", i, (void*)currentAddress);
            OutputDebugStringW(errorMsg);
            return 0;
        }
        
        // 添加偏移
        currentAddress = nextAddress + offsets[i];
        
        wchar_t stepMsg[256];
        swprintf_s(stepMsg, L"级别 %d: 读取地址 0x%p 得到值 0x%p, 加上偏移 0x%X = 0x%p", 
                   i, (void*)currentAddress, (void*)nextAddress, offsets[i], (void*)(nextAddress + offsets[i]));
        OutputDebugStringW(stepMsg);
        wprintf(L"%s\n", stepMsg);
    }
    
    wchar_t finalMsg[128];
    swprintf_s(finalMsg, L"指针链解析完成，最终地址: 0x%p", (void*)currentAddress);
    OutputDebugStringW(finalMsg);
    wprintf(L"%s\n", finalMsg);
    
    return currentAddress;
}

//
// 函数: UpdatePersonXCoordinate()
//
// 目标: 解析人物X坐标的指针链并验证地址可读性
//
void UpdatePersonXCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 60 + 5C
    int offsets[] = { PERSON_X_LEVEL1_OFFSET, PERSON_X_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + PERSON_X_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isMemoryAddressValid = FALSE;
        personXAddress = 0;
        return;
    }
    
    personXAddress = finalAddress;
    isMemoryAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(personXAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[人物X坐标] 地址验证成功: 0x%p", (void*)personXAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isMemoryAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[人物X坐标] 地址验证失败: 0x%p", (void*)personXAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WritePersonXCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WritePersonXCoordinate(int value)
{
    if (!isProcessConnected || !isMemoryAddressValid || personXAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[人物X坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isMemoryAddressValid, (void*)personXAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(personXAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[人物X坐标] 写入成功: %d -> 0x%p", value, (void*)personXAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: UpdatePersonYCoordinate()
//
// 目标: 解析人物Y坐标的指针链并验证地址可读性
//
void UpdatePersonYCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 60 + 5C
    int offsets[] = { PERSON_Y_LEVEL1_OFFSET, PERSON_Y_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + PERSON_Y_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isPersonYAddressValid = FALSE;
        personYAddress = 0;
        wprintf(L"[人物Y坐标] 指针链解析失败，无法获取有效地址\n");
        OutputDebugStringW(L"[人物Y坐标] 指针链解析失败，无法获取有效地址\n");
        return;
    }
    
    personYAddress = finalAddress;
    isPersonYAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(personYAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[人物Y坐标] 地址验证成功: 0x%p", (void*)personYAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isPersonYAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[人物Y坐标] 地址验证失败: 0x%p", (void*)personYAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WritePersonYCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WritePersonYCoordinate(int value)
{
    if (!isProcessConnected || !isPersonYAddressValid || personYAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[人物Y坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isPersonYAddressValid, (void*)personYAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(personYAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[人物Y坐标] 写入成功: %d -> 0x%p", value, (void*)personYAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: UpdateMonster1XCoordinate()
//
// 目标: 解析怪物1 X坐标的指针链并验证地址可读性
//
void UpdateMonster1XCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 64 + 58
    int offsets[] = { MONSTER1_X_LEVEL1_OFFSET, MONSTER1_X_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + MONSTER1_X_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isMonster1XAddressValid = FALSE;
        monster1XAddress = 0;
        wprintf(L"[怪物1 X坐标] 指针链解析失败，无法获取有效地址\n");
        OutputDebugStringW(L"[怪物1 X坐标] 指针链解析失败，无法获取有效地址\n");
        return;
    }
    
    monster1XAddress = finalAddress;
    isMonster1XAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(monster1XAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[怪物1 X坐标] 地址验证成功: 0x%p", (void*)monster1XAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isMonster1XAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[怪物1 X坐标] 地址验证失败: 0x%p", (void*)monster1XAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WriteMonster1XCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WriteMonster1XCoordinate(int value)
{
    if (!isProcessConnected || !isMonster1XAddressValid || monster1XAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[怪物1 X坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isMonster1XAddressValid, (void*)monster1XAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(monster1XAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[怪物1 X坐标] 写入成功: %d -> 0x%p", value, (void*)monster1XAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: UpdateMonster1YCoordinate()
//
// 目标: 解析怪物1 Y坐标的指针链并验证地址可读性
//
void UpdateMonster1YCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 64 + 5C
    int offsets[] = { MONSTER1_Y_LEVEL1_OFFSET, MONSTER1_Y_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + MONSTER1_Y_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isMonster1YAddressValid = FALSE;
        monster1YAddress = 0;
        wprintf(L"[怪物1 Y坐标] 指针链解析失败，无法获取有效地址\n");
        OutputDebugStringW(L"[怪物1 Y坐标] 指针链解析失败，无法获取有效地址\n");
        return;
    }
    
    monster1YAddress = finalAddress;
    isMonster1YAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(monster1YAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[怪物1 Y坐标] 地址验证成功: 0x%p", (void*)monster1YAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isMonster1YAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[怪物1 Y坐标] 地址验证失败: 0x%p", (void*)monster1YAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WriteMonster1YCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WriteMonster1YCoordinate(int value)
{
    if (!isProcessConnected || !isMonster1YAddressValid || monster1YAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[怪物1 Y坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isMonster1YAddressValid, (void*)monster1YAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(monster1YAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[怪物1 Y坐标] 写入成功: %d -> 0x%p", value, (void*)monster1YAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: UpdateMonster2XCoordinate()
//
// 目标: 解析怪物2 X坐标的指针链并验证地址可读性
//
void UpdateMonster2XCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 68 + 58
    int offsets[] = { MONSTER2_X_LEVEL1_OFFSET, MONSTER2_X_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + MONSTER2_X_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isMonster2XAddressValid = FALSE;
        monster2XAddress = 0;
        wprintf(L"[怪物2 X坐标] 指针链解析失败，无法获取有效地址\n");
        OutputDebugStringW(L"[怪物2 X坐标] 指针链解析失败，无法获取有效地址\n");
        return;
    }
    
    monster2XAddress = finalAddress;
    isMonster2XAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(monster2XAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[怪物2 X坐标] 地址验证成功: 0x%p", (void*)monster2XAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isMonster2XAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[怪物2 X坐标] 地址验证失败: 0x%p", (void*)monster2XAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WriteMonster2XCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WriteMonster2XCoordinate(int value)
{
    if (!isProcessConnected || !isMonster2XAddressValid || monster2XAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[怪物2 X坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isMonster2XAddressValid, (void*)monster2XAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(monster2XAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[怪物2 X坐标] 写入成功: %d -> 0x%p", value, (void*)monster2XAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: UpdateMonster2YCoordinate()
//
// 目标: 解析怪物2 Y坐标的指针链并验证地址可读性
//
void UpdateMonster2YCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 68 + 5C
    int offsets[] = { MONSTER2_Y_LEVEL1_OFFSET, MONSTER2_Y_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + MONSTER2_Y_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isMonster2YAddressValid = FALSE;
        monster2YAddress = 0;
        wprintf(L"[怪物2 Y坐标] 指针链解析失败，无法获取有效地址\n");
        OutputDebugStringW(L"[怪物2 Y坐标] 指针链解析失败，无法获取有效地址\n");
        return;
    }
    
    monster2YAddress = finalAddress;
    isMonster2YAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(monster2YAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[怪物2 Y坐标] 地址验证成功: 0x%p", (void*)monster2YAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isMonster2YAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[怪物2 Y坐标] 地址验证失败: 0x%p", (void*)monster2YAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WriteMonster2YCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WriteMonster2YCoordinate(int value)
{
    if (!isProcessConnected || !isMonster2YAddressValid || monster2YAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[怪物2 Y坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isMonster2YAddressValid, (void*)monster2YAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(monster2YAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[怪物2 Y坐标] 写入成功: %d -> 0x%p", value, (void*)monster2YAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: UpdateScorpionXCoordinate()
//
// 目标: 解析蝎子 X坐标的指针链并验证地址可读性
//
void UpdateScorpionXCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 6C + 60
    int offsets[] = { SCORPION_X_LEVEL1_OFFSET, SCORPION_X_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + SCORPION_X_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isScorpionXAddressValid = FALSE;
        scorpionXAddress = 0;
        wprintf(L"[蝎子 X坐标] 指针链解析失败，无法获取有效地址\n");
        OutputDebugStringW(L"[蝎子 X坐标] 指针链解析失败，无法获取有效地址\n");
        return;
    }
    
    scorpionXAddress = finalAddress;
    isScorpionXAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(scorpionXAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[蝎子 X坐标] 地址验证成功: 0x%p", (void*)scorpionXAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isScorpionXAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[蝎子 X坐标] 地址验证失败: 0x%p", (void*)scorpionXAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WriteScorpionXCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WriteScorpionXCoordinate(int value)
{
    if (!isProcessConnected || !isScorpionXAddressValid || scorpionXAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[蝎子 X坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isScorpionXAddressValid, (void*)scorpionXAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(scorpionXAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[蝎子 X坐标] 写入成功: %d -> 0x%p", value, (void*)scorpionXAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: UpdateScorpionYCoordinate()
//
// 目标: 解析蝎子 Y坐标的指针链并验证地址可读性
//
void UpdateScorpionYCoordinate()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 解析指针链: WinMM.exe+AC54C + 6C + 64
    int offsets[] = { SCORPION_Y_LEVEL1_OFFSET, SCORPION_Y_LEVEL2_OFFSET };
    DWORD_PTR baseAddressWithOffset = targetBaseAddress + SCORPION_Y_BASE_OFFSET;
    DWORD_PTR finalAddress = ResolvePointerChain(baseAddressWithOffset, offsets, 2);
    
    if (finalAddress == 0)
    {
        isScorpionYAddressValid = FALSE;
        scorpionYAddress = 0;
        wprintf(L"[蝎子 Y坐标] 指针链解析失败，无法获取有效地址\n");
        OutputDebugStringW(L"[蝎子 Y坐标] 指针链解析失败，无法获取有效地址\n");
        return;
    }
    
    scorpionYAddress = finalAddress;
    isScorpionYAddressValid = TRUE;
    
    // 验证地址是否可读
    int currentValue = 0;
    if (ReadMemoryValue(scorpionYAddress, &currentValue, sizeof(int)))
    {
        wchar_t debugMsg[256];
        swprintf_s(debugMsg, L"[蝎子 Y坐标] 地址验证成功: 0x%p", (void*)scorpionYAddress);
        OutputDebugStringW(debugMsg);
        wprintf(L"%s\n", debugMsg);
    }
    else
    {
        isScorpionYAddressValid = FALSE;
        wchar_t errorMsg[256];
        swprintf_s(errorMsg, L"[蝎子 Y坐标] 地址验证失败: 0x%p", (void*)scorpionYAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
    }
}

//
// 函数: WriteScorpionYCoordinate(int value)
//
// 目标: 将用户输入的值写入内存
//
void WriteScorpionYCoordinate(int value)
{
    if (!isProcessConnected || !isScorpionYAddressValid || scorpionYAddress == 0)
    {
        wchar_t errorMsg[128];
        swprintf_s(errorMsg, L"[蝎子 Y坐标] 无法写入内存: 连接=%d, 地址有效=%d, 地址=0x%p", 
                   isProcessConnected, isScorpionYAddressValid, (void*)scorpionYAddress);
        OutputDebugStringW(errorMsg);
        wprintf(L"%s\n", errorMsg);
        return;
    }
    
    // 写入内存
    if (WriteMemoryValue(scorpionYAddress, &value, sizeof(int)))
    {
        wchar_t successMsg[128];
        swprintf_s(successMsg, L"[蝎子 Y坐标] 写入成功: %d -> 0x%p", value, (void*)scorpionYAddress);
        OutputDebugStringW(successMsg);
        wprintf(L"%s\n", successMsg);
    }
}

//
// 函数: CheckMemoryAddresses()
//
// 目标: 检查所有内存地址是否可读，失败时弹出警告
//
void CheckMemoryAddresses()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    wprintf(L"[内存检查] 开始检查所有坐标地址的可读性...\n");
    OutputDebugStringW(L"[内存检查] 开始检查所有坐标地址的可读性...\n");
    
    // 检查所有坐标地址
    UpdatePersonXCoordinate();
    UpdatePersonYCoordinate();
    UpdateMonster1XCoordinate();
    UpdateMonster1YCoordinate();
    UpdateMonster2XCoordinate();
    UpdateMonster2YCoordinate();
    UpdateScorpionXCoordinate();
    UpdateScorpionYCoordinate();
    
    // 检查是否有地址验证失败
    BOOL hasFailure = FALSE;
    wchar_t failureMessage[1024] = {0};
    wchar_t tempMsg[256];
    
    if (!isMemoryAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"人物X坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (!isPersonYAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"人物Y坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (!isMonster1XAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"怪物1 X坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (!isMonster1YAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"怪物1 Y坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (!isMonster2XAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"怪物2 X坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (!isMonster2YAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"怪物2 Y坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (!isScorpionXAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"蝎子 X坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (!isScorpionYAddressValid)
    {
        hasFailure = TRUE;
        swprintf_s(tempMsg, L"蝎子 Y坐标地址验证失败\n");
        wcscat_s(failureMessage, tempMsg);
    }
    
    if (hasFailure)
    {
        wcscat_s(failureMessage, L"\n请检查游戏是否正常运行，或重新启动游戏。");
        ShowMemoryWarning(GetActiveWindow(), failureMessage);
    }
    else
    {
        wprintf(L"[内存检查] 所有坐标地址验证成功！\n");
        OutputDebugStringW(L"[内存检查] 所有坐标地址验证成功！\n");
    }
}

//
// 函数: ShowMemoryWarning(HWND hWnd, const wchar_t* message)
//
// 目标: 显示内存地址验证失败的警告
//
void ShowMemoryWarning(HWND hWnd, const wchar_t* message)
{
    wchar_t warningTitle[128];
    swprintf_s(warningTitle, L"内存地址验证警告");
    
    // 显示警告消息框
    MessageBoxW(hWnd, message, warningTitle, MB_OK | MB_ICONWARNING | MB_TOPMOST);
    
    // 输出调试信息
    OutputDebugStringW(L"显示内存地址验证失败警告\n");
    wprintf(L"警告：%s\n", message);
}

//
// 函数: CreateLockCheckboxes(HWND hWnd)
//
// 目标: 创建锁定复选框控件
//
void CreateLockCheckboxes(HWND hWnd)
{
    // 创建怪物1锁定复选框
    hMonster1LockCheckbox = CreateWindowW(
        L"BUTTON",
        L"锁定怪物1坐标",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        10, 350,        // 位置：左下角，进程状态控件下方
        120, 20,
        hWnd,
        (HMENU)(UINT_PTR)IDC_MONSTER1_LOCK_CHECKBOX,
        hInst,
        NULL
    );
    
    // 创建怪物2锁定复选框
    hMonster2LockCheckbox = CreateWindowW(
        L"BUTTON",
        L"锁定怪物2坐标",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        140, 350,
        120, 20,
        hWnd,
        (HMENU)(UINT_PTR)IDC_MONSTER2_LOCK_CHECKBOX,
        hInst,
        NULL
    );
    
    // 创建蝎子锁定复选框
    hScorpionLockCheckbox = CreateWindowW(
        L"BUTTON",
        L"锁定蝎子坐标",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        270, 350,
        120, 20,
        hWnd,
        (HMENU)(UINT_PTR)IDC_SCORPION_LOCK_CHECKBOX,
        hInst,
        NULL
    );
    
    OutputDebugStringW(L"锁定复选框创建完成\n");
}

//
// 函数: CreateRefreshButton(HWND hWnd)
//
// 目标: 创建手动刷新按钮
//
void CreateRefreshButton(HWND hWnd)
{
    // 创建刷新按钮
    hRefreshButton = CreateWindowW(
        L"BUTTON",
        L"🔄 刷新",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        10, 380,        // 位置：左下角，锁定复选框下方
        120, 30,
        hWnd,
        (HMENU)(UINT_PTR)IDC_REFRESH_BUTTON,
        hInst,
        NULL
    );
    
    OutputDebugStringW(L"刷新按钮创建完成\n");
}

//
// 函数: StartLockDetection(HWND hWnd)
//
// 目标: 启动锁定检测定时器
//
void StartLockDetection(HWND hWnd)
{
    // 启动定时器，每100毫秒检测一次
    lockCheckTimer = SetTimer(hWnd, TIMER_LOCK_CHECK, LOCK_CHECK_INTERVAL, NULL);
    
    if (lockCheckTimer == 0)
    {
        OutputDebugStringW(L"启动锁定检测定时器失败\n");
    }
    else
    {
        OutputDebugStringW(L"锁定检测定时器启动成功，间隔100毫秒\n");
    }
}

//
// 函数: StopLockDetection()
//
// 目标: 停止锁定检测定时器
//
void StopLockDetection()
{
    if (lockCheckTimer != 0)
    {
        KillTimer(GetParent(hMonster1LockCheckbox), TIMER_LOCK_CHECK);
        lockCheckTimer = 0;
        OutputDebugStringW(L"锁定检测定时器已停止\n");
    }
}

//
// 函数: CheckAndRestoreLockedValues()
//
// 目标: 检查并恢复锁定的坐标值
//
void CheckAndRestoreLockedValues()
{
    if (!isProcessConnected)
    {
        return;
    }
    
    // 如果地址无效，尝试重新验证
    if (!isMonster1XAddressValid || !isMonster1YAddressValid)
    {
        UpdateMonster1XCoordinate();
        UpdateMonster1YCoordinate();
    }
    if (!isMonster2XAddressValid || !isMonster2YAddressValid)
    {
        UpdateMonster2XCoordinate();
        UpdateMonster2YCoordinate();
    }
    if (!isScorpionXAddressValid || !isScorpionYAddressValid)
    {
        UpdateScorpionXCoordinate();
        UpdateScorpionYCoordinate();
    }
    
    // 检查怪物1锁定
    if (isMonster1Locked && isMonster1XAddressValid && isMonster1YAddressValid)
    {
        int currentX = 0, currentY = 0;
        
        // 读取当前X坐标
        if (ReadMemoryValue(monster1XAddress, &currentX, sizeof(int)))
        {
            // 如果X坐标发生变化，恢复锁定值
            if (currentX != monster1LockedX)
            {
                WriteMemoryValue(monster1XAddress, &monster1LockedX, sizeof(int));
                wprintf(L"[怪物1锁定] X坐标已恢复: %d -> %d\n", currentX, monster1LockedX);
            }
        }
        
        // 读取当前Y坐标
        if (ReadMemoryValue(monster1YAddress, &currentY, sizeof(int)))
        {
            // 如果Y坐标发生变化，恢复锁定值
            if (currentY != monster1LockedY)
            {
                WriteMemoryValue(monster1YAddress, &monster1LockedY, sizeof(int));
                wprintf(L"[怪物1锁定] Y坐标已恢复: %d -> %d\n", currentY, monster1LockedY);
            }
        }
    }
    
    // 检查怪物2锁定
    if (isMonster2Locked && isMonster2XAddressValid && isMonster2YAddressValid)
    {
        int currentX = 0, currentY = 0;
        
        // 读取当前X坐标
        if (ReadMemoryValue(monster2XAddress, &currentX, sizeof(int)))
        {
            // 如果X坐标发生变化，恢复锁定值
            if (currentX != monster2LockedX)
            {
                WriteMemoryValue(monster2XAddress, &monster2LockedX, sizeof(int));
                wprintf(L"[怪物2锁定] X坐标已恢复: %d -> %d\n", currentX, monster2LockedX);
            }
        }
        
        // 读取当前Y坐标
        if (ReadMemoryValue(monster2YAddress, &currentY, sizeof(int)))
        {
            // 如果Y坐标发生变化，恢复锁定值
            if (currentY != monster2LockedY)
            {
                WriteMemoryValue(monster2YAddress, &monster2LockedY, sizeof(int));
                wprintf(L"[怪物2锁定] Y坐标已恢复: %d -> %d\n", currentY, monster2LockedY);
            }
        }
    }
    
    // 检查蝎子锁定
    if (isScorpionLocked && isScorpionXAddressValid && isScorpionYAddressValid)
    {
        int currentX = 0, currentY = 0;
        
        // 读取当前X坐标
        if (ReadMemoryValue(scorpionXAddress, &currentX, sizeof(int)))
        {
            // 如果X坐标发生变化，恢复锁定值
            if (currentX != scorpionLockedX)
            {
                WriteMemoryValue(scorpionXAddress, &scorpionLockedX, sizeof(int));
                wprintf(L"[蝎子锁定] X坐标已恢复: %d -> %d\n", currentX, scorpionLockedX);
            }
        }
        
        // 读取当前Y坐标
        if (ReadMemoryValue(scorpionYAddress, &currentY, sizeof(int)))
        {
            // 如果Y坐标发生变化，恢复锁定值
            if (currentY != scorpionLockedY)
            {
                WriteMemoryValue(scorpionYAddress, &scorpionLockedY, sizeof(int));
                wprintf(L"[蝎子锁定] Y坐标已恢复: %d -> %d\n", currentY, scorpionLockedY);
            }
        }
    }
}

//
// 函数: LockMonster1Coordinates()
//
// 目标: 锁定怪物1的坐标
//
void LockMonster1Coordinates()
{
    if (!isProcessConnected || !isMonster1XAddressValid || !isMonster1YAddressValid)
    {
        wprintf(L"[怪物1锁定] 无法锁定：进程未连接或地址无效\n");
        return;
    }
    
    // 读取当前坐标值
    int currentX = 0, currentY = 0;
    
    if (ReadMemoryValue(monster1XAddress, &currentX, sizeof(int)) &&
        ReadMemoryValue(monster1YAddress, &currentY, sizeof(int)))
    {
        monster1LockedX = currentX;
        monster1LockedY = currentY;
        isMonster1Locked = TRUE;
        
        // 设置复选框状态
        if (hMonster1LockCheckbox != NULL)
        {
            CheckDlgButton(GetParent(hMonster1LockCheckbox), IDC_MONSTER1_LOCK_CHECKBOX, BST_CHECKED);
        }
        
        wprintf(L"[怪物1锁定] 坐标已锁定: X=%d, Y=%d\n", monster1LockedX, monster1LockedY);
        OutputDebugStringW(L"[怪物1锁定] 坐标已锁定\n");
        
        // 启动锁定检测定时器（如果还没启动）
        if (lockCheckTimer == 0)
        {
            StartLockDetection(GetParent(hMonster1LockCheckbox));
        }
    }
    else
    {
        wprintf(L"[怪物1锁定] 读取当前坐标失败\n");
    }
}

//
// 函数: UnlockMonster1Coordinates()
//
// 目标: 解锁怪物1的坐标
//
void UnlockMonster1Coordinates()
{
    isMonster1Locked = FALSE;
    
    // 设置复选框状态
    if (hMonster1LockCheckbox != NULL)
    {
        CheckDlgButton(GetParent(hMonster1LockCheckbox), IDC_MONSTER1_LOCK_CHECKBOX, BST_UNCHECKED);
    }
    
    wprintf(L"[怪物1锁定] 坐标已解锁\n");
    OutputDebugStringW(L"[怪物1锁定] 坐标已解锁\n");
    
    // 如果没有其他锁定，停止定时器
    if (!isMonster1Locked && !isMonster2Locked && !isScorpionLocked)
    {
        StopLockDetection();
    }
}

//
// 函数: LockMonster2Coordinates()
//
// 目标: 锁定怪物2的坐标
//
void LockMonster2Coordinates()
{
    if (!isProcessConnected || !isMonster2XAddressValid || !isMonster2YAddressValid)
    {
        wprintf(L"[怪物2锁定] 无法锁定：进程未连接或地址无效\n");
        return;
    }
    
    // 读取当前坐标值
    int currentX = 0, currentY = 0;
    
    if (ReadMemoryValue(monster2XAddress, &currentX, sizeof(int)) &&
        ReadMemoryValue(monster2YAddress, &currentY, sizeof(int)))
    {
        monster2LockedX = currentX;
        monster2LockedY = currentY;
        isMonster2Locked = TRUE;
        
        // 设置复选框状态
        if (hMonster2LockCheckbox != NULL)
        {
            CheckDlgButton(GetParent(hMonster2LockCheckbox), IDC_MONSTER2_LOCK_CHECKBOX, BST_CHECKED);
        }
        
        wprintf(L"[怪物2锁定] 坐标已锁定: X=%d, Y=%d\n", monster2LockedX, monster2LockedY);
        OutputDebugStringW(L"[怪物2锁定] 坐标已锁定\n");
        
        // 启动锁定检测定时器（如果还没启动）
        if (lockCheckTimer == 0)
        {
            StartLockDetection(GetParent(hMonster2LockCheckbox));
        }
    }
    else
    {
        wprintf(L"[怪物2锁定] 读取当前坐标失败\n");
    }
}

//
// 函数: UnlockMonster2Coordinates()
//
// 目标: 解锁怪物2的坐标
//
void UnlockMonster2Coordinates()
{
    isMonster2Locked = FALSE;
    
    // 设置复选框状态
    if (hMonster2LockCheckbox != NULL)
    {
        CheckDlgButton(GetParent(hMonster2LockCheckbox), IDC_MONSTER2_LOCK_CHECKBOX, BST_UNCHECKED);
    }
    
    wprintf(L"[怪物2锁定] 坐标已解锁\n");
    OutputDebugStringW(L"[怪物2锁定] 坐标已解锁\n");
    
    // 如果没有其他锁定，停止定时器
    if (!isMonster1Locked && !isMonster2Locked && !isScorpionLocked)
    {
        StopLockDetection();
    }
}

//
// 函数: LockScorpionCoordinates()
//
// 目标: 锁定蝎子的坐标
//
void LockScorpionCoordinates()
{
    if (!isProcessConnected || !isScorpionXAddressValid || !isScorpionYAddressValid)
    {
        wprintf(L"[蝎子锁定] 无法锁定：进程未连接或地址无效\n");
        return;
    }
    
    // 读取当前坐标值
    int currentX = 0, currentY = 0;
    
    if (ReadMemoryValue(scorpionXAddress, &currentX, sizeof(int)) &&
        ReadMemoryValue(scorpionYAddress, &currentY, sizeof(int)))
    {
        scorpionLockedX = currentX;
        scorpionLockedY = currentY;
        isScorpionLocked = TRUE;
        
        // 设置复选框状态
        if (hScorpionLockCheckbox != NULL)
        {
            CheckDlgButton(GetParent(hScorpionLockCheckbox), IDC_SCORPION_LOCK_CHECKBOX, BST_CHECKED);
        }
        
        wprintf(L"[蝎子锁定] 坐标已锁定: X=%d, Y=%d\n", scorpionLockedX, scorpionLockedY);
        OutputDebugStringW(L"[蝎子锁定] 坐标已锁定\n");
        
        // 启动锁定检测定时器（如果还没启动）
        if (lockCheckTimer == 0)
        {
            StartLockDetection(GetParent(hScorpionLockCheckbox));
        }
    }
    else
    {
        wprintf(L"[蝎子锁定] 读取当前坐标失败\n");
    }
}

//
// 函数: UnlockScorpionCoordinates()
//
// 目标: 解锁蝎子的坐标
//
void UnlockScorpionCoordinates()
{
    isScorpionLocked = FALSE;
    
    // 设置复选框状态
    if (hScorpionLockCheckbox != NULL)
    {
        CheckDlgButton(GetParent(hScorpionLockCheckbox), IDC_SCORPION_LOCK_CHECKBOX, BST_UNCHECKED);
    }
    
    wprintf(L"[蝎子锁定] 坐标已解锁\n");
    OutputDebugStringW(L"[蝎子锁定] 坐标已解锁\n");
    
    // 如果没有其他锁定，停止定时器
    if (!isMonster1Locked && !isMonster2Locked && !isScorpionLocked)
    {
        StopLockDetection();
    }
}

//
// 函数: PerformManualRefresh(HWND hWnd)
//
// 目标: 执行手动刷新操作，重新连接进程并恢复所有功能
//
void PerformManualRefresh(HWND hWnd)
{
    wprintf(L"[手动刷新] 开始执行刷新操作...\n");
    OutputDebugStringW(L"[手动刷新] 开始执行刷新操作...\n");
    
    // 保存当前的锁定状态
    BOOL wasMonster1Locked = isMonster1Locked;
    BOOL wasMonster2Locked = isMonster2Locked;
    BOOL wasScorpionLocked = isScorpionLocked;
    
    // 停止所有定时器
    StopProcessDetection();
    StopLockDetection();
    
    // 清理进程句柄
    if (targetProcessHandle != NULL)
    {
        CloseHandle(targetProcessHandle);
        targetProcessHandle = NULL;
    }
    
    // 重置所有地址有效性标志
    isMemoryAddressValid = FALSE;
    isPersonYAddressValid = FALSE;
    isMonster1XAddressValid = FALSE;
    isMonster1YAddressValid = FALSE;
    isMonster2XAddressValid = FALSE;
    isMonster2YAddressValid = FALSE;
    isScorpionXAddressValid = FALSE;
    isScorpionYAddressValid = FALSE;
    
    // 重置所有地址
    personXAddress = 0;
    personYAddress = 0;
    monster1XAddress = 0;
    monster1YAddress = 0;
    monster2XAddress = 0;
    monster2YAddress = 0;
    scorpionXAddress = 0;
    scorpionYAddress = 0;
    
    // 重新启动进程检测
    StartProcessDetection(hWnd);
    
    // 等待一小段时间确保进程连接完成
    Sleep(1000);
    
    // 如果之前有锁定状态，重新恢复锁定
    if (wasMonster1Locked)
    {
        wprintf(L"[手动刷新] 恢复怪物1锁定状态...\n");
        LockMonster1Coordinates();
    }
    if (wasMonster2Locked)
    {
        wprintf(L"[手动刷新] 恢复怪物2锁定状态...\n");
        LockMonster2Coordinates();
    }
    if (wasScorpionLocked)
    {
        wprintf(L"[手动刷新] 恢复蝎子锁定状态...\n");
        LockScorpionCoordinates();
    }
    
    wprintf(L"[手动刷新] 刷新操作完成！\n");
    OutputDebugStringW(L"[手动刷新] 刷新操作完成！\n");
    
    // 显示刷新完成消息
    MessageBoxW(hWnd, L"刷新操作完成！\n\n已重新连接进程并恢复所有功能。", 
                L"刷新完成", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
}
