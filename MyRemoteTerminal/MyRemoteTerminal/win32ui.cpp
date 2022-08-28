#include "win32ui.h"
#include "str_table.h"
using namespace std;


extern HINSTANCE hInst;

static WCHAR szSetupClass[256]{ 0 };

static HFONT hFontDefault;

static DWORD mInitWndClasses(HINSTANCE i);
static LRESULT CALLBACK WndProc_SetupWnd(HWND, UINT, WPARAM, LPARAM);

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


DWORD mInitWndClasses(HINSTANCE i) {
	WNDCLASSEXW wcex_setup{};
	wcex_setup.hbrBackground = CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));
	if (!LoadStringW(i, IDS_STRING_UI_SETUPWINDOW_CLASS, szSetupClass, 255)) {
		return ERROR_INVALID_DATA;
	}
	s7::MyRegisterClassW(szSetupClass, WndProc_SetupWnd, wcex_setup);

	//if (!LoadStringW(i, IDS_STRING_UI_CLT_CLASS, szMClass, 255)) {
	//	return ERROR_INVALID_DATA;
	//}
	//WNDCLASSEXW wcex_mainui{};
	//wcex_mainui.lpszMenuName = MAKEINTRESOURCEW(IDR_MENU_CLTMAIN);
	//s7::MyRegisterClassW(szMClass, WndProc_MainUI, wcex_mainui);

	//if (!LoadStringW(i, IDS_STRING_UI_SRV_CLASS, szSClass, 255)) {
	//	return ERROR_INVALID_DATA;
	//}
	//s7::MyRegisterClassW(szSClass, WndProc_SrvStap);

	return 0;
}

int __stdcall mWinDispatchUIMain(const CmdLineW& args) {

	INITCOMMONCONTROLSEX icce{};
	icce.dwSize = sizeof(icce);
	icce.dwICC = ICC_WIN95_CLASSES | ICC_INTERNET_CLASSES | ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&icce);

	DWORD dwClassErr = mInitWndClasses(hInst);
	if (dwClassErr) return (int)dwClassErr;

	hFontDefault = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));

	WCHAR lpszUuid[64], lpszCaption[256]{};
	LoadStringW(hInst, IDS_STRING_APP_UUID, lpszUuid, 64);
	LoadStringW(hInst, IDS_STRING_UI_SETUPWINDOW_TITLE, lpszCaption, 256);
	wstring szCaption = lpszCaption;
	str_replace(szCaption, L"%uuid%", lpszUuid);
	
	HWND hwnd = CreateWindowExW(0, szSetupClass, szCaption.c_str(),
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
		0, 0, 640, 480, 0, 0, 0, 0);
	if (!hwnd) {
		MessageBoxW(NULL, (L"Cannot create window because " + LastErrorStrW() +
			L" (" + to_wstring(GetLastError()) + L").").c_str(), 0, MB_ICONHAND);
		return GetLastError();
	}
	CenterWindow(hwnd);
	ShowWindow(hwnd, SW_NORMAL);

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (hFontDefault) DeleteObject(hFontDefault);
	return (int)msg.wParam;
	return 0;
}


#if 0
// Example
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}
#endif

#define W32UI_CTLS_ROW(n) ((void)(n))


typedef struct {
	HWND
		svcname,
		svcstartup,
		svcinstd,
		srvip,
		srvport;
} WndData_SetupWnd;
static LRESULT CALLBACK WndProc_SetupWnd(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
	{
		constexpr auto defws = WS_CHILD | WS_VISIBLE;

		WCHAR wCapt[256]{};
		GetWindowTextW(hwnd, wCapt, 256);
		WCHAR wSnam[256]{};
		LoadStringW(hInst, IDS_STRING_SVC_DEFAULT_NAME, wSnam, 256);
		WCHAR wDfpt[256]{};
		LoadStringW(hInst, IDS_STRING_NETWORK_DEFAULTPORT, wDfpt, 256);

		WndData_SetupWnd* dat = new WndData_SetupWnd;

		W32UI_CTLS_ROW(0);
		SendMessage(CreateWindowExW(0,
			L"Static", wCapt, defws | SS_CENTER, 10, 10, 600, 20,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);

		SendMessage(CreateWindowExW(0,
			L"Static", L"", defws | WS_BORDER, 10, 40, 600, 1,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);

		W32UI_CTLS_ROW(1);
		SendMessage(CreateWindowExW(0,
			L"Static", L"Service Name: ", defws, 10, 55, 100, 25,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		dat->svcname = CreateWindowExW(0,
			L"Edit", wSnam, defws | WS_BORDER, 120, 52, 490, 25,
			hwnd, 0, 0, 0);
		SendMessage(dat->svcname, WM_SETFONT, (WPARAM)hFontDefault, 0);

		W32UI_CTLS_ROW(2);
		SendMessage(CreateWindowExW(0,
			L"Static", L"Service Start Type:", defws, 10, 85+2, 150, 25,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		dat->svcstartup = CreateWindowExW(0,
			WC_COMBOBOXW, L"", defws | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
			170, 85, 440, 25, hwnd, 0, 0, 0);
		SendMessage(dat->svcstartup, WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(dat->svcstartup, CB_ADDSTRING, 0, (LPARAM)L"Auto");
		SendMessage(dat->svcstartup, CB_ADDSTRING, 0, (LPARAM)L"Demand");
		SendMessage(dat->svcstartup, CB_ADDSTRING, 0, (LPARAM)L"Disabled");
		SendMessage(dat->svcstartup, CB_SETCURSEL, 1, 0);

		W32UI_CTLS_ROW(3);
		SendMessage(CreateWindowExW(0,
			L"Static", L"Install Path: ", defws, 10, 120, 100, 25,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		dat->svcinstd = CreateWindowExW(0, L"Edit", L"(Default)",
			defws | WS_BORDER, 120, 120, 490, 25, hwnd, 0, 0, 0);
		SendMessage(dat->svcinstd, WM_SETFONT, (WPARAM)hFontDefault, 0);

		SendMessage(CreateWindowExW(0,
			L"Static", L"", defws | WS_BORDER, 10, 155, 600, 1,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);

		W32UI_CTLS_ROW(3);
		SendMessage(CreateWindowExW(0,
			L"Static", L"Server IP: ", defws, 10, 165, 100, 25,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		dat->srvip = CreateWindowExW(0,
			L"Edit", L"Default (If you are installing this is unused)",
			defws | WS_BORDER, 120, 165, 490, 25,
			hwnd, 0, 0, 0);
		SendMessage(dat->srvip, WM_SETFONT, (WPARAM)hFontDefault, 0);

		W32UI_CTLS_ROW(4);
		SendMessage(CreateWindowExW(0,
			L"Static", L"Server Port: ", defws, 10, 200, 100, 25,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		dat->srvport = CreateWindowExW(0,
			L"Edit", wDfpt, defws | WS_BORDER | ES_NUMBER, 120, 200, 490, 25,
			hwnd, 0, 0, 0);
		SendMessage(dat->srvport, WM_SETFONT, (WPARAM)hFontDefault, 0);

		SendMessage(CreateWindowExW(0,
			L"Static", L"", defws | WS_BORDER, 10, 235, 600, 1,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);

		W32UI_CTLS_ROW(5);
		SendMessage(CreateWindowExW(0,
			L"Static", L"Already have a server?", defws | SS_CENTERIMAGE
			, 10, 245, 160, 30,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(CreateWindowExW(0,
			L"Button", L"&Activate", defws | BS_VCENTER, 180, 245, 120, 30,
			hwnd, (HMENU)IDNO, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(CreateWindowExW(0, L"Static", L"or",
			defws | SS_CENTERIMAGE , 310, 245, 20, 30, hwnd, 0, 0, 0),
			WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(CreateWindowExW(0,
			L"Button", L"Con&figure", defws | BS_VCENTER, 340, 245, 270, 30,
			hwnd, (HMENU)IDRETRY, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);

		W32UI_CTLS_ROW(-1);
		SendMessage(CreateWindowExW(0,
			L"Static", L"", defws | WS_BORDER, 10, 390, 600, 1,
			hwnd, 0, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(CreateWindowExW(0,
			L"Button", L"&Uninstall", defws, 330, 400, 100, 30,
			hwnd, (HMENU)IDABORT, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(CreateWindowExW(0,
			L"Button", L"&Install", defws, 440, 400, 80, 30,
			hwnd, (HMENU)IDYES, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);
		SendMessage(CreateWindowExW(0,
			L"Button", L"&Close", defws, 530, 400, 80, 30,
			hwnd, (HMENU)IDCANCEL, 0, 0), WM_SETFONT, (WPARAM)hFontDefault, 0);


		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)dat);

	}
		break;

	case WM_KEYDOWN:
		if (wParam == VK_TAB) {
			HWND h = GetFocus();
			if (!h) h = hwnd;
			UINT f = HIWORD(GetKeyState(VK_SHIFT)) ? GW_HWNDPREV : GW_HWNDNEXT;
			HWND n = GetNextWindow(h, f);
			if (GetParent(n) != hwnd) n = GetNextWindow(hwnd, f);
			SetFocus(n);
			return true;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDCANCEL:
			SendMessageW(hwnd, WM_CLOSE, 0, 0);
			break;

		case IDRETRY:
		{
			WndData_SetupWnd* dat = (WndData_SetupWnd*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!dat) break;

			WCHAR buffer[256]{};
			wstring url;
			url = L"http://";
			GetWindowTextW(dat->srvip, buffer, 256);
			url += buffer;
			url += L":";
			GetWindowTextW(dat->srvport, buffer, 256);
			url += buffer;
			url += L"/config";

			int btn = 0;
			TaskDialog(hwnd, NULL, L"Server Configuration",
				L"Do you want to open the following URL?",
				url.c_str(),
				TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON,
				NULL, &btn);
			if (btn == IDYES) {
				ShellExecuteW(hwnd, L"open", url.c_str(), NULL, NULL, SW_NORMAL);
			}
		}
			break;

		case IDNO:
		{
			WndData_SetupWnd* dat = (WndData_SetupWnd*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!dat) break;

			WCHAR buffer[256]{};
			GetWindowTextW(dat->svcname, buffer, 256);

			if (!IsRunAsAdmin()) {
				int btn = 0;
				TaskDialog(hwnd, NULL, L"Activate - My Remote Terminal",
					L"Administrator privileges are required to continue.",
					L"Do you want to elevate?",
					TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
					TD_SHIELD_ICON, &btn);
				if (btn != IDOK) break;
			}

			ShellExecuteW(NULL, (IsRunAsAdmin() ? L"open" : L"runas"),
				s2wc(GetProgramDir()), (L"--type=service-activator --name=\""s
					+ buffer + L"\"").c_str(), NULL, SW_NORMAL);
		}
			break;

		case IDYES:
		{
			WndData_SetupWnd* dat = (WndData_SetupWnd*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!dat) break;

			WCHAR buffer[1024]{};
			wstring buffer2;
			wstring cmdl;// = L"\""+s2ws(GetProgramDir())+L"\" ";
			cmdl += L"--service --install --name=\"";
			GetWindowTextW(dat->svcname, buffer, 1024);
			cmdl += buffer;
			cmdl += L"\" --port=\"";
			GetWindowTextW(dat->srvport, buffer, 1024);
			cmdl += buffer;
			cmdl += L"\" --INSTALLDIR=\"";
			GetWindowTextW(dat->svcinstd, buffer, 1024);
			if (wstring(buffer) != L"(Default)") cmdl += buffer;
			cmdl += L"\" --startup=\"";
			static std::map<LRESULT, std::wstring> StartupTypeMapping;
			if (StartupTypeMapping.empty()) {
				StartupTypeMapping[0] = L"auto";
				StartupTypeMapping[1] = L"demand";
				StartupTypeMapping[2] = L"disabled";
			}
			try {
				buffer2 = StartupTypeMapping.at(
					SendMessage(dat->svcstartup, CB_GETCURSEL, 0, 0)
				);
			}
			catch (...) {
				buffer2 = L"demand";
			}
			cmdl += buffer2;
			cmdl += L"\"  --interface --hwndOwner=";
			cmdl += to_wstring((LONG_PTR)hwnd);
			cmdl += L" --create-config --start ";

			if (!IsRunAsAdmin()) {
				int btn = 0;
				TaskDialog(hwnd, NULL, L"Install - My Remote Terminal",
					L"Administrator privileges are required to continue.",
					L"Do you want to elevate?",
					TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
					TD_SHIELD_ICON, &btn);
				if (btn != IDOK) break;
			}

			ShellExecuteW(NULL, (IsRunAsAdmin() ? L"open" : L"runas"),
				s2wc(GetProgramDir()), cmdl.c_str(), NULL, SW_NORMAL);
			

		}
			break;

		case IDABORT:
		{
			WndData_SetupWnd* dat = (WndData_SetupWnd*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (!dat) break;

			wstring cl = L"--service --uninstall --name=\"";
			WCHAR buffer[1024]{};
			GetWindowTextW(dat->svcname, buffer, 1024);
			cl += buffer;
			cl += L"\"";
			
			int btn = 0;
			TaskDialog(hwnd, NULL, L"Uninstall - My Remote Terminal",
				L"Are you sure you want to delete the following service?",
				buffer, TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON,
				NULL, &btn);
			if (btn != IDYES) break;

			if (!IsRunAsAdmin()) {
				TaskDialog(hwnd, NULL, L"Uninstall - My Remote Terminal",
					L"Administrator privileges are required to continue.",
					L"Do you want to elevate?",
					TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
					TD_SHIELD_ICON, &btn);
				if (btn != IDOK) break;
			}

			SHELLEXECUTEINFO execinfo;
			memset(&execinfo, 0, sizeof(execinfo));
			std::wstring pd = s2ws(GetProgramDir());
			execinfo.lpFile			= pd.c_str();
			execinfo.cbSize			= sizeof(execinfo);
			execinfo.lpVerb			= (IsRunAsAdmin() ? L"open" : L"runas");
			execinfo.fMask			= SEE_MASK_NOCLOSEPROCESS;
			execinfo.nShow			= SW_NORMAL;
			execinfo.lpParameters	= cl.c_str();
 
			if (!ShellExecuteExW(&execinfo)) {
				MessageBoxW(hwnd, LastErrorStrW().c_str(), 0, MB_ICONHAND);
				break;
			}
 
			WaitForSingleObject(execinfo.hProcess, INFINITE);
			DWORD exitCode = 0;
			GetExitCodeProcess(execinfo.hProcess, &exitCode);
			CloseHandle(execinfo.hProcess);

			if (exitCode == ERROR_RESTART_APPLICATION) {
				MessageBoxTimeoutW(hwnd, ErrorCodeToStringW(ERROR_RESTART_APPLICATION)
					.c_str(), L"Information", MB_ICONHAND, 0, 2000);
				ExitProcess(ERROR_RESTART_APPLICATION);
			}

			TaskDialog(hwnd, NULL, L"My Remote Terminal",
				ErrorCodeToStringW(exitCode).c_str(),
				(L"Process returned code " + to_wstring(exitCode)).c_str(),
				TDCBF_CLOSE_BUTTON,
				((exitCode == 0) ? TD_INFORMATION_ICON : TD_ERROR_ICON), NULL);

		}
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;

	case WM_DESTROY:
	{
		WndData_SetupWnd* dat = (WndData_SetupWnd*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (dat) try {
			delete dat;
		}
		catch (...) {};
	}
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}



