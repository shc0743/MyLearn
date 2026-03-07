#include <Windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <conio.h>

using namespace std;


string ws2s(const wstring wstr) {
    string result;
    size_t len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
        (int)(wstr.size()), NULL, 0, NULL, NULL);
    if (len < 0) return result;
    char* buffer = new char[len + 1];
    if (buffer == NULL) return result;
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
        (int)(wstr.size()), buffer, (int)len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}
wstring s2ws(const string str) {
    wstring result;
    size_t len = MultiByteToWideChar(CP_ACP, 0, str.c_str(),
        (int)(str.size()), NULL, 0);
    if (len < 0) return result;
    wchar_t* buffer = new wchar_t[len + 1];
    if (buffer == NULL) return result;
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)(str.size()),
        buffer, (int)len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}


// 注入 DLL 到目标进程
bool InjectDll(HANDLE hProcess, const std::wstring& dllPath) {
    // 1. 在目标进程中分配内存，用于存储 DLL 路径
    SIZE_T pathLen = (dllPath.size() + 1) * sizeof(WCHAR);
    LPVOID remoteMem = VirtualAllocEx(hProcess, NULL, pathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteMem) return false;

    // 2. 写入 DLL 路径到目标进程
    if (!WriteProcessMemory(hProcess, remoteMem, dllPath.c_str(), pathLen, NULL)) {
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    // 3. 获取 LoadLibraryA 地址（kernel32.dll 在所有进程中地址一致）
    LPVOID loadLibraryAddr = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
    if (!loadLibraryAddr) {
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    // 4. 创建远程线程执行 LoadLibraryA
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, remoteMem, 0, NULL);
    if (!hThread) {
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        return false;
    }

    // 5. 等待注入完成
    WaitForSingleObject(hThread, INFINITE);

    // 6. 清理
    CloseHandle(hThread);
    VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
    return true;
}

int _tmain(int argc, _TCHAR* argv[]) {
    // 1. 获取输入
    std::string targetPath;
    std::cout << "Enter target program path: ";
    std::getline(std::cin, targetPath);

    std::string blockPidStr;
    std::cout << "Enter PID to block: ";
    std::cin >> blockPidStr;

    // 2. 设置环境变量（传递给目标进程及其子进程）
    SetEnvironmentVariableA("BLOCK_PID", blockPidStr.c_str());

    // 3. 获取当前 DLL 的完整路径（假设 Injector.exe 和 ProtectPID.dll 在同一目录）
    char dllPath[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, dllPath, MAX_PATH);
    std::string injectorDir = dllPath;
    size_t pos = injectorDir.find_last_of("\\");
    if (pos != std::string::npos) {
        injectorDir = injectorDir.substr(0, pos + 1);
    }
    std::string fullDllPath = injectorDir + "Protect.dll";

    // 4. 以 CREATE_SUSPENDED 创建目标进程
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    wstring targetPathW = s2ws(targetPath);
    std::unique_ptr<WCHAR[]> targetPathW_ptr(new WCHAR[targetPathW.size() + 1]);
    wcscpy_s(targetPathW_ptr.get(), targetPathW.size() + 1, targetPathW.c_str());
    if (!CreateProcessW(
        NULL,
        targetPathW_ptr.get(),
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        &pi
    )) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        return 1;
    }

    std::cout << "Target process created (PID: " << pi.dwProcessId << "), injecting DLL..." << std::endl;

    // 5. 注入 DLL
    if (!InjectDll(pi.hProcess, s2ws(fullDllPath))) {
        std::cerr << "DLL injection failed: " << GetLastError() << std::endl;
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }

    std::cout << "DLL injected successfully. Resuming target process..." << std::endl;

    // 6. 恢复目标进程主线程
    ResumeThread(pi.hThread);

    // 7. 清理
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    //std::cout << "Press any key to exit..." << std::endl;
    //_getch();
    return 0;
}