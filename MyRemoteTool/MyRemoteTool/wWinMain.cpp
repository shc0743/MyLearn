#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include "../../resource/tool.h"
#include "globaldef.h"
#include "winui.h"
#include "server.h"
#include "resource.h"
#include "sciter/bin_sciter.h"
using namespace std;

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")


HINSTANCE hInst;

bool mmInitWinSock2() {
	WSADATA wsaData{};
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}
	return true;
}
bool mmCleanupWinSock2() {
	return (WSACleanup() == 0);
}


static int __stdcall _wWinMainCallTarget() {
	CmdLineW cl(GetCommandLineW());
	
	if (cl.argc() < 2 || cl[1] == L"--ui") {
		return mWinDispatchUIMain(cl);
	}

	if (cl.getopt(L"srv")) {
		if (cl.getopt(L"service")) {
			// TODO
			return 0;
		}

		if (cl.getopt(L"once")) {
			wstring ip, pswd;
			cl.getopt(L"i", ip);
			cl.getopt(L"p", pswd);
			if (ip.empty()) return ERROR_INVALID_PARAMETER;

			vector<wstring> ip_strs;
			str_split(ip, L":", ip_strs);
			if (ip_strs.size() != 2) return ERROR_INVALID_PARAMETER;
			if (ip_strs[0][0] != L'{' ||
				ip_strs[1][ip_strs[1].length() - 1] != L'}') return ERROR_INVALID_PARAMETER;
			ip_strs[0] = ip_strs[0].substr(1);
			ip_strs[1].erase(ip_strs[1].length() - 1);
			vector<wstring> ipc;
			str_split(ip_strs[0], L".", ipc);
			if (ipc.size() != 4) return ERROR_INVALID_PARAMETER;

			t_mStartServerInstance data{ 0 };
			for (unsigned char i = 0; i < 4; ++i) {
				data.ip[i] = (unsigned char)atoi(ws2c(ipc[i]));
			}
			data.port = (USHORT)atoi(ws2c(ip_strs[1]));
			wcscpy_s(data.password, pswd.c_str());
			if (cl.getopt(L"ipv6")) data.ipv6 = true;

			if (!mmInitWinSock2()) {
				MessageBoxTimeoutW(NULL, L"WSA Init Failed", NULL, MB_ICONHAND, 0, 2000);
				return WSAGetLastError();
			}

			HANDLE hThread = CreateThread(0, 0, mStartServerInstance, &data, 0, 0);
			if (!hThread) return GetLastError();
			DWORD dwCode = 1;
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &dwCode);
			CloseHandle(hThread);

			mmCleanupWinSock2();
			return (int)dwCode;

			return 0;
		}

	}

	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	::hInst = hInstance;

	WCHAR TempPath[1024]{};
	WCHAR TempPathName[1024]{};
	if (!GetTempPathW(1023, TempPath)) {
		return GetLastError();
	}
	wcscat_s(TempPath, (L"\\.p"s + to_wstring(GetCurrentProcessId())
		+ L"_temp\\").c_str());
	if (!CreateDirectoryW(TempPath, NULL)) {
		return GetLastError();
	}
	SetFileAttributesW(TempPath, FILE_ATTRIBUTE_HIDDEN);
	SetCurrentDirectoryW(TempPath);
	wcscat_s(TempPathName, TempPath);
	if (!FreeResFile(IDR_BIN_SCITER, L"BIN", TempPathName + L"sciter.dll"s)) {
		FileDeleteTreeW(TempPath);
		return GetLastError();
	}
	if (!FreeResFile(IDR_BIN_SINSPECT, L"BIN", TempPathName + L"sciter_inspector.exe"s)) {
		FileDeleteTreeW(TempPath);
		return GetLastError();
	}
	if (!FreeResFile(IDR_BIN_SBROWSER, L"BIN", TempPathName + L"sBrowserProcess.exe"s)) {
		FileDeleteTreeW(TempPath);
		return GetLastError();
	}
	if (!FreeResFile(IDR_BIN_7z, L"BIN", TempPathName + L"7_zip.exe"s)) {
		FileDeleteTreeW(TempPath);
		return GetLastError();
	}

	int _result = _wWinMainCallTarget();

	Sleep(1000);
	HMODULE sciter_dll = GetModuleHandle(TEXT("sciter.dll"));
	if (sciter_dll) FreeLibrary(sciter_dll);
	SetCurrentDirectoryW(L"..");
	FileDeleteTreeW(TempPath);

	return _result;
}



