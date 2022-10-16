#pragma once
#include <Windows.h>
#include <string>


DWORD __stdcall subprocess_loader_worker(
	std::wstring str, HANDLE* p, HANDLE i, HANDLE o
);
DWORD __stdcall subprocess_loader_worker_ex(
	std::wstring str, HANDLE* p, HANDLE i, HANDLE o,
	bool(__stdcall* condition)(),
	bool enableReturnWhenValue, DWORD returnWhenValue,
	DWORD dwWaitForChildExitTimeout, HANDLE hExitEvt
);
HANDLE MyCreateMemmap(DWORD size, DWORD size_hword);



