#include <iostream>
#include "../../../win32-oop-util/Window.hpp"
#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
using namespace w32oop;
using namespace w32oop::foundation;
using namespace std;
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <string>
#include <cmath>
#include <set>
#include <thread>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

using namespace w32oop;

class SnakeWindow;
set<SnakeWindow *> windows;
class SnakeWindow : public Window {
private:
    HFONT hFont, hFontMs;
    int windowPosX = 100;
    int windowPosY = 100;
    int windowSpeedX = 10;
    int windowSpeedY = 4;
    double wavePosition = 0.0;
    double waveSpeed = 0.2;
    int waveAmplitude = 100;
    int baseY = 200;
    bool paused = false;
    int logic = 0;
    bool muted = false;
	bool destroyed = false;

public:
    SnakeWindow()
        : Window(L" ", 300, 200, 100, 100, WS_OVERLAPPED | WS_SYSMENU) {
        // 创建字体
        hFont = CreateFontW(
            72, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            VARIABLE_PITCH | FF_DECORATIVE | FF_MODERN, L"Segoe UI Emoji"
        );

        hFontMs = CreateFontW(
            72, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
            VARIABLE_PITCH | FF_DECORATIVE | FF_MODERN, L"Microsoft YaHei"
        );

		muteButton = pauseButton = switchModeButton = increaseSpeedButton = decreaseSpeedButton = nullptr;
    }

    ~SnakeWindow() override {
        if (hFont) DeleteObject(hFont);
        if (hFontMs) DeleteObject(hFontMs);
    }

    const wstring get_class_name() const override final {
        return L"🐍🐍🐍WindowClass";
    }

	const bool is_destroyed() const {
		return destroyed;
	}
protected:
    Button* pauseButton, * muteButton, * switchModeButton,
		* increaseSpeedButton, * decreaseSpeedButton;

    void onCreated() override {
        // 播放当前目录下的🐍.wav文件
        if (GetFileAttributesW(L"🐍.wav") != INVALID_FILE_ATTRIBUTES)
            PlaySoundW(L"🐍.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

        waveAmplitude = GetSystemMetrics(SM_CYSCREEN) / 4;
        SetTimer(*this, 1, 30, NULL);

        windows.insert(this);

        // 初始化
		set_topmost(true);
        // 创建控件
		pauseButton = new Button(*this, L"Pause", 70, 30, 10, 10);
		muteButton = new Button(*this, L"Mute", 60, 30, 90, 10);
		switchModeButton = new Button(*this, L"Switch Mode", 120, 30, 160, 10);
		increaseSpeedButton = new Button(*this, L"+", 20, 20, 10, 130);
		decreaseSpeedButton = new Button(*this, L"-", 20, 20, 40, 130);
		// 需要调用create()方法来创建控件
		pauseButton->create();
		muteButton->create();
		switchModeButton->create();
		increaseSpeedButton->create();
		decreaseSpeedButton->create();
		// 设置控件的事件处理函数
        pauseButton->onClick([this](Button*) {
            paused = !paused;
			if (paused) {
				pauseButton->text(L"Resume");
			}
			else {
				pauseButton->text(L"Pause");
			}
            return 0;
        });
		muteButton->onClick([this](Button*) {
			toggleMute();
			return 0;
		});
		switchModeButton->onClick([this](Button*) {
			logic = (logic == 0) ? 1 : 0;
			return 0;
		});
		increaseSpeedButton->onClick([this](Button*) {
			increaseSpeed();
			return 0;
		});
		decreaseSpeedButton->onClick([this](Button*) {
			decreaseSpeed();
			return 0;
		});
    }

private:
    LRESULT onTimer(WPARAM wParam, LPARAM lParam) {
        if (paused) return 0;

        if (logic == 0) {
            updateBouncingWindow();
        }
        else {
            updateWaveWindow();
        }

        return 0;
    }

    void updateBouncingWindow() {
        // 更新窗口位置
        windowPosX += windowSpeedX;
        windowPosY += windowSpeedY;

        // 获取屏幕尺寸
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // 边界检测（反弹效果）
        RECT windowRect;
        GetWindowRect(*this, &windowRect);
        int windowWidth = windowRect.right - windowRect.left;
        int windowHeight = windowRect.bottom - windowRect.top;

        if (windowPosX <= 0 || windowPosX + windowWidth >= screenWidth) {
            windowSpeedX = -windowSpeedX;
        }
        if (windowPosY <= 0 || windowPosY + windowHeight >= screenHeight) {
            windowSpeedY = -windowSpeedY;
        }

        // 移动窗口
        move(windowPosX, windowPosY);
    }

    void updateWaveWindow() {
        wavePosition += waveSpeed;

        // 计算X位置（线性移动）
        int xPos = static_cast<int>(wavePosition * 100) % GetSystemMetrics(SM_CXSCREEN);
        baseY = GetSystemMetrics(SM_CYSCREEN) / 2;

        // 计算Y位置（波浪形）
        int yPos = baseY + static_cast<int>(sin(wavePosition) * waveAmplitude);

        // 确保窗口不会移出屏幕右侧
        RECT windowRect;
        GetWindowRect(*this, &windowRect);
        int windowWidth = windowRect.right - windowRect.left;

        if (xPos + windowWidth > GetSystemMetrics(SM_CXSCREEN)) {
            xPos = GetSystemMetrics(SM_CXSCREEN) - windowWidth;
        }

        // 移动窗口
        move(xPos, yPos);
    }

    LRESULT onKeyDown(WPARAM wParam, LPARAM lParam) {
        switch (wParam) {
        case VK_RETURN:
            logic = (logic == 0) ? 1 : 0;
            break;

        case VK_SPACE:
            paused = !paused;
            break;

        case 'M':
            toggleMute();
            break;

        case VK_OEM_PLUS:
        case VK_ADD:
        case VK_UP:
        case VK_RIGHT:
            increaseSpeed();
            break;

        case VK_OEM_MINUS:
        case VK_SUBTRACT:
        case VK_DOWN:
        case VK_LEFT:
            decreaseSpeed();
            break;
        }
        return 0;
    }

    void toggleMute() {
        if (!muted) {
            PlaySoundW(NULL, NULL, 0);
            muted = true;
			muteButton->text(L"Play");
        }
        else {
            if (GetFileAttributesW(L"🐍.wav") != INVALID_FILE_ATTRIBUTES)
                PlaySoundW(L"🐍.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            muted = false;
			muteButton->text(L"Mute");
        }
    }

    void increaseSpeed() {
        // 线性模式：加快速度
        if (windowSpeedX > 0) windowSpeedX++;
        else windowSpeedX--;
        if (windowSpeedY > 0) windowSpeedY++;
        else windowSpeedY--;
        // 波浪模式：加快波浪速度和幅度
        waveAmplitude += 10;
        waveSpeed += 0.05;
    }

    void decreaseSpeed() {
        // 线性模式：减慢速度
        if (abs(windowSpeedX) > 1) windowSpeedX += (windowSpeedX > 0 ? -1 : 1);
        if (abs(windowSpeedY) > 1) windowSpeedY += (windowSpeedY > 0 ? -1 : 1);
        // 波浪模式：减慢波浪速度和幅度
        if (waveAmplitude > 10) waveAmplitude -= 10;
        if (waveSpeed > 0.05) waveSpeed -= 0.05;
    }

    LRESULT onPaint(WPARAM wParam, LPARAM lParam) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(*this, &ps);

        // 设置背景颜色
        RECT rect;
        GetClientRect(*this, &rect);
        HBRUSH hBrush = CreateSolidBrush(RGB(240, 240, 240));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);

        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

        // 设置文本颜色
        SetTextColor(hdc, RGB(0, 128, 0)); // 绿色

        // 绘制🐍形字符
        std::wstring snake = L"🐍";
        RECT rc{ 50, 50, 100, 100 };
        DrawTextW(hdc, snake.c_str(), (int)snake.length(), &rc, DT_TOP | DT_LEFT | DT_NOCLIP);

        // 恢复原来的字体
        SelectObject(hdc, hOldFont);
        hOldFont = (HFONT)SelectObject(hdc, hFontMs);

        // 设置文本颜色
        SetTextColor(hdc, RGB(0, 0, 0)); // 黑色

        // 绘制🐍形字符
        rc.left = 150;
        rc.right = 200;
        DrawTextW(hdc, snake.c_str(), (int)snake.length(), &rc, DT_TOP | DT_LEFT | DT_NOCLIP);

        // 恢复原来的字体
        SelectObject(hdc, hOldFont);

        EndPaint(*this, &ps);
        return 0;
    }

    LRESULT onNcLButtonDown(WPARAM wParam, LPARAM lParam) {
        paused = true;
        return DefWindowProc(*this, WM_NCLBUTTONDOWN, wParam, lParam);
    }

    LRESULT onMoving(WPARAM wParam, LPARAM lParam) {
        paused = true;
        return DefWindowProc(*this, WM_MOVING, wParam, lParam);
    }

    LRESULT onMove(WPARAM wParam, LPARAM lParam) {
        paused = false;
        // 设置新的窗口位置
        RECT windowRect;
        GetWindowRect(*this, &windowRect);
        windowPosX = windowRect.left;
        windowPosY = windowRect.top;
        return DefWindowProc(*this, WM_MOVE, wParam, lParam);
    }

    LRESULT onNcHitTest(WPARAM wParam, LPARAM lParam) {
        return HTCAPTION;
    }

    LRESULT onDestroy(WPARAM wParam, LPARAM lParam) {
        KillTimer(*this, 1);
        windows.erase(this);
        if (windows.size() < 1) PostQuitMessage(0); // 确保所有窗口都关闭后退出程序
		//delete this; // 不可以delete this，否则会导致访问冲突
		destroyed = true; // gc线程会删除对象
        return 0;
    }

    LRESULT onCreateNewInstance(WPARAM wParam, LPARAM lParam) {
        SnakeWindow* newWindow = new SnakeWindow();
        newWindow->create();
        newWindow->show();
        return 0;
    }

    WINDOW_EVENT_HANDLER_DECLARE_BEGIN()
        WINDOW_add_handler(WM_TIMER, onTimer);
        WINDOW_add_handler(WM_KEYDOWN, onKeyDown);
        WINDOW_add_handler(WM_PAINT, onPaint);
        WINDOW_add_handler(WM_NCLBUTTONDOWN, onNcLButtonDown);
        WINDOW_add_handler(WM_MOVING, onMoving);
        WINDOW_add_handler(WM_MOVE, onMove);
        WINDOW_add_handler(WM_NCHITTEST, onNcHitTest);
        WINDOW_add_handler(WM_DESTROY, onDestroy);
        WINDOW_add_handler(WM_USER + 1, onCreateNewInstance);
    WINDOW_EVENT_HANDLER_DECLARE_END()
};

// GC
void gc() {
    do {
        Sleep(50);
		for (auto& window : windows) {
			if (window->is_destroyed()) {
                Sleep(10);
				windows.erase(window); // 从集合中删除已销毁的窗口
				delete window; // 删除窗口对象
				break; // 直接开始下一轮gc,因为iterator失效了
			}
		}
	} while (windows.size() > 0); // 等待所有窗口关闭
}

// state
DWORD WINAPI state_thread(PVOID) {
    while (1) {
        SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);
		Sleep(60000);
	}
    return 0;
}


int main() {
    HWND existing = FindWindowW(SnakeWindow().get_class_name().c_str(), nullptr);
    if (existing) {
        SendMessageW(existing, WM_USER + 1, 0, 0); // 发送自定义消息
        return 0;
    }

    try {
		std::thread gcThread(gc);
		HANDLE hStateThread = CreateThread(NULL, 0, state_thread, NULL, 0, NULL);

        SnakeWindow* window = new SnakeWindow();
        window->create();
        window->show();

        int result = Window::run();
		gcThread.join(); // 等待GC线程结束
#pragma warning(suppress: 28167) // 关闭警告：可能会导致访问冲突
		TerminateThread(hStateThread, 0); // 终止状态线程
		CloseHandle(hStateThread); // 关闭线程句柄
		return result;
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
        return -1;
    }
}