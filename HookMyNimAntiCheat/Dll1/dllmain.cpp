// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "../utils/hook.h"
#include "../../resource/tool.h"


DWORD
WINAPI
GetWindowThreadProcessId2(
    _In_ HWND hWnd,
    _Out_opt_ LPDWORD lpdwProcessId)
{
    if (lpdwProcessId != NULL) {
        *lpdwProcessId = GetCurrentProcessId();
    }
    return GetCurrentThreadId();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ApiInlineHook(L"user32.dll", codetostr(GetWindowThreadProcessId), GetWindowThreadProcessId2);
        break;
    case DLL_PROCESS_DETACH:
        ExitProcess(-1);
        break;
    default:;
    }
    return TRUE;
}

