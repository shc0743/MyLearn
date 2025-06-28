//: FixUsingAdvancedWin32Verification.C
// Using XxxxProcessMemory to *safely* operate memory from a process.
#include <Windows.h>

HANDLE hThreads[3];

BOOL memory_canread(void* address) {
    int value;
    SIZE_T bytesRead;
    // Attempt to read memory at the specified address
    if (!ReadProcessMemory(GetCurrentProcess(), address, &value, sizeof(value), &bytesRead)) {
        return FALSE;
    }
    return TRUE;
}
BOOL memory_canwrite(void* address, int new_value) {
    SIZE_T bytesWritten;
    // Attempt to write memory at the specified address
    if (!WriteProcessMemory(GetCurrentProcess(), address, &new_value, sizeof(new_value), &bytesWritten)) {
        return FALSE;
    }
    return TRUE;
}
BOOL memory_canexecute(void* address) {
    if (!memory_canread(address)) return FALSE;
    // Get the current protection of the memory region
    DWORD oldProtect = 0, op2 = 0;
    if (!VirtualProtectEx(GetCurrentProcess(), address, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return FALSE;
    }
    // Restore the original protection of the memory region
    if (!VirtualProtectEx(GetCurrentProcess(), address, sizeof(DWORD), oldProtect, &op2)) {
        return FALSE;
    }
    return (oldProtect & 0x10) != 0;
}

DWORD __stdcall bugless1(PVOID a) {
    if (!memory_canread((void *)(long long)0xcafebabe)) {
        return 1;
    }
    // try to READ
    return (DWORD)*((int *)(void *)(long long)0xcafebabe);
}
DWORD __stdcall bugless2(PVOID a) {
    // try to WRITE
    if (memory_canwrite((int *)(void *)(long long)0xcafebabe, 0xccccdddd)) return 0;
    else return 1;
}
DWORD __stdcall bugless3(PVOID a) {
    if (!memory_canexecute((void *)(long long)0xcafebabe)) {
        return 1;
    }
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
    hThreads[0] = CreateThread(0, 0, bugless1, 0, CREATE_SUSPENDED, 0);
    hThreads[1] = CreateThread(0, 0, bugless2, 0, CREATE_SUSPENDED, 0);
    hThreads[2] = CreateThread(0, 0, bugless3, 0, CREATE_SUSPENDED, 0);
    CreateThread(0, 0, Loader, 0, 0, 0);
    WaitForMultipleObjects(3, hThreads, TRUE, INFINITE);
    for (int i = 0; i < 3; i++) {
        CloseHandle(hThreads[i]); 
    }
    return 0;
}
//:~