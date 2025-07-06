#include <iostream>
#include <w32use.hpp>
#include "../../resource/tool.h"
using namespace std;

#pragma comment(linker, "/entry:mainCRTStartup")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class ApplicationForwarderWindow :public Window {
public:
	ApplicationForwarderWindow() : Window(L"Application Forwarder", 640, 480, 0, 0, WS_OVERLAPPEDWINDOW) {};

protected:
	Edit appName, target, default0;
	Button createBtn, deleteBtn;
	Edit logBox;

	void onCreated() {
		appName.set_parent(this);
		appName.create(L"Original App Name", 600, 25, 10, 10);

		target.set_parent(this);
		target.create(L"Target App Cmd-Line (Use %1 to refer arguments)", 600, 25, 10, 40);

		default0.set_parent(this);
		default0.create(L"Default %1 value", 600, 25, 10, 40);

		createBtn.set_parent(this);
		createBtn.create(L"Create", 100, 30, 10, 70, Button::STYLE | BS_DEFPUSHBUTTON);

		deleteBtn.set_parent(this);
		deleteBtn.create(L"Delete", 100, 30, 120, 70);

		logBox.set_parent(this);
		logBox.create(L"", 600, 320, 10, 110, Edit::STYLE | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL);
		logBox.readonly(true);
		logBox.text(L"Logs will show here");

		createBtn.onClick([this](EventData& ev) { 
			if (target.text().empty()) {
				MessageBoxW(hwnd, L"Please enter a target command line", L"Error", MB_OK | MB_ICONERROR);
				return;
			}
			if (appName.text().empty()) {
				MessageBoxW(hwnd, L"Please enter original app", L"Error", MB_OK | MB_ICONERROR);
				return;
			}

			wstring gid = GenerateUUIDWithoutDelimW();
			wstring debugger_value = L"\"" + GetProgramDirW() + L"\" -x" + gid + L" -- ";
			wstring location = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options";

			// 设置注册表
			try {
				RegistryKey hklm(HKEY_LOCAL_MACHINE);
				RegistryKey appreg = hklm.create(L"SOFTWARE\\MyApplicationForwarderConfig");
				// 保存命令行到注册表
				RegistryValue appValue((target.text()), REG_SZ);
				appreg.set(gid, appValue);
				RegistryValue appValue2((default0.text()), REG_SZ);
				appreg.set(gid + L"_default", appValue2);
				// 创建 Image File Execution Options 子键
				RegistryKey reg(hklm, location);
				RegistryKey subkey = reg.create(appName.text());
				RegistryValue value(debugger_value, REG_SZ);
				subkey.set(L"Debugger", value);
				MessageBoxW(hwnd, L"Application Forwarder created successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
				logBox.text(L"Application Forwarder created successfully for: " + appName.text() + L"\r\nTarget: " +
					target.text() + L"\r\nDebugger: " + debugger_value);
			}
			catch (w32oop::exceptions::registry_exception& exc) {
				MessageBoxW(hwnd, w32oop::util::str::converts::str_wstr(exc.what()).c_str(), L"Registry Error", MB_OK | MB_ICONERROR);
			}
		});

		deleteBtn.onClick([this](EventData& ev) {
			if (appName.text().empty()) {
				MessageBoxW(hwnd, L"Please enter original app", L"Error", MB_OK | MB_ICONERROR);
				return;
			}
			wstring location = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options";
			// 删除注册表
			try {
				RegistryKey reg(HKEY_LOCAL_MACHINE, location);
				if (!reg.exists(appName.text()) || !reg.open(appName.text()).exists_value(L"Debugger")) {
					MessageBoxW(hwnd, L"Application Forwarder does not exist, no need to remove",
						L"Information", MB_OK | MB_ICONINFORMATION);
					return;
				}
				// 先查出来GID是什么？
				logBox.text(L"");
				try {
					RegistryKey subkey = reg.open(appName.text());
					wstring cmd = subkey.get(L"Debugger").get<wstring>();
					CmdLineW cl(cmd);
					wstring gid; cl.getopt(L"x", gid);
					if (!gid.empty()) {
						RegistryKey lm(HKEY_LOCAL_MACHINE);
						RegistryKey appreg = lm.open(L"SOFTWARE\\MyApplicationForwarderConfig");
						appreg.delete_value(gid);
						appreg.delete_value(gid + L"_default");
					}
				}
				catch (...) {
					logBox.text(L"Failed to remove data; ignored");
				}
				if (reg.delete_key_value(appName.text(), L"Debugger")) {
					if (reg.create(appName.text()).keys().empty()) {
						// 如果子键没有其他值，则删除整个子键
						reg.delete_key(appName.text());
					}
					MessageBoxW(hwnd, L"Application Forwarder deleted successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
					logBox.text(logBox.text() + L"\r\nApplication Forwarder deleted for: " + appName.text());
				}
				else {
					throw w32oop::exceptions::registry_exception("Failed!!" + LastErrorStrA());
				}
			}
			catch (w32oop::exceptions::registry_exception& exc) {
				MessageBoxW(hwnd, w32oop::util::str::converts::str_wstr(exc.what()).c_str(), L"Registry Error", MB_OK | MB_ICONERROR);
			}
		});

		DragAcceptFiles(hwnd, TRUE); // 允许拖放文件到窗口
	}

protected:
	void OnResize(EventData& e) {
		RECT rc{};
		GetClientRect(hwnd, &rc);
		int width = rc.right - rc.left;  // 窗口客户区宽度
		int height = rc.bottom - rc.top; // 窗口客户区高度

		// 控件布局参数
		constexpr int margin = 10;           // 边距
		constexpr int btnWidth = 100;        // 按钮宽度
		constexpr int btnHeight = 30;        // 按钮高度
		constexpr int editHeight = 25;       // 输入框高度
		constexpr int spacing = 10;          // 控件间距

		// 调整控件位置和大小
		appName.resize(margin, margin, width - 2 * margin, editHeight);
		target.resize(margin, margin + editHeight + spacing, width - 2 * margin, editHeight);
		default0.resize(margin, margin + 2 * (editHeight + spacing), width - 2 * margin, editHeight);

		int buttonContainerWidth = width - 2 * margin;          // 按钮容器总宽度
		int buttonWidth = (buttonContainerWidth - spacing) / 2; // 每个按钮的宽度（减去间距）
		createBtn.resize(
			margin, margin + 3 * (editHeight + spacing),
			buttonWidth, btnHeight
		);
		deleteBtn.resize(
			margin + buttonWidth + spacing, margin + 3 * (editHeight + spacing),
			buttonWidth, btnHeight
		);

		// 日志框填满剩余空间
		int logBoxY = margin + 3 * (editHeight + spacing) + btnHeight + spacing;
		int logBoxHeight = height - logBoxY - margin;
		logBox.resize(margin, logBoxY, width - 2 * margin, logBoxHeight);
	}

	void OnDrop(EventData& ev) {
		ev.preventDefault();

		// 获取拖放的文件列表
		HDROP hDrop = (HDROP)ev.wParam;
		UINT fileCount = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0); // 获取文件数量
		if (fileCount > 1) {
			logBox.text(L"Too many files dropped. Please drop only one file.");
		}
		else if (fileCount == 1) {
			wchar_t filePath[MAX_PATH]{};
			DragQueryFileW(hDrop, 0, filePath, MAX_PATH); // 获取第一个文件的路径
			// 检查文件是否存在
			if (GetFileAttributesW(filePath) != INVALID_FILE_ATTRIBUTES) {
				// 提取文件名
				wstring filename = filePath;
				filename = filename.substr(filename.find_last_of(L"\\") + 1);
				appName.text(filename); // 设置应用名称为文件路径
				logBox.text(L"File dropped: " + wstring(filePath));
			}
			else {
				logBox.text(L"File does not exist: " + wstring(filePath));
			}
		}
		else {
			logBox.text(L"No files dropped.");
		}
	}

	void setup_event_handlers() override {
		WINDOW_add_handler(WM_SIZE, OnResize);
		WINDOW_add_handler(WM_SIZING, OnResize);
		WINDOW_add_handler(WM_DROPFILES, OnDrop);
	}
};

int main() {
	CmdLineW cl(GetCommandLineW());

	wstring x; cl.getopt(L"x", x);
	if (!x.empty()) {
		// x is gid
		bool isAppArgs = false, isTargetArgs = false, isTargetArgv0 = false;
		wstring ccl, params;
		for (auto& i : cl) {
			if (isAppArgs) {
				if (i == L"--") {
					// 结束appArgs
					isAppArgs = false;
					isTargetArgs = true;
					isTargetArgv0 = true;
					continue;
				}
				ccl += L"\"" + i + L"\" ";
				continue;
			}
			if (isTargetArgs) {
				if (isTargetArgv0) {
					// 跳过 argv[0]
					isTargetArgv0 = false;
					continue;
				}
				params += L"\"" + i + L"\" ";
				continue;
			}
			if (i.starts_with(L"-x")) {
				// 找到 -x 参数
				isAppArgs = true;
				ccl = L"";
				continue;
			}
		}
		try {
			RegistryKey reg(HKEY_LOCAL_MACHINE, L"SOFTWARE\\MyApplicationForwarderConfig", KEY_READ);
			ccl = reg.get(x).get<wstring>();
			if (ccl.empty()) {
				throw w32oop::exceptions::registry_exception("No command line found for gid: " + w32oop::util::str::converts::wstr_str(x));
			}
			if (params.empty() || params == L" ") {
				// 获取默认参数
				params = reg.get(x + L"_default").get<wstring>();
			}
		}
		catch (std::exception& exc) {
			MessageBoxA(NULL, ("Execution Failed: "s + exc.what()).c_str(), NULL, MB_ICONERROR);
			return GetLastError();
		}
		w32oop::util::str::operations::replace(ccl, L"%1", params);
		// 启动
		Process.StartOnly(ccl);
		return 0;
	}

	// GUI
	if (!IsRunAsAdmin()) {
		ShellExecuteW(NULL, L"runas", GetProgramDirW().c_str(), L"", L"", SW_SHOW);
		return 0;
	}

	ApplicationForwarderWindow w;
	w.create();
	w.set_main_window();
	w.show();
	w.center();
	return w.run();
}
