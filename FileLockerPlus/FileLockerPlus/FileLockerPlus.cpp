#include <w32use.hpp>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class FileLockerPlusMainWnd : public Window {
public:
	FileLockerPlusMainWnd() : Window(L"File Locker Plus", 640, 480, 0, 0, WS_OVERLAPPEDWINDOW) {}
private:
	Static text;
	Button btn;
	Edit textBox;
	void onCreated() override {
		register_hot_key(true, false, false, 'A', [this](HotKeyProcData& ev) {
			MessageBoxW(hwnd, L"Hot key pressed", L"File Locker Plus", MB_OK);
		});
	}
	void onDestroy() override {

	}
protected:
	virtual void setup_event_handlers() override {

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