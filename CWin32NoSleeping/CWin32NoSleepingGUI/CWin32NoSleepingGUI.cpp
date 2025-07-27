#include <w32use.hpp>
using namespace std;

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


class CWin32NoSleepingGUIWindow : public Window {
public:
	CWin32NoSleepingGUIWindow() : Window(L"CWin32NoSleeping Application", 320, 200, 0, 0, WS_OVERLAPPEDWINDOW) {};

protected:
	Static canSleep;
	CheckBox cSystem, cDisplay;
	HFONT hFontBig = NULL;
	thread myThread;
	bool endThread = false;

	void onCreated() override {
		canSleep = Static(hwnd, L"Can sleep: NO", 1, 1, 0, 0, Static::STYLE | SS_CENTER);
		canSleep.create();
		hFontBig = CreateFontW(
			40, 20, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas"
		);
		canSleep.font(hFontBig);

		cSystem = CheckBox(hwnd, L"System", 100, 30, 10, 50);
		cSystem.create();
		cSystem.onChanged([this](EventData& ev) {
			ev.preventDefault();
		});
		cSystem.show();

		cDisplay = CheckBox(hwnd, L"Display", 100, 30, 120, 50);
		cDisplay.create();
		cDisplay.onChanged([this](EventData& ev) {
			ev.preventDefault();
		});
		cDisplay.show();

		cDisplay.check(); // 默认选中Display

		myThread = thread([this]() {
			while (1) {
				Sleep(1000);
				if (endThread) return;
				if (cSystem.checked() || cDisplay.checked()) {
					canSleep.text(L"Can sleep: NO");
				} else {
					canSleep.text(L"Can sleep: YES");
				}
				DWORD state = 0;
				if (cSystem.checked()) {
					state |= ES_SYSTEM_REQUIRED;
				}
				if (cDisplay.checked()) {
					state |= ES_DISPLAY_REQUIRED;
				}
				SetThreadExecutionState(state);
			}
		});
	}
	void onDestroy() override {
		DeleteFont(hFontBig);
		endThread = true;
		if (myThread.joinable()) {
			myThread.join();
		}
	}

	virtual void setup_event_handlers() {
		WINDOW_add_handler(WM_SIZING, onResize);
		WINDOW_add_handler(WM_SIZE, onResize);
		WINDOW_add_handler(WM_CLOSE, onBeforeClose);
	}
private:
	void onResize(EventData& ev) {
		ev.preventDefault();
		RECT rc{}; GetClientRect(hwnd, &rc);
		canSleep.resize(10, 10, rc.right - rc.left - 20, 40);
		cSystem.resize(10, 60, rc.right - rc.left - 20, 20);
		cDisplay.resize(10, 90, rc.right - rc.left - 20, 20);
	}
	void onBeforeClose(EventData& ev) {
		if (!(cSystem.checked() || cDisplay.checked())) {
			return; // 如果没有选中，则允许关闭
		}
		if (MessageBoxW(hwnd, L"Are you sure you want to close the application?", L"Confirm Close", MB_YESNO | MB_ICONQUESTION) != IDYES) {
			ev.preventDefault();
		}
	}
};

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	try {
		Window::set_global_option(Window::Option_DebugMode, 1);
		CWin32NoSleepingGUIWindow app;
		app.create();
		app.set_main_window();
		app.center();
		app.show();
		return app.run();
	}
	catch (exception& exc) {
		MessageBoxW(NULL, (w32oop::util::str::converts::str_wstr(exc.what()) + L"\n"s + ErrorChecker().message()).c_str(), L"Fatal Error", MB_ICONERROR);
		return -1;
	}
}