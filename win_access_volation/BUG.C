//: BUG.C
// Demonstration of access violation.
// {ThrowsException}
#include <Windows.h>

HANDLE hThreads[3];

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