#include <Windows.h>

void fuck() {
    PROCESS_MITIGATION_CHILD_PROCESS_POLICY f{};
    f.NoChildProcessCreation = 1;
	SetProcessMitigationPolicy(ProcessChildProcessPolicy, &f, sizeof f) || TerminateProcess(GetCurrentProcess(), GetLastError());
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#ifdef AUTOMODE
        fuck();
#endif
        break;
    default:
        break;
    }
    return TRUE;
}

