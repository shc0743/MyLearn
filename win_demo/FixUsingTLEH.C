//: FixUsingTLEH.C
// Fix using Top-Level Exception Handler (TLEH).
#include <Windows.h>
#include <stdio.h>
HANDLE hThreads[3];

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo)
{
    printf(("Uncaught exception caught!\n"));
    printf(("Code: 0x%08X\n"), ExceptionInfo->ExceptionRecord->ExceptionCode);

    // Note:
    // In production environment you should report all
    // unexpected exceptions (usually upload to a server)

    //upload_to_server(ExceptionInfo);

    static size_t counter = 0;
    if (++counter > 100) return EXCEPTION_EXECUTE_HANDLER; // will terminate the program *immediately* (WerFaultless)
    // return EXCEPTION_CONTINUE_SEARCH; // will call the System default handler (a WerFault.exe started to dump the process)
    return EXCEPTION_CONTINUE_EXECUTION; // will REPEAT the exception UNLESS you fix it
    // To show the repeated exception, we use a counter
}

DWORD __stdcall bug1(PVOID a) {
    // try to READ
    return (DWORD)*((int *)(void *)(long long)0xcafebabe);
}
DWORD __stdcall bug2(PVOID a) {
    // try to WRITE
    *((int *)(void *)(long long)0xcafebabe) = 0xccccdddd;
    return 0;
}
DWORD __stdcall bug3(PVOID a) {
    // try to EXECUTE
    ((void (*)(void))(long long)0xcafebabe)();
    return 0; 
}
DWORD __stdcall Loader(PVOID a) {
    // start the fault.
    Sleep(100);
    ResumeThread(hThreads[0]);
    ResumeThread(hThreads[1]);
    ResumeThread(hThreads[2]);
    Sleep(60000);
    return 0;
}
int main() {
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    hThreads[0] = CreateThread(0, 0, bug1, 0, CREATE_SUSPENDED, 0);
    hThreads[1] = CreateThread(0, 0, bug2, 0, CREATE_SUSPENDED, 0);
    hThreads[2] = CreateThread(0, 0, bug3, 0, CREATE_SUSPENDED, 0);
    CreateThread(0, 0, Loader, 0, 0, 0);
    WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
    for (int i = 0; i < 3; i++) {
        CloseHandle(hThreads[i]); 
    }
    return 0;
}
//:~