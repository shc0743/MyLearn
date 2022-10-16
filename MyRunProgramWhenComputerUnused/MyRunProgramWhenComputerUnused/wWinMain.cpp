// Entry point of this application.
//


// Begin System Headers
#include <Windows.h>
// End System Headers

// Begin Tool Headers
#include "../../resource/tool.h"
// End Tool Headers

// Begin Project Headers
#include "ui.h"
#include "svc.h"
// End Project Headers



/* -------- Global variables -------- */
HINSTANCE hInst;



/* -------- Functions -------- */
static int SvcInstall(CmdLineW& cl);


/* -------- Entry Point -------- */

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	::hInst = hInstance;

	CmdLineW cl(GetCommandLineW());

	std::wstring type;
	if (cl.getopt(L"type", type)) {

		if (type == L"service") {
			std::wstring name, cfg;
			cl.getopt(L"name", name);
			cl.getopt(L"config", cfg);
			MyServiceSetName_svc(name);
			MyServiceSetConfig_svc(cfg);

			return MyStartAsServiceW(name, MyServiceMain_svc);
		}

		if (type == L"service-main") {
			return MyServiceChildMain_svc(cl);
		}
		if (type == L"service-child") {
			return MyServiceChildMain_svc(cl);
		}

		if (type == L"ui") {
			std::wstring ui_type;
			if (1 != cl.getopt(L"ui-type", ui_type)) return ERROR_INVALID_PARAMETER;

			if (0 != InitializeApplicationUserInterface()) {
				return GetLastError();
			}

			if (ui_type == L"exectip") {
				return UiFunc_ExecTip(cl);
			}

			if (ui_type == L"config") {
				return UiFunc_Config(cl);
			}

			return ERROR_INVALID_PARAMETER;
		}
		
		if (type == L"error-control") {
			// TODO
			Sleep(5000);
			return 0;
		}



		return ERROR_INVALID_PARAMETER;
	}

	if (cl.getopt(L"install-service")) {
		return SvcInstall(cl);
	}


	if (cl.argc() < 2) {
		auto pi = Process.Start_Suspended(L"\"" + (GetProgramDirW()) +
			L"\" --type=ui --ui-type=config ");
		if (!pi.hProcess) return GetLastError();

		DWORD dwExitCode = 0;
		CloseHandle(pi.hThread);
		Process.resume(pi.hProcess);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &dwExitCode);
		CloseHandle(pi.hProcess);
		return (int)dwExitCode;
	}


	return ERROR_INVALID_PARAMETER;
	return 0;
}


	
static int SvcInstall(CmdLineW& cl) {
	using namespace std;

	std::wstring svcname, cf, sta, svcdesc;
	cl.getopt(L"service-name", svcname);
	cl.getopt(L"config", cf);
	if (svcname.empty() || cf.empty()) return ERROR_INVALID_PARAMETER;
	bool interf = cl.getopt(L"interface");
	cl.getopt(L"startup", sta);
	DWORD startup = (
		sta == L"auto" ? SERVICE_AUTO_START : (
			sta == L"disabled" ? SERVICE_DISABLED : (
				SERVICE_DEMAND_START
	)));
	if (cl.getopt(L"description", svcdesc) != 1) {
		svcdesc = L"Run Program when Computer is unused";
	}

	std::wstring cmd = L"\"" + GetProgramDirW() + L"\" --type=service --name=\""
		+ svcname + L"\" --config=\"" + cf + L"\"";

	int result = -1;

	do {
		SC_HANDLE sch = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!sch) break;

		DWORD err = 0;
		SC_HANDLE svc = CreateServiceW(sch, svcname.c_str(), svcname.c_str(),
			SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, startup,
			SERVICE_ERROR_SEVERE, cmd.c_str(),
			NULL, NULL, NULL, NULL, NULL);
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
		lpc = L"\"" + GetProgramDirW() + L"\" --type=error-control "
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

		StartServiceW(svc, 0, NULL);

		CloseServiceHandle(svc);
		CloseServiceHandle(sch);
		//result = 0;
		result = 0;
	} while (0);

	if (interf) {
		MessageBoxW(0, ErrorCodeToStringW(result).c_str(), L"Result",
			result == 0 ? MB_ICONINFORMATION : MB_ICONERROR
		);
	}

	return result;
}





