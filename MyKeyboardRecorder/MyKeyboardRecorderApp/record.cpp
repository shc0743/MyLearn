#include <chrono>
#include "record.hpp"
#include "start.hpp"
#include "vk2hid.hpp"
#include "Splash.hpp"
using namespace std;

inline uint64_t getCurrentTimeMillis() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

void app::Record::onCreated() {
	btnElevate = Button(hwnd, L"Elevate", 1, 1);
	staticFile = Static(hwnd, L"Record file:", 1, 1);
	editFilesave = Edit(hwnd, L"", 1, 1);
	btnChooseFile = Button(hwnd, L"Choose", 1, 1);
	btnStart = Button(hwnd, L"Start", 1, 1); // 开始之后会变成“Pause”
	btnStop = Button(hwnd, L"Stop", 1, 1);

	btnElevate.create();
	staticFile.create();
	editFilesave.create();
	btnChooseFile.create();
	btnStart.create();
	btnStop.create();

	//hook.owner = this;

	// 设置事件处理
	btnElevate.onClick([this](EventData&) {
		auto a = app_path();
		SHELLEXECUTEINFOW sei = { sizeof(sei) };
		sei.lpVerb = L"runas";  // 请求管理员权限
		sei.lpFile = a.c_str();
		sei.lpParameters = L"record";  // 可以添加需要的命令行参数
		sei.hwnd = nullptr;
		sei.nShow = SW_NORMAL;
		if (ShellExecuteExW(&sei)) {
			// 提升权限成功，退出当前实例
			close();
		}
	});

	btnChooseFile.onClick([this](EventData&) {
		OPENFILENAME ofn{};
		wchar_t filePath[MAX_PATH] = L"";

		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"Record Files (*.kbr)\0*.kbr\0All Files (*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST;

		ofn.lpstrDefExt = L"kbr";

		if (GetSaveFileNameW(&ofn)) {
			editFilesave.text(filePath);
		}
	});

	btnStart.onClick([this](EventData&) {
		StartOrPause();
	});

	btnStop.onClick([this](EventData&) {
		Stop();
	});

	// 初始状态设置
	btnStop.disable();
}


void app::Record::onDestroy() {
	if (isStarted) {
		isRunning = isStarted = false;
		hFile.close();
		unregisterRawInput();
	}
}


void app::Record::StartOrPause() {
	// 判断情况
	if (!isStarted) {
		// 开始
		try {
			// 打开文件
			DWORD mode = OPEN_ALWAYS; bool alreadyExists = false;
			if (GetFileAttributesW(editFilesave.text().c_str()) != INVALID_FILE_ATTRIBUTES) {
				alreadyExists = true;
				TASKDIALOG_BUTTON btns[] = {
					{1,L"Override"},{2,L"Append"},{3,L"Cancel"},
				};
				TASKDIALOGCONFIG cfg{};
				cfg.cbSize = sizeof(cfg);
				cfg.cButtons = 3;
				cfg.hwndParent = hwnd;
				cfg.pszWindowTitle = L"Open File";
				cfg.pszMainInstruction = L"This file already exists.";
				cfg.pszContent = L"Would you want to override it, or append to it?";
				cfg.pButtons = btns;
				cfg.nDefaultButton = 3;
				int user = 0;
				TaskDialogIndirect(&cfg, &user, NULL, NULL);
				if (user == 3) return;
				if (user == 1) mode = CREATE_ALWAYS;
			}
			hFile = CreateFileW(editFilesave.text().c_str(), GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ, NULL, mode, FILE_ATTRIBUTE_NORMAL, NULL);
			if (alreadyExists) SetFilePointer(hFile, 0, 0, FILE_END);

#if 0
			// 设置hook
			hook.set(WH_KEYBOARD_LL, 0);
#endif
#if 1
			registerRawInput();
			if (!rawInputRegistered) throw exception();
#endif
		}
		catch (...) {
			TaskDialog(hwnd, NULL, L"Keyboard Recorder", 
				L"Unable to start recording", ErrorChecker().message().c_str(),
				TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, NULL);
			return;
		}
		isStarted = isRunning = true;
		btnStart.text(L"Pause");
		btnElevate.disable();
		btnStop.enable();
		editFilesave.disable();
		btnChooseFile.disable();
		startTime = getCurrentTimeMillis();
	}
	else if (!isRunning) {
		// 继续
		isRunning = true;
		btnStart.text(L"Pause");
	}
	else {
		// 暂停
		isRunning = false;
		btnStart.text(L"Continue");
	}
}


void app::Record::Stop() {
	isRunning = isStarted = false;
	unregisterRawInput();
	enable(false);
	std::thread([this]() {
		Splash splash;
		splash.create();
		splash.center(hwnd);
		splash.show();
		splash.setSplashText(L"Saving records to disk...");
		std::thread([this](Splash* p) {
			auto start = getCurrentTimeMillis();
			// Dump records to file
			DWORD sizeToWrite = sizeof(RecordItem), written = 0;
			for (auto& i : records) {
				bool ok = WriteFile(hFile, &i, sizeToWrite, &written, NULL);
				if (!ok) break;
			}
			auto timeUsed = getCurrentTimeMillis() - start;
			if (timeUsed < 2000) {
				Sleep(DWORD(2000 - timeUsed)); // 显得正在干活
			}
			// Finalize
			p->finish();
			hFile.close();
			enable(true);
			// clear records in memory
			records.clear();
			// reset ui
			btnStop.disable();
			btnStart.enable();
			btnElevate.enable();
			editFilesave.enable();
			btnChooseFile.enable();
			btnStart.text(L"Start");
		}, &splash).detach();
		splash.run();
	}).detach();
}


void app::Record::addRecord(RecordItem item) {
	item.t = getCurrentTimeMillis() - startTime;
	records.push_back(item);
}


void app::Record::onClose(EventData& ev) {
	if (isStarted) {
		ev.preventDefault();
	}
}


#if 1

void app::Record::registerRawInput() {
	if (rawInputRegistered) return;
	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01;   // Generic Desktop Controls
	rid.usUsage = 0x06;       // Keyboard
	rid.dwFlags = RIDEV_INPUTSINK; // 接收所有键盘输入，即使窗口无焦点
	rid.hwndTarget = hwnd;    // 你的窗口句柄（Window::hwnd）

	if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
		rawInputRegistered = false;
		return;
	}
	rawInputRegistered = true;
}

void app::Record::unregisterRawInput() {
	if (!rawInputRegistered) return;
	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x06;
	rid.dwFlags = RIDEV_REMOVE; // 移除之前的注册
	rid.hwndTarget = NULL;

	RegisterRawInputDevices(&rid, 1, sizeof(rid));
	rawInputRegistered = false;
}

void app::Record::onRawInput(EventData& ev) {
	// 期望 EventData 有成员 wParam/lParam； 若你的框架成员名不同（例如 ev.lparam），
	// 请把下面 (LPARAM)ev.lParam 改为正确字段名。
	// （你在项目里其他地方也许能看到 EventData 用法；通常是 ev.wParam / ev.lParam）
	LPARAM lParam = ev.lParam; // ← 如果编译报错，改为 ev.lparam 或 ev.get_lParam() 等你的框架接口

	UINT dwSize = 0;
	// 第一次调用取得缓冲区大小
	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER)) == (UINT)-1) {
		return;
	}
	std::vector<BYTE> buffer(dwSize);
	if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
		return;
	}
	RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());
	if (!raw) return;
	if (raw->header.dwType != RIM_TYPEKEYBOARD) return;

	const RAWKEYBOARD& kb = raw->data.keyboard;
	bool isBreak = (kb.Flags & RI_KEY_BREAK) != 0; // true = key up, false = key down (make)
	// kb.MakeCode = scan code; kb.VKey = virtual key code (may be 255 for some keys)
	UINT vKey = kb.VKey;

	// 计算 HID modifier (按你原有代码的规则)
	uint8_t modifier = 0;
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) modifier |= 0x01;
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) modifier |= 0x02;
	if (GetAsyncKeyState(VK_LMENU) & 0x8000) modifier |= 0x04;
	if (GetAsyncKeyState(VK_LWIN) & 0x8000) modifier |= 0x08;
	if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) modifier |= 0x10;
	if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) modifier |= 0x20;
	if (GetAsyncKeyState(VK_RMENU) & 0x8000) modifier |= 0x40;
	if (GetAsyncKeyState(VK_RWIN) & 0x8000) modifier |= 0x80;

	app::RecordItem item{};
	item.t = getCurrentTimeMillis() - startTime;
	item.modifier = modifier;
	item.reserved = 0;
	for (int i = 0; i < 6; ++i) item.keycode[i] = 0;

	// 若是 keydown (make)，把第一个有效 key 填入； 若是 keyup，发送 release (全0 keycode)
	if (!isBreak) {
		// 首先尝试直接用 VKey 去映射到 HID（你 project 中已有 VK_TO_HID 映射）
		// 如果 VKey == 255 或未映射，再尝试使用 MapVirtualKey 将 scancode 转成 VK
		uint8_t hid = 0;
		// 假设 vk2hid.hpp 中定义了 VK_TO_HID map<UINT,uint8_t>
		auto it = VK_TO_HID.find((int)vKey);
		if (it != VK_TO_HID.end()) {
			hid = it->second;
		}
		else {
			// 尝试从 scancode（MakeCode）转换到 VK，再查表
			UINT sc = kb.MakeCode;
			// MAPVK_VSC_TO_VK_EX 可把扩展键区分开（需要键盘布局句柄）
			HKL layout = GetKeyboardLayout(0);
			UINT vk_from_sc = MapVirtualKeyEx(sc, MAPVK_VSC_TO_VK_EX, layout);
			auto it2 = VK_TO_HID.find((int)vk_from_sc);
			if (it2 != VK_TO_HID.end()) hid = it2->second;
		}

		if (hid != 0) {
			item.keycode[0] = hid;
			// 记录按下
			addRecord(item);
		}
		else {
			// 未知键：如果需要也可以把 scancode 写入日志或作为特殊处理
			// 目前忽略未知键（不写入记录）
		}
	}
	else {
		// key up: 记录 release 报告（keycode 全 0），保留 modifier 状态（按你原来实现）
		for (int i = 0; i < 6; ++i) item.keycode[i] = 0;
		addRecord(item);
	}
}
#endif
#if 0
LRESULT app::Record::RecordHook::callback(int nCode, WPARAM wParam, LPARAM lParam) {
	// 如果 代码 小于零，挂钩过程必须将消息传递给 CallNextHookEx 函数，而无需进一步处理，并且应返回 CallNextHookEx 返回的值。
	// https://learn.microsoft.com/zh-cn/windows/win32/winmsg/keyboardproc
	PKBDLLHOOKSTRUCT p = reinterpret_cast<PKBDLLHOOKSTRUCT>(lParam);
	if ((nCode < 0) || (!p)) {
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	}
	int vk = p->vkCode;

	// --- 计算 modifier 字节（USB HID modifier 位）
	// HID modifier 位: bit0 LCtrl, bit1 LShift, bit2 LAlt, bit3 LGUI, bit4 RCtrl, bit5 RShift, bit6 RAlt, bit7 RGUI
	uint8_t modifier = 0;
	if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) modifier |= 0x01;
	if (GetAsyncKeyState(VK_LSHIFT) & 0x8000) modifier |= 0x02;
	if (GetAsyncKeyState(VK_LMENU) & 0x8000) modifier |= 0x04;
	if (GetAsyncKeyState(VK_LWIN) & 0x8000) modifier |= 0x08;
	if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) modifier |= 0x10;
	if (GetAsyncKeyState(VK_RSHIFT) & 0x8000) modifier |= 0x20;
	if (GetAsyncKeyState(VK_RMENU) & 0x8000) modifier |= 0x40;
	if (GetAsyncKeyState(VK_RWIN) & 0x8000) modifier |= 0x80;

	// 构造记录项（默认全 0）
	RecordItem item{};
	item.modifier = modifier;
	item.reserved = 0;
	for (int i = 0; i < 6; ++i) item.keycode[i] = 0;

	// 只在按下时填入 keycode（简化：只填入首个按键槽；若需要支持多键并存，需维护一个按键集合）
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		auto it = VK_TO_HID.find(vk);
		if (it != VK_TO_HID.end()) {
			item.keycode[0] = it->second;
		}
		else {
			// 未找到映射：尝试按 ASCII 字符映射（字母/数字）
			if (vk >= 'A' && vk <= 'Z') {
				item.keycode[0] = static_cast<uint8_t>(0x04 + (vk - 'A'));
			}
			else if (vk >= '0' && vk <= '9') {
				// 注意：'0' 的 HID 是 0x27
				if (vk == '0') item.keycode[0] = 0x27;
				else item.keycode[0] = static_cast<uint8_t>(0x1E + (vk - '1'));
			}
			else {
				// 未知键：保留 keycode 为 0
				item.keycode[0] = 0;
			}
		}

		// 发送/记录 HID 报告（按键按下）
		if (owner) owner->addRecord(item);
	}
	else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
		// 按键释放：发送空 keycode（保留 modifier 的当前状态）
		// （某些实现也会把 modifier 在释放时一起清理；此处使用即时的 GetAsyncKeyState 结果）
		item.modifier = modifier;
		for (int i = 0; i < 6; ++i) item.keycode[i] = 0;
		if (owner) owner->addRecord(item);
	}

	// 最后把消息传给下一个钩子
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}
#endif


// ------

void app::Record::doLayout(EventData& ev) {
	if (!created()) return;
	RECT rc{}; GetClientRect(hwnd, &rc);

	const int margin = 10;
	const int buttonHeight = 30;
	const int controlHeight = 25;

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	// 第一行：[Elevate]按钮放在最右边
	int elevateBtnWidth = 80;
	int elevateBtnX = width - elevateBtnWidth - margin;
	btnElevate.resize(elevateBtnX, margin, elevateBtnWidth, buttonHeight);

	// 第二行：[staticFile] [editFilesave] [btnChooseFile]
	int secondLineY = margin + buttonHeight + margin;

	// staticFile - 固定宽度
	int staticWidth = 90;
	staticFile.resize(margin, secondLineY, staticWidth, controlHeight);

	// btnChooseFile - 固定宽度
	int chooseBtnWidth = 60;
	int chooseBtnX = width - chooseBtnWidth - margin;
	btnChooseFile.resize(chooseBtnX, secondLineY, chooseBtnWidth, controlHeight);

	// editFilesave - 填满中间剩余空间
	int editX = margin + staticWidth + margin;
	int editWidth = chooseBtnX - editX - margin;
	editFilesave.resize(editX, secondLineY, editWidth, controlHeight);

	// 第三部分：按钮区域（填满剩余垂直空间）
	int areaTop = secondLineY + controlHeight + margin;
	int areaHeight = height - areaTop - margin;

	// [btnStart] 和 [btnStop] 平分区域宽度
	int btnWidth = (width - 3 * margin) / 2; // 减去3个边距（左、中间、右）
	int btnY = areaTop;
	int btnAreaHeight = areaHeight - 0 * margin;

	// btnStart - 左边
	btnStart.resize(margin, btnY, btnWidth, btnAreaHeight);

	// btnStop - 右边
	int stopBtnX = margin + btnWidth + margin;
	btnStop.resize(stopBtnX, btnY, btnWidth, btnAreaHeight);
}
