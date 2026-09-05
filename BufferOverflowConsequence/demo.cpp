#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <conio.h>
#include <Windows.h>
using namespace std;

const uint8_t Return0[]{
#ifdef _WIN64
    0x48, 0x31, 0xC0, 0xC3,
#else
    0x55, 0x8B, 0xEC, 0x31, 0xC0, 0x5D, 0xC3,
#endif
};

int main() {
    struct unnamed_struct_1 {
        CHAR userFile[256 + 8];
        HANDLE hFile;
        uint8_t fileDataBuffer[2048];
        uint8_t padding1[1776];

        uint8_t NextCommand[4096];
    };
    cout << "Welcome!!! Please wait for me to get some memory!!" << endl;
    auto RuntimeData = (unnamed_struct_1*)VirtualAlloc(0, sizeof(unnamed_struct_1), MEM_COMMIT, PAGE_READWRITE);
    if (!RuntimeData) {
        cerr << "Sorry!!! I cannot get memory!!" << endl;
        return 1;
    }
    memset(RuntimeData, 0, sizeof(unnamed_struct_1));
    memcpy(RuntimeData->NextCommand, Return0, sizeof(Return0));
    cout << "Thank you!" << endl;
    int returnCode{};
    goto start;

end:
    if (RuntimeData->hFile) CloseHandle(RuntimeData->hFile);
    VirtualFree(RuntimeData, 0, MEM_RELEASE);
    cout << "Press any key to exit!" << endl;
    (void)_getch();
    return returnCode;

    start:
    cout << "Please input a file path so that I can open it!! You can input up to 259 chars, the more will be ignored!!" << endl;
    cin.getline(RuntimeData->userFile, 260);
    cout << "Thank you! I am going to read it!" << endl;
    RuntimeData->hFile = CreateFileA(RuntimeData->userFile, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (RuntimeData->hFile == INVALID_HANDLE_VALUE || !RuntimeData->hFile) {
        RuntimeData->hFile = 0;
        cerr << "Sorry!!! I cannot open the file specified!!" << endl;
        cerr << "The error is " << GetLastError() << "!" << endl;
        goto end;
    }
    cout << "I have opened the file!! Let me read it now!!!" << endl;
    cout.flush();
    DWORD got{};
    if (!ReadFile(RuntimeData->hFile, RuntimeData->fileDataBuffer, 7920, &got, 0)) {
        cerr << "Sorry!!! I cannot read the file specified!!" << endl;
        cerr << "The error is " << GetLastError() << "!" << endl;
        goto end;
    }
    cout << "Thank you!! I have read " << got << " bytes!!!" << endl;
    DWORD flOldProt{};
    VirtualProtect(RuntimeData->NextCommand, 4096, PAGE_EXECUTE_READ, &flOldProt);
    FlushInstructionCache(GetCurrentProcess(), RuntimeData->NextCommand, 4096);
    cout << "Press any key to continue!" << endl;
    (void)_getch();
    cout << "Finally I will get the exit code!!!" << endl;
    cout.flush();

    returnCode = ((int(__stdcall*)())(PVOID)RuntimeData->NextCommand)();
    cout << "I have gotten the exit code!!!" << endl;
    cout.flush();

    goto end;
}