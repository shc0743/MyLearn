#include "ui.h"
#include "wndproc-helper.h"
#include "resource.h"


extern HINSTANCE hInst;

static HFONT common_font;


DefineWndProcDeclare(WndProc_Config);
DefineWndProcDeclare(WndProc_ExecTip);



signed char __stdcall InitializeApplicationUserInterface() {
	signed char result = 0;
	bool haveAtLeast1Succeed = false;
	wchar_t buffer[256]{ 0 };
	DWORD last_err = 0;
	const auto exec = 
		[&result, &buffer, &haveAtLeast1Succeed, &last_err]
		(DWORD strid, WNDCLASSEXW cinfo) -> ATOM {
			ATOM atom = 0;
			if (LoadStringW(hInst, strid, buffer, 256)) {
				cinfo.lpszClassName = buffer;
				atom = RegisterClassExW(&cinfo);
				if (!atom) last_err = GetLastError();
			} else {
				last_err = GetLastError();
			}
			if (!atom) {
				if (haveAtLeast1Succeed) {
					result = 2;
				} else {
					result = 1;
				}
			} else {
				haveAtLeast1Succeed = true;
			}
			return atom;
		};
	constexpr char _2c1eaadf39f347408d6813e524039941 = 0x0;


	HBRUSH dlgBrush = CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));
	HBRUSH whiteBrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	::common_font = CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));

	INITCOMMONCONTROLSEX icce{
		.dwSize = sizeof(INITCOMMONCONTROLSEX),
		.dwICC = ICC_ALL_CLASSES
	};
	InitCommonControlsEx(&icce);


	exec(IDS_STRING_UI_CLASS_CONFIG, WNDCLASSEXW{
		.cbSize = sizeof(WNDCLASSEXW),
		.lpfnWndProc = WndProc_Config,
		.hInstance = ::hInst,
		.hbrBackground = dlgBrush,
	});

	exec(IDS_STRING_UI_CLASS_EXECTIP, WNDCLASSEXW{
		.cbSize = sizeof(WNDCLASSEXW),
		.lpfnWndProc = WndProc_ExecTip,
		.hInstance = ::hInst,
		.hbrBackground = whiteBrush,
	});


	SetLastError(last_err);

	return result;
}
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")





int __stdcall UiFunc_Config(CmdLineW& cl) {

	AllocConsole();
	
	HANDLE
		i = GetStdHandle(STD_INPUT_HANDLE),
		o = GetStdHandle(STD_OUTPUT_HANDLE),
		e = GetStdHandle(STD_ERROR_HANDLE);


	char buffer[2048]{};
	wchar_t wbuffer[2048]{};

	const auto read = [i](char* buffer, DWORD bufflen) {
		DWORD readen = 0;
		ReadConsoleA(i, buffer, bufflen, &readen, NULL);
		return readen;
	};
	const auto readw = [i](wchar_t* buffer, DWORD bufflen) {
		DWORD readen = 0;
		ReadConsoleW(i, buffer, bufflen, &readen, NULL);
		return readen;
	};
	const auto write = [](HANDLE h, const char* buffer) {
		DWORD written = 0;
		DWORD bufflen = (DWORD)strlen(buffer);
		WriteConsoleA(h, buffer, bufflen, &written, NULL);
		return written;
	};


	OPENFILENAMEW ofn{};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetConsoleWindow();
	ofn.lpstrFile = wbuffer;
	ofn.lpstrFile[0] = L'\0';
	ofn.nMaxFile = 1023;
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = L"Choose a file";
	ofn.Flags = OFN_PATHMUSTEXIST;
	
	write(o, "Config File, type an unexisting file to create: ");
	//read(buffer, 2048);
	if (!GetOpenFileNameW(&ofn)) {

		return GetLastError();
	}

	if (!file_exists(wbuffer)) {
		//write(o, "File to create: ");
		//read(buffer, 2048);
		std::fstream conf;
		conf.open(ws2s(wbuffer), std::ios::out);
		if (!conf) {
			write(e, "Cannot open file! \n");
			write(o, "Press Enter to continue...");
			read(buffer, 1);
			write(o, "\n");
			return 1;
		}
		conf.close();
	}

	std::fstream conf;
	conf.open(ws2s(wbuffer), std::ios::in);
	if (!conf) {
		write(e, "Cannot open file! \n");
		write(o, "Press Enter to continue...");
		read(buffer, 1);
		write(o, "\n");
		return 1;
	}

	write(o, "\nMyRunProgramWhenComputerUnused Config\n"
		"1- Install this file\n"
		"2- Uninstall service\n"
		"3- Config\n"
		"0- Exit\n"
		"Please choose: ");
	read(buffer, 2048);

	{
		if (buffer[0] == '0') goto end;
		if (buffer[0] == '3') {
			std::wstring on, nn;
			on = wbuffer;
			nn = std::wstring(wbuffer) + L".tmp";
			//std::wfstream fp(nn, std::ios::out | std::ios::binary);
			HANDLE fp = CreateFileW(nn.c_str(), GENERIC_WRITE, FILE_SHARE_READ,
				0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (INVALID_HANDLE_VALUE == fp) {
				write(e, "Cannot open file! \n");
				write(o, "Press Enter to continue...");
				read(buffer, 1);
				write(o, "\n");
				goto end;
			}
			std::map<size_t, std::wstring> confs;
			size_t n = 0;
			DWORD nBytesWritten = 0;

			const auto mydo = [&] {
				AutoZeroMemory(wbuffer);
				readw(wbuffer, 2048);
				confs[n] = wbuffer;
				WriteFile(fp, wbuffer, DWORD((wcslen(wbuffer) - 1) * sizeof(wchar_t)),
					&nBytesWritten, NULL);
				FlushFileBuffers(fp);
			};

			write(o, "Follow the steps to config:\n");

			n = 0;
			write(o, "[0] When the service start, run the following program first:\n");
			mydo();
			write(o, "[+] Config saved.\n");

			n = 1;
			write(o, "[1] After the following miliseconds, start main application.\n");
			mydo();
			write(o, "[+] Config saved.\n");

			n = 2;
			write(o, "[2] If the computer is idle for more than the following seconds:\n");
			mydo();

			n = 3;
			write(o, "[3] After the following miliseconds:\n");
			mydo();

			n = 4;
			write(o, "[4] run the following program:\n");
			mydo();

			write(o, "[+] Config saved.\n");

			n = 12;
			write(o, "[12] When the computer leaves the idle state and "
				"is busy again, run the following program:\n");
			mydo();
			write(o, "[+] Config saved.\n");

			write(o, "Do you want to customize the run-information window? (y/N) ");
			char _c = _getch();
			if (_c == 'y' || _c == 'Y') {
				write(o, "\nTip: Keep a option empty to apply its default value.\n");

				n = 5;
				write(o, "[5] The caption of it (required) ? ");
				mydo();
				write(o, "[+] Config saved.\n");

				n = 6;
				write(o, "[6] The text in the editbox? ");
				mydo();
				write(o, "[+] Config saved.\n");

				n = 7;
				write(o, "[7] The text in the OK button? ");
				mydo();
				write(o, "[+] Config saved.\n");

				n = 8;
				write(o, "[8] The text in the Cancel button? ");
				mydo();
				write(o, "[+] Config saved.\n");

				n = 9;
				write(o, "[9] The text in the countdown? "
					"(Use \"%s\" instead of real time) ");
				mydo();
				write(o, "[+] Config saved.\n");

				n = 10;
				write(o, "[10] The text in the Do-Not-Show-Again-Within prompt?");
				mydo();
				write(o, "[+] Config saved.\n");

				n = 11;
				write(o, "[11] Is it closable? (true/false) ");
				mydo();
				write(o, "[+] Config saved.\n");

			}


			CloseHandle(fp);
			write(o, "You've finished all the steps!\n");
			// Please check your details : \n");

#if 0
			write(o, "When the service start, run the following program first: ");
			write(o, confs[0].c_str());
			write(o, "After ");
			write(o, confs[1].c_str());
			write(o, "seconds, start main application. \n\n");
#endif

			write(o, "Save your changes now? (y/N) ");
			buffer[0] = buffer[1] = 0;
			read(buffer, 1);
			if (buffer[0] == 'y' || buffer[0] == 'Y') {
				write(o, "Saving your changes... 0%\r");
				conf.close();
				if (!(DeleteFileW(wbuffer) && MoveFileW(
					(std::wstring(wbuffer) + L".tmp").c_str(), wbuffer))
				) {
					DWORD err = GetLastError();
					write(e, "Unable to save your changes!!!");
					write(e, ("Error " + std::to_string(err) + ": "
						+ ErrorCodeToStringA(err) + "\n").c_str());
					write(o, "Press Enter to continue...");
					read(buffer, 1);
					write(o, "\n");
					return -1;
				}
				Sleep(rand() % 2000 + 1000);
				write(o, "[+] Changes saved.\n");
			}
			else {
				DeleteFileW((std::wstring(wbuffer) + L".tmp").c_str());
			}
			write(o, "Press any key to continue...");
			pause;
			goto end;
		}
		if (buffer[0] == '2') {
			write(o, "Service to uninstall: ");
			read(buffer, 2048);
			std::string s = buffer;
			s.erase(s.end() - 2);
			str_replace(s, "\r", "");
			str_replace(s, "\n", "");
			write(o, "Uninstall ");
			write(o, s.c_str());
			write(o, "? ");
			char _ch = _getch();
			if (_ch == 'y' || _ch == 'Y') {
				ShellExecuteW(0, IsRunAsAdmin() ? L"open" : L"runas",
					L"cmd.exe", (L"/c net stop \"" + s2ws(s) + L"\" & sc delete \""
						+ s2ws(s)+ L"\" & pause").c_str(), 0, 1);
				return 0;
			}
			return 0;
		}
		if (buffer[0] == '1') {
			write(o, "Service to install: ");
			read(buffer, 2048);
			std::string s = buffer;
			s.erase(s.end() - 2);
			str_replace(s, "\r", "");
			str_replace(s, "\n", "");
			write(o, "Install ");
			write(o, s.c_str());
			write(o, "? ");
			char _ch = _getch();
			if (_ch == 'y' || _ch == 'Y') {
				Process.StartOnly((L"\"" + GetProgramDirW()) +
					L"\" --install-service --service-name=\"" + s2ws(s) +
					L"\" --config=\"" + wbuffer + L"\" --interface");
				return 0;
			}
			return 0;
		}

	}

	end:
	conf.close();
	return 0;



	WCHAR className[256]{};
	if (!LoadStringW(hInst, IDS_STRING_UI_CLASS_CONFIG, className, 256))
		return GetLastError();

	HWND hwnd = CreateWindowExW(0, className, L"Config - MyRunProgramWhenComputerUnused",
		WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, hInst, &cl);
	if (!hwnd) return GetLastError();

	CenterWindow(hwnd);
	ShowWindow(hwnd, SW_NORMAL);

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
	return 0;
}


static HANDLE MySetForegroundWindow(HWND hwnd) {
	DWORD dwProcessId = 0;
	GetWindowThreadProcessId(GetForegroundWindow(), &dwProcessId);

	if (!dwProcessId) return NULL;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (!hProcess) return NULL;

	HMODULE user32 = LoadLibraryW(L"user32.dll");
	if (!user32) {
		CloseHandle(hProcess);
		return NULL;
	}
	LPTHREAD_START_ROUTINE lpSFW = (LPTHREAD_START_ROUTINE)
		GetProcAddress(user32, "SetForegroundWindow");
	if (!lpSFW) {
		FreeLibrary(user32);
		CloseHandle(hProcess);
		return NULL;
	}

	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0,
		lpSFW, (LPVOID)hwnd, CREATE_SUSPENDED, NULL);
	if (hRemoteThread) {
		ResumeThread(hRemoteThread);
	}

	FreeLibrary(user32);
	CloseHandle(hProcess);
	return hRemoteThread;
}
static bool MySetForegroundWindowWrapped(HWND hwnd) {
	HANDLE hThread = MySetForegroundWindow(hwnd);
	if (!hThread) return false;
	CloseHandle(hThread);
	return true;
}


static int __stdcall UiFunc_ExecTip_Worker(CmdLineW& cl);
int __stdcall UiFunc_ExecTip(CmdLineW& cl) {
	if (cl.getopt(L"worker")) {
		return UiFunc_ExecTip_Worker(cl);
	}
	HANDLE hExitEvt = NULL;
	std::wstring sExitEvt;
	if (cl.getopt(L"exit-event", sExitEvt)) {
		hExitEvt = (HANDLE)(LONG_PTR)atoll(ws2s(sExitEvt).c_str());
	}

	HANDLE h_l[2]{};
	DWORD nhCount = (hExitEvt ? 2 : 1);
	h_l[1] = hExitEvt;

	std::wstring cmd = GetCommandLineW();
	cmd += L" --worker";

	{
		STARTUPINFOW si{};
		PROCESS_INFORMATION pi{};

		si.cb = sizeof(si);

		DWORD code = 0;
		PWSTR cl = new WCHAR[cmd.length() + 1];

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
					if (err == 0) err = -1;
					delete[] cl;
					if (act_desk) CloseDesktop(act_desk);
					return err;
				}

				continue;
			}
			CloseHandle(pi.hThread);

			h_l[0] = pi.hProcess;
			Process.resume(pi.hProcess);
			{
				while (WAIT_TIMEOUT ==
					WaitForMultipleObjects(nhCount, h_l, FALSE, 2000)
				) {
					HDESK dsk2 = OpenInputDesktop(0, FALSE, GENERIC_ALL);
					WCHAR name1[256]{}, name2[256]{};
					DWORD nLen = 0;
					if (act_desk) GetUserObjectInformationW(act_desk,
						UOI_NAME, name1, sizeof(name1), &nLen);
					if (dsk2) GetUserObjectInformationW(dsk2,
						UOI_NAME, name2, sizeof(name2), &nLen);
					if (dsk2) CloseDesktop(dsk2);
					if (wcscmp(name1, name2) != 0) {
						// switched desktop, terminate it
						TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
						CloseHandle(pi.hProcess);
						// wait for moment
						Sleep(200);
						// and restart
						goto cp_start;
					}
				}
			}
			GetExitCodeProcess(pi.hProcess, &code);
			if (code == STILL_ACTIVE) {
				TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
			}
			CloseHandle(pi.hProcess);

			delete[] cl;
			cl = nullptr;
			if (act_desk) CloseDesktop(act_desk);
			if (hExitEvt) CloseHandle(hExitEvt);
			return code;

		} while (true);
	}

	if (hExitEvt) CloseHandle(hExitEvt);
	return 0;
}

static int __stdcall UiFunc_ExecTip_Worker(CmdLineW& cl) {
	HDESK hDesk = OpenInputDesktop(0, FALSE, GENERIC_ALL);
	if (hDesk) {
		SetThreadDesktop(hDesk);
		CloseDesktop(hDesk);
	}

	WCHAR className[256]{};
	if (!LoadStringW(hInst, IDS_STRING_UI_CLASS_EXECTIP, className, 256))
		return GetLastError();

	HWND hwnd = CreateWindowExW(0,
		className, L"MyRunProgramWhenComputerUnused",
		0, 0, 0, 1, 1, 0, 0, hInst, &cl);
	if (!hwnd) return GetLastError();

	ShowWindow(hwnd, SW_NORMAL);
	MySetForegroundWindowWrapped(hwnd);
	Sleep(100); MySetForegroundWindowWrapped(hwnd); // just in case

	MSG msg{};
	while (GetMessage(&msg, 0, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
	return 0;
}









BeginWndProc(WndProc_Config)
BeginWndProcSwitchBlock(message)

WndProcSwitchCase(WM_CREATE) {

	break;
}


WndProcSwitchCase(WM_CLOSE) {
	DestroyWindow(hwnd);
	break;
}

WndProcSwitchCase(WM_DESTROY) {
	PostQuitMessage(0);
	break;
}

WndProcSwitchDefault()

EndWndProcSwitchBlock()
WndProcReturnDefault()
EndWndProc()



struct WndData_ExecTip
{
	int code = -1;
	CmdLineW* cl = NULL;

	HWND hCaption = NULL;
	HWND hComboNsOpt = NULL;
	HWND hProgress = NULL;
	HWND hwTimer = NULL;
	std::wstring timer_text;
	WCHAR time_end_text[64]{};
	time_t time_start = NULL;
	time_t time_end = NULL;
	HANDLE hNoShowMap = NULL;
};

static DWORD __stdcall _ExecTip_timerfunc_1(PVOID p) {
	struct info {
		DWORD dwTime;
		HWND hwnd;
	};
	info* i = (info*)p;
	if (!i || !i->dwTime) return -1;
	Sleep(i->dwTime);
	SendMessage(i->hwnd, WM_USER + ERROR_TIMEOUT, 0, 0);
	return 0;
}
static DWORD __stdcall _ExecTip_timerfunc_2(PVOID p) {
	struct info_2 {
		time_t endtime;
		HWND hwnd;
	};
	info_2* i = (info_2*)p;
	if (!i || !i->endtime) return -1;
	while (time(0) < i->endtime) Sleep(1000);
	SendMessage(i->hwnd, WM_USER + ERROR_TIMEOUT, 0, 0);
	return 0;
}

BeginWndProc(WndProc_ExecTip)
WndData_ExecTip* data = (WndData_ExecTip*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
if (message != WM_CREATE && data == NULL)
return WndProcDefault();

BeginWndProcSwitchBlock(message)

WndProcSwitchCase(WM_CREATE) {
	CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
	CmdLineW* cl = reinterpret_cast<CmdLineW*>(pCreate->lpCreateParams);
	if (!cl) return DestroyWindow(hwnd);
	WndData_ExecTip* data = new WndData_ExecTip;
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)data);
	data->cl = cl;

	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE)
		& ~WS_CAPTION & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX & ~WS_POPUP
		| WS_OVERLAPPED | WS_BORDER | WS_SYSMENU);
	SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE)
		& ~WS_EX_APPWINDOW | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED);

	SetLayeredWindowAttributes(hwnd, 0, 0xF0, LWA_ALPHA);

	HMENU sysmenu = GetSystemMenu(hwnd, FALSE);
	EnableMenuItem(sysmenu, SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

	{
		const auto p = ERROR_INVALID_PARAMETER;
		const auto E = [hwnd, &data](int n) {
			data->code = n;
			DestroyWindow(hwnd);
		};
		
#define MYCTLS_VAR_HWND hwnd
#define MYCTLS_VAR_HINST hInst
#define MYCTLS_VAR_HFONT common_font
#include "ctls.h"

		std::wstring
			caption, tip,
			timer, timeout,
			ok, cancel,
			noshow_des,
			closable,
			hNsMap;

		if (1 != cl->getopt(L"data0", caption)) E(p);
		if (0 == cl->getopt(L"data1", tip)) E(p);
		if (0 == cl->getopt(L"data2", timer)) E(p);
		if (0 == cl->getopt(L"data3", timeout)) E(p);
		if (0 == cl->getopt(L"data4", ok)) E(p);
		if (0 == cl->getopt(L"data5", cancel)) E(p);
		if (0 == cl->getopt(L"data6", noshow_des)) E(p);
		if (0 == cl->getopt(L"data7", closable)) E(p);
		if (0 == cl->getopt(L"data8", hNsMap)) E(p);

		if (tip.empty()) tip = L"You haven't used this computer for some time, "
			"so we're about to run the scheduled task.";
		if (timer.empty()) timer = L"Run after %s second(s) (%t)";
		if (ok.empty()) ok = L"Run &now (Enter)";
		if (cancel.empty()) cancel = L"&Cancel this schedule (Esc)";
		if (noshow_des.empty()) noshow_des = L"Do not show again within:";

		str_replace(tip, L"\\n", L"\r\n");


		if (!timeout.empty()) {
			UINT Timeout = (UINT)atoi(ws2s(timeout).c_str());
			if (Timeout) {
				data->time_end = time(0) + (Timeout / 1000);
				struct info {
					DWORD dwTime;
					HWND hwnd;
				};
				info* myinfo = new info;
				myinfo->dwTime = Timeout;
				myinfo->hwnd = hwnd;
				HANDLE hThread = CreateThread(0, 0,
					_ExecTip_timerfunc_1, myinfo, 0, 0);
				if (hThread) CloseHandle(hThread);
				struct info_2 {
					time_t endtime;
					HWND hwnd;
				};
				info_2* myinfo2 = new info_2;
				myinfo2->endtime = data->time_end;
				myinfo2->hwnd = hwnd;
				HANDLE hThread2 = CreateThread(0, 0,
					_ExecTip_timerfunc_2, myinfo2, 0, 0);
				if (hThread2) CloseHandle(hThread2);

				{

					time_t t = data->time_end;
					struct tm info {};
					localtime_s(&info, &t);
					wcsftime(data->time_end_text, 63, L"%Y-%m-%d %X", &info);
				}
				str_replace(timer, L"%t", data->time_end_text);

				data->timer_text = timer;

				str_replace(timer, L"%s", std::to_wstring(Timeout / 1000));
				data->hwTimer = text(timer.c_str(), 10, 370, 620, 20,
					SS_CENTER | SS_CENTERIMAGE);
			} else {
				data->hwTimer = text(L"--", 10, 370, 620, 20,
					SS_CENTER | SS_CENTERIMAGE);
			}

		} else {
			data->hwTimer = text(L"--", 10, 370, 620, 20, SS_CENTER | SS_CENTERIMAGE);
		}
		data->time_start = time(0);

		data->hCaption = text(caption.c_str(), 10, 10, 600, 20,
			SS_CENTER | SS_CENTERIMAGE);

		HWND close = button(L"x", IDCLOSE, 610, 10, 20, 20);
		if (closable == L"false") EnableWindow(close, FALSE);

		edit(tip.c_str(), 10, 40, 620, 300,
			ES_READONLY | 
			ES_MULTILINE | ES_WANTRETURN |
			ES_AUTOVSCROLL | WS_VSCROLL);

		data->hProgress = custom(L"", PROGRESS_CLASSW, 10, 350, 620, 10);
		SendMessage(data->hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 10000));

		button(cancel.c_str(), IDCANCEL, 10, 400, 305, 30, BS_CENTER);
		button(ok.c_str(), IDOK, 325, 400, 305, 30, BS_CENTER);

		/* -- */

		text(noshow_des.c_str(), 10, 445, 200, 25, SS_CENTERIMAGE);
		data->hComboNsOpt = custom(L"", WC_COMBOBOXW, 220, 445, 320, 25, CBS_DROPDOWN);
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"10 seconds");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"30 seconds");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"60 seconds");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"2 minutes");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"5 minutes");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"10 minutes");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"15 minutes");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"30 minutes");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"60 minutes");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"2 hours");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"6 hours");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"12 hours");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"24 hours");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"48 hours");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"3 days");
		SendMessageW(data->hComboNsOpt, CB_ADDSTRING, 0, (LPARAM)L"7 days");
		button(L"Apply", IDABORT, 550, 445, 80, 25, BS_CENTER);

		data->hNoShowMap = (HANDLE)(LONG_PTR)atoll(ws2s(hNsMap).c_str());
			
	}

	SetTimer(hwnd, 0x6ebb708, 500, 0);
	SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 640, 480, SWP_NOMOVE);
	CenterWindow(hwnd);


	break;
}


WndProcSwitchCase(WM_COMMAND) {
	WPARAM wmId = LOWORD(wParam);


	BeginWndProcSwitchBlock(wmId)

		WndProcSwitchCase(IDABORT) {
			HWND lParam = data->hComboNsOpt;
			WCHAR  ListItem[256]{};
			SendMessage((HWND)lParam, (UINT)WM_GETTEXT,
				(WPARAM)256, (LPARAM)ListItem);
			//MessageBox(hwnd, (LPCWSTR)ListItem, TEXT("Item Selected"), MB_OK);

			std::wstring origin, n, u;
			origin = ListItem;
			auto space0 = origin.find(L" ");
			if (space0 == origin.npos) {
				MessageBoxW(hwnd, L"data error", NULL, MB_ICONERROR);
				break;
			}
			n = origin.substr(0, space0 + 1);
			u = origin.substr(space0 + 1);
			//MessageBoxW(hwnd, (L"n=" + n + L" u=" + u).c_str(), 0, 0);
			size_t N = (size_t)atol(ws2s(n).c_str());
			if (N == 0) break;
			size_t scale = 1;
			if (u == L"seconds") scale *= 1;
			else if (u == L"minutes") scale *= 60;
			else if (u == L"hours") scale *= 3600;
			else if (u == L"days") scale *= 86400;
			else {
				MessageBoxW(hwnd, L"Invalid time unit", 0, MB_ICONERROR);
				break;
			}
			size_t $final = N * scale;
			if ($final == 0) {
				MessageBoxW(hwnd, L"Invalid value \"0\"", 0, MB_ICONERROR);
				break;
			}
		
			bool bSuccess = false;
			if (data->hNoShowMap) {
				time_t* tt = (time_t*)MapViewOfFile(data->hNoShowMap,
					FILE_MAP_ALL_ACCESS, 0, 0, sizeof(time_t));
				if (tt) {
					*tt = time(0) + $final;

					UnmapViewOfFile(tt);

					bSuccess = true;
				}
			}

			if (!bSuccess) {
				MessageBoxW(hwnd, L"Failed", 0, MB_ICONERROR);
			}
			else {
				data->code = ERROR_USER_MAPPED_FILE;
				DestroyWindow(hwnd);
			}

			break;
		}

		WndProcSwitchCase(IDCLOSE)
		data->code = ERROR_CANCELLED;
		DestroyWindow(hwnd);
		break;
		WndProcSwitchCase(IDOK)
		data->code = ERROR_SUCCESS;
		DestroyWindow(hwnd);
		break;
		WndProcSwitchCase(IDCANCEL)
		data->code = ERROR_CANCELLED;
		DestroyWindow(hwnd);
		break;

	WndProcSwitchDefault()
	EndWndProcSwitchBlock()
	

	return 0;
}


WndProcSwitchCase(WM_TIMER) {
	BeginWndProcSwitchBlock(wParam)

		WndProcSwitchCase(0x6ebb708) {
			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 1, 1,
				SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
			if (!data->timer_text.empty()) {
				std::wstring tx = data->timer_text;
				time_t left = data->time_end - time(0);
				str_replace(tx, L"%s", std::to_wstring(left));
				SetWindowTextW(data->hwTimer, tx.c_str());

				WPARAM $1 = WPARAM(data->time_end);
				WPARAM $2 = WPARAM(data->time_start);
				WPARAM $3 = $1 - $2;
				WPARAM $4 = WPARAM(time(0));
				WPARAM $5 = $4 - $2;
				double $6 = double($5) / $3;
				WPARAM $final = WPARAM(($6) * 10000);
				SendMessage(data->hProgress, PBM_SETPOS, $final, 0);
			}
		}
		break;

	WndProcSwitchDefault()
	EndWndProcSwitchBlock()
	return 0;
}

WndProcSwitchCase(WM_KEYDOWN) {
	if (wParam == VK_RETURN) return SendMessage(hwnd, WM_COMMAND, IDOK, 0);
	if (wParam == VK_ESCAPE) return SendMessage(hwnd, WM_COMMAND, IDCANCEL, 0);
	return WndProcDefault();
}

WndProcSwitchCase(WM_NCHITTEST) {
	const auto val = WndProcDefault();
	if (val == HTCLIENT) return HTCAPTION;
	return val;
	break;
}

WndProcSwitchCase(WM_SETFOCUS) {
	EnableWindow(data->hCaption, 1);

	return WndProcDefault();
}
WndProcSwitchCase(WM_KILLFOCUS) {
	EnableWindow(data->hCaption, 0);

	return WndProcDefault();
}

WndProcSwitchCase(WM_CLOSE) {
	break;
}

WndProcSwitchCase(WM_USER + ERROR_TIMEOUT) {
	data->code = ERROR_SUCCESS;
	DestroyWindow(hwnd);
	break;
}

WndProcSwitchCase(WM_DESTROY) {
	auto code = data->code;
	if (data->hNoShowMap) CloseHandle(data->hNoShowMap);
	delete data;
	PostQuitMessage(code);
	break;
}

WndProcSwitchDefault()

EndWndProcSwitchBlock()
WndProcReturnDefault()
EndWndProc()









