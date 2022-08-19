#pragma once
#include <Windows.h>
#include <AclAPI.h>
#include <string>
using std::string;

class ServiceWorker_t {
public:
	ServiceWorker_t() { ZeroMemory(this, sizeof(*this)); };
	~ServiceWorker_t() = default;
	WCHAR ServiceName[256];
	std::wstring szNginx, szPhp;

	static void WINAPI ServiceLaunch(DWORD, LPWSTR*);

protected:
	static void __stdcall ReportErrorAndExit(DWORD dwErrCode);
	static void WINAPI ServiceHandler(DWORD fdwControl);
	static BOOL UpdateServiceStatus();
	static DWORD __stdcall StoppingThrd(PVOID);

	static DWORD __stdcall Thrd_nginx(PVOID);
	static DWORD __stdcall Thrd_php(PVOID);

	static bool __stdcall MyProcessPsCreateWait(
		std::wstring str, size_t n, size_t& error_count);

	SERVICE_STATUS SvcStat;
	SERVICE_STATUS_HANDLE hSvcStatus;
	
	bool exit;
	HANDLE hChilds[2];
	HANDLE hChildsJob;
};

extern ServiceWorker_t* global_SvcObj;


