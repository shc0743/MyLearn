#include <Windows.h>
#include <wtsapi32.h>
#include <userenv.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "wtsapi32.lib")
#pragma comment(lib, "Userenv.lib")

#pragma warning(push)
#pragma warning(disable: 6101)
BOOL CreateProcessInSession(_In_ DWORD dwSessionId,
	_In_opt_ LPCSTR lpApplicationName,
	_Inout_opt_ LPSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFO lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
) {
	auto& si = *lpStartupInfo;         // 
	//auto& pi = *lpProcessInformation;  // 
	HANDLE hUserToken = NULL;          // 当前登录用户的令牌  
	// HANDLE hUserTokenDup = NULL;       // 复制的用户令牌  
	HANDLE hPToken = NULL;             // 进程令牌

	//// 不需要获取用户token,子进程以父进程权限运行
	WTSQueryUserToken(dwSessionId, &hUserToken);
	dwCreationFlags |= NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE;

	TCHAR lpDesktop[] = TEXT("winsta0\\default");
	si.lpDesktop = lpDesktop;
	//指定创建进程的窗口站，Windows下唯一可交互的窗口站就是WinSta0\Default  

	TOKEN_PRIVILEGES tp{ 0 };
	LUID luid;

	// //打开进程令牌  
	// if (!OpenProcessToken(GetCurrentProcess(),
	// 	TOKEN_ADJUST_PRIVILEGES |
	// 	TOKEN_QUERY | TOKEN_DUPLICATE |
	// 	TOKEN_ASSIGN_PRIMARY |
	// 	TOKEN_ADJUST_SESSIONID |
	// 	TOKEN_READ | TOKEN_WRITE, &hPToken)) {
	// 	return FALSE;
	// }

	// //查找DEBUG权限的UID  
	// if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
	// 	CloseHandle(hPToken);
	// 	return FALSE;
	// }

	// //设置令牌信息  
	// tp.PrivilegeCount = 1;
	// tp.Privileges[0].Luid = luid;
	// tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// //复制当前用户的令牌  
	// if (!DuplicateTokenEx(hPToken, MAXIMUM_ALLOWED, NULL,
	// 	SecurityIdentification, TokenPrimary, &hUserTokenDup)) {
	// 	CloseHandle(hPToken);
	// 	return FALSE;
	// }

	// //设置当前进程的令牌信息  
	// if (!SetTokenInformation(hUserTokenDup, TokenSessionId,
	// 	(void*)&dwSessionId, sizeof(DWORD))) {
	// 	CloseHandle(hUserTokenDup);
	// 	CloseHandle(hPToken);
	// 	return FALSE;
	// }

	// //应用令牌权限  
	// if (!AdjustTokenPrivileges(hUserTokenDup, FALSE, &tp,
	// 	sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
	// 	CloseHandle(hUserTokenDup);
	// 	CloseHandle(hPToken);
	// 	return FALSE;
	// }

	//创建进程环境块，保证环境块是在用户桌面的环境下  
	LPVOID pEnv = NULL;
	if (CreateEnvironmentBlock(&pEnv, hUserToken, TRUE)) {
		dwCreationFlags |= CREATE_UNICODE_ENVIRONMENT;
	}

	//创建用户进程  
	if (!CreateProcessAsUser(hUserToken, lpApplicationName, lpCommandLine,
		lpProcessAttributes, lpThreadAttributes, bInheritHandles,
		dwCreationFlags, lpEnvironment ? lpEnvironment : pEnv,
		lpCurrentDirectory, lpStartupInfo, lpProcessInformation))
	{
		CloseHandle(hUserToken);
		CloseHandle(hPToken);
		return FALSE;
	}

	//关闭句柄
	//CloseHandle(pi.hProcess);
	//CloseHandle(pi.hThread);
	if (pEnv) DestroyEnvironmentBlock(pEnv);
	if (hUserToken) CloseHandle(hUserToken);
	// if (hUserTokenDup) CloseHandle(hUserTokenDup);
	if (hPToken) CloseHandle(hPToken);

	return TRUE;
}
#pragma warning(pop)


int main(int argc,char*argv[]){
    if (argc < 2) return -1;
	STARTUPINFO si{};
	PROCESS_INFORMATION pi{};
	si.cb = sizeof(si);
	bool result = CreateProcessInSession(WTSGetActiveConsoleSessionId(), 0, argv[1], 0, 0, 0, 0, 0, 0, &si, &pi);
	if (result) {
		CloseHandle(pi.hThread);
		DWORD code = 0;
                // 不等待
		//WaitForSingleObject(pi.hProcess, INFINITE);
		//GetExitCodeProcess(pi.hProcess, &code);
		CloseHandle(pi.hProcess);
		return int(code);
	}
	else return GetLastError();
}

