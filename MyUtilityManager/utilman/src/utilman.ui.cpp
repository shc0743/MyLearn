#include "utilman.ui.hpp"
#include "../../../resource/tool.h"
#include "../res/utilman_1.h"


// defines WndProc s
#define _WndProc_arglist HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam
static LRESULT CALLBACK WndProc_CtlWnd(_WndProc_arglist);
static LRESULT CALLBACK WndProc_UiWnd(_WndProc_arglist);
static LRESULT CALLBACK WndProc_dllhost(_WndProc_arglist);

static UINT WM_TASKBARCREATED = 0;

extern HANDLE hJobMain;


BOOL MyInitWndClasses() {
	static bool registered = false;
	if (registered) {
		SetLastError(ERROR_ALREADY_INITIALIZED);
		return FALSE;
	}

	WCHAR wclass[256]{};
	HINSTANCE inst = ThisInst;
	BOOL successful = TRUE;
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(wcex);

	successful &= (WM_TASKBARCREATED = RegisterWindowMessage(_T("TaskbarCreated")));

	LoadStringW(inst, IDS_STRING_UI_CLASS_CTLWND, wclass, 255);
	successful &= s7::MyRegisterClassW(wclass, WndProc_CtlWnd);

	LoadStringW(inst, IDS_STRING_UI_CLASS_UIWND, wclass, 255);
	wcex.hbrBackground = CreateSolidBrush(RGB(0xf0, 0xf0, 0xf0));
	successful &= s7::MyRegisterClassW(wclass, WndProc_UiWnd, wcex);

	LoadStringW(inst, IDS_STRING_UI_CLASS_DLLHOST, wclass, 255);
	successful &= s7::MyRegisterClassW(wclass, WndProc_dllhost);

	registered = successful;
	return successful;
}


// WndProc s 

static LRESULT WndProc_CtlWnd(_WndProc_arglist) {
	switch (msg) {
	case WM_CREATE:

		break;

	case MYWM_CTLWND_OPENUI:
		Process.StartOnly(to__str("\"" + GetProgramDir() + "\" --type=ui"));
		break;

	case MYWM_DIE:
		DestroyWindow(hwnd);
		break;

	case MYWM_TERMALL: {
		TerminateJobObject(hJobMain, (UINT)wParam);
		CloseHandle(hJobMain);
		return 0;
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

std::map<HWND, PNOTIFYICONDATAW> WndProc_UiWnd_icondata;
static LRESULT WndProc_UiWnd(_WndProc_arglist) {
	switch (msg) {
	case WM_CREATE:
		SendMessage(hwnd, MYWM_CREATEICON, TRUE, 0);
		break;

	case MYWM_TASKBARICON:
		if (0) break;
		else if (lParam == WM_LBUTTONUP) {
			ShowWindow(hwnd, IsWindowVisible(hwnd) ? SW_HIDE : SW_RESTORE);
			if (IsWindowVisible(hwnd)) SetForegroundWindow(hwnd);
			break;
		}
		else if (lParam == WM_LBUTTONDBLCLK) {
			ShowWindow(hwnd, SW_RESTORE);
			SetForegroundWindow(hwnd);
			break;
		}
		else if (lParam == WM_RBUTTONUP) {
			HMENU menu = CreatePopupMenu();
			if (!menu) break;
			
			constexpr int ID_MAIN = 2;
			constexpr int ID_EXIT = 3;
			constexpr int ID_EXITANDTERM = 4;
			constexpr int ID_SETTINGS = 11;
			constexpr int ID_SUPPORTS = 12;
			constexpr int ID_ABOUT = 13;
			constexpr int ID_BUGREPORT = 14;

			AppendMenuW(menu, MF_STRING, ID_MAIN, L"&Main Window");
			AppendMenuW(menu, MF_SEPARATOR, 0, 0);
			AppendMenuW(menu, MF_STRING, ID_SETTINGS, L"&Settings");
			AppendMenuW(menu, MF_STRING, ID_SUPPORTS, L"S&upports");
			AppendMenuW(menu, MF_STRING, ID_ABOUT, L"&About");
			AppendMenuW(menu, MF_SEPARATOR, 0, 0);
			AppendMenuW(menu, MF_STRING, ID_BUGREPORT, L"Bug && Crash report");
			AppendMenuW(menu, MF_SEPARATOR, 0, 0);
			AppendMenuW(menu, MF_STRING, ID_EXITANDTERM, L"Exit and &Terminate");
			AppendMenuW(menu, MF_STRING, ID_EXIT, L"&Exit");

			POINT pt = { 0 };
			GetCursorPos(&pt);
			SetForegroundWindow(hwnd);

			int resp = TrackPopupMenu(menu,
				TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
				pt.x, pt.y, NULL, hwnd, NULL);

			DestroyMenu(menu);

			if (!resp) break;
			else if (resp == ID_MAIN) {
				ShowWindow(hwnd, SW_RESTORE);
				SetForegroundWindow(hwnd);
			}
			else if (resp == ID_EXIT || resp == ID_EXITANDTERM) {
				SendMessage(hwnd, MYWM_CREATEICON, FALSE, 0);
				WCHAR wclass[256]{ 0 };
				LoadStringW(ThisInst, IDS_STRING_UI_CLASS_CTLWND, wclass, 255);
				HWND hwnd2 = NULL;
				if ((hwnd2 = FindWindowW(wclass, NULL))) {
					return (int)SendMessage(hwnd2, resp == ID_EXIT ?
						MYWM_DIE : MYWM_TERMALL, 0, 0);
				}
				DestroyWindow(hwnd);
			}
			else if (resp == ID_BUGREPORT) {
				WCHAR support_url[256]{ 0 };
				if (!LoadStringW(NULL, IDS_STRING_UI_BUGRPT_URL,
					support_url, 255)) return 1;
				ShellExecuteW(NULL, L"open", support_url, NULL, NULL, SW_NORMAL);
			}
			break;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case MYWM_CREATEICON:
		if (wParam) {
			try { WndProc_UiWnd_icondata.erase(hwnd); }
			catch (...) {}
			PNOTIFYICONDATAW nidw = new NOTIFYICONDATAW;
			memset(nidw, 0, sizeof(NOTIFYICONDATAW));
			nidw->cbSize = sizeof(NOTIFYICONDATAW);
			nidw->hWnd = hwnd;
			nidw->uFlags = NIF_ICON | NIF_MESSAGE | NIF_INFO | NIF_TIP;
			nidw->uCallbackMessage = MYWM_TASKBARICON;
			nidw->hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wcscpy_s(nidw->szTip, L"My Utility Manager");
			WndProc_UiWnd_icondata[hwnd] = nidw;

			Shell_NotifyIconW(NIM_ADD, WndProc_UiWnd_icondata[hwnd]);
		}
		else {
			try {
				Shell_NotifyIconW(NIM_DELETE, WndProc_UiWnd_icondata[hwnd]);
				delete WndProc_UiWnd_icondata[hwnd];
				WndProc_UiWnd_icondata.erase(hwnd);
			}
			catch (...) {}
		}
		break;

	case WM_CLOSE:
#if 0
		if (MessageBoxW(hwnd, L"Hide window or exit?\n\nPress [OK] to exit\n"
			"Press [Cancel] to hide", L"Exit",
			MB_ICONQUESTION | MB_OKCANCEL) == IDOK) {
			WCHAR wclass[256]{ 0 };
			LoadStringW(ThisInst, IDS_STRING_UI_CLASS_CTLWND, wclass, 255);
			HWND hwnd2 = NULL;
			if ((hwnd2 = FindWindowW(wclass, NULL))) {
				return (int)SendMessage(hwnd2, MYWM_DIE, 0, 0);
			}
			DestroyWindow(hwnd);
		}
		else ShowWindow(hwnd, SW_HIDE);
#else
		ShowWindow(hwnd, SW_HIDE);
#endif
		break;

	case MYWM_DIE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		SendMessage(hwnd, MYWM_CREATEICON, FALSE, 0);
		PostQuitMessage(0);
		break;

	default:
		if (msg == WM_TASKBARCREATED) {
			SendMessage(hwnd, MYWM_CREATEICON, TRUE, 0);
			break;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

static LRESULT WndProc_dllhost(_WndProc_arglist) {
	switch (msg) {
	case WM_CREATE:

		break;

	case MYWM_DIE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


