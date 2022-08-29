#include "svc.h"
#include "srv.h"
#include "../../resource/tool.h"
#include "str_table.h"
#include "workbench/workbench.h"
#include "workbench/PlatformResource/PlatformResource.h"
#include "rt_user_auth.h"

static SERVICE_STATUS          gSvcStatus; 
static SERVICE_STATUS_HANDLE   gSvcStatusHandle;
static std::wstring            gSvcName;
static MyServiceConfig_svc*    gSvcConfig;
static HANDLE                  gSvcConfigHandle;
static HANDLE                  gSvcConFile;
static HANDLE                  gSvcUserFile;
static rt_srv_config           gSrvConfig;
static rt_srv_config*          gSrvCfgMap;
static HANDLE                  gSrvCfgMapHandle;
static HANDLE                  gSrvUsersMapHandle;
static HANDLE                  gSrvProcess
							 , gSrvProcess_pipe_in
							 , gSrvProcess_pipe_out;
static HANDLE                  gSrvAuthProcess;

static VOID WINAPI    SvcCtrlHandler( DWORD ); 
static bool __stdcall UpdateSvcStatus();
static void __stdcall SvcInit( DWORD, LPTSTR * ); 

static DWORD WINAPI rt_srv_subprocess_server_loader(PVOID);
static DWORD WINAPI rt_srv_subprocess_authsrv_loader(PVOID);

static DWORD WINAPI SvcHandle_Stop(PVOID);
static DWORD WINAPI SvcHandle_Pause(PVOID);
static DWORD WINAPI SvcHandle_Continue(PVOID);


void MyServiceSetName_svc(std::wstring name) {
	gSvcName = name;
}

HANDLE MyCreateMemmap(DWORD size, DWORD hw) {
	SECURITY_ATTRIBUTES sa{};
	sa.bInheritHandle = TRUE;

	HANDLE hMap = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		&sa,                     // custom security
		PAGE_READWRITE,          // read/write access
		hw,                      // maximum object size (high-order DWORD)
		size,                    // maximum object size (low-order DWORD)
		NULL);                   // name of mapping object

	if (hMap == NULL) {
		return NULL;
	}

	return hMap;
}

static DWORD __stdcall SvcHandle_Start(PVOID) {
	// Perform service-specific initialization and work.
	using namespace std;

	auto report_last_error = [](DWORD err = -1) {
		if (err == -1) err = GetLastError();

		if (gSrvCfgMap) {
			UnmapViewOfFile(gSrvCfgMap);
		}
		if (gSrvCfgMapHandle) {
			CloseHandle(gSrvCfgMapHandle);
		}
		if (gSvcUserFile) {
			SetHandleInformation(gSvcUserFile,
				HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
			CloseHandle(gSvcUserFile);
		}
		if (gSvcConFile) {
			SetHandleInformation(gSvcConFile,
				HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
			CloseHandle(gSvcConFile);
		}

		gSvcStatus.dwCurrentState = SERVICE_STOPPED;
		gSvcStatus.dwWin32ExitCode = err;
		gSvcStatus.dwWaitHint = 0;
		gSvcStatus.dwCheckPoint = 0;
		UpdateSvcStatus();
		ExitProcess(err);
		return 1;
	};

	constexpr unsigned char max_retry_count = 3;

	{
#if 0
		WCHAR Path[1024]{};
		if (!GetWindowsDirectoryW(Path, 1023)) {
			return report_last_error();
		}
		WCHAR uuid[48]{};
		LoadStringW(NULL, IDS_STRING_APP_UUID, uuid, 48);

		wstring szPath = (Path + L"\\"s + uuid);

		if (!file_exists(szPath)) CreateDirectoryW(szPath.c_str(), 0);
		szPath += L"\\"s + gSvcName;
		if (!file_exists(szPath)) CreateDirectoryW(szPath.c_str(), 0);
#else
		wstring szPath = GetProgramDirW();
		szPath = szPath.substr(0, szPath.find_last_of(L"\\"));
#endif
		if (!SetCurrentDirectoryW(szPath.c_str())) {
			return report_last_error();
		}
	}

	for (unsigned char i = 0; i < max_retry_count + 1 ; ++i) {
		using namespace std;
		WCHAR szdisp[1024]{};
		LoadStringW(NULL, IDS_STRING_SVC_DEFAULT_DISPLAY_NAME, szdisp, 1024);
		wstring name = szdisp;
		str_replace(name, L"%ServiceName%", gSvcName);

		auto pi = Process.Start_HiddenWindow(to__str(
			L"netsh advfirewall firewall show rule name=\"" + name + L"\""));
		if (!pi.hProcess) return report_last_error();

		DWORD dwc = 0;
		CloseHandle(pi.hThread);
		if (WaitForSingleObject(pi.hProcess, 5000) == WAIT_TIMEOUT) {
			s7::CallNtTerminateProcess(pi.hProcess, ERROR_TIMEOUT);
			CloseHandle(pi.hProcess);
			// Retry or report 
			if (i >= (max_retry_count - 1)) return report_last_error(ERROR_TIMEOUT);
			continue;
		}
		GetExitCodeProcess(pi.hProcess, &dwc);
		CloseHandle(pi.hProcess);

		if (dwc) {
			pi = Process.Start_HiddenWindow(to__str(
				L"netsh advfirewall firewall add rule name=\"" + name + L"\" "
				"dir=in action=allow program=\"" + s2ws(GetProgramDir()) + L"\" "
				"profile=any "
			));
			if (!pi.hProcess) report_last_error();
			CloseHandle(pi.hThread);
			if (WaitForSingleObject(pi.hProcess, 5000) == WAIT_TIMEOUT) {
				s7::CallNtTerminateProcess(pi.hProcess, ERROR_TIMEOUT);
				CloseHandle(pi.hProcess);
				// Retry or report 
				if (i >= (max_retry_count - 1)) return report_last_error(ERROR_TIMEOUT);
				continue;
			}
			GetExitCodeProcess(pi.hProcess, &dwc);
			CloseHandle(pi.hProcess);

			if (dwc) report_last_error(dwc);
			break;
		}
		else break;

		if (i >= max_retry_count) return report_last_error();
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	try {
		if (rt_srv_checkupd_resource()) {
			if (!rt_srv_update_resource()) return report_last_error();
		}
	}
	catch (DWORD& error) {
		return report_last_error(error);
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	{
		CmdLineW cl(GetCommandLineW());
		std::wstring sz;
		if (1 != cl.getopt(L"config", sz)) {
			return report_last_error(ERROR_INVALID_PARAMETER);
		}

		gSvcConFile = ::CreateFileW(sz.c_str(), FILE_ALL_ACCESS, 0,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (gSvcConFile == NULL || gSvcConFile == INVALID_HANDLE_VALUE) {
			return report_last_error();
		}
		SetHandleInformation(gSvcConFile, HANDLE_FLAG_PROTECT_FROM_CLOSE,
			HANDLE_FLAG_PROTECT_FROM_CLOSE);
		gSvcStatus.dwCheckPoint++;
		UpdateSvcStatus();

		gSvcConfigHandle = MyCreateMemmap(sizeof(MyServiceConfig_svc));
		if (gSvcConfigHandle == NULL) {
			return report_last_error();
		}
		gSvcConfig = (MyServiceConfig_svc*)MapViewOfFile(gSvcConfigHandle,
			FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MyServiceConfig_svc));
		if (gSvcConfig == NULL) {
			return report_last_error();
		}

		DWORD n = 0;
		if (!ReadFile(gSvcConFile, gSvcConfig, sizeof(gSvcConfig), &n, NULL)) {
			return report_last_error();
		}

	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	if (!rt_srv_load_users(&gSvcUserFile, &gSrvUsersMapHandle)) {
		return report_last_error();
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	gSrvConfig.lpConfig = NULL;
	gSrvConfig.serviceStatus = SERVICE_RUNNING;
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	{
		gSrvCfgMapHandle = MyCreateMemmap(sizeof(rt_srv_config));

		if (gSrvCfgMapHandle == NULL) {
			return report_last_error();
		}
		gSrvCfgMap = (rt_srv_config*)MapViewOfFile(gSrvCfgMapHandle,
			FILE_MAP_ALL_ACCESS, 0, 0, sizeof(rt_srv_config));

		if (gSrvCfgMap == NULL) {
			return report_last_error();
		}
		memcpy(gSrvCfgMap, &gSrvConfig, sizeof(rt_srv_config));
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	{
		SECURITY_ATTRIBUTES sa{};
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&gSrvProcess_pipe_in, &gSrvProcess_pipe_out, &sa, 0)) {
			return report_last_error();
		}
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	//gSrvThread = CreateThread(0, 0, rt_srv_main, &gSrvConfig, 0, 0);
	//if (!gSrvThread) {
	//	if (gSvcUserFile) {
	//		SetHandleInformation(gSvcUserFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
	//		CloseHandle(gSvcUserFile);
	//	}
	//	if (gSvcConFile) {
	//		SetHandleInformation(gSvcConFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
	//		CloseHandle(gSvcConFile);
	//	}
	//	return report_last_error();
	//}
	if (HANDLE h = CreateThread(0, 0, rt_srv_subprocess_authsrv_loader, 0, 0, 0))
		CloseHandle(h);
	if (HANDLE h = CreateThread(0, 0, rt_srv_subprocess_server_loader, 0, 0, 0))
		CloseHandle(h);
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	//if (HANDLE h = CreateThread(0, 0, SvcWaiter_srvthread, 0, 0, 0))
	//	CloseHandle(h);
	//gSvcStatus.dwCheckPoint++;
	//UpdateSvcStatus();


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

	if(!gSvcStatusHandle) { 
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
	return SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
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
#if 1
		gSvcStatus.dwCurrentState = SERVICE_STOPPED;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 0;
		//UpdateSvcStatus();
		break;
#endif
	}

	case SERVICE_CONTROL_STOP:
		gSvcStatus.dwControlsAccepted = 0;
		gSvcStatus.dwCurrentState = SERVICE_STOP_PENDING;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 8192;

		if (HANDLE h = CreateThread(0, 0, SvcHandle_Stop, 0, 0, 0))
			CloseHandle(h);
		return;

	case SERVICE_CONTROL_PAUSE:
		gSvcStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
		gSvcStatus.dwWin32ExitCode = 0;
		gSvcStatus.dwCheckPoint = 0;
		gSvcStatus.dwWaitHint = 8192;

		if (HANDLE h = CreateThread(0, 0, SvcHandle_Pause, 0, 0, 0))
			CloseHandle(h);
		break;

	case SERVICE_CONTROL_CONTINUE:
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

DWORD WINAPI SvcHandle_Stop(PVOID) {

	if (gSrvProcess) {
		//s7::CallNtTerminateProcess(gSrvProcess, 0);
		if (gSrvProcess_pipe_out) {
			std::string sz = "stop-server\n";
			DWORD temp = DWORD(sz.length() + 1);
			WriteFile(gSrvProcess_pipe_out, sz.c_str(), temp, &temp, NULL);
		}
		CloseHandle(gSrvProcess);
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	if (gSrvAuthProcess) {
		CloseHandle(gSrvAuthProcess);
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	if (gSrvUsers) {
		UnmapViewOfFile(gSrvUsers);
	}
	if (gSrvUsersMapHandle) {
		CloseHandle(gSrvUsersMapHandle);
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	if (gSrvCfgMap) {
		gSrvCfgMap->serviceStatus = SERVICE_STOPPED;
		UnmapViewOfFile(gSrvCfgMap);
	}
	if (gSrvCfgMapHandle) {
		CloseHandle(gSrvCfgMapHandle);
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	if (gSvcConfig)	UnmapViewOfFile(gSvcConfig);
	if (gSvcConfigHandle) CloseHandle(gSvcConfigHandle);
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	if (gSvcUserFile) {
		SetHandleInformation(gSvcUserFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
		CloseHandle(gSvcUserFile);
	}
	if (gSvcConFile) {
		SetHandleInformation(gSvcConFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
		CloseHandle(gSvcConFile);
	}
	if (gSrvProcess_pipe_in) CloseHandle(gSrvProcess_pipe_in);
	if (gSrvProcess_pipe_out) CloseHandle(gSrvProcess_pipe_out);
	
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();


	Sleep(1024);
	gSvcStatus.dwCurrentState = SERVICE_STOPPED;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}

DWORD WINAPI SvcHandle_Pause(PVOID) {


	gSrvCfgMap->serviceStatus = SERVICE_PAUSED;

	gSvcStatus.dwCurrentState = SERVICE_PAUSED;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}

DWORD WINAPI SvcHandle_Continue(PVOID) {


	gSrvCfgMap->serviceStatus = SERVICE_RUNNING;

	gSvcStatus.dwCurrentState = SERVICE_RUNNING;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}


static int __stdcall rt_subprocess_loader_worker(
	std::wstring str, HANDLE i, HANDLE o, HANDLE* p
) {
	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};

	si.cb = sizeof(si);
	si.dwFlags |= (i && o) ? STARTF_USESTDHANDLES : 0;
	si.hStdInput = i;
	si.hStdOutput = si.hStdError = o;

	DWORD code = 0;
	PWSTR cl = new WCHAR[str.length() + 1];

	wcscpy_s(cl, str.length() + 1, str.c_str());

	size_t retryCount = 0;
	time_t retryTime = 0;
	constexpr size_t maxRetryCountIn1s = 5; // Retry 5 times in 1s
	while ((gSvcStatus.dwCurrentState != SERVICE_STOPPED) &&
		(gSvcStatus.dwCurrentState != SERVICE_STOP_PENDING)) {
		if (!CreateProcessW(GetProgramDirW().c_str(), cl, NULL, NULL, TRUE,
			CREATE_SUSPENDED, NULL, NULL, &si, &pi
		)) {
			if (retryTime != time(0)) {
				retryCount = 0;
				retryTime = time(0);
			}
			if (retryCount++ < maxRetryCountIn1s) continue; // retry

			DWORD err = GetLastError();
			if (err == 0) err = -1;
			return err;
		}
		CloseHandle(pi.hThread);

		*p = pi.hProcess;
		Process.resume(pi.hProcess);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &code);

	}

	delete[] cl;
	cl = nullptr;

	return 0;
}


DWORD __stdcall rt_srv_subprocess_server_loader(PVOID _in) {
	using namespace std;

#if 0
	HANDLE in = (HANDLE)_in;
	if (!in) return 1;

	WaitForSingleObject(in, INFINITE);
	GetExitCodeThread(in, &code);

	ServiceManager.Stop(ws2s(gSvcName));
#endif

	wstring sc = L"ServiceSubProcess --service-sub-process --type=server ";
	sc += L"--stdin=" + to_wstring((LONG_PTR)gSrvProcess_pipe_in) + L" "
		L"--stdout=" + to_wstring((LONG_PTR)gSrvProcess_pipe_out) + L" "
		L"--server-config=" + to_wstring((LONG_PTR)gSrvCfgMapHandle) + L" "
		L"--service-config=" + to_wstring((LONG_PTR)gSvcConfigHandle) + L" ";

	return rt_subprocess_loader_worker(sc,
		gSrvProcess_pipe_in, gSrvProcess_pipe_out, &gSrvProcess);

	return 0;
}


DWORD __stdcall rt_srv_subprocess_authsrv_loader(PVOID) {
	using namespace std;

	wstring sc = L"ServiceSubProcess --service-sub-process --type=auth-server ";
	sc += L"--user=" + to_wstring((LONG_PTR)gSrvUsersMapHandle) + L" "
		L"--service-name=\"" + gSvcName + L"\" ";

	return rt_subprocess_loader_worker(sc, 0, 0, &gSrvAuthProcess);

	return 0;
}


bool rt_srv_checkupd_resource() {
	using namespace std;

	auto report_last_error = [](DWORD error = -1) {
		if (error != -1) SetLastError(error);
		throw ((error == -1) ? GetLastError() : error);
		return false;
	};

	if (!FreeResFile(IDR_BIN_7z, L"BIN", L"____.exe")) {
		return report_last_error();
	}
	if (!FreeResFile(IDR_BIN_HTMLPACK, L"BIN", L"____.7z")) {
		return report_last_error();
	}

	auto z = Process.Start_HiddenWindow(_T(
		"____ x -y ____.7z html_pack_version.txt"));
	if (!z.hProcess) return report_last_error();
	CloseHandle(z.hThread);
	if (WaitForSingleObject(z.hProcess, 5000) == WAIT_TIMEOUT) {
		s7::CallNtTerminateProcess(z.hProcess, ERROR_TIMEOUT);
		CloseHandle(z.hProcess);
		return report_last_error(ERROR_TIMEOUT);
	}
	CloseHandle(z.hProcess);

	unsigned long long currentVersion = 0, pkgVersion = 0;
	if (!file_exists(L"html_pack_version.txt"))
		return report_last_error(ERROR_INVALID_DATA);
	{ fstream fp("html_pack_version.txt", ios::in); if (fp) fp >> pkgVersion; }
	{ fstream fp("html_pkg_version.txt", ios::in); if (fp)fp >> currentVersion; }
	bool result = false;
	if (pkgVersion > currentVersion) {
		result = true;
	}

	DeleteFileW(L"html_pack_version.txt");
	DeleteFileW(L"____.exe");
	DeleteFileW(L"____.7z");

	return result;
}

bool rt_srv_update_resource() {
	using namespace std;

	auto report_last_error = [](DWORD error = -1) {
		if (error != -1) SetLastError(error);
		return false;
	};

	if (!FreeResFile(IDR_BIN_7z, L"BIN", L"____.exe")) {
		return report_last_error();
	}
	if (!FreeResFile(IDR_BIN_HTMLPACK, L"BIN", L"____.7z")) {
		return report_last_error();
	}

	auto z = Process.Start_HiddenWindow(_T(
		"____ x -y ____.7z html_pack_version.txt"));
	if (!z.hProcess) return report_last_error();
	CloseHandle(z.hThread);
	if (WaitForSingleObject(z.hProcess, 5000) == WAIT_TIMEOUT) {
		s7::CallNtTerminateProcess(z.hProcess, ERROR_TIMEOUT);
		CloseHandle(z.hProcess);
		return report_last_error(ERROR_TIMEOUT);
	}
	CloseHandle(z.hProcess);

	unsigned long long currentVersion = 0, pkgVersion = 0;
	if (!file_exists(L"html_pack_version.txt"))
		return report_last_error(ERROR_INVALID_DATA);
	{ fstream fp("html_pack_version.txt", ios::in); if (fp) fp >> pkgVersion; }
	{ fstream fp("html_pkg_version.txt", ios::in); if (fp)fp >> currentVersion; }
	if (pkgVersion > currentVersion) {
		auto z = Process.Start_HiddenWindow(TEXT("____ x -y ____.7z"));
		if (!z.hProcess) return report_last_error();
		CloseHandle(z.hThread);
		if (WaitForSingleObject(z.hProcess, 5000) == WAIT_TIMEOUT) {
			s7::CallNtTerminateProcess(z.hProcess, ERROR_TIMEOUT);
			CloseHandle(z.hProcess);
			return report_last_error(ERROR_TIMEOUT);
		}
		CloseHandle(z.hProcess);
		DeleteFileW(L"html_pkg_version.txt");
		MoveFileW(L"html_pack_version.txt", L"html_pkg_version.txt");
	}
	else {
		DeleteFileW(L"html_pack_version.txt");
	}

	if (!file_exists(L"html_pkg_version.txt"))
		return report_last_error(ERROR_INVALID_DATA);

	DeleteFileW(L"____.exe"); DeleteFileW(L"____.7z");

	return true;
}





