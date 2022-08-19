#include "winui.h"
#include "resource.h"
#include "MyRemoteTool-ui.h"
#include "globaldef.h"
//#include "sciter-include.h"
#include "client.h"
using namespace std;



static WCHAR szRClass[256]{ 0 };
static WCHAR szMClass[256]{ 0 };
static WCHAR szSClass[256]{ 0 };

static HFONT hFontDefault;

static DWORD mInitWndClasses(HINSTANCE i);
static LRESULT CALLBACK WndProc_RoleChoose(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK WndProc_SrvStap(HWND, UINT, WPARAM, LPARAM);
static HWND cCreateSrvStapWnd();

bool mmInitWinSock2();
bool mmCleanupWinSock2();

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


DWORD mInitWndClasses(HINSTANCE i) {
	if (!LoadStringW(i, IDS_STRING_UI_RCH_CLASS, szRClass, 255)) {
		return ERROR_INVALID_DATA;
	}
	s7::MyRegisterClassW(szRClass, WndProc_RoleChoose);

	if (!LoadStringW(i, IDS_STRING_UI_CLT_CLASS, szMClass, 255)) {
		return ERROR_INVALID_DATA;
	}
	WNDCLASSEXW wcex_mainui{};
	wcex_mainui.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU_CLTMAIN);
	s7::MyRegisterClassW(szMClass, WndProc_MainUI, wcex_mainui);

	if (!LoadStringW(i, IDS_STRING_UI_SRV_CLASS, szSClass, 255)) {
		return ERROR_INVALID_DATA;
	}
	s7::MyRegisterClassW(szSClass, WndProc_SrvStap);

	return 0;
}

int __stdcall mWinDispatchUIMain(const CmdLineW& args) {

	INITCOMMONCONTROLSEX icce{};
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_WIN95_CLASSES | ICC_INTERNET_CLASSES | ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&icce);

	DWORD dwClassErr = mInitWndClasses(hInst);
	if (dwClassErr) return (int)dwClassErr;

	bool bSrvSvc = false;
	WCHAR szSvcQn[256]{};
	if (LoadStringW(hInst, IDS_STRING_SRV_Q_PIPE_NAME, szSvcQn, 256)) {
		//MessageBoxW(0, szSvcQn, 0, 0);
		HANDLE hFile = CreateFileW(szSvcQn, GENERIC_READ, 
			FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (hFile && hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
			// server service detected
			bSrvSvc = true;
		}
	}

	hFontDefault = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));

	if (!mmInitWinSock2()) {
		MessageBoxW(0, L"Winsock Error", 0, MB_ICONHAND);
	}

	HWND hRoleC = NULL;
	if (bSrvSvc) {
		if (!HIWORD(GetKeyState(VK_SHIFT))) {
			if (hFontDefault) DeleteObject(hFontDefault);
			return ERROR_ACCESS_DENIED;
		}
		hRoleC = cCreateSrvStapWnd();
	} else {
		hRoleC = CreateWindowExW(0, szRClass, L" - My Remote Tool",
			WS_OVERLAPPEDWINDOW, 0, 0, 500, 300, 0, 0, 0, 0);
	}
	if (!hRoleC) {
		return GetLastError();
	}
	CenterWindow(hRoleC);
	ShowWindow(hRoleC, SW_NORMAL);

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (hFontDefault) DeleteObject(hFontDefault);
	mmCleanupWinSock2();
	return (int)msg.wParam;
	return 0;
}


#if 0
// Example
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
#endif

static bool bRoleChoosed = false;
typedef struct {
	HFONT f2;
} w_RoleChoose_t;
std::map<HWND, w_RoleChoose_t> w_RoleChoose_d;
LRESULT CALLBACK WndProc_RoleChoose(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		HWND a, b, c;
		a = CreateWindowExW(0, L"Button", L"[S]erver (Target Machine)",
			WS_CHILD | WS_VISIBLE | BS_CENTER, 10, 10, 460, 80, hwnd, (HMENU)3, 0, 0);
		b = CreateWindowExW(0, L"Button", L"[C]lient (Main Machine)",
			WS_CHILD | WS_VISIBLE | BS_CENTER, 10, 100, 460, 80, hwnd, (HMENU)4, 0, 0);
		c = CreateWindowExW(0, L"Button", L"Cancel",
			WS_CHILD | WS_VISIBLE | BS_CENTER, 390, 210, 80, 40, hwnd, (HMENU)2, 0, 0);
		HFONT fntb = CreateFont(-32, -16, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
			_T("Consolas"));
		SendMessage(c, WM_SETFONT, (WPARAM)hFontDefault, 0);
		if (fntb) {
			SendMessage(a, WM_SETFONT, (WPARAM)fntb, 0);
			SendMessage(b, WM_SETFONT, (WPARAM)fntb, 0);
		}
		w_RoleChoose_t r{};
		r.f2 = fntb;
		w_RoleChoose_d.insert(std::make_pair(hwnd, r));
		break;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 2:
			DestroyWindow(hwnd);
			break;

		case 3:
		case 4:
			ShowWindow(hwnd, SW_HIDE);
			bRoleChoosed = true;
			{
				HWND hw = NULL;
				if (LOWORD(wParam) == 3) {
					hw = cCreateSrvStapWnd();
				}
				else {
					hw = CreateWindowExW(0, szMClass,
						L" - My Remote Tool",
						WS_OVERLAPPEDWINDOW,
						0, 0, 1024, 768, 0, 0, 0, 0);
				}
				if (!hw) {
					return -1;
				}
				CenterWindow(hw);
				ShowWindow(hw, SW_NORMAL);
			}
			DestroyWindow(hwnd);
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;

	case WM_DESTROY:
		try {
			auto& d = w_RoleChoose_d.at(hwnd);
			DeleteObject(d.f2);
		}
		catch (...) {}
		if (!bRoleChoosed) PostQuitMessage(ERROR_CANCELLED);
		else return DefWindowProc(hwnd, msg, wParam, lParam);
		break;
		
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


typedef struct {
	HWND srvip, srvport, connpswd;
} SrvStapData;
static std::map<HWND, SrvStapData*> SrvStapDatas;
LRESULT CALLBACK WndProc_SrvStap(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE: {
		SrvStapData* data = new SrvStapData;
		////auto a = GetKeyState(VK_SHIFT);
		////MessageBoxW(0, std::to_wstring(a).c_str(), 0, 0);
		//if (!HIWORD(GetKeyState(VK_SHIFT))) {
		//	DestroyWindow(hwnd);
		//	PostQuitMessage(ERROR_ACCESS_DENIED);
		//}
		SendMessage(CreateWindowExW(0, L"Static", L"Server configuration",
			WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 10, 470, 20, hwnd, 0, 0, 0),
			WM_SETFONT, (WPARAM)hFontDefault, 0);

		CreateWindowExW(0, L"Button", L"x", WS_CHILD | WS_VISIBLE,
			470, 10, 20, 20, hwnd, (HMENU)IDCANCEL, 0, 0);

		SendMessage(CreateWindowExW(0, L"Static", L"Server IP Address",
			WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 45, 170, 20, hwnd, 0, 0, 0),
			WM_SETFONT, (WPARAM)hFontDefault, 0);
		data->srvip = CreateWindowExW(0, WC_IPADDRESSW, L"",
			WS_CHILD | WS_VISIBLE, 190, 40, 300, 30, hwnd, 0, 0, 0);

		SendMessage(CreateWindowExW(0, L"Static", L"Server Port",
			WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 85, 170, 20, hwnd, 0, 0, 0),
			WM_SETFONT, (WPARAM)hFontDefault, 0);
		data->srvport = CreateWindowExW(0, L"Edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
			190, 80, 300, 30, hwnd, 0, 0, 0);

		SendMessage(CreateWindowExW(0, L"Static", L"Connection Password",
			WS_CHILD | WS_VISIBLE | SS_CENTER, 10, 125, 170, 20, hwnd, 0, 0, 0),
			WM_SETFONT, (WPARAM)hFontDefault, 0);
		data->connpswd = CreateWindowExW(0, L"Edit", L"",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			190, 120, 300, 30, hwnd, 0, 0, 0);

		SendMessage(CreateWindowExW(0, L"Button", L"Open Once",
			WS_CHILD | WS_VISIBLE, 10, 160, 480, 55, hwnd, (HMENU)5, 0, 0),
			WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(CreateWindowExW(0, L"Button", L"Install",
			WS_CHILD | WS_VISIBLE, 10, 225, 480, 55, hwnd, (HMENU)6, 0, 0),
			WM_SETFONT, (WPARAM)hFontDefault, 0);

		SendMessage(data->srvip, WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(data->srvport, WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(data->connpswd, WM_SETFONT, (WPARAM)hFontDefault, 0);

		SrvStapDatas.insert(std::make_pair(hwnd, data));
	}
		break;

	case WM_COMMAND:
		if (lParam)
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			DestroyWindow(hwnd);
			break;

		case 5:
		case 6:
		{
			wstring ip, port, pswd;
			WCHAR buffer[2048]{};
			auto& d = *SrvStapDatas[hwnd];
			GetWindowTextW(d.srvip, buffer, 2048);
			ip = buffer;
			GetWindowTextW(d.srvport, buffer, 2048);
			port = buffer;
			GetWindowTextW(d.connpswd, buffer, 2048);
			pswd = buffer;
			if (LOWORD(wParam) == 5) {
				wstring cl = L"\"" + s2ws(GetProgramDir()) +
					L"\" --open --srv --once -i{" +
					ip + L":" + port + L"} -p\"" + pswd + L"\"";
				Process.StartOnly(cl);
			}
			else {
				// TODO

			}
			DestroyWindow(hwnd);
			PostQuitMessage(0);
		}
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		else return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_NCHITTEST:
		return HTCAPTION;
		break;

	case WM_DESTROY:
		PostQuitMessage(ERROR_CANCELLED);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

HWND cCreateSrvStapWnd() {
	return CreateWindowExW(0, szSClass,
		L"Server Configuration - My Remote Tool",
		WS_POPUP | WS_DLGFRAME | WS_SYSMENU,
		0, 0, 500, 300, 0, 0, 0, 0);
}



