// To compile: cl /source-charset:utf-8 /std:c++20 🐍.cpp /link /MANIFEST:EMBED

#include <windows.h>
#include <string>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// 全局🐍量
HINSTANCE hInst;
const wchar_t *WINDOW_CLASS = L"🐍🐍🐍WindowClass";
const wchar_t *WINDOW_TITLE = L" ";
HFONT hFont, hFontMs; // 添加全局🐍量
int windowPosX = 100;
int windowPosY = 100;
int windowSpeedX = 10;
int windowSpeedY = 4;
double wavePosition = 0.0;     // 🐍浪位置
double waveSpeed = 0.2; // 🐍浪速度
int waveAmplitude = 100; // 🐍浪幅度
int baseY = 200;         // 基准Y位置

bool paused = false;
int logic = 0;
bool muted = false; // 新增静音状态变量

// 🐍🐍过程🐍数
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // 播放当前目录下的🐍.wav文件
        PlaySoundW(L"🐍.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        // 创建并选择支持Unicode的字体
        hFont = CreateFontW(
            72,                        // 字体高度
            0,                         // 字体宽度(0表示自动)
            0,                         // 文本倾斜角度
            0,                         // 字体倾斜角度
            0,                 // 字体粗细
            FALSE,                     // 是否斜体
            FALSE,                     // 是否下划线
            FALSE,                     // 是否删除线
            DEFAULT_CHARSET,           // 字符集
            OUT_OUTLINE_PRECIS,        // 输出精度
            CLIP_DEFAULT_PRECIS,       // 裁剪精度
            ANTIALIASED_QUALITY,         // 输出质量
            VARIABLE_PITCH | FF_DECORATIVE | FF_MODERN, // 间距和字体族
            L"Segoe UI Emoji"          // 字体名称(支持emoji的字体)
        );
        hFontMs = CreateFontW(
            72,                        // 字体高度
            0,                         // 字体宽度(0表示自动)
            0,                         // 文本倾斜角度
            0,                         // 字体倾斜角度
            0,                 // 字体粗细
            FALSE,                     // 是否斜体
            FALSE,                     // 是否下划线
            FALSE,                     // 是否删除线
            DEFAULT_CHARSET,           // 字符集
            OUT_OUTLINE_PRECIS,        // 输出精度
            CLIP_DEFAULT_PRECIS,       // 裁剪精度
            ANTIALIASED_QUALITY,         // 输出质量
            VARIABLE_PITCH | FF_DECORATIVE | FF_MODERN, // 间距和字体族
            L"Microsoft YaHei"         // 字体名称(支持emoji的字体)
        );
        waveAmplitude = GetSystemMetrics(SM_CYSCREEN) / 4;
        SetTimer(hWnd, 1, 30, NULL);
    }
    break;
    case WM_TIMER:
    if (paused) break;
    if (logic == 0) {
        // 更新窗口位置
        windowPosX += windowSpeedX;
        windowPosY += windowSpeedY;

        // 获取屏幕尺寸
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // 边界检测（反弹效果）
        RECT windowRect;
        GetWindowRect(hWnd, &windowRect);
        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;

        if (windowPosX <= 0 || windowPosX + windowWidth >= screenWidth)
        {
            windowSpeedX = -windowSpeedX;
        }
        if (windowPosY <= 0 || windowPosY + windowHeight >= screenHeight)
        {
            windowSpeedY = -windowSpeedY;
        }

        // 移动窗口
        SetWindowPos(
            hWnd,
            NULL,
            windowPosX,
            windowPosY,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER);
    }
    if (logic == 1) {
        wavePosition += waveSpeed;

        // 计算X位置（线性移动）
        int xPos = static_cast<int>(wavePosition * 100) % GetSystemMetrics(SM_CXSCREEN);
        baseY = GetSystemMetrics(SM_CYSCREEN) / 2;

        // 计算Y位置（🐍浪形）
        int yPos = baseY + static_cast<int>(sin(wavePosition) * waveAmplitude);

        // 确保窗口不会移出屏幕右侧
        RECT windowRect;
        GetWindowRect(hWnd, &windowRect);
        int windowWidth = windowRect.right - windowRect.left;

        if (xPos + windowWidth > GetSystemMetrics(SM_CXSCREEN))
        {
            xPos = GetSystemMetrics(SM_CXSCREEN) - windowWidth;
        }

        // 移动窗口
        SetWindowPos(
            hWnd,
            NULL,
            xPos,
            yPos,
            0,
            0,
            SWP_NOSIZE | SWP_NOZORDER);
    }
    break;
    case WM_KEYDOWN:
    if (wParam == VK_RETURN) {
        logic = (logic == 0) ? 1 : 0;
    }
    if (wParam == VK_SPACE) {
        paused = !paused;
    }
    if (wParam == 'M') {
        // 切换静音状态
        if (!muted) {
            PlaySoundW(NULL, NULL, 0);
            muted = true;
        } else {
            PlaySoundW(L"🐍.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            muted = false;
        }
    }
    // 调整速度和幅度
    if (wParam == VK_OEM_PLUS || wParam == VK_ADD || wParam == VK_UP || wParam == VK_RIGHT) {
        // 线性模式：加快速度
        if (windowSpeedX > 0) windowSpeedX++;
        else windowSpeedX--;
        if (windowSpeedY > 0) windowSpeedY++;
        else windowSpeedY--;
        // 🐍浪模式：加快🐍浪速度和幅度
        waveAmplitude += 10;
        waveSpeed += 0.05;
    }
    if (wParam == VK_OEM_MINUS || wParam == VK_SUBTRACT || wParam == VK_DOWN || wParam == VK_LEFT) {
        // 线性模式：减慢速度
        if (abs(windowSpeedX) > 1) windowSpeedX += (windowSpeedX > 0 ? -1 : 1);
        if (abs(windowSpeedY) > 1) windowSpeedY += (windowSpeedY > 0 ? -1 : 1);
        // 🐍浪模式：减慢🐍浪速度和幅度
        if (waveAmplitude > 10) waveAmplitude -= 10;
        if (waveSpeed > 0.05) waveSpeed -= 0.05;
    }
        break;
    case WM_NCLBUTTONDOWN:
    case WM_MOVING:
        paused = true;
        return DefWindowProcW(hWnd, message, wParam, lParam); // 允许拖动窗口
    case WM_MOVE: // 完成移动
        paused = false;
        // 设置新的窗口位置
        {
            RECT windowRect;
            GetWindowRect(hWnd, &windowRect);
            windowPosX = windowRect.left;
            windowPosY = windowRect.top;
        }
        return DefWindowProcW(hWnd, message, wParam, lParam);
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 设置背景颜色
        RECT rect;
        GetClientRect(hWnd, &rect);
        HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // 设置文本颜色
        SetTextColor(hdc, RGB(0, 128, 0)); // 绿色

        // 绘制🐍形字符
        std::wstring _u_1f40d_ = L"🐍";
        RECT rc{
            .left = 50,
            .top = 50,
            .right = 100,
            .bottom = 100,
        };
        DrawTextW(hdc, _u_1f40d_.c_str(), (int)_u_1f40d_.length(), &rc, DT_TOP | DT_LEFT | DT_NOCLIP);

        // 恢复原来的字体
        SelectObject(hdc, hOldFont);
        
        hOldFont = (HFONT)SelectObject(hdc, hFontMs);

        // 设置文本颜色
        SetTextColor(hdc, RGB(0, 0, 0)); // 绿色

        // 绘制🐍形字符
        rc.left = 150;
        rc.right = 200;
        DrawTextW(hdc, _u_1f40d_.c_str(), (int)_u_1f40d_.length(), &rc, DT_TOP | DT_LEFT | DT_NOCLIP);

        // 恢复原来的字体
        SelectObject(hdc, hOldFont);

        TextOutW(hdc, 0, 0, L"Space - Pause | M - Mute | Enter - Switch Mode", 46);

        EndPaint(hWnd, &ps);
        break;
    }
    case WM_NCHITTEST:
        return HTCAPTION;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 注册🐍🐍类
ATOM RegisterWindowClass()
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInst;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = WINDOW_CLASS;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    
    return RegisterClassExW(&wcex);
}

// 初始化🐍🐍实例
BOOL InitInstance(int nCmdShow)
{
    HWND hWnd = CreateWindowExW(
        WS_EX_TOPMOST,
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPED | WS_SYSMENU,
        CW_USEDEFAULT, 0,
        300, 200,
        NULL,
        NULL,
        hInst,
        NULL
    );
    
    if (!hWnd)
    {
        return FALSE;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    return TRUE;
}

// 主🐍数
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR lpCmdLine,
                     _In_ int nCmdShow)
{
    hInst = hInstance;
    
    // 注册🐍🐍类
    if (!RegisterWindowClass())
    {
        MessageBoxW(NULL, L"🐍🐍类注册失败!", L"🐍误", MB_ICONERROR);
        return 1;
    }
    
    // 初始化🐍🐍实例
    if (!InitInstance(nCmdShow))
    {
        MessageBoxW(NULL, L"🐍🐍创建失败!", L"🐍误", MB_ICONERROR);
        return 1;
    }
    
    // 主消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return (int)msg.wParam;
}