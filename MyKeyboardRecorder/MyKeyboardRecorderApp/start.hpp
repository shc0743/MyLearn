#pragma once
#include <Windows.h>
#include <string>
#include <memory>

inline PROCESS_INFORMATION StartEx(std::wstring cmd_line, PCWSTR app, STARTUPINFOW si, DWORD flags) {
	PROCESS_INFORMATION pi{};
	auto buffer = std::make_unique<WCHAR[]>(cmd_line.length() + 1);
	wcscpy_s(buffer.get(), (cmd_line.size() + 1), cmd_line.data());
#pragma warning(push)
#pragma warning(disable: 6335)
	CreateProcessW(app, buffer.get(), NULL, NULL, FALSE, flags, NULL, NULL, &si, &pi);
#pragma warning(pop)
	return pi;
}

inline DWORD start(std::wstring cmd_line, PCWSTR app = NULL, bool* success = nullptr) {
	STARTUPINFO si{};
	auto pi = StartEx(cmd_line, app, si, CREATE_SUSPENDED);
	if (success) *success = false;
	if (!pi.hProcess) {
		DWORD error = GetLastError();
		if (pi.hThread) CloseHandle(pi.hThread);
		return error;
	}
	DWORD code = -1;
	ResumeThread(pi.hThread);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &code);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	if (success) *success = true;
	return code;
}

inline bool StartNw(std::wstring cmd_line, PCWSTR app = NULL) {
	STARTUPINFO si{};
	auto pi = StartEx(cmd_line, app, si, 0);
	if (!pi.hProcess) {
		if (pi.hThread) CloseHandle(pi.hThread);
		return false;
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return true;
}

inline std::wstring app_path() {
	static std::wstring path;
	if (!path.empty()) return path;
	auto exeFullPath = std::make_unique<WCHAR[]>(32768);
	GetModuleFileNameW(NULL, exeFullPath.get(), 32768);
	path = exeFullPath.get();
	return path;
}
