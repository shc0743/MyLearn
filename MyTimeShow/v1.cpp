#define UNICODE 1
#include<Windows.h>
#include <string>

#pragma comment(lib, "user32.lib")


#define WC_TIMEWND TEXT("Window:#f50df52e-37b7-402a-91ef-d77bb1d265cc")
#pragma region Global Functions

#define _myrclassf(key, _default) \
	if(content. ## key) wcex. ## key = content. ## key;\
	else wcex. ## key = _default;
#define _myrclassc \
	_myrclassf(style, CS_HREDRAW | CS_VREDRAW);\
	_myrclassf(lpfnWndProc, WndProc);\
	_myrclassf(cbClsExtra, 0);\
	_myrclassf(cbWndExtra, 0);\
	_myrclassf(hInstance, NULL);\
	_myrclassf(hIcon, NULL);\
	_myrclassf(hIconSm, NULL);\
	_myrclassf(lpszMenuName, NULL);\
	_myrclassf(hCursor, LoadCursor(nullptr, IDC_ARROW));\
	_myrclassf(hbrBackground, (HBRUSH)(COLOR_WINDOW + 1));\
	_myrclassf(lpszClassName, className);\

#pragma warning(push)
#pragma warning(disable: 4302)
decltype(RegisterClassW(0)) 
MyRegisterClassW(PCWSTR className, WNDPROC WndProc, WNDCLASSEXW content) {
	WNDCLASSEXW wcex{ 0 };

	wcex.cbSize = sizeof(wcex);

	_myrclassc

	return RegisterClassExW(&wcex);
}
decltype(RegisterClassA(0)) 
MyRegisterClassA(PCSTR className, WNDPROC WndProc, WNDCLASSEXA content) {
	WNDCLASSEXA wcex{ 0 };

	wcex.cbSize = sizeof(wcex);

	_myrclassc

	return RegisterClassExA(&wcex);
}
#pragma warning(pop)
#undef _myrclassc
#undef _myrclassf

void CenterWindow(HWND hWnd) {
	//取得窗口尺寸
	RECT rect; GetWindowRect(hWnd, &rect);
	// 获得窗口大小
	auto w = rect.right - rect.left,
		h = rect.bottom - rect.top;
	//重新设置rect里的值
	rect.left = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
	rect.top  = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;
	//移动窗口到指定的位置
	SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top,
		rect.right, rect.bottom, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
}

std::wstring s2ws(const std::string str) {
	using namespace std;
	wstring result;
	size_t len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), 
		(int)(str.size()), NULL, 0);
	if (len < 0) return result;
	wchar_t* buffer = new wchar_t[len + 1];
	if (buffer == NULL) return result;
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)(str.size()), 
		buffer, (int)len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

errno_t CopyText(const std::wstring &text, UINT uFormat)
{
	if (!::OpenClipboard(NULL))
	{
		return 1;
	}
	if (!::EmptyClipboard())
	{
		if (!::CloseClipboard())
		{
			return 2;
		}
		return 3;
	}
	size_t tSize = (text.length() + 1) * sizeof(wchar_t);
	HGLOBAL hMem = ::GlobalAlloc(GHND, tSize);
	if (!hMem)
	{
		if (!::CloseClipboard())
		{
			return 4;
		}
		return 5;
	}
	wchar_t *strText = (wchar_t *)calloc(tSize, 1);
	// wcscpy_s(strText, tSize, text.c_str());
	if (strText == 0)
		return 15;
	memcpy(strText, text.c_str(), tSize);

	// 把数据拷贝到全局内存中
	// 锁住内存区
	LPWSTR lpStr = (LPWSTR)GlobalLock(hMem);

	wchar_t *pMem = reinterpret_cast<wchar_t *>(lpStr);
	if (!pMem)
	{
		if (!::CloseClipboard())
		{
			return 6;
		}
		if ((!::GlobalUnlock(hMem)) && (GetLastError() != NO_ERROR))
		{
			return 7;
		}
		if (!::GlobalFree(hMem))
		{
			return 8;
		}
		return 9;
	}
	// 内存复制
	::memcpy(lpStr, strText, tSize);
	// 字符结束符: GMEM_ZEROINIT
	if ((!::GlobalUnlock(hMem)) && (GetLastError() != NO_ERROR))
	{
		return 10;
	}
	HANDLE hResult = ::SetClipboardData(uFormat, hMem);
	if (!::CloseClipboard())
	{
		return 11;
	}
	::free(strText);
	if ((!hResult) || hResult == INVALID_HANDLE_VALUE)
	{
		return 14;
	}
	return 0;
}

#pragma endregion



static bool bBlockShutdown = false;
static WCHAR dateformat[256]{};
static WCHAR timeformat[256]{};


LRESULT CALLBACK MyWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
		
	case WM_CREATE: 
	{
		HWND hEdit = CreateWindowExW(WS_EX_NOACTIVATE, L"Static", L"", 
			WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
			0, 0, 1, 1, hwnd, 0, 0, 0);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)hEdit);
	}
		break;


	case WM_KEYDOWN:
	{
		if (
			wParam == VK_ADD || wParam == VK_SUBTRACT ||
			wParam == VK_UP || wParam == VK_DOWN
		) {
			COLORREF ref{};
			BYTE alpha = 255;
			DWORD flags = LWA_ALPHA;
			GetLayeredWindowAttributes(hwnd, &ref, &alpha, &flags);

			BYTE n = 1;
			if (HIWORD(GetKeyState(VK_CONTROL))) {
				n = 10;
			}
			if (wParam == VK_SUBTRACT || wParam == VK_DOWN) n = -n;
			alpha += n;
			alpha = alpha % 256;
			SetLayeredWindowAttributes(hwnd, NULL, alpha, LWA_ALPHA);
			break;
		}

		if (wParam == VK_F1) {
			MessageBoxW(hwnd, 
			L"Keyboard Shortcuts Help Information\n"
			L"\n"
			L"F1\t\tShow this information.\n"
			L"F4\t\tPrevent the window from closing and shutdowning. (to revert, press Ctrl+Alt+Shift+F4)\n"
			L"F6\t\tToggle minimize box and maximize box.\n"
			L"Shift+F10\t\tContextmenu\n"
			L"ArrowUp (or +)\tIncrease the opacity of the window by 1 each time. (Press Ctrl, 10 each time)\n"
			L"ArrowDown (or -)\tReduce the opacity of the window by 1 each time. (Press Ctrl, 10 each time)\n"
			, L"Keyboard Shortcuts Help Information", MB_ICONINFORMATION);
			break;
		}

		if (wParam == VK_F6) {
			static bool _internal_flag = false;
			auto _ = GetWindowLongPtr(hwnd, GWL_STYLE);
			if (_internal_flag) {
				_internal_flag = false;
				_ = _ & (~WS_MINIMIZEBOX) & (~WS_MAXIMIZEBOX);
			} else {
				_internal_flag = true;
				_ = _ | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
			}
			SetWindowLongPtr(hwnd, GWL_STYLE, _);
			break;
		}

		if (wParam == VK_F4) {
			if (bBlockShutdown) {
				if (!(HIWORD(GetKeyState(VK_CONTROL)) && HIWORD(GetKeyState(VK_SHIFT)) && HIWORD(GetKeyState(VK_MENU)))) break;
				bBlockShutdown = false;
				ShutdownBlockReasonDestroy(hwnd);
				HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
				if (!hSysMenu) break;
				EnableMenuItem(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
			} else {
				bBlockShutdown = true;
				ShutdownBlockReasonCreate(hwnd, L"Time is ticking!");
				HMENU hSysMenu = GetSystemMenu(hwnd, FALSE);
				if (!hSysMenu) break;
				EnableMenuItem(hSysMenu, SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
			break;
		}
	}
        return DefWindowProc(hwnd, message, wParam, lParam);
		break;

	case WM_CONTEXTMENU:
	{
		POINT pt{}; RECT rc{};
		GetCursorPos(&pt);
		GetWindowRect(hwnd, &rc);
		if (pt.x < rc.left || pt.x > rc.right || pt.y < (rc.top + 30) || pt.y > rc.bottom) {
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		WCHAR wText[256]{};
		GetWindowTextW(hwnd, wText, 256);
		HMENU hMenu = CreatePopupMenu();

		AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, 0, wText);
		AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
		AppendMenuW(hMenu, MF_STRING, WM_COPY, L"[&c] Copy");
		AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
		AppendMenuW(hMenu, MF_STRING, SC_CLOSE, L"[&x] Close");

		int result = TrackPopupMenu(hMenu, TPM_RIGHTBUTTON | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);
		switch (result)
		{
		case SC_CLOSE:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case WM_COPY:
			CopyText(wText, CF_UNICODETEXT);
			break;

		default:;
		}
	}
		break;


	case WM_SIZING:
	case WM_SIZE:
	{
		HWND hEdit = (HWND)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		RECT rc{};
		GetClientRect(hwnd, &rc);
		SetWindowPos(hEdit, 0, 0, 0, rc.right-rc.left, rc.bottom-rc.top, 0);
	}
        return DefWindowProc(hwnd, message, wParam, lParam);
		break;


	case WM_NCHITTEST:
	{
		LRESULT lResult = DefWindowProc(hwnd, message, wParam, lParam);
		if (lResult == HTCLIENT) lResult = HTCAPTION;
		return lResult;
	}
		break;

	case WM_CLOSE:
		if (bBlockShutdown) break;
		DestroyWindow(hwnd);
		break;

	case WM_QUERYENDSESSION:
	case WM_ENDSESSION:
		if (bBlockShutdown) return FALSE;
        return DefWindowProc(hwnd, message, wParam, lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}


DWORD __stdcall WorkerThread(PVOID fWnd) {
	using std::wstring;
	using std::to_wstring;
	HWND hwnd = (HWND)fWnd;
	HWND hwEdit = (HWND)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	SYSTEMTIME st{};
	WCHAR buffer[512]{};
	WCHAR buffer2[512]{};
	wstring result;
	while (IsWindow(hwnd)) {
		GetLocalTime(&st);
		PCWSTR pDateFormat = NULL, pTimeFormat = NULL;
		if (dateformat[0]) pDateFormat = dateformat;
		if (timeformat[0]) pTimeFormat = timeformat;
		GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pTimeFormat, buffer, 512);
		GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, pDateFormat, buffer2, 512, 0);
		result = buffer2;
		result += L" ";
		result += buffer;
		SendMessageW(hwnd, WM_SETTEXT, 0, (WPARAM)result.c_str());
		SendMessageW(hwEdit, WM_SETTEXT, 0, (WPARAM)result.c_str());

		SetWindowPos(hwnd, HWND_TOPMOST, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

		Sleep(150);
	}
	return 0;
}


#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


int main(int argc, char* argv[]) {


	if (HWND hw = FindWindow(WC_TIMEWND, NULL)) {
		if (MessageBoxW(hw, L"There are already running instances of this program.\n"
			"Are you sure you want to open a new instance?", L"Confirm", 
			MB_OKCANCEL | MB_ICONINFORMATION | MB_DEFBUTTON2) != IDOK)
				return ERROR_ALREADY_EXISTS;
	}

	if (!MyRegisterClassW(WC_TIMEWND, MyWndProc, WNDCLASSEXW{
		
	})) {
		::MessageBoxW(NULL, (L"Error during RegisterClassW " + std::to_wstring(GetLastError())).c_str(), NULL, MB_ICONERROR);
		return GetLastError();
	}

	HWND hwnd = CreateWindowExW(
		WS_EX_APPWINDOW | WS_EX_LAYERED,
		WC_TIMEWND, L"Time",
		WS_OVERLAPPED | WS_SIZEBOX | WS_SYSMENU | WS_CAPTION,
		0, 0, 200, 100, NULL, NULL, NULL, NULL);
	if (!hwnd) {
		::MessageBoxW(NULL, (L"Error during CreateWindowExW " + std::to_wstring(GetLastError())).c_str(), NULL, MB_ICONERROR);
		return GetLastError();
	}
	SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
	CenterWindow(hwnd);
	ShowWindow(hwnd, SW_NORMAL);
	SetWindowPos(hwnd, HWND_TOPMOST, 1, 1, 1, 1, SWP_NOMOVE | SWP_NOSIZE);

	if (argc >= 3) {
		std::wstring d = s2ws(argv[1]), t = s2ws(argv[2]);
		wcscpy_s(dateformat, d.c_str());
		wcscpy_s(timeformat, t.c_str());
	}

	{
		HANDLE hThread = CreateThread(0, 0, WorkerThread, hwnd, 0, 0);
		if (hThread) CloseHandle(hThread);
	}


    MSG msg{};
    while (GetMessage(&msg, 0, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	Sleep(1024);
    return (int)msg.wParam;
}


