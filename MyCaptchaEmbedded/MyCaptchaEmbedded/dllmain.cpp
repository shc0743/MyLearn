// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"

#include "InitAPI.h"



HMODULE hInst;



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    BOOL bResult = FALSE;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::hInst = hModule;
        bResult = Initialize(nullptr);
        break;
    case DLL_THREAD_ATTACH:
        bResult = Initialize_Thread(nullptr);
        break;
    case DLL_THREAD_DETACH:
        bResult = UnInitialize_Thread(nullptr);
        break;
    case DLL_PROCESS_DETACH:
        bResult = UnInitialize(nullptr);
        break;
    }
    return bResult;
}


