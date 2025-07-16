#include <w32use.hpp>
#include <ktmw32.h>
#include <format>
#include <map>
using namespace std;

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "ktmw32.lib")
#pragma comment(lib, "comctl32.lib")

class FileLockerPlusMainWnd : public Window {
public:
	FileLockerPlusMainWnd() : Window(L"File Locker Plus", 640, 480, 0, 0, WS_OVERLAPPEDWINDOW) {}
private:
	Static text;
	Button btn, btnUnlock;
	Edit textBox;
	map<wstring, w32FileHandle> files;
	void onCreated() override {
		DragAcceptFiles(hwnd, TRUE);

		text.set_parent(this);
		text.create(L"File Locker Plus: Lock your files safely", 600, 20, 10, 10);

		textBox.set_parent(this);
		textBox.create(L"", 600, 20, 10, 40);

		btn.set_parent(this);
		btn.create(L"Lock File", 300, 30, 10, 70);

		btnUnlock.set_parent(this);
		btnUnlock.create(L"Unlock All Files", 300, 30, 320, 70);

		btn.onClick([this](EventData&) {
			doLock(textBox.text());
			textBox.text(L""); // 清空文本框
			MessageBoxW(hwnd, L"File locked successfully.", L"Info", MB_OK | MB_ICONINFORMATION);
		});

		btnUnlock.onClick([this](EventData&) {
			for (const auto& file : files) {
				const w32FileHandle& hTransaction = file.second;
				if (hTransaction) {
					// 回滚事务(我们是锁定文件，不是真的要删除它们)
					RollbackTransaction(hTransaction);
					// 句柄会在析构的时候自动关闭
				}
			}
			files.clear();// 在这里会自动关闭所有的句柄
			textBox.text(L""); // 清空文本框
			MessageBoxW(hwnd, L"Files unlocked.", L"Info", MB_OK | MB_ICONINFORMATION);
		});
	}
	void onDestroy() override {

	}

	void doLock(wstring file) {
		HANDLE hTransaction = CreateTransaction(NULL, 0, 0, 0, 0, 0, NULL);
		if (DeleteFileTransactedW(file.c_str(), hTransaction)) {
			files.emplace(file, hTransaction);
		}
		else {
			CloseHandle(hTransaction); // 自动回滚
			MessageBoxW(hwnd, format(L"Failed to lock file {} because {}", file, GetLastError()).c_str(), L"Error", MB_OK | MB_ICONERROR);
		}
	}

	void onPaint(EventData& ev) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		// 在(10, 100)位置绘制文本
		const wchar_t* text = L"Files locked:";
		TextOut(hdc, 10, 100, text, (int)wcslen(text));

		EndPaint(hwnd, &ps);
	}
	void onDropFile(EventData& ev) {
		HDROP hDrop = (HDROP)ev.wParam;
		UINT fileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);

		wstring allFiles;
		for (UINT i = 0; i < fileCount; i++) {
			wchar_t filePath[MAX_PATH];
			DragQueryFile(hDrop, i, filePath, MAX_PATH);

			// 添加到文件列表
			doLock(filePath);
		}

		// 重绘窗口显示新文件数
		InvalidateRect(hwnd, NULL, TRUE);

		DragFinish(hDrop);

		MessageBoxW(hwnd, L"Files locked successfully.", L"Info", MB_OK | MB_ICONINFORMATION);
	}
	void whenUserWantsToCloseTheWindow(EventData& ev) {
		if (!files.empty()) {
			int u = 0;
			TaskDialog(hwnd, NULL, L"File Locker Plus", L"There are locked files.",
				L"If you close the application, they will be unlocked.\n\nContinue?",
				TDCBF_CANCEL_BUTTON | TDCBF_YES_BUTTON, 
				TD_INFORMATION_ICON, &u);
			if (u != IDYES) ev.preventDefault();
		}
	}
protected:
	virtual void setup_event_handlers() override {
		WINDOW_add_handler(WM_DROPFILES, onDropFile);
		WINDOW_add_handler(WM_PAINT, onPaint);
		WINDOW_add_handler(WM_CLOSE, whenUserWantsToCloseTheWindow);
	}
};

int __stdcall wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	FileLockerPlusMainWnd window;
	window.create();
	window.set_main_window();
	window.center();
	window.show();
	return window.run();
}