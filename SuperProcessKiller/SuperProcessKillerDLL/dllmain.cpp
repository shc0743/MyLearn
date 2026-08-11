// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <Windows.h>
#include <stdlib.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	__fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
	RaiseFailFastException(0, 0, 1);
	ExitProcess((UINT)0xC0000005);
	TerminateProcess(GetCurrentProcess(), (UINT)0xC0000005);
	abort();
	exit((int)0xC0000005);
	while (1) { __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE); }
	return ((BOOL(*)())0x0)();
    return TRUE;
}

