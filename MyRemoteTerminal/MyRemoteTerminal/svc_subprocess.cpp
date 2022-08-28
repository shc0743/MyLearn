#include "svc.h"
#include "srv.h"
#include "../../resource/tool.h"
#include "str_table.h"
#include "workbench/workbench.h"
#include "workbench/PlatformResource/PlatformResource.h"
#include "rt_user_auth.h"
#include "../rt_server_api_handler/rt_functions_entry.h"
using namespace std;

static HANDLE hExitEvent;
static HANDLE hStdin, hStdout;

static DWORD WINAPI SubprocessPipeDataHandle(PVOID) {
#if 0
	DWORD size = 8192;
	char buffer[8192]{};
	std::string s;

	while (ReadFile(hStdin, buffer, size, &size, NULL)) {
		s = buffer;

#else
	char buffer[8192]{};
	std::string s;

	while (std::cin.getline(buffer, 8192)) {

#endif
		s = buffer;

		if (s == "stop-server") {
			SetEvent(hExitEvent);
			return 0;
		}

	}

	return 0;
}
static DWORD WINAPI SubprocessTerminateOnParentExit(PVOID) {
	DWORD pid = Process.GetParentProcessId(GetCurrentProcessId());
	if (!pid) return ERROR_NOT_FOUND;
	HANDLE p = OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, pid);
	if (!p) return GetLastError();
	
	WaitForSingleObject(p, INFINITE);
	GetExitCodeProcess(p, &pid);
	CloseHandle(p);

	ExitProcess(pid);
	return 0; //?
}

static int rt_srv_han(CmdLineW& cl) {

	HANDLE hSrvConf = NULL, hSvcConf = NULL;
	HMODULE hSAH = NULL;
	if (!FreeResFile(IDR_BIN_rsah, "BIN", "rsah.dll")) {
		return GetLastError();
	}
	hSAH = LoadLibraryW(L"rsah.dll");
	if (!hSAH) {
		return GetLastError();
	}
	else {
		typedef void(__stdcall* t)(void*, size_t);
		t p = (t)GetProcAddress(hSAH, "InitialFunctionsEntry");
		if (p) {
			PVOID f[MAX_RT_FUNCTIONS_COUNT]{};
			// Fill functions
			f[rtfid_LogonUser] = rt_LogonUser;
			// Load functions
			p(f, MAX_RT_FUNCTIONS_COUNT);
		}
	}
	wstring temp_s;
	if (1 != cl.getopt(L"server-config", temp_s)) {
		return ERROR_INVALID_PARAMETER;
	}
	hSrvConf = (HANDLE)(ULONG_PTR)atoll(ws2s(temp_s).c_str());
	if (!hSrvConf) return ERROR_INVALID_PARAMETER;
	rt_srv_config* scfg = NULL;
	scfg = (rt_srv_config*)MapViewOfFile(hSrvConf,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(rt_srv_config));
	if (!scfg) {
		CloseHandle(hSrvConf);
		return GetLastError();
	}
	if (1 != cl.getopt(L"service-config", temp_s)) {
		return ERROR_INVALID_PARAMETER;
	}
	hSvcConf = (HANDLE)(ULONG_PTR)atoll(ws2s(temp_s).c_str());
	if (!hSvcConf) return ERROR_INVALID_PARAMETER;
	MyServiceConfig_svc* svcf = (MyServiceConfig_svc*)MapViewOfFile(hSvcConf,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(MyServiceConfig_svc));
	if (!svcf) {
		CloseHandle(hSvcConf);
		return GetLastError();
	}
	scfg->lpConfig = svcf;

	hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!hExitEvent) return GetLastError();

	{
		wstring sz;
		if (1 == cl.getopt(L"stdin", sz)) {
			hStdin = (HANDLE)(ULONG_PTR)atoll(ws2s(sz).c_str());
		}
		if (1 == cl.getopt(L"stdout", sz)) {
			hStdout = (HANDLE)(ULONG_PTR)atoll(ws2s(sz).c_str());
		}
	}

	HANDLE hPipeThread = CreateThread(0, 0, SubprocessPipeDataHandle, 0, 0, 0);
	if (hPipeThread) CloseHandle(hPipeThread);
	HANDLE hServerThread = CreateThread(0, 0, rt_srv_main, scfg, 0, 0);

	WaitForSingleObject(hExitEvent, INFINITE);

	Sleep(200);
	if (hServerThread) {
		HMODULE ntdll = GetModuleHandle(_T("ntdll.dll"));
		if (ntdll) {
			using a = void(WINAPI*)(HANDLE);
			a b = (a)GetProcAddress(ntdll, "NtTerminateThread");
			if (b) b(hServerThread);
		}
		CloseHandle(hServerThread);
	}
	UnmapViewOfFile(svcf);
	UnmapViewOfFile(scfg);
	FreeLibrary(hSAH);
	CloseHandle(hSrvConf);
	CloseHandle(hExitEvent);
	return 0;
}
static int rt_authsrv(CmdLineW& cl) {

	if (HANDLE $ = CreateThread(0, 0, SubprocessTerminateOnParentExit, 0, 0, 0))
		CloseHandle($);

	// TODO
	Sleep(INFINITE);

	return 0;
}

int rt_svc_ServiceSubProcessHandler(CmdLineW& cl) {
	wstring type;
	if (1 != cl.getopt(L"type", type)) return ERROR_INVALID_PARAMETER;

	if (type == L"server") return rt_srv_han(cl);
	if (type == L"auth-server") return rt_authsrv(cl);

	return ERROR_INVALID_PARAMETER;
}

