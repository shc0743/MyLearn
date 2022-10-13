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


/* -------- Entry Point -------- */

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
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

			if (ui_type == L"config") {
				return UiFunc_Config(cl);
			}

			if (ui_type == L"exectip") {
				return UiFunc_ExecTip(cl);
			}

			return ERROR_INVALID_PARAMETER;
		}



		return ERROR_INVALID_PARAMETER;
	}

	if (cl.getopt(L"install-service")) {
		std::wstring sn, cf;
		cl.getopt(L"service-name", sn);
		cl.getopt(L"config", cf);
		if (sn.empty() || cf.empty()) return ERROR_INVALID_PARAMETER;
		bool interf = cl.getopt(L"interface");

		std::wstring c = L"\"" + GetProgramDirW() + L"\" --type=service --name=\""
			+ sn + L"\" --config=\"" + cf + L"\"";
		auto r = ServiceManager.New(sn, c, ServiceManager_::Manual, sn,
			L"Run Program when Computer is unused", SERVICE_WIN32_OWN_PROCESS);
		if (r) {
			if (interf) {
				MessageBoxW(0, LastErrorStrW().c_str(), 0, MB_ICONHAND);
			}
		}
		ServiceManager.Start(ws2s(sn));

		MessageBoxW(0, LastErrorStrW().c_str(), L"Result", MB_ICONINFORMATION);
		return r;
		return 0;
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



