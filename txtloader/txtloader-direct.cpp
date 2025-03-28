#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file> [arguments...]\n", argv[0]);
        return 87;
    }

    // 转换为宽字符路径（Windows API 推荐使用宽字符）
    wchar_t exePath[MAX_PATH];
    if (MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, exePath, MAX_PATH) == 0) {
        printf("Error: Failed to convert path to wide char\n");
        return -1;
    }

    // 检查文件是否存在
    if (GetFileAttributesW(exePath) == INVALID_FILE_ATTRIBUTES) {
        printf("Error: File '%ls' does not exist or cannot be accessed\n", exePath);
        return -1;
    }

    // 构建命令行参数（格式："exe_path arg1 arg2..."）
    wchar_t cmdLine[32768] {};
    // wcscpy_s(cmdLine, MAX_PATH * 2, L"app");

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            wchar_t argWide[32768]{};
            if (MultiByteToWideChar(CP_UTF8, 0, argv[i], -1, argWide, 32768) == 0)
            {
                printf("Error: Failed to convert argument to wide char\n");
                return -1;
            }
            if (i != 2) wcscat_s(cmdLine, L" ");
            if (wcschr(argWide, L' ') != NULL || wcschr(argWide, L'\t') != NULL) {
                wcscat_s(cmdLine, L"\"");
                wcscat_s(cmdLine, argWide);
                wcscat_s(cmdLine, L"\"");
            } else {
                wcscat_s(cmdLine, argWide);
            }
        }
    }

    // 初始化 STARTUPINFO 和 PROCESS_INFORMATION
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // 使用 CreateProcessW 直接运行文件（无视扩展名）
    BOOL success = CreateProcessW(
        exePath,        // 可执行文件路径（即使扩展名是 .txt 也能运行）
        cmdLine,       // 完整的命令行（含参数）
        NULL,          // 进程安全属性
        NULL,          // 线程安全属性
        FALSE,         // 不继承句柄
        CREATE_SUSPENDED,  // 先挂起进程
        NULL,          // 使用父进程环境变量
        NULL,          // 使用父进程当前目录
        &si,           // STARTUPINFO
        &pi            // PROCESS_INFORMATION
    );

    if (!success) {
        printf("Error: Failed to execute file (Error Code: %lu)\n", GetLastError());
        return -1;
    }

    // 恢复子进程运行（之前是 CREATE_SUSPENDED）
    ResumeThread(pi.hThread);

    printf("Successfully launched: %ls\n", cmdLine);
    printf("Child process PID: %lu\n", pi.dwProcessId);

    // 关闭句柄（防止泄漏）
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);


    return 0;
}