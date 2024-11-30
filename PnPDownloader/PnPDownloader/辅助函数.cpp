#pragma once
#include "辅助函数.h"
#include <tchar.h>
#include <string>
using namespace std;
#define NT_SUCCESS(stat) ((stat)>=0)

#define _myrclassf(key, _default) \
	if(content. ## key) wcex. ## key = content. ## key;\
	else wcex. ## key = _default;
#define _myrclassc \
	_myrclassf(style, CS_HREDRAW | CS_VREDRAW);\
	_myrclassf(lpfnWndProc, WndProc);\
	_myrclassf(cbClsExtra, 0);\
	_myrclassf(cbWndExtra, 0);\
	_myrclassf(hInstance, NULL);\
	_myrclassf(hIcon, NULL);\
	_myrclassf(hIconSm, NULL);\
	_myrclassf(lpszMenuName, NULL);\
	_myrclassf(hCursor, LoadCursor(nullptr, IDC_ARROW));\
	_myrclassf(hbrBackground, (HBRUSH)(COLOR_WINDOW + 1));\
	_myrclassf(lpszClassName, className);\

#pragma warning(push)
#pragma warning(disable: 4302)
decltype(RegisterClassW(0))
MyRegisterClassW(PCWSTR className, WNDPROC WndProc, WNDCLASSEXW content) {
	WNDCLASSEXW wcex{ 0 };

	wcex.cbSize = sizeof(wcex);

	_myrclassc

		return RegisterClassExW(&wcex);
}
#pragma warning(pop)
#undef _myrclassc
#undef _myrclassf

BOOL AdjustPrivilege(
	ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled
) {
	typedef NTSTATUS(WINAPI* pRtlAdjustPrivilege)
		(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
	pRtlAdjustPrivilege RtlAdjustPrivilege = NULL;
	HMODULE ntdll = GetModuleHandle(_T("ntdll.dll"));
	if (!ntdll) return FALSE;
	RtlAdjustPrivilege = (pRtlAdjustPrivilege)
		GetProcAddress(ntdll, "RtlAdjustPrivilege");
	if (!RtlAdjustPrivilege) return FALSE;
	BOOLEAN _tmp0 = 0;
	NTSTATUS result = RtlAdjustPrivilege(
		Privilege, Enable, CurrentThread, Enabled ? Enabled : &_tmp0);
	BOOL success = NT_SUCCESS(result);
	return success;
	/*
	SeCreateTokenPrivilege 0x2
	SeAssignPrimaryTokenPrivilege 0x3
	SeLockMemoryPrivilege 0x4
	SeIncreaseQuotaPrivilege 0x5
	SeUnsolicitedInputPrivilege 0x0
	SeMachineAccountPrivilege 0x6
	SeTcbPrivilege 0x7
	SeSecurityPrivilege 0x8
	SeTakeOwnershipPrivilege 0x9
	SeLoadDriverPrivilege 0xa
	SeSystemProfilePrivilege 0xb
	SeSystemtimePrivilege 0xc
	SeProfileSingleProcessPrivilege 0xd
	SeIncreaseBasePriorityPrivilege 0xe
	SeCreatePagefilePrivilege 0xf
	SeCreatePermanentPrivilege 0x10
	SeBackupPrivilege 0x11
	SeRestorePrivilege 0x12
	SeShutdownPrivilege 0x13
	SeDebugPrivilege 0x14
	SeAuditPrivilege 0x15
	SeSystemEnvironmentPrivilege 0x16
	SeChangeNotifyPrivilege 0x17
	SeRemoteShutdownPrivilege 0x18
	SeUndockPrivilege 0x19
	SeSyncAgentPrivilege 0x1a
	SeEnableDelegationPrivilege 0x1b
	SeManageVolumePrivilege 0x1c
	SeImpersonatePrivilege 0x1d
	SeCreateGlobalPrivilege 0x1e
	SeTrustedCredManAccessPrivilege 0x1f
	SeRelabelPrivilege 0x20
	SeIncreaseWorkingSetPrivilege 0x21
	SeTimeZonePrivilege 0x22
	SeCreateSymbolicLinkPrivilege 0x23
	*/
}

void CenterWindow(HWND hWnd) {
	//取得窗口尺寸
	RECT rect; GetWindowRect(hWnd, &rect);
	//重新设置rect里的值
	rect.left = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
	rect.top = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
	//移动窗口到指定的位置
	SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top,
		rect.right, rect.bottom, SWP_NOACTIVATE);
}

bool IsInsideVMWare() {
	bool rc = true;

	__try {
		__asm
		{
			push   edx
			push   ecx
			push   ebx

			mov    eax, 'VMXh'
			mov    ebx, 0 // any value but not the MAGIC VALUE
			mov    ecx, 10 // get VMWare version
			mov    edx, 'VX' // port number

			in     eax, dx // read port
						   // on return EAX returns the VERSION
						   cmp    ebx, 'VMXh' // is it a reply from VMWare?
						   setz[rc] // set return value

						   pop    ebx
						   pop    ecx
						   pop    edx
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		rc = false;
	}

	return rc;
}

string ws2s(const wstring wstr) {
	string result;
	size_t len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
		(int)(wstr.size()), NULL, 0, NULL, NULL);
	if (len < 0) return result;
	char* buffer = new char[len + 1];
	if (buffer == NULL) return result;
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
		(int)(wstr.size()), buffer, (int)len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}
wstring s2ws(const string str) {
	wstring result;
	size_t len = MultiByteToWideChar(CP_ACP, 0, str.c_str(),
		(int)(str.size()), NULL, 0);
	if (len < 0) return result;
	wchar_t* buffer = new wchar_t[len + 1];
	if (buffer == NULL) return result;
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)(str.size()),
		buffer, (int)len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}
string GetProgramDir() {
	WCHAR exeFullPath[MAX_PATH];
	GetModuleFileNameW(NULL, exeFullPath, MAX_PATH);
	return ws2s(exeFullPath);
}

#ifdef _WIN32
bool FreeResFile(
	DWORD dwResName, wstring lpResType, wstring lpFilePathName, HMODULE hInst
) {
	HMODULE hInstance = hInst ? hInst : GetModuleHandle(NULL);//得到自身实例句柄  
	HRSRC hResID = ::FindResourceW(hInstance,
		MAKEINTRESOURCEW(dwResName), lpResType.c_str());//查找资源  
	if (!hResID) return false;
	HGLOBAL hRes = ::LoadResource(hInstance, hResID);//加载资源  
	if (!hRes) return false;
	LPVOID pRes = ::LockResource(hRes);//锁定资源  

	if (pRes == NULL)//锁定失败  
	{
		return false;
	}
	DWORD dwResSize = ::SizeofResource(hInstance, hResID);//得到待释放文件大小  
	HANDLE hResFile = CreateFileW(
		lpFilePathName.c_str(),
		GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);//创建文件  

	if (!hResFile || INVALID_HANDLE_VALUE == hResFile) {
		return false;
	}

	DWORD dwWritten = 0;//写入文件的大小     
	WriteFile(hResFile, pRes, dwResSize, &dwWritten, NULL);//写入文件  
	CloseHandle(hResFile);//关闭文件句柄  

	return (dwResSize == dwWritten);//若写入大小等于文件大小返回成功否则失败  
}
#endif

wstring ErrorCodeToStringW(DWORD ErrorCode)
{
	typedef LPWSTR str_t;
	str_t LocalAddress = NULL;
	if (NULL == ([&] {
		__try {
			if (!FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_FROM_SYSTEM, NULL,
				ErrorCode, 0, (str_t)&LocalAddress, 0, NULL)) {
				return (str_t)NULL;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return (str_t)NULL; }
		return LocalAddress;
		})()) return L"";
	LocalAddress[wcslen((str_t)LocalAddress) - 1] = 0;
	wstring szText = LocalAddress;
	LocalFree((HLOCAL)LocalAddress);
	return szText;
}

string  ErrorCodeToStringA(DWORD ErrorCode)
{
	typedef LPSTR str_t;
	str_t LocalAddress = NULL;
	if (NULL == ([&] {
		__try {
			if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_IGNORE_INSERTS |
				FORMAT_MESSAGE_FROM_SYSTEM, NULL,
				ErrorCode, 0, (str_t)&LocalAddress, 0, NULL)) {
				return (str_t)NULL;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER) { return (str_t)NULL; }
		return LocalAddress;
		})()) return "";
	LocalAddress[strlen((str_t)LocalAddress) - 1] = 0;
	string szText = LocalAddress;
	LocalFree((HLOCAL)LocalAddress);
	return szText;
}




