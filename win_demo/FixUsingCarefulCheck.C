//: FixUsingCarefulCheck.C
// Check the address carefully.
#include <Windows.h>

HANDLE hThreads[2];

DWORD __stdcall bug1(PVOID a) {
    if (!a) return 1;
    // try to READ
    return (DWORD)*((int *)a);
}
DWORD __stdcall bug2(PVOID a) {
    if (!a) return 1;
    // try to WRITE
    *((int *)a) = 0xccccdddd;
    return 0;
}
DWORD __stdcall Loader(PVOID a) {
    // start the fault.
    Sleep(100);
    ResumeThread(hThreads[0]);
    ResumeThread(hThreads[1]);
    Sleep(60000);
    return 0;
}
int main() {
    hThreads[0] = CreateThread(0, 0, bug1, 0, CREATE_SUSPENDED, 0);
    hThreads[1] = CreateThread(0, 0, bug2, 0, CREATE_SUSPENDED, 0);
    CreateThread(0, 0, Loader, 0, 0, 0);
    WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);
    for (int i = 0; i < 2; i++) {
        CloseHandle(hThreads[i]); 
    }
    return 0;
}
//:~