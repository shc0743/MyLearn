#include "select.hpp"
#include "start.hpp"

void app::Select::onCreated() {
	btn1 = Button(hwnd, L"Record Mode", 1, 1);
	btn2 = Button(hwnd, L"Replay Mode", 1, 1);
	btn1.create(); btn2.create();
	btn1.onClick([this](EventData&) {
		StartNw(L"app record", app_path().c_str());
		close();
	});
	btn2.onClick([this](EventData&) {
		StartNw(L"app replay", app_path().c_str());
		close();
	});
}

void app::Select::doLayout(EventData& ev) {
	if (!created()) return;
	
	RECT rc{}; GetClientRect(hwnd, &rc);
	// 计算可用区域（减去边距）
	int width = rc.right - rc.left - 20;  // 左右各10px边距
	int height = (rc.bottom - rc.top) / 2 - 20; // 每个按钮高度为窗口一半，减去上下边距
	// 计算按钮位置
	int btnWidth = width;
	int btnHeight = height;
	// 第一个按钮：上半部分
	int btn1X = 10;  // 左边距10px
	int btn1Y = 10;  // 上边距10px
	btn1.resize(btn1X, btn1Y, btnWidth, btnHeight);
	// 第二个按钮：下半部分
	int btn2X = 10;  // 左边距10px
	int btn2Y = (rc.bottom - rc.top) / 2;  // 从窗口中间开始，加上10px边距
	btn2.resize(btn2X, btn2Y, btnWidth, btnHeight);
}
