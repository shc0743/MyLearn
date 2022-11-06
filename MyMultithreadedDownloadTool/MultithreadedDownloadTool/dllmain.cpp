// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "framework.h"
#include "resource.h"

HMODULE hInst;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        ::hInst = hModule;
    }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



void __stdcall OpenLicense() {
    WCHAR link[256]{};
    if (LoadStringW(hInst, IDS_STRING_LICENSE, link, 256))
    ShellExecuteW(NULL, L"open", link, L"", NULL, SW_NORMAL);
}


