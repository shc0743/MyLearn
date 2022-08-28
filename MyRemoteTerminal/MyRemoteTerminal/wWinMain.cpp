#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include "../../resource/tool.h"
#include "win32ui.h"
#include "svc.h"
#include "srv.h"
#include "str_table.h"
#include "mycryp.h"
using namespace std;


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")


HINSTANCE hInst;

static int __stdcall srvInstall(CmdLineW& cl);
static int __stdcall srvUnInstall(CmdLineW& cl);

static int __stdcall _wWinMainCallTarget() {
	CmdLineW cl(GetCommandLineW());

	if (cl.getopt(L"type")) {
		wstring t;
		cl.getopt(L"type", t);

		if (t == L"ui") {
			return mWinDispatchUIMain();
		}

		if (t == L"error-control") {
			// TODO: Run error control function
			Sleep(5000);
			return 0;
		}

		if (t == L"update") {
			wstring src, target;
			cl.getopt(L"updateSource", src);
			cl.getopt(L"targetService", target);
			if (src.empty() || target.empty()) return ERROR_INVALID_PARAMETER;

			if (ServiceManager.Stop(ws2s(target))) {
				//return GetLastError() | 0x10000000;
			}
			Sleep(3000);

			SC_HANDLE sch, sc;
			sch = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
			if (!sch) return GetLastError();
			sc = OpenServiceW(sch, target.c_str(), SERVICE_QUERY_CONFIG);
			if (!sc) {
				CloseServiceHandle(sch);
				return GetLastError() | 0x10000000;
			}
			DWORD pcb = 0, cbBufSize = 0;
			LPQUERY_SERVICE_CONFIGW pqsc = NULL;
			CmdLineW scl;
			if (!QueryServiceConfigW(sc, NULL, 0, &pcb)) {
				DWORD dwError = GetLastError();
				if (ERROR_INSUFFICIENT_BUFFER == dwError) {
					cbBufSize = pcb;
					pqsc = (LPQUERY_SERVICE_CONFIGW)calloc(1, cbBufSize);
					if (!pqsc) {
						CloseServiceHandle(sc);
						CloseServiceHandle(sch);
						return GetLastError() | 0x10000000;
					}
				}
				else {
					CloseServiceHandle(sc);
					CloseServiceHandle(sch);
					return ERROR_INTERNAL_ERROR;
				}
			}
			if (!pqsc) {
				CloseServiceHandle(sc);
				CloseServiceHandle(sch);
				return GetLastError() | 0x10000000;
			}
			if (!QueryServiceConfigW(sc, pqsc, cbBufSize, &pcb)) {
				CloseServiceHandle(sc);
				CloseServiceHandle(sch);
				return GetLastError() | 0x10000000;
			}
			CloseServiceHandle(sc);
			CloseServiceHandle(sch);
			scl.parse(pqsc->lpBinaryPathName);
			free(pqsc);
			if (scl.argc() < 1) return ERROR_INVALID_DATA;

			if (file_exists(scl[0]))
			if (!DeleteFileW(scl[0].c_str())) return GetLastError();
			if (!CopyFileW(src.c_str(), scl[0].c_str(), 1)) return GetLastError();

			ServiceManager.Start(ws2s(target));

			return 0;
		}

		if (t == L"test") {

			if (cl.argc() >= 4 && cl[2] == L"sha256") {
				cout << rt_sha256(ws2s(cl[3])) << endl;
			}

			return 0;
		}


	}

	if (cl.getopt(L"service-sub-process")) {
		return rt_svc_ServiceSubProcessHandler(cl);
	}

	if (cl.getopt(L"service")) {

		if (cl.getopt(L"install")) return srvInstall(cl);
		if (cl.getopt(L"uninstall")) return srvUnInstall(cl);

		wstring sname;
		if (cl.getopt(L"name", sname) == 1) {
			MyServiceSetName_svc(sname);
			return MyStartAsServiceW(sname, MyServiceMain_svc) ? 0 : GetLastError();
		}

		return ERROR_INVALID_PARAMETER;
		//return 0;
	}
	
	if (cl.argc() < 2 || cl[1] == L"--ui") {
		return mWinDispatchUIMain(cl);
	}

	return ERROR_INVALID_PARAMETER;
	return 0;
}

int __stdcall srvInstall(CmdLineW& cl) {
	wstring
		svcname,
		dispname,
		svcdesc,
		svccfg,
		instdir,
		httport,
		startype,
		shwndOwner;
	HWND hwndOwner = 0;
	DWORD startup = SERVICE_DEMAND_START;
	WCHAR buffer[2048]{ 0 };
	bool isInterface = cl.getopt(L"interface");
	if (1 != cl.getopt(L"name", svcname)) return ERROR_INVALID_PARAMETER;
	if (1 != cl.getopt(L"port", httport)) return ERROR_INVALID_PARAMETER;
	if (1 == cl.getopt(L"startup", startype)) {
		if (startype == L"auto") startup = SERVICE_AUTO_START;
		if (startype == L"disabled") startup = SERVICE_DISABLED;
	}
	if (1 == cl.getopt(L"hwndOwner", shwndOwner)) {
		hwndOwner = (HWND)(LONG_PTR)atoll(ws2c(shwndOwner));
	}
	if (1 != cl.getopt(L"display-name", dispname)) {
		WCHAR szdisp[1024]{};
		LoadStringW(hInst, IDS_STRING_SVC_DEFAULT_DISPLAY_NAME, szdisp, 1024);
		dispname = szdisp;
		str_replace(dispname, L"%ServiceName%", svcname);
	}
	if (1 != cl.getopt(L"description", svcdesc)) {
		WCHAR desc[1024]{};
		LoadStringW(hInst, IDS_STRING_SVC_DEFAULT_DESCRIPTION, desc, 1024);
		svcdesc = desc;
		str_replace(svcdesc, L"%ServiceName%", svcname);
	}
	if (1 != cl.getopt(L"INSTALLDIR", instdir) || instdir.empty()) {
		ZeroMemory(buffer, sizeof(buffer));
		if (!GetWindowsDirectoryW(buffer, MAX_PATH)) {
			return GetLastError();
		}
		WCHAR uuid[48]{};
		LoadStringW(NULL, IDS_STRING_APP_UUID, uuid, 48);
		instdir = (buffer + L"\\"s + uuid + L"\\"s + svcname);
	}
	if (!file_exists(instdir)) {
		WCHAR oldir[MAX_PATH + 1]{};
		GetCurrentDirectoryW(MAX_PATH, oldir);
		vector<wstring> paths;
		str_replace(instdir, L"/", L"\\");
		str_split(instdir, L"\\", paths);
		for (auto i : paths) {
			i += L"\\";
			if (!SetCurrentDirectoryW(i.c_str())) {
				if (!(CreateDirectoryW(i.c_str(), NULL) &&
					SetCurrentDirectoryW(i.c_str()))) {
					break;
				}
			}
		}
		SetCurrentDirectoryW(oldir);
		if (!file_exists(instdir)) {
			if (isInterface) {
				MessageBoxW(NULL, LastErrorStrW().c_str(), NULL, MB_ICONHAND);
			}
			return GetLastError();
		}
	}
	else {
		instdir = instdir + L"\\" + svcname + L"\\";
		if (!CreateDirectoryW(instdir.c_str(), NULL)) {
			if (isInterface) {
				MessageBoxW(NULL, LastErrorStrW().c_str(), NULL, MB_ICONHAND);
			}
			return GetLastError();
		}
	}
	bool bFlagCreateConfig = bool(cl.getopt(L"create-config"));
	if (1 != cl.getopt(L"config", svccfg)) {
		svccfg = instdir + (L"\\config.cfg");
		bFlagCreateConfig = true;
	}
	if (bFlagCreateConfig) {
		HANDLE hFile = CreateFileW(svccfg.c_str(), FILE_ALL_ACCESS, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) {
			if (isInterface) {
				MessageBoxW(NULL, LastErrorStrW().c_str(), NULL, MB_ICONHAND);
			}
			return -1;
		}

		DWORD temp = 0;
		MyServiceConfig_svc cfg;
		cfg.server_port = (int)atoi(ws2s(httport).c_str());
		WriteFile(hFile, &cfg, sizeof(cfg), &temp, NULL);

		CloseHandle(hFile);
	}

	WCHAR szDependencies[1024]{};
	LoadStringW(hInst, IDS_STRING_SVC_DEPENDENCIES, szDependencies, 1023);

	wstring pd = GetProgramDirW(), ph;
	try {
		ph = (instdir + L"\\" + pd.substr(pd.find_last_of(L"\\") + 1));
		if (!CopyFileW(pd.c_str(), ph.c_str(), TRUE)) {
			DeleteFileW(ph.c_str());
			if (!CopyFileW(pd.c_str(), ph.c_str(), TRUE))
			throw nullptr;
		}
	}
	catch (...) {
		if (isInterface) {
			MessageBoxW(NULL, LastErrorStrW().c_str(),
				L"Failed to CopyFileW", MB_ICONHAND);
		}
		return -1;
	}

	wstring cmd;
	cmd = L"\"" + ph + L"\" --service --type=service "
		"--name=\"" + svcname + L"\" --config=\"" + svccfg + L"\" ";

	int result = -1;

	do {
		SC_HANDLE sch = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!sch) break;

		DWORD err = 0;
		SC_HANDLE svc = CreateServiceW(sch, svcname.c_str(), dispname.c_str(),
			SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, startup,
			SERVICE_ERROR_SEVERE, cmd.c_str(),
			NULL, NULL, szDependencies, NULL, NULL);
		if (!svc) {
			err = GetLastError();
			CloseServiceHandle(sch);
			SetLastError(err);
			result = err;
			break;
		}

		SERVICE_DESCRIPTIONW a{};
		LPWSTR des2 = (LPWSTR)calloc(sizeof(WCHAR), svcdesc.length() + 1);
		if (des2) {
			wcscpy_s(des2, svcdesc.length() + 1, svcdesc.c_str());
			a.lpDescription = des2;
			ChangeServiceConfig2W(svc, SERVICE_CONFIG_DESCRIPTION, &a);
			free(des2);
		}

		wstring lpc;
		lpc = L"\"" + ph + L"\" --type=error-control "
			"--service=\"" + svcname + L"\" --failure --recovery "
			"--failure-count=%1%";
		PWSTR pwc = NULL;
		pwc = (PWSTR)calloc(2, lpc.length() + 1);
		if (pwc) {
			wcscpy_s(pwc, lpc.length() + 1, lpc.c_str());
			SERVICE_FAILURE_ACTIONSW sa{};
			SC_ACTION san[5]{};
			sa.dwResetPeriod = 120;
			sa.lpRebootMsg = NULL;
			sa.lpCommand = pwc;
			sa.cActions = 5;
			sa.lpsaActions = san;
			san[0].Type = SC_ACTION_RESTART;
			san[0].Delay = 1000;
			san[1].Type = SC_ACTION_RESTART;
			san[1].Delay = 2000;
			san[2].Type = SC_ACTION_RESTART;
			san[2].Delay = 5000;
			san[3].Type = SC_ACTION_RUN_COMMAND;
			san[3].Delay = 1;
			san[4].Type = SC_ACTION_NONE;
			san[4].Delay = 0;
			ChangeServiceConfig2W(svc, SERVICE_CONFIG_FAILURE_ACTIONS, &sa);

			free(pwc);
		}

		CloseServiceHandle(svc);
		CloseServiceHandle(sch);
		result = 0;
	} while (0);

	/* Uninstaller */ do {
		HKEY hkey = NULL;
		wstring keyname = L"SOFTWARE\\Microsoft\\Windows\\"
			"CurrentVersion\\Uninstall\\" + svcname;
		LSTATUS stat = 0;
		if (ERROR_SUCCESS == RegOpenKeyExW(
			HKEY_LOCAL_MACHINE, keyname.c_str(), 0, KEY_READ, &hkey
		)) {
			RegCloseKey(hkey);
		}
		else {
			if (stat = RegCreateKeyExW(HKEY_LOCAL_MACHINE, keyname.c_str(),
				0, 0, 0, KEY_ALL_ACCESS, NULL, &hkey, NULL)) {
				// An error occurred...
				break;
			}
			else {
				RegSetKeyValueW(hkey, L"", L"DisplayName", REG_SZ,
					dispname.c_str(), (DWORD)dispname.length() * 2);

				wstring data_buffer = L"\"" + ph + L"\" --"
					"service --uninstall --name=\"" + svcname + L"\" --interface";
				RegSetKeyValueW(hkey, L"", L"UninstallString", REG_SZ,
					data_buffer.c_str(), (DWORD)data_buffer.length() * 2 + 2);

				DWORD data_buffer_2 = 1;
				RegSetKeyValueW(hkey, L"", L"NoModify", REG_DWORD,
					&data_buffer_2, sizeof(DWORD));
				RegSetKeyValueW(hkey, L"", L"NoRepair", REG_DWORD,
					&data_buffer_2, sizeof(DWORD));

				RegCloseKey(hkey);
			}
		}
	} while (0);

	if (result == 0 && cl.getopt(L"start")) {
		ServiceManager.Start(ws2s(svcname));
	}

	if (isInterface) {
		TaskDialog(hwndOwner, NULL, L"Install - My Remote Terminal",
			LastErrorStrW().c_str(), (result == 0) ?
			L"You should click \"Activate\" to activate your product." :
			(L"GetLastError() returned " + to_wstring(GetLastError()))
			.c_str(), TDCBF_CLOSE_BUTTON, (result == 0) ?
			TD_INFORMATION_ICON : TD_ERROR_ICON, NULL);
	}

	return result;
}
#if 0
extern "C" __kernel_entry NTSYSCALLAPI NTSTATUS NtQueryInformationFile(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	FILE_INFORMATION_CLASS FileInformationClass
);
#endif
int __stdcall srvUnInstall(CmdLineW& cl) {
	wstring name;
	if (1 != cl.getopt(L"name", name)) return ERROR_INVALID_PARAMETER;

	if (cl.getopt(L"interface") && cl.getopt(L"no-interface") == 0) {
		int btn = 0; HWND hwnd = NULL;
		TaskDialog(hwnd, NULL, L"Uninstall - My Remote Terminal",
			L"Are you sure you want to delete the following service?",
			name.c_str(), TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON,
			NULL, &btn);
		if (btn != IDYES) return ERROR_CANCELLED;

		if (!IsRunAsAdmin()) {
			TaskDialog(hwnd, NULL, L"Uninstall - My Remote Terminal",
				L"Administrator privileges are required to continue.",
				L"Do you want to elevate?",
				TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON,
				TD_SHIELD_ICON, &btn);
			if (btn != IDOK) return ERROR_CANCELLED;
		}

		SHELLEXECUTEINFO execinfo;
		memset(&execinfo, 0, sizeof(execinfo));
		std::wstring pd = s2ws(GetProgramDir());
		std::wstring ct = GetCommandLineW() + L" --no-interface"s;
		execinfo.lpFile = pd.c_str();
		execinfo.cbSize = sizeof(execinfo);
		execinfo.lpVerb = (IsRunAsAdmin() ? L"open" : L"runas");
		execinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		execinfo.nShow = SW_NORMAL;
		execinfo.lpParameters = ct.c_str();

		if (!ShellExecuteExW(&execinfo)) {
			MessageBoxW(hwnd, LastErrorStrW().c_str(), 0, MB_ICONHAND);
			return GetLastError();
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


		return exitCode;
	}

	DWORD code = 0;
#define ECTLD(n) {code = GetLastError(); goto clean##n; }

	SC_HANDLE sm, sc;
	sm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!sm) ECTLD(1);
	sc = OpenServiceW(sm, name.c_str(), SERVICE_ALL_ACCESS);
	if (!sc) ECTLD(2);
	if (cl.getopt(L"uninst-temp")) {
		Sleep(5000);
	}
	{
		DWORD pcb = 0, cbBufSize = 0;
		LPQUERY_SERVICE_CONFIGW pqsc = NULL;
		CmdLineW scl;
		if (!QueryServiceConfigW(sc, NULL, 0, &pcb)) {
			DWORD dwError = GetLastError();
			if (ERROR_INSUFFICIENT_BUFFER == dwError) {
				cbBufSize = pcb;
				pqsc = (LPQUERY_SERVICE_CONFIGW)calloc(1, cbBufSize);
				if (!pqsc) ECTLD(3);
			}
			else ECTLD(3);
		}
		if (!pqsc) ECTLD(3);
		if (!QueryServiceConfigW(sc, pqsc, cbBufSize, &pcb)) ECTLD(3);
		scl.parse(pqsc->lpBinaryPathName);
		if (scl.size() > 0) do {
			{
#if 0
				HANDLE hFile = CreateFileW(scl[0].c_str(), GENERIC_READ,
					FILE_SHARE_READ, NULL, OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);
				if (!hFile) break;
				const UINT size =
					sizeof(WCHAR) * (MAX_PATH + 1) +
					sizeof(FILE_NAME_INFO);
				FILE_NAME_INFO* fni = (FILE_NAME_INFO*)calloc(1, size);
				if (GetFileInformationByHandleEx(hFile, FileNameInfo, fni,
					size)) {
					CloseHandle(hFile);
					WCHAR wc[MAX_PATH + 10]{};
					memcpy_s(wc, MAX_PATH + 9, fni->FileName, fni->FileNameLength);
					wstring str = wc;
					MessageBoxW(0, wc, 0, 0);
					free(fni);

					if (str != GetProgramDirW()) {
						break;
					}
				}
				else {
					CloseHandle(hFile);
					free(fni);
					break;
				}
#else
				STARTUPINFOW si{};
				PROCESS_INFORMATION p{};

				WCHAR s[1]{};
				if (!CreateProcessW(scl[0].c_str(), s, 0, 0, 0,
					CREATE_SUSPENDED, 0, 0, &si, &p)) break;
				CloseHandle(p.hThread);
				WCHAR buffer[2048] = { 0 }; DWORD size = 2047;
				QueryFullProcessImageNameW(p.hProcess, 0, buffer, &size);
				TerminateProcess(p.hProcess, 0);
				CloseHandle(p.hProcess);

				if (GetProgramDirW() != buffer) break;
#endif
			}

			WCHAR temp[MAX_PATH + 16]{};
			if (!GetTempPathW(MAX_PATH, temp)) {
				return GetLastError();
			}
			wstring newPath = temp + L"\\__uninst-"s + name + L"-temp.exe";
			if (GetProgramDirW() == newPath) {
				return ERROR_INTERNAL_ERROR;
			}
			if (!CopyFileW(GetProgramDirW().c_str(), newPath.c_str(), TRUE)) {
				DeleteFileW(newPath.c_str());
				if (!CopyFileW(GetProgramDirW().c_str(), newPath.c_str(), TRUE))
				return GetLastError();
			}
			wstring cl = L"\"" + newPath + L"\" " + GetCommandLineW()
				+ L" --uninst-temp --delete-self";
			if (!Process.StartOnly(cl)) {
				return GetLastError();
			}
			ExitProcess(ERROR_RESTART_APPLICATION);
		} while (0);
		ServiceManager.Stop(ws2s(name));
		Sleep(3000);
		if (!DeleteService(sc)) {
			if (cbBufSize) free(pqsc);
			ECTLD(3);
		}
		if (!cl.getopt(L"no-delete-config")) {
			wstring cfg_file;
			if (scl.getopt(L"config", cfg_file) == 1) {
				DeleteFileW(cfg_file.c_str());
			}
		}
		if (!cl.getopt(L"no-delete-data")) {
			using namespace std;
			wstring cfg_file = scl[0];
			try {
				wstring szPath = cfg_file.substr(0, cfg_file.find_last_of(L"\\"));
				FileDeleteTreeW(szPath);
				RemoveDirectoryW(szPath.c_str());
			}
			catch (...) {}
		}
		if (true) {
			using namespace std;
			WCHAR szdisp[1024]{};
			LoadStringW(NULL, IDS_STRING_SVC_DEFAULT_DISPLAY_NAME, szdisp, 1024);
			wstring sname = szdisp;
			str_replace(sname, L"%ServiceName%", name);

			auto pi = Process.Start_HiddenWindow(to__str(
				L"netsh advfirewall firewall delete rule name=\"" + sname + L"\""));
			if (pi.hProcess) {
				CloseHandle(pi.hThread);
				if (WaitForSingleObject(pi.hProcess, 3000) == WAIT_TIMEOUT) {
					s7::CallNtTerminateProcess(pi.hProcess, ERROR_TIMEOUT);
				}
				CloseHandle(pi.hProcess);
			}

			RegDeleteTreeW(HKEY_LOCAL_MACHINE, (L"SOFTWARE\\Microsoft\\"
				"Windows\\CurrentVersion\\Uninstall\\" + name).c_str());
		}
		if (cbBufSize) free(pqsc);
	}

clean3:
	CloseServiceHandle(sc);
clean2:
	CloseServiceHandle(sm);
clean1:

	if (cl.getopt(L"delete-self")) {
		WCHAR temp[MAX_PATH + 16]{};
		if (!GetTempPathW(MAX_PATH, temp)) {
			return GetLastError();
		}
		wstring newPath = temp + L"\\__temp-uninst-"s + name + L"-delself.bat";
		fstream fp(ws2s(newPath), ios::out);
		if (fp) {
			fp << "@echo off" << endl <<
				":a\ntimeout 1\ndel /f /s /q \"" << GetProgramDir() <<
				"\"\nif exist \"" << GetProgramDir() << "\" goto a" <<
				endl << "\ndel /f /s /q \"%~0\"\n";
			fp.close();
		}
		Process.
#if 0
			StartOnly
#else
			StartOnly_HiddenWindow
#endif
		(L"cmd.exe /c \"" + newPath + L"\"");
	}

	return (int)code;

#undef ECTLD
}






/*  - SPLIT -  */

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	::hInst = hInstance;

#if 0
	WCHAR TempPath[1024]{};
	WCHAR TempPathName[1024]{};
	if (!GetTempPathW(1023, TempPath)) {
		return GetLastError();
	}
	wcscat_s(TempPath, (L"\\.p"s + to_wstring(GetCurrentProcessId())
		+ L"_temp\\").c_str());
	if (!CreateDirectoryW(TempPath, NULL)) {
		return GetLastError();
	}
	SetFileAttributesW(TempPath, FILE_ATTRIBUTE_HIDDEN);
	SetCurrentDirectoryW(TempPath);
	wcscat_s(TempPathName, TempPath);
#endif

	int _result = _wWinMainCallTarget();

#if 0
	SetCurrentDirectoryW(L"..");
	FileDeleteTreeW(TempPath);
#endif

	return _result;
}



#if 0
typedef enum _FILE_INFORMATION_CLASS {
	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,                   // 2
	FileBothDirectoryInformation,                   // 3
	FileBasicInformation,                           // 4
	FileStandardInformation,                        // 5
	FileInternalInformation,                        // 6
	FileEaInformation,                              // 7
	FileAccessInformation,                          // 8
	FileNameInformation,                            // 9
	FileRenameInformation,                          // 10
	FileLinkInformation,                            // 11
	FileNamesInformation,                           // 12
	FileDispositionInformation,                     // 13
	FilePositionInformation,                        // 14
	FileFullEaInformation,                          // 15
	FileModeInformation,                            // 16
	FileAlignmentInformation,                       // 17
	FileAllInformation,                             // 18
	FileAllocationInformation,                      // 19
	FileEndOfFileInformation,                       // 20
	FileAlternateNameInformation,                   // 21
	FileStreamInformation,                          // 22
	FilePipeInformation,                            // 23
	FilePipeLocalInformation,                       // 24
	FilePipeRemoteInformation,                      // 25
	FileMailslotQueryInformation,                   // 26
	FileMailslotSetInformation,                     // 27
	FileCompressionInformation,                     // 28
	FileObjectIdInformation,                        // 29
	FileCompletionInformation,                      // 30
	FileMoveClusterInformation,                     // 31
	FileQuotaInformation,                           // 32
	FileReparsePointInformation,                    // 33
	FileNetworkOpenInformation,                     // 34
	FileAttributeTagInformation,                    // 35
	FileTrackingInformation,                        // 36
	FileIdBothDirectoryInformation,                 // 37
	FileIdFullDirectoryInformation,                 // 38
	FileValidDataLengthInformation,                 // 39
	FileShortNameInformation,                       // 40
	FileIoCompletionNotificationInformation,        // 41
	FileIoStatusBlockRangeInformation,              // 42
	FileIoPriorityHintInformation,                  // 43
	FileSfioReserveInformation,                     // 44
	FileSfioVolumeInformation,                      // 45
	FileHardLinkInformation,                        // 46
	FileProcessIdsUsingFileInformation,             // 47
	FileNormalizedNameInformation,                  // 48
	FileNetworkPhysicalNameInformation,             // 49
	FileIdGlobalTxDirectoryInformation,             // 50
	FileIsRemoteDeviceInformation,                  // 51
	FileUnusedInformation,                          // 52
	FileNumaNodeInformation,                        // 53
	FileStandardLinkInformation,                    // 54
	FileRemoteProtocolInformation,                  // 55

	//
	//  These are special versions of these operations (defined earlier)
	//  which can be used by kernel mode drivers only to bypass security
	//  access checks for Rename and HardLink operations.  These operations
	//  are only recognized by the IOManager, a file system should never
	//  receive these.
	//

	FileRenameInformationBypassAccessCheck,         // 56
	FileLinkInformationBypassAccessCheck,           // 57

	//
	// End of special information classes reserved for IOManager.
	//

	FileVolumeNameInformation,                      // 58
	FileIdInformation,                              // 59
	FileIdExtdDirectoryInformation,                 // 60
	FileReplaceCompletionInformation,               // 61
	FileHardLinkFullIdInformation,                  // 62
	FileIdExtdBothDirectoryInformation,             // 63
	FileDispositionInformationEx,                   // 64
	FileRenameInformationEx,                        // 65
	FileRenameInformationExBypassAccessCheck,       // 66
	FileDesiredStorageClassInformation,             // 67
	FileStatInformation,                            // 68
	FileMemoryPartitionInformation,                 // 69
	FileStatLxInformation,                          // 70
	FileCaseSensitiveInformation,                   // 71
	FileLinkInformationEx,                          // 72
	FileLinkInformationExBypassAccessCheck,         // 73
	FileStorageReserveIdInformation,                // 74
	FileCaseSensitiveInformationForceAccessCheck,   // 75
	FileKnownFolderInformation,   // 76

	FileMaximumInformation
} FILE_INFORMATION_CLASS, * PFILE_INFORMATION_CLASS;
typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID    Pointer;
	};
	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;
#endif




