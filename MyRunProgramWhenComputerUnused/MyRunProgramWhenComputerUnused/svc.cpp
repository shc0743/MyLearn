#include "svc.h"
#include "util.h"





static SERVICE_STATUS          gSvcStatus;
static SERVICE_STATUS_HANDLE   gSvcStatusHandle;
static std::wstring            gSvcName;
static MyServiceConfig_svc*    gSvcConfig;
static std::wstring            gSvcConfigFile;
static HANDLE                  gSvcProcess;
static HANDLE                  gSvcThread;
static HANDLE                  gSvcExitEvent;
static HANDLE                  gSvcChildEvt$2;

static VOID WINAPI    SvcCtrlHandler(DWORD);
static bool __stdcall UpdateSvcStatus();
static void __stdcall SvcInit(DWORD, LPTSTR*);

static DWORD WINAPI SvcHandle_Stop(PVOID);
static DWORD WINAPI SvcHandle_Pause(PVOID);
static DWORD WINAPI SvcHandle_Continue(PVOID);

static DWORD WINAPI svc_main(PVOID);



void MyServiceSetName_svc(std::wstring name) {
	gSvcName = name;
}
void MyServiceSetConfig_svc(std::wstring cfg) {
	gSvcConfigFile = cfg;
}

static DWORD __stdcall SvcHandle_Start(PVOID) {
	// Perform service-specific initialization and work.
	using namespace std;

	auto report_last_error = [](DWORD err = -1) {
		if (err == -1) err = GetLastError();

		gSvcStatus.dwCurrentState = SERVICE_STOPPED;
		gSvcStatus.dwWin32ExitCode = err;
		gSvcStatus.dwWaitHint = 0;
		gSvcStatus.dwCheckPoint = 0;
		UpdateSvcStatus();
		ExitProcess(err);
#pragma warning(push)
#pragma warning(disable: 4457)
		return err;
#pragma warning(pop)
	};

	//constexpr unsigned char max_retry_count = 3;

	{
		wstring szPath = GetProgramDirW();
		szPath = szPath.substr(0, szPath.find_last_of(L"\\"));
		if (!SetCurrentDirectoryW(szPath.c_str())) {
			return report_last_error();
		}
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	SECURITY_ATTRIBUTES sa{};
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	gSvcExitEvent = CreateEvent(&sa, TRUE, FALSE, NULL);
	if (!gSvcExitEvent) {
		return report_last_error();
	}
	gSvcChildEvt$2 = CreateEvent(&sa, TRUE, FALSE, NULL);
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	EnableAllPrivileges();
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	gSvcThread = CreateThread(0, 0, svc_main, 0, 0, 0);
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();


	gSvcStatus.dwCurrentState = SERVICE_RUNNING;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwWaitHint = 0;
	gSvcStatus.dwCheckPoint = 0;
	UpdateSvcStatus();

	return 0;

}

void __stdcall MyServiceMain_svc(DWORD, LPWSTR*) {
	// Register the handler function for the service

	gSvcStatusHandle = RegisterServiceCtrlHandlerW(
		gSvcName.c_str(), SvcCtrlHandler);

	if (!gSvcStatusHandle) {
		return;
	}

	// These SERVICE_STATUS members remain as set here

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;
	gSvcStatus.dwControlsAccepted =
		SERVICE_ACCEPT_STOP |
		SERVICE_ACCEPT_PAUSE_CONTINUE |
		SERVICE_ACCEPT_SHUTDOWN;

	// Report initial status to the SCM

	gSvcStatus.dwCurrentState = SERVICE_START_PENDING;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwWaitHint = 30000;
	gSvcStatus.dwCheckPoint = 0;
	UpdateSvcStatus();

	if (HANDLE _ = CreateThread(0, 0, SvcHandle_Start, 0, 0, 0))
		CloseHandle(_);

}

bool __stdcall UpdateSvcStatus() {
	// Report the status of the service to the SCM.
	return SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code. 

	switch (dwCtrl)
	{
	case SERVICE_CONTROL_SHUTDOWN: {
		/* 
		[Fatal bug]:
		Exit the service directly will cause childs always restart
		and the computer couldn't shutdown.
		[Solution]:
		Just exit the service normally.
		*/
#if 0
		gSvcStatus.dwCurrentState = SERVICE_STOPPED;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 0;
		//UpdateSvcStatus();
		break;
#else
		gSvcStatus.dwControlsAccepted = 0;
		gSvcStatus.dwCurrentState = SERVICE_STOP_PENDING;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 4096;
		UpdateSvcStatus();

		if (HANDLE h = CreateThread(0, 0, SvcHandle_Stop,
			(PVOID)ERROR_SHUTDOWN_IN_PROGRESS, 0, 0))
			CloseHandle(h);

		gSvcStatus.dwCheckPoint ++;
		break;
#endif
	}

	case SERVICE_CONTROL_STOP:
		gSvcStatus.dwControlsAccepted = 0;
		gSvcStatus.dwCurrentState = SERVICE_STOP_PENDING;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 8192;
		UpdateSvcStatus();

		if (HANDLE h = CreateThread(0, 0, SvcHandle_Stop, 0, 0, 0))
			CloseHandle(h);

		gSvcStatus.dwCheckPoint ++;
		break;

	case SERVICE_CONTROL_PAUSE:
		if (gSvcStatus.dwCurrentState != SERVICE_RUNNING) {
			gSvcStatus.dwWin32ExitCode = ERROR_SERVICE_CANNOT_ACCEPT_CTRL;
			UpdateSvcStatus();
			return;
		}

		gSvcStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 8192;

		if (HANDLE h = CreateThread(0, 0, SvcHandle_Pause, 0, 0, 0))
			CloseHandle(h);
		break;

	case SERVICE_CONTROL_CONTINUE:
		if (gSvcStatus.dwCurrentState != SERVICE_PAUSED) {
			gSvcStatus.dwWin32ExitCode = ERROR_SERVICE_CANNOT_ACCEPT_CTRL;
			UpdateSvcStatus();
			return;
		}

		gSvcStatus.dwCurrentState = SERVICE_CONTINUE_PENDING;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 8192;

		if (HANDLE h = CreateThread(0, 0, SvcHandle_Continue, 0, 0, 0))
			CloseHandle(h);
		break;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		return;
	}
	UpdateSvcStatus();
}

DWORD WINAPI SvcHandle_Stop(PVOID param) {

	// 
	if ((DWORD)(INT_PTR)(param) == (ERROR_SHUTDOWN_IN_PROGRESS)) {
		// shutdown in progress

	}
	else {
		// common stop
		Sleep(DWORD(rand() % 2000 + 500));
	}

	if (gSvcExitEvent) {
		SetEvent(gSvcExitEvent);
		CloseHandle(gSvcExitEvent);
	}
	if (gSvcChildEvt$2) {
		SetEvent(gSvcChildEvt$2);
		CloseHandle(gSvcChildEvt$2);
	}
	if (gSvcThread) {
		SuspendThread(gSvcThread);
		CloseHandle(gSvcThread);
	}
	if (gSvcProcess) {
		//s7::CallNtTerminateProcess(gSvcProcess, 0);
		CloseHandle(gSvcProcess);
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();


	gSvcStatus.dwCurrentState = SERVICE_STOPPED;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}

DWORD WINAPI SvcHandle_Pause(PVOID) {

	if (gSvcThread) SuspendThread(gSvcThread);
	if (gSvcProcess) Process.suspend(gSvcProcess);

	gSvcStatus.dwCurrentState = SERVICE_PAUSED;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}

DWORD WINAPI SvcHandle_Continue(PVOID) {

	if (gSvcThread) ResumeThread(gSvcThread);
	if (gSvcProcess) Process.resume(gSvcProcess);

	gSvcStatus.dwCurrentState = SERVICE_RUNNING;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}





DWORD __stdcall svc_main(PVOID) {
	std::wstring cmd = L"\"" + GetProgramDirW() + L"\" ";
	cmd += L"--type=service-main --config=\"" + gSvcConfigFile + L"\" "
		"--service-name=\"" + gSvcName + L"\" "
		"--exit-event=" + std::to_wstring(LONG_PTR(gSvcExitEvent)) + L" "
		"--exit-event-2=" + std::to_wstring(LONG_PTR(gSvcChildEvt$2)) + L" ";

	subprocess_loader_worker_ex(cmd, &gSvcProcess, 0, 0, [] {
		return (gSvcStatus.dwCurrentState != SERVICE_STOPPED) &&
			(gSvcStatus.dwCurrentState != SERVICE_STOP_PENDING);
	}, false, 0, 2000, gSvcChildEvt$2);

	return 0;
}







