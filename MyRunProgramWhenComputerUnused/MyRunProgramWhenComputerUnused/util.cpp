#include <Windows.h>
#include <string>
#include "../../resource/tool.h"




DWORD __stdcall subprocess_loader_worker(
	std::wstring str, HANDLE i, HANDLE o, HANDLE* p,
	bool(__stdcall*condition)(),
	bool enableReturnWhenValue, DWORD returnWhenValue
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
	size_t failureCount = 0;
	time_t retryTime = 0;
	time_t lastTime = 0;
	constexpr size_t maxRetryCountIn1s = 5; // Retry 5 times in 1s
	constexpr size_t maxFailureCount = 20; // Max fatal error allowed
	while (condition == nullptr || condition()) {
		if (!CreateProcessW(NULL, cl, NULL, NULL, TRUE,
			CREATE_SUSPENDED, NULL, NULL, &si, &pi
		)) {
			if (retryTime != time(0)) {
				retryCount = 0;
				retryTime = time(0);
			}
			if (retryCount++ < maxRetryCountIn1s) continue; // retry
			if (++failureCount > maxFailureCount) {
				DWORD err = code;
				if (err == 0) err = -1;
				delete[] cl;
				return err;
			}

			continue; // retry
		}
		CloseHandle(pi.hThread);
		lastTime = time(0);

		if (p) *p = pi.hProcess;
		Process.resume(pi.hProcess);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &code);
		CloseHandle(pi.hProcess);

		if (enableReturnWhenValue && code == returnWhenValue) return returnWhenValue;

		if (time(0) - lastTime <= 2) retryCount++;
		if (retryCount > maxRetryCountIn1s) {
			if (++failureCount > maxFailureCount) {
				DWORD err = code;
				if (err == 0) err = -1;
				delete[] cl;
				return err;
			}
			Sleep(3000);
			retryCount = 0;
			continue;
		}

	}

	delete[] cl;
	cl = nullptr;

	return 0;
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




