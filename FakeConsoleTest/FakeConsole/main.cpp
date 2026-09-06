#include "targetver.h"
#include "basedef.hpp"
#include "ipc_window.hpp"
#include "console_window.hpp"
using namespace std;

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

namespace app {
	vector<shared_ptr<ui::ConsoleWindow>> windows;
	unique_ptr<ipc::IPCWindow> ipcWindow;
}

int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
) {
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) __fastfail(FAST_FAIL_FATAL_APP_EXIT);
	w32oop::util::RAIIHelper comUninit([] { CoUninitialize(); });

	Window::set_global_option(Window::Option_QuitWhenWindowAllClosed, true);

	app::ipcWindow = unique_ptr<app::ipc::IPCWindow>(new app::ipc::IPCWindow());
	app::ipcWindow->create();

	if (lpCmdLine != L"--hidden"s) {
		STARTUPINFOW si{};
		GetStartupInfoW(&si);
		if (!app::CreateConsoleWindow(NULL, NULL, &si)) {
			thread([](DWORD e) {MessageBoxW(NULL, ErrorChecker(e).message().c_str(), L"Console", MB_ICONERROR);},
				GetLastError()).join();
		}
	}

	if (app::windows.size() == 0) {
		if (IsWindow(*app::ipcWindow)) app::ipcWindow->close();
		return ERROR_NO_DATA;
	}
	return Window::run();
}


bool app::CreateConsoleWindow(LPCWSTR lpApplication, LPCWSTR lpCommand, LPSTARTUPINFOW lpStartupInfo) {
	ipc::CreateConsoleRequestInfo ccri{};
	ccri.x = ccri.y = CW_USEDEFAULT;
	ccri.w = 120;
	ccri.h = 30;
	int fontSize = app::ui::ConsoleWindow::getDefaultFontSize();
	if (lpStartupInfo) {
		if (lpStartupInfo->dwFlags & STARTF_USEPOSITION) {
			ccri.x = lpStartupInfo->dwX;
			ccri.y = lpStartupInfo->dwY;
		}
		if (lpStartupInfo->dwFlags & STARTF_USESIZE) {
			ccri.w = lpStartupInfo->dwXSize / (fontSize / 2);
			ccri.h = lpStartupInfo->dwYSize / fontSize;
		}
	}
	ccri.lpApplication = lpApplication;
	ccri.lpCommand = lpCommand;
	ccri.nCmdShow = (lpStartupInfo && (lpStartupInfo->dwFlags & STARTF_USESHOWWINDOW)) ?
		(lpStartupInfo->wShowWindow) : SW_NORMAL;
	
	auto result = ipcWindow->send(ipc::IPC_RequestCreateConsole, 1885, (LPARAM)&ccri);
	SetLastError((DWORD)result);
	return 0 == result;
}

