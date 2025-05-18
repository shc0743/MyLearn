// compile command: cl /EHsc snake_demo.cpp /std:c++20
#define UNICODE 1
#define _UNICODE 1
#include "Window.hpp"
#include <random>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(linker, "/entry:mainCRTStartup /subsystem:windows")

using namespace w32oop;
using namespace w32oop::foundation;

class SnakeWindow : public Window {
private:
    const int SNAKE_SIZE = 20;
    int snakeX, snakeY;
    int velocityX, velocityY;
    COLORREF snakeColor;
    UINT_PTR timerId;

public:
    COLORREF generateRandomColor() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 255);
        
        return RGB(dis(gen), dis(gen), dis(gen));
    }

    SnakeWindow(const wstring& title, int width, int height, int x = 0, int y = 0)
        : Window(title, width, height, x, y, WS_OVERLAPPEDWINDOW),
          snakeX(100), snakeY(100), velocityX(10), velocityY(10), snakeColor(RGB(0, 0, 0)) {
    }

    ~SnakeWindow() override
    {
        if (timerId)
        {
            KillTimer(hwnd, timerId);
        }
        if (emojiFont)
        {
            DeleteObject(emojiFont);
        }
    }

private:
    HFONT emojiFont = nullptr;

    const wstring get_class_name() const override
    {
        return L"SnakeWindowClass";
    }

protected:
    void onCreated() override
    {
        // Initialize snake position at center
        RECT rect;
        GetClientRect(hwnd, &rect);
        snakeX = (rect.right - rect.left) / 2;
        snakeY = (rect.bottom - rect.top) / 2;

        // Create emoji font (Segoe UI Emoji)
        emojiFont = CreateFontW(
            48,                  // Height
            0,                   // Width
            0,                   // Escapement
            0,                   // Orientation
            FW_NORMAL,           // Weight
            FALSE,               // Italic
            FALSE,               // Underline
            FALSE,               // Strikeout
            DEFAULT_CHARSET,     // CharSet
            OUT_OUTLINE_PRECIS,  // OutPrecision
            CLIP_DEFAULT_PRECIS, // ClipPrecision
            CLEARTYPE_QUALITY,   // Quality
            VARIABLE_PITCH,      // PitchAndFamily
            L"Segoe UI Emoji"    // Font name
        );

        // Start a timer for animation (30 FPS)
        timerId = SetTimer(hwnd, 1, 33, nullptr);
    }

    LRESULT onPaint(WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw background
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        FillRect(hdc, &clientRect, (HBRUSH)(COLOR_WINDOW + 1));

        // Select emoji font
        HFONT oldFont = (HFONT)SelectObject(hdc, emojiFont);

        // Set text color and background mode
        SetTextColor(hdc, snakeColor);
        SetBkMode(hdc, TRANSPARENT);

        // Prepare rect for snake emoji (adjust size as needed)
        RECT snakeRect = {
            snakeX - 24, // Center the emoji on the position
            snakeY - 24,
            snakeX + 24,
            snakeY + 24};

        // Draw snake emoji "🐍"
        DrawTextW(hdc, L"🐍", -1, &snakeRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // Restore old font
        SelectObject(hdc, oldFont);

        EndPaint(hwnd, &ps);
        return 0;
    }

    void updateSnakePosition()
    {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Update position
        snakeX += velocityX;
        snakeY += velocityY;

        // Check for collisions with window edges (adjust bounds for emoji size)
        if (snakeX - 24 <= 0 || snakeX + 24 >= clientRect.right)
        {
            velocityX = -velocityX;
            snakeColor = generateRandomColor();
        }
        if (snakeY - 24 <= 0 || snakeY + 24 >= clientRect.bottom)
        {
            velocityY = -velocityY;
            snakeColor = generateRandomColor();
        }

        // Request a repaint
        InvalidateRect(hwnd, nullptr, FALSE);
    }
    
    LRESULT onTimer(WPARAM wParam, LPARAM lParam) {
        updateSnakePosition();
        return 0;
    }

private:
    WINDOW_EVENT_HANDLER_DECLARE_BEGIN()
        WINDOW_add_handler(WM_PAINT, onPaint);
        WINDOW_add_handler(WM_TIMER, onTimer);
    WINDOW_EVENT_HANDLER_DECLARE_END()
};

int main() {
    try {
        SnakeWindow window(L"Snake Demo", 480, 320);
        window.create();
        window.set_main_window();
        window.center();
        window.show();
        return Window::run();
    }
    catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Error", MB_OK | MB_ICONERROR);
        return -1;
    }
}