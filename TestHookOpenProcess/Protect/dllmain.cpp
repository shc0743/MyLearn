#include <Windows.h>
#include "../include/detours.h"
#include <string>
#include <tchar.h>
#include <wchar.h>
#include <cstdlib>

#ifdef _WIN64
#pragma comment(lib, "../lib-x64/detours.lib")
#else
//#pragma comment(lib, "detours32.lib")
#endif

// 全局变量：DLL 模块句柄
HMODULE g_hModule = NULL;

// ==========================================
// 1. OpenProcess 拦截
// ==========================================
typedef HANDLE(WINAPI* OpenProcess_t)(DWORD, BOOL, DWORD);
OpenProcess_t TrueOpenProcess = OpenProcess;

HANDLE WINAPI MyOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
    char blockPidStr[32] = { 0 };
    GetEnvironmentVariableA("BLOCK_PID", blockPidStr, 32);
    DWORD blockPid = atoi(blockPidStr);

    if (dwProcessId == blockPid && blockPid) {
        dwDesiredAccess = PROCESS_QUERY_LIMITED_INFORMATION;
    }

    return TrueOpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
}

// ==========================================
// 2. CreateProcessA/W 拦截（子进程注入）
// ==========================================
typedef BOOL(WINAPI* CreateProcessA_t)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);
CreateProcessA_t TrueCreateProcessA = CreateProcessA;

typedef BOOL(WINAPI* CreateProcessW_t)(LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
CreateProcessW_t TrueCreateProcessW = CreateProcessW;

// 辅助函数：注入 DLL 到子进程
void InjectToChild(HANDLE hProcess) {
    WCHAR dllPath[MAX_PATH]{};
    GetModuleFileNameW(g_hModule, dllPath, MAX_PATH);

    SIZE_T pathLen = (wcslen(dllPath) + 1) * sizeof(WCHAR);
    LPVOID remoteMem = VirtualAllocEx(hProcess, NULL, pathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    bool ok = false;
    if (remoteMem) {
        WriteProcessMemory(hProcess, remoteMem, dllPath, pathLen, NULL);
        LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryW");
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteMem, 0, NULL);
        if (hThread) {
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
            ok = true;
        }
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    }
    if (!ok) TerminateProcess(hProcess, STATUS_ACCESS_VIOLATION);
}

BOOL WINAPI MyCreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation) {
    // 先以 CREATE_SUSPENDED 创建子进程
    DWORD newFlags = dwCreationFlags | CREATE_SUSPENDED;
    BOOL result = TrueCreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, newFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

    if (result) {
        // 注入 DLL 到子进程
        InjectToChild(lpProcessInformation->hProcess);
        // 恢复子进程主线程
        if (!(dwCreationFlags & CREATE_SUSPENDED)) 
            ResumeThread(lpProcessInformation->hThread);
    }

    return result;
}

BOOL WINAPI MyCreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation) {
    // 逻辑同 MyCreateProcessA，只是使用宽字符
    DWORD newFlags = dwCreationFlags | CREATE_SUSPENDED;
    BOOL result = TrueCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, newFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

    if (result) {
        InjectToChild(lpProcessInformation->hProcess);
        if (!(dwCreationFlags & CREATE_SUSPENDED))
            ResumeThread(lpProcessInformation->hThread);
    }

    return result;
}

// ==========================================
// 3. DLL 入口点
// ==========================================
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);

        // 安装 Detours 拦截
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)TrueOpenProcess, MyOpenProcess);
        DetourAttach(&(PVOID&)TrueCreateProcessA, MyCreateProcessA);
        DetourAttach(&(PVOID&)TrueCreateProcessW, MyCreateProcessW);
        DetourTransactionCommit();
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        // 卸载 Detours 拦截
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)TrueOpenProcess, MyOpenProcess);
        DetourDetach(&(PVOID&)TrueCreateProcessA, MyCreateProcessA);
        DetourDetach(&(PVOID&)TrueCreateProcessW, MyCreateProcessW);
        DetourTransactionCommit();
    }
    return TRUE;
}