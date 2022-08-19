#include "service.h"
#include "../../resource/tool.h"

ServiceWorker_t* global_SvcObj;


#pragma warning(push)
#pragma warning(disable: 6258)
void WINAPI ServiceWorker_t::ServiceHandler(DWORD fdwControl) {
	switch (fdwControl)
	{
	case SERVICE_CONTROL_SHUTDOWN: {
		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOPPED;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 0;
		ExitProcess(0);
		break;
	}
	case SERVICE_CONTROL_STOP: {
		HANDLE hThr = CreateThread(0, 0, StoppingThrd, global_SvcObj, 0, 0);
		if (hThr) CloseHandle(hThr);
		else ExitProcess(GetLastError());

		global_SvcObj->SvcStat.dwWin32ExitCode = 0;
		global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOP_PENDING;
		global_SvcObj->SvcStat.dwCheckPoint = 0;
		global_SvcObj->SvcStat.dwWaitHint = 16384;

		break;
	}
	default:
		return;
	};
	SetServiceStatus(global_SvcObj->hSvcStatus, &global_SvcObj->SvcStat);
}
#pragma warning(pop)

void __stdcall ServiceWorker_t::ReportErrorAndExit(DWORD dwErrCode) {
	global_SvcObj->SvcStat.dwWin32ExitCode = dwErrCode;
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOPPED;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 0;
	SetServiceStatus(global_SvcObj->hSvcStatus, &global_SvcObj->SvcStat);
	ExitProcess(dwErrCode);
}

BOOL ServiceWorker_t::UpdateServiceStatus() {
	return SetServiceStatus(global_SvcObj->hSvcStatus,
		&global_SvcObj->SvcStat);
}

void __stdcall ServiceWorker_t::ServiceLaunch(DWORD, LPWSTR*) {
	global_SvcObj->SvcStat.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_START_PENDING;
	global_SvcObj->SvcStat.dwControlsAccepted = 0;
	global_SvcObj->SvcStat.dwWin32ExitCode = 0;
	global_SvcObj->SvcStat.dwServiceSpecificExitCode = 0;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 16384;
	global_SvcObj->hSvcStatus = RegisterServiceCtrlHandlerW(
		global_SvcObj->ServiceName, ServiceHandler);
	if (global_SvcObj->hSvcStatus == 0) {
		DWORD nError = GetLastError();
		ReportErrorAndExit(nError);
		return;
	}

	//add your init code here

	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();

	global_SvcObj->hChildsJob = CreateJobObject(0, 0);
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobeli{};
	jobeli.BasicLimitInformation.LimitFlags =
		JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
	SetInformationJobObject(global_SvcObj->hChildsJob,
		JobObjectExtendedLimitInformation, &jobeli, sizeof(jobeli));
	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();

	//add your service thread here

	HANDLE hth = 0;
	hth = CreateThread(0, 0, Thrd_nginx, 0, 0, 0);
	if (hth) CloseHandle(hth);
	hth = CreateThread(0, 0, Thrd_php, 0, 0, 0);
	if (hth) CloseHandle(hth);
	global_SvcObj->SvcStat.dwCheckPoint++;
	UpdateServiceStatus();


	// Initialization complete - report running status 
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_RUNNING;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 0;
	global_SvcObj->SvcStat.dwControlsAccepted |=
		SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	//#pragma warning(push)
	//#pragma warning(disable: 6387)
	if (!UpdateServiceStatus()) {
		//#pragma warning(pop)
		ExitProcess(1);
	}
}

DWORD __stdcall ServiceWorker_t::StoppingThrd(PVOID) {
	global_SvcObj->exit = true;

	for (size_t i = 0; i < (sizeof(global_SvcObj->hChilds) / 
		sizeof(global_SvcObj->hChilds[0])); ++i) {
		if (global_SvcObj->hChilds[i])
			CloseHandle(global_SvcObj->hChilds[i]);
	}
	if (global_SvcObj->hChildsJob) {
		TerminateJobObject(global_SvcObj->hChildsJob, 0);
		CloseHandle(global_SvcObj->hChildsJob);
	}

	global_SvcObj->SvcStat.dwWin32ExitCode = 0;
	global_SvcObj->SvcStat.dwCurrentState = SERVICE_STOPPED;
	global_SvcObj->SvcStat.dwCheckPoint = 0;
	global_SvcObj->SvcStat.dwWaitHint = 0;
	UpdateServiceStatus();
	ExitProcess(0);
	return 0;
}

DWORD __stdcall ServiceWorker_t::Thrd_nginx(PVOID) {
	size_t error_count = 0;
	while (!global_SvcObj->exit) {
		MyProcessPsCreateWait(global_SvcObj->szNginx, 0, error_count);
	}
	return 0;
}

DWORD __stdcall ServiceWorker_t::Thrd_php(PVOID) {
	size_t error_count = 0;
	while (!global_SvcObj->exit) {
		MyProcessPsCreateWait(global_SvcObj->szPhp, 1, error_count);
	}
	return 0;
}

bool __stdcall ServiceWorker_t::MyProcessPsCreateWait(
	std::wstring str, size_t n, size_t& error_count
) {
	PROCESS_INFORMATION pi{};
	STARTUPINFO si{};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	std::wstring dir;
	dir = str.substr(0, str.find_last_of(L"\\"));
	PWSTR lpBuffer = new WCHAR[str.length() + 1];
	wcscpy_s(lpBuffer, str.length() + 1, str.c_str());

	if (!(CreateProcessW(NULL, lpBuffer,
		0, 0, 0, 0, 0, dir.c_str(), &si, &pi) && pi.hProcess)) {
		delete[] lpBuffer;
		if (++error_count > 16) {
			Sleep(5000);
			error_count /= 4;
		}
		return false;
	}
	else error_count = 0;
	delete[] lpBuffer;

	CloseHandle(pi.hThread);
	global_SvcObj->hChilds[n] = pi.hProcess;
	AssignProcessToJobObject(global_SvcObj->hChildsJob, pi.hProcess);

	WaitForSingleObject(pi.hProcess, INFINITE);
	global_SvcObj->hChilds[n] = NULL;
	CloseHandle(pi.hProcess);
	return true;
}


