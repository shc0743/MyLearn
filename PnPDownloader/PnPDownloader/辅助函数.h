#pragma once
#include <Windows.h>
#include <string>

bool IsInsideVMWare();
#define 程序是否在虚拟机中运行() (IsInsideVMWare())
void CenterWindow(HWND hWnd);
#define 居中窗口 CenterWindow

decltype(RegisterClassW(0)) MyRegisterClassW(PCWSTR className, WNDPROC WndProc,
	WNDCLASSEXW content = WNDCLASSEXW{ 0 });
BOOL AdjustPrivilege(ULONG Privilege, BOOLEAN Enable,
	BOOLEAN CurrentThread, PBOOLEAN Enabled = NULL);

extern "C" {
	int WINAPI MessageBoxTimeoutA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption,
		UINT uType, WORD wLanguageId, DWORD dwMilliseconds);
	int WINAPI MessageBoxTimeoutW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption,
		UINT uType, WORD wLanguageId, DWORD dwMilliseconds);
};

std::wstring s2ws(const std::string);
std::string ws2s(const std::wstring);
std::string GetProgramDir();

bool FreeResFile(DWORD dwResName, std::wstring lpResType,
	std::wstring lpFilePathName, HMODULE hInst = nullptr);
inline bool FreeResFile(DWORD dwResName, std::string lpResType,
	std::wstring lpFilePathName, HMODULE hInst = nullptr) {
	return FreeResFile(dwResName, s2ws(lpResType), lpFilePathName, hInst);
}
inline bool FreeResFile(DWORD dwResName, std::string lpResType,
	std::string lpFilePathName, HMODULE hInst = nullptr) {
	return FreeResFile(dwResName, s2ws(lpResType), s2ws(lpFilePathName), hInst);
}

// Translate a error-code such as GetLastError() to std::wstring
std::wstring ErrorCodeToStringW(DWORD ErrorCode);
// Translate a error-code such as GetLastError() to std::string
std::string  ErrorCodeToStringA(DWORD ErrorCode);
inline std::wstring LastErrorStrW() {
	auto error = GetLastError();
	auto result = ErrorCodeToStringW(error);
	SetLastError(error);
	return result;
}
inline std::string  LastErrorStrA() {
	auto error = GetLastError();
	auto result = ErrorCodeToStringA(error);
	SetLastError(error);
	return result;
}
#ifdef UNICODE
// Translate a error-code such as GetLastError() to std::wstring
inline std::wstring ErrorCodeToString(DWORD ErrorCode) {
	return ErrorCodeToStringW(ErrorCode);
}
inline std::wstring LastErrorStr() {
	return ErrorCodeToString(GetLastError());
}
#else
// Translate a error-code such as GetLastError() to std::string
inline std::string ErrorCodeToString(DWORD ErrorCode) {
	return ErrorCodeToStringA(ErrorCode);
}
inline std::string LastErrorStr() {
	return ErrorCodeToString(GetLastError());
}
#endif



