#include "svc.h"
#include "util.h"

#include "resource.h"

using namespace std;

#pragma warning(disable: 6258)



extern HINSTANCE hInst;
static HANDLE hConfMap;
static HANDLE hExitEvent, hExitEventC;
static HANDLE hIdleNotifyEvent;
static wstring gSvcName;

struct _config {
	DWORD cb = sizeof (_config);

	using str = WCHAR[2048];

	time_t
		idle_mintime = 0,
		idle_timeout = 0;

	str
		idle_run{},
		nidle_run{},
		u_caption{},
		u_text{},
		u_ok{},
		u_cancel{},
		u_timep{},
		u_dnsaw{};

	bool u_closable = true;
};
static _config* config;


static DWORD __stdcall MyServiceChildThread_IdleNotify(PVOID);
static int __stdcall MyServiceChild(CmdLineW& cl);
static int __stdcall MyServiceChild_IdleNotify(CmdLineW& cl);


#if 0
static DWORD __stdcall th_ExitOnParentDied(PVOID p = NULL) {
	if (p == NULL) {
		HANDLE hThread = CreateThread(0, 0,
			th_ExitOnParentDied, INVALID_HANDLE_VALUE, 0, 0);
		bool success = (bool)hThread;
		if (hThread) CloseHandle(hThread);
		return (DWORD)(success ? 0 : GetLastError());
	}
	DWORD pid = Process.GetParentProcessId(GetCurrentProcessId());
	HANDLE hProc = OpenProcess(
		PROCESS_QUERY_INFORMATION |
		PROCESS_QUERY_LIMITED_INFORMATION |
		SYNCHRONIZE,
		FALSE, pid);
	if (!hProc) return GetLastError();
	WaitForSingleObject(hProc, INFINITE);
	GetExitCodeProcess(hProc, &pid);
	CloseHandle(hProc);
	Process.close();
	ExitProcess((UINT)pid);
	return (DWORD)-1;
}
#endif

static DWORD __stdcall MyServiceChildMain_th_ExitOnParentDied(PVOID p = NULL) {
	if (p == NULL) {
		HANDLE hThread = CreateThread(0, 0, MyServiceChildMain_th_ExitOnParentDied,
			INVALID_HANDLE_VALUE, 0, 0);
		bool success = (bool)hThread;
		if (hThread) CloseHandle(hThread);
		return (DWORD)(success ? 0 : GetLastError());
	}
	DWORD pid = Process.GetParentProcessId(GetCurrentProcessId());
	HANDLE hProc = OpenProcess(
		PROCESS_QUERY_INFORMATION |
		PROCESS_QUERY_LIMITED_INFORMATION |
		SYNCHRONIZE,
		FALSE, pid);
	if (!hProc) return GetLastError();
	WaitForSingleObject(hProc, INFINITE);
	GetExitCodeProcess(hProc, &pid);
	CloseHandle(hProc);
	Process.close();
	SetEvent(hIdleNotifyEvent);
	ExitProcess((UINT)pid);
	return (DWORD)-1;
}

static void __stdcall WaitIfSession0OrInvalid(DWORD default_wait_delay = 5000) {
	DWORD dwActiveSessionId, dwTemp2;
	WTSINFOW wtsInfo{};
	bool needWait;
	//PWTS_SESSION_INFOW pWtsSessionInfo;
	PWSTR pWtsUserName;

begin:

	dwActiveSessionId = WTSGetActiveConsoleSessionId();
	needWait = false;
	dwTemp2 = 0;
	//pWtsSessionInfo = NULL;
	pWtsUserName = NULL;

	if (dwActiveSessionId == 0 || dwActiveSessionId == (DWORD)0xFFFFFFFF) {
		needWait = true;
	}
#if 0
	else if(pWtsSessionInfo) {
#if 0

#elif 0
		if (WTSEnumerateSessionsW(WTS_CURRENT_SERVER_HANDLE,
			0, 1, &pWtsSessionInfo, &dwTemp2)
		) {
			needWait = true;
			for (DWORD dwI = 0; dwI < dwTemp2; ++dwI) {
				if (pWtsSessionInfo[dwI].SessionId == dwActiveSessionId) {
					needWait = false;
					break;
				}
			}
			WTSFreeMemory(pWtsSessionInfo);
		}
#elif 1
#endif
	}
	else {
		// failed to alloc memory...

		goto end;
	}
#else
	else {
		if (WTSQuerySessionInformationW(WTS_CURRENT_SERVER_HANDLE,
			dwActiveSessionId, WTSUserName, &pWtsUserName, &dwTemp2)
		) {
			if (
				std::wstring(pWtsUserName).empty()
			) {
				// in system session
				needWait = true;
			}
			else {
				needWait = false;
			}

			// Debug only
#if 0
			system(("echo [log] [DEBUG] svc_child.cpp:157, " +
				ws2s(pWtsUserName) + " >> svc_my.log").c_str());
#endif

			WTSFreeMemory(pWtsUserName);
		}
	}
#endif

	if (needWait) {
		//while ((dwTemp2 = WTSGetActiveConsoleSessionId()) == dwActiveSessionId)
		{
			Sleep(default_wait_delay);
		}
		dwActiveSessionId = dwTemp2;
		goto begin;
	}

//end:

	return;
}



int __stdcall MyServiceChildMain_svc(CmdLineW& cl) {
	using namespace std;

	wstring type;
	cl.getopt(L"type", type);
	if (type == L"service-child") {
		return MyServiceChild(cl);
	}

	if (type != L"service-main") {
		return -1;
	}

	EnableAllPrivileges();

	MyServiceChildMain_th_ExitOnParentDied();

	wstring cfg;
	if (cl.getopt(L"config", cfg) != 1) return ERROR_INVALID_PARAMETER;

	cl.getopt(L"service-name", gSvcName);

	wstring exit_evt;
	if (1 != cl.getopt(L"exit-event", exit_evt)) {
		return ERROR_INVALID_PARAMETER;
	}
	else {
		hExitEvent = (HANDLE)(ULONG_PTR)atoll(ws2s(exit_evt).c_str());
	}
	if (1 == cl.getopt(L"exit-event-2", exit_evt)) {
		hIdleNotifyEvent = (HANDLE)(ULONG_PTR)atoll(ws2s(exit_evt).c_str());
	}


	wstring
		rwh/*run when start*/,
		sdt/*start delay time*/,
		idletime,
		idleout,
		idlerun,
		nidlerun,
		uCaption,
		uText,
		uok,
		ucancel,
		utimep,
		udnsaw,
		uclosable;
	volatile ULONGLONG bufsize = MyGetFileSizeW(cfg);
	if (bufsize > 0xFFFF) bufsize = 0xFFFF; // do not too large
	char* buffer = NULL;
	if (bufsize) {
		buffer = (char*)calloc(1, bufsize);
	}

	fstream fcfg((cfg), ios::in);
	if (!fcfg) return GetLastError();
	if (buffer) {
		ZeroMemory(buffer, bufsize);
		fcfg.read(buffer, bufsize);

		vector<string> str;
		str_split(buffer, "\r", str);
		if (str.size() == 1) {
			str.clear();
			str_split(buffer, "\n", str);
		}

		try {
			size_t n = 0;
			rwh = s2ws(str.at(n++));
			sdt = s2ws(str.at(n++));
			idletime = s2ws(str.at(n++));
			idleout = s2ws(str.at(n++));
			idlerun = s2ws(str.at(n++));
			nidlerun = s2ws(str.at(n++));
			uCaption = s2ws(str.at(n++));
			uText = s2ws(str.at(n++));
			uok = s2ws(str.at(n++));
			ucancel = s2ws(str.at(n++));
			utimep = s2ws(str.at(n++));
			udnsaw = s2ws(str.at(n++));
			uclosable = s2ws(str.at(n++));
		}
		catch (...) {}

		free(buffer);
	}
	fcfg.close();

	if (0 == atoll(ws2s(idletime).c_str())) {
		return ERROR_INVALID_PARAMETER;
	}

	if (!rwh.empty()) {
		Process.StartOnly(rwh);
	}
	if (!sdt.empty()) {
		DWORD d = (DWORD)atoi(ws2s(sdt).c_str());
		if (d) Sleep(d);
	}

	HANDLE hMap = MyCreateMemmap(sizeof(_config), 0);
	if (!hMap) return GetLastError();
	::hConfMap = hMap;
	config = (_config*)MapViewOfFile(hMap,
		FILE_MAP_ALL_ACCESS, 0, 0, sizeof(_config));
	if (config == NULL) {
		CloseHandle(hMap);
		return GetLastError();
	}

	/* parse config */ {
		config->cb = sizeof(config);

		config->idle_mintime = (time_t)atoll(ws2s(idletime).c_str());
		config->idle_timeout = (time_t)atoll(ws2s(idleout).c_str());

		wcscpy_s(config->idle_run, idlerun.c_str());
		wcscpy_s(config->nidle_run, nidlerun.c_str());
		wcscpy_s(config->u_caption, uCaption.c_str());
		wcscpy_s(config->u_text, uText.c_str());
		wcscpy_s(config->u_ok, uok.c_str());
		wcscpy_s(config->u_cancel, ucancel.c_str());
		wcscpy_s(config->u_timep, utimep.c_str());
		wcscpy_s(config->u_dnsaw, udnsaw.c_str());

		config->u_closable = !(uclosable == L"false");
	}

	HANDLE h_list[20]{};
	DWORD count_to_wait = 2;

	h_list[0] = hExitEvent;
	h_list[1] = CreateThread(0, 0, MyServiceChildThread_IdleNotify, 0, 0, 0);

	for (DWORD i = count_to_wait; i > 0; --i) {
		if (!(h_list[i - 1])) count_to_wait--;
	}

	HANDLE* phList = h_list;
	if (h_list[0] == NULL) phList += 1;

	WaitForMultipleObjects(count_to_wait, phList, FALSE, INFINITE);

	if (h_list[1]) SuspendThread(h_list[1]);

	if (hIdleNotifyEvent) {
		SetEvent(hIdleNotifyEvent);
		CloseHandle(hIdleNotifyEvent);
	}

	UnmapViewOfFile(config);
	CloseHandle(hMap);

	return 0;
}



static DWORD __thread_idle_notifier(wstring cmd, HANDLE thExitEvent, size_t type) {
	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};

	si.cb = sizeof(si);

	DWORD code = 0;
	PWSTR cl = new WCHAR[cmd.length() + 1];

	wcscpy_s(cl, cmd.length() + 1, cmd.c_str());

	size_t retryCount = 0;
	size_t failureCount = 0;
	time_t retryTime = 0;
	time_t lastTime = 0;
	constexpr size_t maxRetryCountIn1s = 5; // Retry 5 times in 1s
	constexpr size_t maxFailureCount = 20; // Max fatal error allowed
	DWORD active_session = WTSGetActiveConsoleSessionId();
	bool bFlag = false;
	if (type == 0) bFlag = true;
	do {
	cp_start:
		WaitIfSession0OrInvalid(5000);
		// Debug only
#if 0
		system(("echo execute __thread_idle_notifier, code " + to_string(type) +
			", active session " + to_string(active_session) +
			" >> mylog-service.log").c_str());
#endif
		if (!CreateProcessInSession(active_session,
			GetProgramDirW().c_str(), cl, NULL, NULL, TRUE,
			CREATE_SUSPENDED, NULL, NULL, &si, &pi
		)) {
			if (retryTime != time(0)) {
				retryCount = 0;
				retryTime = time(0);
			}
			if (retryCount++ < maxRetryCountIn1s) continue; // retry
			if (++failureCount > maxFailureCount) {
				DWORD err = code;
				if (err == 0) err = (DWORD)-1;
				delete[] cl;
				return err;
			}

			continue;
		}
		CloseHandle(pi.hThread);
		lastTime = time(0);

		Process.resume(pi.hProcess);
		if (type == 0 || type == 2 || type == 1) {
			do {
				WaitIfSession0OrInvalid(5000);
				if (active_session != WTSGetActiveConsoleSessionId()) {
					// switched session,
					if (type == 2) {
						// someone is using the computer,
						// so the function should return
						SetEvent(thExitEvent);
						if (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 5000)) {
							TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
						}
						ResetEvent(thExitEvent);
						CloseHandle(pi.hProcess);
						return 0;
					}
					//  terminate it
					//
					SetEvent(thExitEvent);
					if (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 5000)) {
						TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
					}
					ResetEvent(thExitEvent);
					CloseHandle(pi.hProcess);
					// wait for moment, or
					// it will start program over and over again
					if (type == 1) Sleep(1000);
					else Sleep(5000);
					// and reload the active session id
					active_session = WTSGetActiveConsoleSessionId();
					goto cp_start;
				}
			} while (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 2000));
		}
		else WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &code);
		CloseHandle(pi.hProcess);

		SetEvent(thExitEvent);
		Sleep(1000);
		ResetEvent(thExitEvent);

		if (type == 0) {
			switch (code) {

			case -ERROR_ACTIVE_CONNECTIONS: {
				delete[] cl;
				cl = nullptr;
				return code;
				break;
			}

			case ERROR_SHUTDOWN_IN_PROGRESS:
			{
				delete[] cl;
				cl = nullptr;
				return code;
			}
			break;

			default:
				// Unexpected Exit
				continue;
			}
		}
		else if (type == 1 || type == 2) {
			delete[] cl;
			cl = nullptr;
			return code;
		}

	} while (bFlag);

	return 0;
}

static DWORD _MyServiceChild_IdleExec(size_t type) {
	if (type == 0) {
		if (config->idle_timeout == 0) {
			Process.StartOnly(config->idle_run);
			return 0;
		}

		wstring data0, data1, data2, data3, data4, data5, data6, data7, data8;

		data0 = config->u_caption;
		if (data0.empty()) data0 = gSvcName;
		if (data0.empty()) data0 = L"The program is about to run...";
		data1 = config->u_text;
		data2 = config->u_timep;
		data3 = to_wstring(config->idle_timeout);
		data4 = config->u_ok;
		data5 = config->u_cancel;
		data6 = config->u_dnsaw;
		data7 = config->u_closable ? L"true" : L"false";

		time_t* pNoShowAgainWithin = NULL;
		HANDLE hFileMap = MyCreateMemmap(sizeof(time_t), 0);
		if (hFileMap) {
			pNoShowAgainWithin = (time_t*)MapViewOfFile(hFileMap,
				FILE_MAP_ALL_ACCESS, 0, 0, sizeof(time_t));
		}
		data8 = to_wstring((LONG_PTR)hFileMap);

		if (hExitEvent) ResetEvent(hExitEvent);

		wstring cmd = L"ServiceChildProcess --type=ui --ui-type=exectip "s +
			L"--exit-event=\"" + to_wstring((LONG_PTR)hExitEvent) + L"\" "
			L"--data0=\"" + data0 + L"\" "
			L"--data1=\"" + data1 + L"\" "
			L"--data2=\"" + data2 + L"\" "
			L"--data3=\"" + data3 + L"\" "
			L"--data4=\"" + data4 + L"\" "
			L"--data5=\"" + data5 + L"\" "
			L"--data6=\"" + data6 + L"\" "
			L"--data7=\"" + data7 + L"\" "
			L"--data8=\"" + data8 + L"\" ";

		DWORD ret = 0;
		auto val = __thread_idle_notifier(cmd, NULL, 1);
		if (val == 0) {
			Process.StartOnly(config->idle_run);
		}
		else {
			ret = 0xF0006690;
		}

		if (pNoShowAgainWithin && (*pNoShowAgainWithin)) {
			DWORD active_session = WTSGetActiveConsoleSessionId();
			while (time(0) < *pNoShowAgainWithin) {
				Sleep(5000);
				if (active_session != WTSGetActiveConsoleSessionId()) {
					break; // switched user, reset time
				}
			}

			ret = 0xF0006680;
		}

		if (pNoShowAgainWithin) UnmapViewOfFile(pNoShowAgainWithin);
		if (hFileMap) CloseHandle(hFileMap);

		return ret;
	}
	if (type == 1) {
		Process.StartOnly(config->nidle_run);

		return 0;
	}
	return ERROR_INVALID_PARAMETER;
}

DWORD __stdcall MyServiceChildThread_IdleNotify(PVOID) {
	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (!hIdleNotifyEvent)
		hIdleNotifyEvent = CreateEvent(&sa, TRUE, FALSE, NULL);
	if (!hIdleNotifyEvent) return GetLastError();

	wstring cmd = L"ServiceChildProcess --type=service-child "
		"--child-type=idle-notify --exit-event=" +
		to_wstring((LONG_PTR)hIdleNotifyEvent) + L" "
		L"--config=" + to_wstring((LONG_PTR)hConfMap) + L" ";
	wstring cmd_eic = L"ServiceChildProcess --type=service-child "
		"--child-type=idle-notify --wait-for-not-idle "
		"--exit-event=" + to_wstring((LONG_PTR)hIdleNotifyEvent);

	__thread_idle_notifier(cmd_eic, hIdleNotifyEvent, 2);
	_MyServiceChild_IdleExec(1);

	while (1) {

		ResetEvent(hIdleNotifyEvent);

		// idle notifier
		__thread_idle_notifier(cmd, hIdleNotifyEvent, 0);
		WaitIfSession0OrInvalid(5000);
		// Computer is idle now!
		// start idle_run
		auto run_result = _MyServiceChild_IdleExec(0);
		if (run_result == 0xF0006680) {
			// didn't run: user had set a wait value 
			// either the program waited
			// or the session switched

		}
		else if (run_result == 0xF0006690) {
			// didn't run: user cancelled

		}
		else /* runned */ {
			ResetEvent(hIdleNotifyEvent);
			// wait for not idle
			__thread_idle_notifier(cmd_eic, hIdleNotifyEvent, 2);
		}

		// either someone used this computer
		// or the user did something
		// so, start nidle_run
		_MyServiceChild_IdleExec(1);

	}

	return 0;
}


int __stdcall MyServiceChild(CmdLineW& cl) {
	using namespace std;
	wstring ctype;
	cl.getopt(L"child-type", ctype);

	//th_ExitOnParentDied();


	if (ctype == L"idle-notify") {
		return MyServiceChild_IdleNotify(cl);
	}


	return 0;
}




struct idle_info {
	HWND hwnd;
	time_t idle_time_min; // 至少idle_time_min秒后////给hwnd发消息
} _idlechk_info;

static HHOOK hIdleCheckHook;
static time_t _idlechk_last_noidle_time;
static LRESULT CALLBACK _IdleCheck_KbHookProc(int, WPARAM, LPARAM);

static DWORD __stdcall _IdleCheck_Cursor(PVOID) {
	const idle_info& info = _idlechk_info;
	//HWND hwnd = info.hwnd;

	POINT pt{}, pt_last{};
	GetCursorPos(&pt_last);

	while (1) {
		GetCursorPos(&pt);

		if (pt.x != pt_last.x || pt.y != pt_last.y) {
			// Not idle
			_idlechk_last_noidle_time = time(0);
			pt_last = pt;
			Sleep(1000);
			continue;
		}

		time_t idletime = time(0) - _idlechk_last_noidle_time;
		if (idletime >= info.idle_time_min) {
			Sleep(500);
			break;
		}

		Sleep(1000);

	}

	//SendMessage(hwnd, WM_USER + WM_ENTERIDLE, 0, 0);
	if (hIdleCheckHook) UnhookWindowsHookEx(hIdleCheckHook);
	Process.close();
	ExitProcess((UINT) - ERROR_ACTIVE_CONNECTIONS);

#pragma warning(push)
#pragma warning(disable: 4702)
	return 0;
#pragma warning(pop)
}
static LRESULT CALLBACK _IdleCheck_KbHookProc(
	int    nCode,
	WPARAM wParam,
	LPARAM lParam
) {
	if (nCode >= 0) {
		// a valid keyboard input
		_idlechk_last_noidle_time = time(0);

		////SendMessage(_idlechk_info.hwnd, WM_USER + WM_ENTERIDLE, 0, 0);
	}

	return CallNextHookEx(hIdleCheckHook, nCode, wParam, lParam);
}
static DWORD __stdcall _IdleCheck_Thread_$02(PVOID) {
	POINT pt{}, pt_last{};
	GetCursorPos(&pt_last);
	while (_idlechk_last_noidle_time == 0) {
		GetCursorPos(&pt);
		if (
			(pt.x != pt_last.x || pt.y != pt_last.y) ||
			(WaitForSingleObject(hExitEvent, 0) == WAIT_OBJECT_0)
		) {
			// Not idle
			break;
		}
		Sleep(1000);
		continue;
	}
	//SendMessage(_idlechk_info.hwnd, WM_USER + WM_QUIT, 0, 0);
	if (hIdleCheckHook) UnhookWindowsHookEx(hIdleCheckHook);
	Process.close();
	ExitProcess(0);
#pragma warning(push)
#pragma warning(disable: 4702)
	return 0;
#pragma warning(pop)
}

static LRESULT CALLBACK WndProc_IdleNotify(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
	switch (msg)
	{

	case WM_USER+WM_ENTERIDLE:
		PostQuitMessage(-ERROR_ACTIVE_CONNECTIONS);
		break;

	case WM_USER+WM_QUIT:
		PostQuitMessage(ERROR_CANCELLED);
		break;

	case WM_DESTROY:
		PostQuitMessage(-1);
		break;

	case WM_ENDSESSION:
		PostQuitMessage(ERROR_SHUTDOWN_IN_PROGRESS);
		break;

	default:
		return DefWindowProc(hw, msg, wp, lp);
	}
	return 0;
}
int __stdcall MyServiceChild_IdleNotify_Worker(CmdLineW& cl);
int __stdcall MyServiceChild_IdleNotify(CmdLineW& cl) {
	if (cl.getopt(L"worker")) {
		return MyServiceChild_IdleNotify_Worker(cl);
	}

	wstring exev;
	if (1 != cl.getopt(L"exit-event", exev)) {
		return ERROR_INVALID_PARAMETER;
	}
	::hExitEvent = (HANDLE)(LONG_PTR)atoll(ws2s(exev).c_str());

	std::wstring cmd = GetCommandLineW();
	cmd += L" --worker";
	
	bool isWaitForNotIdle = bool(cl.getopt(L"wait-for-not-idle"));

	HANDLE h_l[2]{};
	h_l[0] = hExitEvent;

	{
		STARTUPINFOW si{};
		PROCESS_INFORMATION pi{};

		si.cb = sizeof(si);

		DWORD code = 0;
#pragma warning(push)
#pragma warning(disable: 4457)
		PWSTR cl = new WCHAR[cmd.length() + 1];
#pragma warning(pop)

		wcscpy_s(cl, cmd.length() + 1, cmd.c_str());

		size_t failureCount = 0;
		constexpr size_t maxFailureCount = 5; // Max fatal error allowed
		HDESK act_desk = NULL;
		do {
		cp_start:
			if (act_desk) CloseDesktop(act_desk);
			act_desk = OpenInputDesktop(0, FALSE, GENERIC_ALL);
			if (!CreateProcessW(
				GetProgramDirW().c_str(), cl, NULL, NULL, TRUE,
				CREATE_SUSPENDED, NULL, NULL, &si, &pi
			)) {
				if (++failureCount > maxFailureCount) {
					DWORD err = code;
					if (err == 0) err = (DWORD)-1;
					delete[] cl;
					if (act_desk) CloseDesktop(act_desk);
					return err;
				}

				continue;
			}
			CloseHandle(pi.hThread);

			h_l[1] = pi.hProcess;
			Process.resume(pi.hProcess);
			{
				while (WAIT_TIMEOUT == WaitForMultipleObjects(2, h_l, FALSE, 2000)) {
					HDESK dsk2 = OpenInputDesktop(0, FALSE, GENERIC_ALL);
					WCHAR name1[256]{}, name2[256]{};
					DWORD nLen = 0;
					if (act_desk) GetUserObjectInformationW(act_desk,
						UOI_NAME, name1, sizeof(name1), &nLen);
					if (dsk2) GetUserObjectInformationW(dsk2,
						UOI_NAME, name2, sizeof(name2), &nLen);
					if (dsk2) CloseDesktop(dsk2);
					if (wcscmp(name1, name2) != 0) {
						// switched desktop,
						if (!isWaitForNotIdle) {
							// terminate it
							SetEvent(hExitEvent);
							if (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 5000)) {
								TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
							}
							ResetEvent(hExitEvent);
							CloseHandle(pi.hProcess);
							// wait for moment
							Sleep(200);
							// and restart
							goto cp_start;
						}
						else {
							// someone used the computer!
							// quit...
							delete[] cl;
							cl = nullptr;
							SetEvent(hExitEvent);
							if (WAIT_TIMEOUT == WaitForSingleObject(pi.hProcess, 5000)) {
								TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
							}
							ResetEvent(hExitEvent);
							CloseHandle(pi.hProcess);
							if (act_desk) CloseDesktop(act_desk);
							return 0;
						}
					}
				}
			}
			GetExitCodeProcess(pi.hProcess, &code);
			CloseHandle(pi.hProcess);

			delete[] cl;
			cl = nullptr;
			if (act_desk) CloseDesktop(act_desk);
			return code;

		} while (true);
	}

	return 0;
}
int __stdcall MyServiceChild_IdleNotify_Worker(CmdLineW& cl) {
	HDESK hDesk = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (hDesk) {
		SetThreadDesktop(hDesk);
		CloseDesktop(hDesk);
	}

	wstring exev;
	if (1 != cl.getopt(L"exit-event", exev)) {
		return ERROR_INVALID_PARAMETER;
	}
	::hExitEvent = (HANDLE)(LONG_PTR)atoll(ws2s(exev).c_str());

	if (cl.getopt(L"wait-for-not-idle")) {
		WCHAR wclass[256]{};
		LoadStringW(hInst, IDS_STRING_UI_CLASS_LIDLENTF, wclass, 256);

		WNDCLASSEXW wcex{};
		wcex.cbSize = sizeof(wcex);
		wcex.hInstance = hInst;
		wcex.lpszClassName = wclass;
		wcex.lpfnWndProc = WndProc_IdleNotify;
		RegisterClassExW(&wcex);

		HWND hw = CreateWindowExW(0, wclass, L"", WS_OVERLAPPED, 0, 0, 1, 1, 0, 0, 0, 0);
		if (!hw) return GetLastError();
		_idlechk_info.hwnd = hw;

		hIdleCheckHook = SetWindowsHookEx(WH_KEYBOARD_LL,
			_IdleCheck_KbHookProc, hInst, 0);
		_idlechk_last_noidle_time = 0;

		HANDLE ht = CreateThread(0, 0, _IdleCheck_Thread_$02, 0, 0, 0);
		if (ht) CloseHandle(ht);

		MSG msg{};
		while (GetMessage(&msg, 0, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (hIdleCheckHook) UnhookWindowsHookEx(hIdleCheckHook);
		return 0;
	}

	if (1 != cl.getopt(L"config", exev)) {
		return ERROR_INVALID_PARAMETER;
	}
	::hConfMap = (HANDLE)(LONG_PTR)atoll(ws2s(exev).c_str());
	if (!hConfMap) return ERROR_INVALID_PARAMETER;
	config = (_config*)MapViewOfFile(hConfMap,
		FILE_MAP_ALL_ACCESS, 0, 0, sizeof(_config));
	if (config == NULL) {
		CloseHandle(hConfMap);
		return GetLastError();
	}

	WCHAR wclass[256]{};
	LoadStringW(hInst, IDS_STRING_UI_CLASS_IDLENOTIFY, wclass, 256);

	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(wcex);
	wcex.hInstance = hInst;
	wcex.lpszClassName = wclass;
	wcex.lpfnWndProc = WndProc_IdleNotify;
	RegisterClassExW(&wcex);

	HWND hw = CreateWindowExW(0, wclass, L"", WS_OVERLAPPED, 0, 0, 1, 1, 0, 0, 0, 0);
	if (!hw) return GetLastError();

	_idlechk_last_noidle_time = time(0);

	HANDLE hNewThreadWait = CreateThread(0, 0, [](PVOID) -> DWORD {
		WaitForSingleObject(hExitEvent, INFINITE);
		//SendMessage((HWND)p, WM_USER + WM_QUIT, 0, 0);
		if (hIdleCheckHook) UnhookWindowsHookEx(hIdleCheckHook);
		Process.close();
		ExitProcess(ERROR_TIMEOUT);
#pragma warning(push)
#pragma warning(disable: 4702)
		return 0;
#pragma warning(pop)
		}, hw, 0, 0);
	if (hNewThreadWait) CloseHandle(hNewThreadWait);

	
	_idlechk_info.hwnd = hw;
	_idlechk_info.idle_time_min = config->idle_mintime;
	HANDLE hNewThreadCurChk =
		CreateThread(0, 0, _IdleCheck_Cursor, 0, 0, 0);

	hIdleCheckHook = SetWindowsHookEx(WH_KEYBOARD_LL,
		_IdleCheck_KbHookProc, hInst, 0);

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UnmapViewOfFile(config);
	CloseHandle(hConfMap);
	if (hIdleCheckHook) UnhookWindowsHookEx(hIdleCheckHook);
	if (hNewThreadCurChk) {
		TerminateThread(hNewThreadCurChk, DWORD(-1));
		CloseHandle(hNewThreadCurChk);
	}
	return (int)msg.wParam;
	return 0;
}












