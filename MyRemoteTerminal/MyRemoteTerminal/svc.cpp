#include "svc.h"
#include "srv.h"
#include "../../resource/tool.h"
#include "str_table.h"
#include "workbench/workbench.h"

static SERVICE_STATUS          gSvcStatus; 
static SERVICE_STATUS_HANDLE   gSvcStatusHandle;
static std::wstring            gSvcName;
static MyServiceConfig_svc     gSvcConfig;
static HANDLE                  gSvcConFile;
static HANDLE                  gSvcUserFile;
static rt_srv_config           gSrvConfig;
static HANDLE                  gSrvProcess
							 , gSrvProcess_pipe_in
							 , gSrvProcess_pipe_out;

std::map<std::wstring, rt_userinfo> gSrvUsers;
unsigned long long nSrvUserCount;

static VOID WINAPI    SvcCtrlHandler( DWORD ); 
static bool __stdcall UpdateSvcStatus();
static void __stdcall SvcInit( DWORD, LPTSTR * ); 

static DWORD WINAPI rt_srv_subprocess_loader(PVOID);

static DWORD WINAPI SvcHandle_Stop(PVOID);
static DWORD WINAPI SvcHandle_Pause(PVOID);
static DWORD WINAPI SvcHandle_Continue(PVOID);


static bool rt_srv_load_users();


void MyServiceSetName_svc(std::wstring name) {
	gSvcName = name;
}

static DWORD __stdcall SvcHandle_Start(PVOID) {
	// Perform service-specific initialization and work.
	using namespace std;

	auto report_last_error = [](DWORD err = -1) {
		gSvcStatus.dwCurrentState = SERVICE_STOPPED;
		if (err == -1) err = GetLastError();
		gSvcStatus.dwWin32ExitCode = err;
		gSvcStatus.dwWaitHint = 0;
		gSvcStatus.dwCheckPoint = 0;
		UpdateSvcStatus();
		ExitProcess(err);
		return 1;
	};

	constexpr unsigned char max_retry_count = 3;

	{
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

		DWORD n = 0;
		if (!ReadFile(gSvcConFile, &gSvcConfig, sizeof(gSvcConfig), &n, NULL)) {
			CloseHandle(gSvcConFile);
			return report_last_error();
		}

	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	if (!rt_srv_load_users()) {
		if (gSvcUserFile) {
			SetHandleInformation(gSvcUserFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
			CloseHandle(gSvcUserFile);
		}
		if (gSvcConFile) {
			SetHandleInformation(gSvcConFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
			CloseHandle(gSvcConFile);
		}
		return report_last_error();
	}
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	gSrvConfig.lpConfig = &gSvcConfig;
	gSrvConfig.serviceStatus = SERVICE_RUNNING;
	gSvcStatus.dwCheckPoint++;
	UpdateSvcStatus();

	{
		SECURITY_ATTRIBUTES sa{};
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&gSrvProcess_pipe_in, &gSrvProcess_pipe_out, &sa, 0)) {
			DWORD err = GetLastError();
			if (gSvcUserFile) {
				SetHandleInformation(gSvcUserFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
				CloseHandle(gSvcUserFile);
			}
			if (gSvcConFile) {
				SetHandleInformation(gSvcConFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
				CloseHandle(gSvcConFile);
			}
			return report_last_error(err);
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
	if (HANDLE h = CreateThread(0, 0, rt_srv_subprocess_loader, 0, 0, 0))
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

bool UpdateSvcStatus() {
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
			DWORD temp = sz.length() + 1;
			WriteFile(gSrvProcess_pipe_out, sz.c_str(), temp, &temp, NULL);
		}
		CloseHandle(gSrvProcess);
		gSvcStatus.dwCheckPoint++;
		UpdateSvcStatus();
	}
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


	gSrvConfig.serviceStatus = SERVICE_STOPPED;

	Sleep(1024);
	gSvcStatus.dwCurrentState = SERVICE_STOPPED;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}

DWORD WINAPI SvcHandle_Pause(PVOID) {


	gSrvConfig.serviceStatus = SERVICE_PAUSED;

	gSvcStatus.dwCurrentState = SERVICE_PAUSED;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}

DWORD WINAPI SvcHandle_Continue(PVOID) {


	gSrvConfig.serviceStatus = SERVICE_RUNNING;

	gSvcStatus.dwCurrentState = SERVICE_RUNNING;
	gSvcStatus.dwWin32ExitCode = 0;
	gSvcStatus.dwCheckPoint = 0;
	gSvcStatus.dwWaitHint = 0;
	UpdateSvcStatus();
	return 0;
}


DWORD __stdcall rt_srv_subprocess_loader(PVOID _in) {
	using namespace std;

	DWORD code = 0;
#if 0
	HANDLE in = (HANDLE)_in;
	if (!in) return 1;

	WaitForSingleObject(in, INFINITE);
	GetExitCodeThread(in, &code);

	ServiceManager.Stop(ws2s(gSvcName));
#endif

	STARTUPINFOW si{};
	PROCESS_INFORMATION pi{};

	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdInput = gSrvProcess_pipe_in;
	si.hStdOutput = si.hStdError = gSrvProcess_pipe_out;

	wstring sc = L"ServiceSubProcess --type=service-sub-process ";
	sc += L"--stdin=\"" + to_wstring((LONG_PTR)gSrvProcess_pipe_in) + L"\" "
		L"--stdout=\"" + to_wstring((LONG_PTR)gSrvProcess_pipe_out) + L"\" ";

	PWSTR cl = new WCHAR[sc.length() + 1];
	wcscpy_s(cl, sc.length() + 1, sc.c_str());

	while ((gSvcStatus.dwCurrentState != SERVICE_STOPPED) &&
		(gSvcStatus.dwCurrentState != SERVICE_STOP_PENDING)) {
		if (!CreateProcessW(GetProgramDirW().c_str(), cl, NULL, NULL, TRUE,
			CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
			DWORD err = GetLastError();
			delete[] cl;
			if (gSvcUserFile) {
				SetHandleInformation(gSvcUserFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
				CloseHandle(gSvcUserFile);
			}
			if (gSvcConFile) {
				SetHandleInformation(gSvcConFile, HANDLE_FLAG_PROTECT_FROM_CLOSE, 0);
				CloseHandle(gSvcConFile);
			}
			gSvcStatus.dwCurrentState = SERVICE_STOPPED;
			gSvcStatus.dwWin32ExitCode = err;
			gSvcStatus.dwCheckPoint = 0;
			gSvcStatus.dwWaitHint = 0;
			UpdateSvcStatus();
			return err;
		}
		CloseHandle(pi.hThread);

		gSrvProcess = pi.hProcess;
		Process.resume(gSrvProcess);
		WaitForSingleObject(gSrvProcess, INFINITE);
		GetExitCodeProcess(gSrvProcess, &code);

	}

	delete[] cl;
	cl = nullptr;


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

bool rt_srv_load_users() {

	WCHAR ufl[256]{};
	if (!LoadStringW(NULL, IDS_STRING_SRV_USERFILE_LOCATION, ufl, 256))
		return false;

	if (!file_exists(ufl)) {
		gSvcUserFile = CreateFileW(ufl, FILE_ALL_ACCESS, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!gSvcUserFile || gSvcUserFile == INVALID_HANDLE_VALUE) {
			return false;
		}

		DWORD temp = 0;
		DWORD cb = sizeof(unsigned long long);
		WriteFile(gSvcUserFile, &temp, cb, &cb, NULL);

		CloseHandle(gSvcUserFile);
	}

	gSvcUserFile = CreateFileW(ufl, FILE_ALL_ACCESS, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!gSvcUserFile || gSvcUserFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	SetHandleInformation(gSvcUserFile, HANDLE_FLAG_PROTECT_FROM_CLOSE,
		HANDLE_FLAG_PROTECT_FROM_CLOSE);

	DWORD size = sizeof(unsigned long long), size2 = size;
	if (!ReadFile(gSvcUserFile, &nSrvUserCount, size, &size2, NULL)) return false;
	if (size2 != size) {
		SetLastError(ERROR_INVALID_DATA);
		return false;
	}
	for (unsigned long long i = 0; i < nSrvUserCount; ++i) {

	}


	return true;
}


#if 0
#include <openssl/sha.h>
std::string rt_sha256(const std::string& str) {
	char buf[2];
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);
	std::string NewString = "";
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(buf, "%02x", hash[i]);
		NewString = NewString + buf;
	}
	return NewString;
}
#endif



