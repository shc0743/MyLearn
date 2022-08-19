#include "svc.h"
#include "srv.h"
#include "../../resource/tool.h"
#include "str_table.h"
#include "workbench/workbench.h"
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

int rt_svc_ServiceSubProcessHandler(CmdLineW& cl) {

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

	WaitForSingleObject(hExitEvent, INFINITE);

	CloseHandle(hExitEvent);
	return 0;
}

