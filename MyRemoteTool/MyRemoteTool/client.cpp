//#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include "client.h"
#include "../../resource/tool.h"
#include "sciter/sciter-include.h"
#include "MyRemoteTool-ui.h"
#include "msock_datastruct.h"
#include "html_res.h"
#include "resource.h"
//#include "electron_include.h"


#define FUSE_SBROWSERPROCESS FALSE
constexpr auto MYWM_BROWSERDIED = WM_USER + 0xb + 0xdead;
static HHOOK hHook_CheckSciterWindowStat = NULL;
static size_t nHook_CheckSciterWindowStat = 0;
static std::map<HWND, bool> SciterWindowsToCheck;
static bool DenySciterWindowClose;
static bool connecting;

static SOCKET clientSocket = INVALID_SOCKET;

static DWORD __stdcall Thread_ConnectServer(PVOID pData);
static void CleanupSocket(HWND hwnd);

class frame_WndWrapper : public sciter::window {
public:
	frame_WndWrapper() : window(SW_CONTROLS | SW_RESIZEABLE |
		SW_TITLEBAR | SW_ENABLE_DEBUG) { }
};

class frame_PasswordInputDialog : public sciter::window {
public:
	frame_PasswordInputDialog() : window(SW_MAIN | SW_ENABLE_DEBUG) { }

	std::wstring pswd;

	SOM_PASSPORT_BEGIN(frame_PasswordInputDialog)
	SOM_FUNCS(
		SOM_FUNC(submit_pswd)
	)
	SOM_PASSPORT_END

	void submit_pswd(sciter::string pswd) {
		this->pswd = pswd;
	}
};

class MainUI_wnd;
typedef struct {
	std::wstring ip;
	unsigned short port;

	MainUI_wnd* callbk;
} ConnectServerData;
typedef struct {
	WCHAR _ip_raw[32];
	unsigned short port;
} ConnectServerData_r;
class MainUI_wnd : public sciter::window {
public:
	MainUI_wnd() : window(SW_ENABLE_DEBUG), hwnd_parent(0) { }

	std::wstring url;
	HWND hwnd_parent;

	// passport
	SOM_PASSPORT_BEGIN(MainUI_wnd)
	SOM_FUNCS(
		SOM_FUNC(nativeReloadPage),
		SOM_FUNC(openWebPage),
		SOM_FUNC(getConnectionInfo),
		SOM_FUNC(tryConnect),
		SOM_FUNC(Confirm),
		SOM_FUNC(setClosable),
		SOM_FUNC(ApplicationQuit),
		SOM_FUNC(openWebPageSciter),
		SOM_FUNC(disconnectSocket),
		SOM_FUNC(getArchString)
	)
	SOM_PASSPORT_END

	// function expsed to script:
	void nativeReloadPage() {
		this->load(url.c_str());
		this->expand();
		SendMessage(hwnd_parent, WM_SIZE, 0, 0);
	}

	void openWebPage(sciter::string url) {
		ShellExecuteW(this->get_hwnd(), L"open", url.c_str(), 0, 0, SW_NORMAL);
	}

	sciter::value getConnectionInfo() {
		sciter::value val;
		val.make_map();
		val.set_item("connected", bConnectionLiveFlag);
		val.set_item("connectstate", bConnectionLiveFlag ? 1 : (connecting ? 2 : 0));
		return val;
	}

	bool tryConnect(sciter::value val) {
		auto dat = (ConnectServerData*)VirtualAlloc(NULL,
			sizeof(ConnectServerData), MEM_COMMIT, PAGE_READWRITE);
		if (!dat) return false;
		dat->ip = val.get_item("ip").to_string();
		dat->port = (USHORT)atoi(ws2c(val.get_item("port").to_string()));
		dat->callbk = this;
		HANDLE hThread = CreateThread(0, 0, Thread_ConnectServer, dat, 0, 0);
		if (hThread) {
			CloseHandle(hThread);
			return true;
		}
		else return false;
	}

	bool Confirm(sciter::string val, sciter::string title = WSTR("Confirm"),
		int flag = 0, int removeflag = ~0) {
		return (IDYES == MessageBoxW(hwnd_parent, val.c_str(), title.c_str(),
			MB_YESNO | MB_ICONQUESTION | flag & ~removeflag));
	}

	void setClosable(bool val) {
		DenySciterWindowClose = !val;
	}

	void ApplicationQuit() {
		SendMessage(hwnd_parent, MYWM_QUIT, 0, WM_USER | WM_QUIT);
	}

	void openWebPageSciter(sciter::string url) {
		sciter::om::hasset<frame_WndWrapper> phBrowser;
		phBrowser = new frame_WndWrapper;
		phBrowser->load(url.c_str());
		phBrowser->expand();
	}

	int msgbox(sciter::string text, sciter::string title, UINT opt) {
		return MessageBoxW(hwnd_parent, text.c_str(), title.c_str(), opt);
	}

	void disconnectSocket() {
		CleanupSocket(hwnd_parent);
		nativeReloadPage();
	}

	sciter::string getArchString() {
		return
#ifdef _WIN64
			L"x64";
#else
			L"x86";
#endif
	}

};

class MainUI_data {
public:
	MainUI_data() :
		hWnd(0), hProcessJob(0), hBrowserProcess(0),
		hThreadWaitForBrowserProcessQuit(0), dwBrowserPid(0), hBrowser(0),
		windowInDestroyProgress(false)
	{};

	HWND hWnd;
	bool windowInDestroyProgress;

	HANDLE hProcessJob;
	HANDLE hBrowserProcess;
	HANDLE hThreadWaitForBrowserProcessQuit;
	DWORD dwBrowserPid;
	HWND hBrowser;

	sciter::om::hasset<MainUI_wnd> phBrowser;
};

static DWORD WINAPI Thrd_WaitForBrowserProcessQuit(PVOID p) {
	MainUI_data* d = (MainUI_data*)p;
	if (!d) return 1;

	DWORD dwExitCode = 1;
	WaitForSingleObject(d->hBrowserProcess, INFINITE);
	GetExitCodeProcess(d->hBrowserProcess, &dwExitCode);
	SendMessage(d->hWnd, MYWM_BROWSERDIED, dwExitCode, 0);

	return 0;
}

LRESULT CALLBACK HookProc_CheckSciterWindowStat(int code, WPARAM wParam, LPARAM lParam) {
	if (code == HCBT_DESTROYWND) {
		try {
			((void)(void*)(SciterWindowsToCheck.at((HWND)wParam)));

			if (DenySciterWindowClose) return 1;

			WCHAR lpsz[256]{};
			if (LoadStringW(hInst, IDS_STRING_UI_CLT_CLASS, lpsz, 256)) {
				HWND hw = FindWindowW(lpsz, NULL);
				if (hw) SendMessage(hw, MYWM_BROWSERDIED, 0, 0);
			}
		}
		catch (...) {}
	}
	return CallNextHookEx(hHook_CheckSciterWindowStat, code, wParam, lParam);
}

static INT_PTR CALLBACK DlgProc_PswdDlg(HWND hw, UINT msg, WPARAM w, LPARAM l) {
	switch (msg)
	{
	case WM_INITDIALOG:
		SetWindowLongPtr(hw, GWLP_USERDATA, (LONG_PTR)l);
		return TRUE;
		break;

	case WM_COMMAND:
		switch (w)
		{
		case IDOK: {
			WCHAR pswd[2048]{};
			GetDlgItemTextW(hw, IDC_EDIT_PSWDDLG_PASSWORD, pswd, 2048);
			((std::wstring*)GetWindowLongPtr(hw, GWLP_USERDATA))->operator=(pswd);
		}
			EndDialog(hw, IDOK);
			break;

		case IDCANCEL:
			EndDialog(hw, IDCANCEL);
			break;

		default:
			return DefWindowProc(hw, msg, w, l);
		}
		break;

	default:
		return DefWindowProc(hw, msg, w, l);
	}
	return 0;
}

static DWORD __stdcall Thread_ConnectServer(PVOID pData) {
	ConnectServerData* d = (ConnectServerData*)pData;
	if (!d) return 1;

	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints{};
	int iResult;
	connecting = true;

	// Resolve the server address and port
	iResult = getaddrinfo(ws2c(d->ip), std::to_string(d->port).c_str(), &hints, &result);
	if (iResult != 0) {
		MessageBoxW(d->callbk->hwnd_parent,
			L"Connection failed (getaddrinfo)", 0, MB_ICONHAND);
		d->callbk->nativeReloadPage();
		VirtualFree(d, 0, MEM_RELEASE);
		connecting = false;
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		clientSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (clientSocket == INVALID_SOCKET) {
			//printf("socket failed with error: %ld\n", WSAGetLastError());
			MessageBoxW(d->callbk->hwnd_parent,
				L"Connection failed (socket)", 0, MB_ICONHAND);
			d->callbk->nativeReloadPage();
			VirtualFree(d, 0, MEM_RELEASE);
			connecting = false;
			return 1;
		}

		// Connect to server.
		iResult = connect(clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (clientSocket == INVALID_SOCKET) {
		//printf("Unable to connect to server!\n");
		MessageBoxW(d->callbk->hwnd_parent,
			L"Connection failed (clientSocket)", 0, MB_ICONHAND);
		connecting = false;
		d->callbk->nativeReloadPage();
		VirtualFree(d, 0, MEM_RELEASE);
		return 1;
	}

	do{
		HKEY hkHist = NULL;
		WCHAR lpszReg[256]{};
		DWORD var1 = 0;
		if (!LoadStringW(hInst, IDS_STRING_UI_RECENTSESS_REGPATH, lpszReg, 256)) break;
		if (RegCreateKeyExW(HKEY_CURRENT_USER, lpszReg, 0, NULL,
			0, KEY_READ | KEY_WRITE, 0, &hkHist, &var1)) break;

		ConnectServerData_r r{};
		wcscpy_s(r._ip_raw, d->ip.c_str());
		r.port = d->port;
		RegSetValueExW(hkHist, (std::to_wstring(time(0)) + L"_" + std::to_wstring(
			GetCurrentProcessId())).c_str(), 0, REG_BINARY,
			(PBYTE)&r, sizeof(ConnectServerData_r));

		RegCloseKey(hkHist);
	} while (0);

	char* buf = new char[2048];
	// Send an initial buffer
send_initial:
	mNetwordPackHead header_hello = NPACK_TYPE_HELLO;
	iResult = send(clientSocket, (PCSTR)&header_hello, sizeof(mNetwordPackHead), 0);
	if (iResult == SOCKET_ERROR) {
		CleanupSocket(d->callbk->hwnd_parent);
		MessageBoxW(d->callbk->hwnd_parent,
			L"Connection failed (send)", 0, MB_ICONHAND);
		d->callbk->nativeReloadPage();
		VirtualFree(d, 0, MEM_RELEASE);
		delete[] buf;
		connecting = false;
		return 1;
	}
	iResult = recv(clientSocket, buf, 2048, 0);
	if (iResult > 0) {
		if (((mNetwordPackHead*)buf)[0] == NPACK_TYPE_STARTCONNECT) {
			ShutdownBlockReasonCreate(d->callbk->hwnd_parent, (L"Your remote session (Process "
				 +std::to_wstring(GetCurrentProcessId()) + L") is still live.").c_str());
			bConnectionLiveFlag = true;
			d->callbk->nativeReloadPage();
			VirtualFree(d, 0, MEM_RELEASE);
			connecting = false;
			delete[] buf;
			return 0;
		}
		else if (((mNetwordPackHead*)buf)[0] == NPACK_TYPE_VERIFYPSWD ||
			((mNetwordPackHead*)buf)[0] == NPACK_TYPE_RETRY) {
			{
				std::wstring pw;

				if (DialogBoxParamW(hInst, MAKEINTRESOURCEW(IDD_DIALOG_PSWDINPUT),
					d->callbk->hwnd_parent, DlgProc_PswdDlg, (LPARAM)&pw) == IDCANCEL) {
					CleanupSocket(d->callbk->hwnd_parent);
					d->callbk->nativeReloadPage();
					VirtualFree(d, 0, MEM_RELEASE);
					connecting = false;
					delete[] buf;
					return ERROR_CANCELLED;
				}

				mNetwordPackHead* buffer = new mNetwordPackHead[2048];
				buffer[0] = NPACK_TYPE_PASSWORDSENT;
				buffer[1] = (mNetwordPackHead)pw.length() + 1;
				memcpy(buffer + 2, pw.c_str(),
					size_t(buffer[1] > 2045 ? 2045 : buffer[1]) * sizeof(WCHAR));
#if 0
				WCHAR tmp2[2048]{};
				memcpy(tmp2, buffer + 2, size_t(buffer[1]) * sizeof(WCHAR));
				std::wfstream fp("debug.txt", std::ios::out);
				fp.write((wchar_t*)tmp2, buffer[1] - 1);
				fp.close();
#endif
				int bytes_sent = send(clientSocket, (char*)buffer, int(pw.length() + 3), 0);
				//bytes_sent = (bytes_sent++); // Prevent optimization
				//MessageBoxW(0, (L"Sent data " + std::to_wstring(bytes_sent)).c_str(), 0, 0);
				delete[] buffer;
			}
			goto send_initial;
		}
		else {

		}
	}
	else {
		CleanupSocket(d->callbk->hwnd_parent);
		MessageBoxW(d->callbk->hwnd_parent,
			L"Connection failed (recv)", 0, MB_ICONHAND);
		d->callbk->nativeReloadPage();
		VirtualFree(d, 0, MEM_RELEASE);
		connecting = false;
		delete[] buf;
		return 1;
	}

	d->callbk->nativeReloadPage();
	VirtualFree(d, 0, MEM_RELEASE);
	connecting = false;
	delete[] buf;
	return 0;
}

static void CleanupSocket(HWND hwnd) {
	if (!clientSocket || clientSocket == INVALID_SOCKET) return ;

	// shutdown the connection since no more data will be sent
	int iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		closesocket(clientSocket);
		return ;
	}
	closesocket(clientSocket);
	clientSocket = INVALID_SOCKET;
	bConnectionLiveFlag = false;
	ShutdownBlockReasonDestroy(hwnd);
}

LRESULT CALLBACK WndProc_MainUI(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_CREATE: {
		MainUI_data* dat = new MainUI_data;
		memset(dat, 0, sizeof(MainUI_data));
		dat->hWnd = hwnd;

		FreeResFile(IDR_BIN_HTML_FILES, "BIN", "html.7z");
		auto _pi2 = Process.Start_HiddenWindow(_T("7_zip x html.7z -y"));
		if (_pi2.hProcess) {
			CloseHandle(_pi2.hThread);
			if (WAIT_TIMEOUT == WaitForSingleObject(_pi2.hProcess, 5000)) {
				MessageBoxW(0, L"Timeout while loading resource file", 0, MB_ICONHAND);
			}
			CloseHandle(_pi2.hProcess);
		}

		WCHAR pCurrentDir[MAX_PATH + 16]{};
		GetCurrentDirectoryW(MAX_PATH + 16, pCurrentDir);
		using namespace std;
		std::wstring full_file_name = pCurrentDir + L"\\html\\client.html"s;
#if FUSE_SBROWSERPROCESS
		// sBrowserProcess trashed
		wchar_t szCmdLine[2048] = L"sBrowserProcess \"";
		wcscat_s(szCmdLine, full_file_name.c_str());
		wcscat_s(szCmdLine, L"\"");
		STARTUPINFOW si{ sizeof(si) };
		PROCESS_INFORMATION pi{};
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;
		if (!CreateProcessW(0, szCmdLine, 0, 0, 0, 0, 0, L".\\html", &si, &pi)) {
			delete dat;
			PostQuitMessage(GetLastError());
		}
		CloseHandle(pi.hThread);
		dat->hBrowserProcess = pi.hProcess;
		dat->dwBrowserPid = pi.dwProcessId;
		dat->hProcessJob = CreateJobObject(0, 0);
		AssignProcessToJobObject(dat->hProcessJob, dat->hBrowserProcess);

		dat->hThreadWaitForBrowserProcessQuit =
			CreateThread(0, 0, Thrd_WaitForBrowserProcessQuit, dat, 0, 0);

		SetTimer(hwnd, 0xcbe01, 250, 0);
#else
		dat->phBrowser = new MainUI_wnd;
		dat->phBrowser->url = full_file_name;
		dat->phBrowser->hwnd_parent = hwnd;
		dat->phBrowser->load(full_file_name.c_str());
		dat->phBrowser->expand(false);

		dat->hBrowser = dat->phBrowser->get_hwnd();

		DenySciterWindowClose = true;
		
		SetParent(dat->hBrowser, hwnd);
		SetWindowPos(dat->hBrowser, 0, 0, 0, 0, 0, SWP_FRAMECHANGED
			| SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
		SetForegroundWindow(dat->hBrowser);

		SciterWindowsToCheck.insert(std::make_pair(dat->hBrowser, true));

		nHook_CheckSciterWindowStat++;
		if (!hHook_CheckSciterWindowStat) {
			hHook_CheckSciterWindowStat = SetWindowsHookExW(WH_CBT,
				HookProc_CheckSciterWindowStat, hInst, GetCurrentThreadId());
		}
#endif

		HMENU pRecentSess = GetMenu(hwnd);
		pRecentSess = GetSubMenu(pRecentSess, 0);
		pRecentSess = GetSubMenu(pRecentSess, 3);

		do {
			HKEY hkHist = NULL;
			WCHAR lpszReg[256]{};
			DWORD var1 = 0;
			if (!LoadStringW(hInst, IDS_STRING_UI_RECENTSESS_REGPATH, lpszReg, 256)) break;
			if (RegCreateKeyExW(HKEY_CURRENT_USER, lpszReg, 0, NULL,
				0, KEY_READ | KEY_WRITE, 0, &hkHist, &var1)) break;

			WCHAR szName[256]{};
			DWORD var2 = 256;
			ConnectServerData_r buffer{};
			DWORD dwIndex = 0;
			while ((var1 = RegEnumValueW(hkHist, dwIndex++, szName, &var2,
				NULL, NULL, NULL, NULL)) == 0) {
				var1 = sizeof(buffer);
				var2 = 256;
				if (RegGetValueW(hkHist, NULL, szName, RRF_RT_REG_BINARY, NULL, &buffer, &var1))
					continue;

				AppendMenuW(pRecentSess, MF_STRING, 41001, (L"&" + to_wstring(dwIndex) +
					L": " + buffer._ip_raw + L":" + to_wstring(buffer.port)).c_str());
			}

			RegCloseKey(hkHist);
		} while (0);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)dat);
	}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_CLTMAIN_FILE_CLOSE:
		case ID_CLTMAIN_WINDOW_CLOSE:
			SendMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case ID_CLTMAIN_FILE_EXIT:
			SendMessage(hwnd, MYWM_QUIT, 0, 0);
			break;

		case ID_CLTMAIN_HELP_ENABLEDEBUG:
			if (!HIWORD(GetKeyState(VK_SHIFT))) { // not in press
				MessageBoxW(hwnd, L"This is a dev function.", 0, MB_ICONHAND);
				return FALSE;
			}
			else {
				HMENU mn = GetMenu(hwnd);
				if (!mn) return -1;
				HMENU mn2 = GetSubMenu(mn, 5);
				if (!mn2) return -1;
				EnableMenuItem(mn2, 4, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
				EnableMenuItem(mn2, 5, MF_BYPOSITION | MF_ENABLED);
				return TRUE;
			}
			break;

		case ID_CLTMAIN_HELP_INSPECTSCITER:
			Process.StartOnly(_T("sciter_inspector"));
			break;

		case ID_CLTMAIN_FORCESETBCONLIVE_F:
			bConnectionLiveFlag = false;
			break;
		case ID_CLTMAIN_FORCESETBCONLIVE_T:
			bConnectionLiveFlag = true;
			break;

		case ID_CLTMAIN_FOCUSINBROWSER:
		{
			auto d = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (d && d->hBrowser) {
				SetForegroundWindow(d->hBrowser);
			}
		}
			break;

		case ID_CLTMAIN_LICENSE_LICENSE:
		{
			sciter::om::hasset<frame_WndWrapper> phBrowser;
			phBrowser = new frame_WndWrapper;
			phBrowser->load(L"https://www.gnu.org/licenses/lgpl-3.0-standalone.html");
			phBrowser->expand();
		}
			break;

		case ID_CLTMAIN_SHELLABOUT:
			ShellAboutW(hwnd, L"My Remote Tool", L"License: LGPL", NULL);
			break;

		case ID_CLTMAIN_DISCONNECT:
			CleanupSocket(hwnd);
			{
				auto d = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
				if (d && d->phBrowser) {
					d->phBrowser->nativeReloadPage();
				}
			}
			break;

		case ID_CLTMAIN_WINDOW_RELOAD:
			{
				auto d = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
				if (d && d->phBrowser) {
					d->phBrowser->nativeReloadPage();
				}
			}
			break;

		case ID_CLTMAIN_RECENTSESSIONS_CLEAR:
		{
			WCHAR lpszReg[256]{};
			if (!LoadStringW(hInst, IDS_STRING_UI_RECENTSESS_REGPATH, lpszReg, 256)) break;
			RegDeleteTreeW(HKEY_CURRENT_USER, lpszReg);
		}
			break;

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
		break;

	case WM_SIZING:
	case WM_SIZE:
	{
		auto d = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (d && d->hBrowser) {
			if (msg == WM_SIZE) {
				switch (wParam) {
				case SIZE_MAXIMIZED:
					ShowWindow(d->hBrowser, SW_MAXIMIZE);
					break;
				case SIZE_MINIMIZED:
					ShowWindow(d->hBrowser, SW_MINIMIZE);
					break;
				case SIZE_RESTORED:
					ShowWindow(d->hBrowser, SW_NORMAL);
					break;
				default:;
				}
			}
				RECT rc{};
				GetClientRect(hwnd, &rc);
				SetWindowPos(d->hBrowser, 0, rc.left, rc.top, rc.right,
					rc.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_TIMER: 
#pragma warning(push)
#pragma warning(disable: 4065)
		switch (wParam) {
#if 0
		case 0xcbe01:
		{ auto d = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		if (d) {
			try {
				std::vector<HWND> windows;
				Process.findWindows(Process_t::ProcessInfo(TEXT(""),
					d->dwBrowserPid), windows);
				WCHAR buffer[256]{};
				if (!windows.empty())
					for (auto& wnd : windows) {
						if (!IsWindowEnabled(wnd)) continue;

						d->hBrowser = wnd;
						SetForegroundWindow(hwnd);
						SendMessage(hwnd, WM_USER + 0xcd0, (WPARAM)wnd, 0);
						SetParent(wnd, hwnd);
						ShowWindow(wnd, SW_NORMAL);
						SetWindowPos(wnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED
							| SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
						KillTimer(hwnd, wParam);
						SendMessage(hwnd, WM_SIZING, 0, 0);
						SendMessage(hwnd, WM_SIZE, 0, 0);
						SetForegroundWindow(wnd);
						break;
					}
			}
			catch (...) {}
		} }
			break;
#endif

		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}
#pragma warning(pop)
		break;

	case WM_CLOSE:
		return SendMessage(hwnd, MYWM_QUIT, 0, WM_CLOSE);
		break;

	case WM_QUERYENDSESSION:
	case WM_ENDSESSION:
		{
			auto _r = SendMessage(hwnd, MYWM_QUIT, 0, WM_ENDSESSION | (
				(msg == WM_QUERYENDSESSION) ? WM_USER : 0));
			if (_r) {
				return TRUE;
			}
			else {
				return FALSE;
			}
		}
		break;

	case MYWM_BROWSERDIED:
		if (wParam != ERROR_SUCCESS)
			MessageBoxW(hwnd, L"Embedded Browser died unexpectly",
				L"FATAL ERROR - My Remote Tool", MB_ICONHAND);
		if (auto d = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA)) {
			if (!d->windowInDestroyProgress) {
				DestroyWindow(hwnd);
			}
		}
		break;

	case MYWM_QUIT:
		if (bConnectionLiveFlag) {
			HWND hw = hwnd;
			UINT flag = MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2;
			if (lParam == (WM_ENDSESSION | WM_USER)) {
				flag |= MB_SERVICE_NOTIFICATION;
				hw = 0;
			}
			if (IDYES == MessageBoxW(hw, L"Connection is live now.\n"
				"Do you want to end the session?",
				L"Warning - My Remote Tool", flag)) {
				CleanupSocket(hwnd);
				bConnectionLiveFlag = false;
				DestroyWindow(hwnd);
				return 1;
			}
			return 0;
		}

		return DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		{
			auto d = (MainUI_data*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (d) {
				d->windowInDestroyProgress = true;
				DenySciterWindowClose = false;
				SendMessage(d->hBrowser, WM_CLOSE, 0, 0);
#pragma warning(push)
#pragma warning(disable: 6258)
#if FUSE_SBROWSERPROCESS
				SuspendThread(d->hThreadWaitForBrowserProcessQuit);
				TerminateThread(d->hThreadWaitForBrowserProcessQuit, 0);
				TerminateJobObject(d->hProcessJob, 0);
				Process.kill(d->hBrowserProcess);
				WaitForSingleObject(d->hBrowserProcess, 1000);
				CloseHandle(d->hBrowserProcess);
				CloseHandle(d->hProcessJob);
#else
				try {
					SciterWindowsToCheck.erase(d->hBrowser);
				}
				catch (...) {}
#endif
				delete d;
				SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
#pragma warning(pop)
			}
		}
		if ((nHook_CheckSciterWindowStat > 0) && ((--nHook_CheckSciterWindowStat) <= 0)) {
			UnhookWindowsHookEx(hHook_CheckSciterWindowStat);
			hHook_CheckSciterWindowStat = NULL;
		}
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}


