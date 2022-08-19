#include <Windows.h>
#include <ShlObj.h>
#include <Shlwapi.h>
#include "../../resource/tool.h"
#include "win32ui.h"
#include "svc.h"
#include "srv.h"
#include "str_table.h"
using namespace std;


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Shlwapi.lib")


HINSTANCE hInst;

static int __stdcall _wWinMainCallTarget() {
	CmdLineW cl(GetCommandLineW());

	if (cl.getopt(L"type")) {
		wstring t;
		cl.getopt(L"type", t);

		if (t == L"ui") {
			return mWinDispatchUIMain();
		}

		if (t == L"service-sub-process") {
			return rt_svc_ServiceSubProcessHandler(cl);
		}

		if (t == L"error-control") {
			// TODO: Run error control function
			Sleep(5000);
			return 0;
		}


	}

	if (cl.getopt(L"service")) {

		if (cl.getopt(L"install")) {
			wstring
				svcname,
				dispname,
				svcdesc,
				svccfg,
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
			bool bFlagCreateConfig = bool(cl.getopt(L"create-config"));
			if (1 != cl.getopt(L"config", svccfg)) {
				ZeroMemory(buffer, sizeof(buffer));
				if (!GetWindowsDirectoryW(buffer, MAX_PATH)) {
					return GetLastError();
				}
				WCHAR uuid[48]{};
				LoadStringW(NULL, IDS_STRING_APP_UUID, uuid, 48);

				wstring szPath = (buffer + L"\\"s + uuid);

				if (!file_exists(szPath)) CreateDirectoryW(szPath.c_str(), 0);
				szPath += L"\\"s + svcname;
				if (!file_exists(szPath)) CreateDirectoryW(szPath.c_str(), 0);

				svccfg = szPath + (L"\\config.cfg");
				bFlagCreateConfig = true;
			}
			if (bFlagCreateConfig) {
				HANDLE hFile = CreateFileW(svccfg.c_str(), FILE_ALL_ACCESS, 0, 0,
					CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFile == NULL || hFile == INVALID_HANDLE_VALUE) {
					return GetLastError();
				}

				DWORD temp = 0;
				MyServiceConfig_svc cfg;
				cfg.server_port = (int)atoi(ws2s(httport).c_str());
				WriteFile(hFile, &cfg, sizeof(cfg), &temp, NULL);

				CloseHandle(hFile);
			}

			WCHAR szDependencies[1024]{};
			LoadStringW(hInst, IDS_STRING_SVC_DEPENDENCIES, szDependencies, 1023);

			wstring cmd;
			cmd = L"\"" + s2ws(GetProgramDir()) + L"\" --service --type=service "
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
				lpc = L"\"" + s2ws(GetProgramDir()) + L"\" --type=error-control "
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

			if (result == 0 && cl.getopt(L"start")) {
				ServiceManager.Start(ws2s(svcname));
			}

			if (isInterface) {
				TaskDialog(hwndOwner, NULL, L"Install - My Remote Terminal",
					LastErrorStrW().c_str(), (result == 0) ?
					L"The operation completed successfully." :
					(L"GetLastError() returned " + to_wstring(GetLastError())).c_str(),
					TDCBF_CLOSE_BUTTON, (result == 0) ?
					TD_INFORMATION_ICON : TD_ERROR_ICON, NULL);
			}

			return result;
		}

		if (cl.getopt(L"uninstall")) {
			wstring name;
			if (1 != cl.getopt(L"name", name)) return ERROR_INVALID_PARAMETER;

			DWORD code = 0;
#define ECTLD(n) {code = GetLastError(); goto clean##n; }

			SC_HANDLE sm, sc;
			sm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
			if (!sm) ECTLD(1);
			sc = OpenServiceW(sm, name.c_str(), SERVICE_ALL_ACCESS);
			if (!sc) ECTLD(2);
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
				SERVICE_STATUS aaa{};
				ControlService(sc, SERVICE_STOP, &aaa);
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
					WCHAR Path[1024]{};
					if (GetWindowsDirectoryW(Path, 1023)) {
						WCHAR uuid[48]{};
						LoadStringW(NULL, IDS_STRING_APP_UUID, uuid, 48);

						wstring szPath = (Path + L"\\"s + uuid);

						if (!file_exists(szPath)) CreateDirectoryW(szPath.c_str(), 0);

						FileDeleteTreeW(szPath + L"\\"s + name);
						RemoveDirectoryW(szPath.c_str());
					}

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

				}
				if (cbBufSize) free(pqsc);
			}

		clean3:
			CloseServiceHandle(sc);
		clean2:
			CloseServiceHandle(sm);
		clean1:
				
			return (int)code;

#undef ECTLD
		}

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



