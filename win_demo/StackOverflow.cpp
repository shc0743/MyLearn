#define UNICODE 1
#define _UNICODE 1
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <iostream>
#include <thread>
#pragma comment(lib, "user32.lib")

using namespace std;

int fn(int a) {
    return fn(a + 1);
}

// Maximium of about 21,477 Stack frames

// 被调试进程函数
void DebuggeeProcess() {
    fn(0);
}

// 调试器进程函数
void DebuggerProcess() {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    TCHAR szPath[MAX_PATH];
    TCHAR szCmdLine[MAX_PATH] = _T("p x"); // 传递参数x表示是被调试进程
    
    // 获取当前可执行文件路径
    GetModuleFileName(NULL, szPath, MAX_PATH);
    
    // 创建被调试进程
    if (!CreateProcess(szPath, szCmdLine, NULL, NULL, FALSE, 
                      DEBUG_ONLY_THIS_PROCESS | CREATE_SUSPENDED, NULL, NULL, &si, &pi))
    {
        MessageBoxW(NULL, L"CreateProcess failed!", NULL, MB_ICONERROR);
        return;
    }
    
    // 调试循环
    DEBUG_EVENT debugEvent;
    BOOL bContinueDebugging = TRUE;
    DWORD dwContinueStatus = DBG_CONTINUE;

    cout << "Process created, PID: " << pi.dwProcessId << ", hProcess: " << pi.hProcess << endl;
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);

    bool ss = false;
    std::thread tDaemon([pi, &ss] {
        WaitForSingleObject(pi.hProcess, INFINITE);
        Sleep(100);
        if (ss) return;
        cout << "Debuggee process died (unexpectedly), PID: " << pi.dwProcessId << endl;
        ExitProcess(1);
    });

    while (bContinueDebugging)
    {
        // 等待调试事件
        // cout << "Waiting for debug event..." << endl;

        if (!WaitForDebugEvent(&debugEvent, INFINITE))
        {
            MessageBoxW(NULL, L"WaitForDebugEvent failed!", NULL, MB_ICONERROR);
            break;
        }

        // cout << "Debug Event! " << debugEvent.dwDebugEventCode << endl;
        
        switch (debugEvent.dwDebugEventCode)
        {
            case EXCEPTION_DEBUG_EVENT:
                // 遇到异常事件
                cout << "Exception Code: " << debugEvent.u.Exception.ExceptionRecord.ExceptionCode << endl;
                if (debugEvent.u.Exception.ExceptionRecord.ExceptionCode > 0xC0000000) {
                    // real exception
                    MessageBoxW(NULL, std::to_wstring(debugEvent.u.Exception.ExceptionRecord.ExceptionCode).c_str(), L"Exception!", MB_ICONERROR);
                    bContinueDebugging = FALSE;
                    dwContinueStatus = DBG_EXCEPTION_NOT_HANDLED;
                    DebugActiveProcessStop(pi.dwProcessId);
                }
                break;
                
            case CREATE_PROCESS_DEBUG_EVENT:
                CloseHandle(debugEvent.u.CreateProcessInfo.hFile);
                break;
                
            case EXIT_PROCESS_DEBUG_EVENT:
                bContinueDebugging = FALSE;
                break;
                
            default:
                break;
        }
        
        ContinueDebugEvent(debugEvent.dwProcessId, 
                          debugEvent.dwThreadId, 
                          dwContinueStatus);
    }

    ss = true;
    CloseHandle(pi.hProcess);
    tDaemon.join();
}

int _tmain(int argc, TCHAR* argv[])
{
    if (argc > 1 && _tcscmp(argv[1], _T("x")) == 0)
    {
        // 被调试进程
        DebuggeeProcess();
    }
    else
    {
        // 调试器进程
        DebuggerProcess();
    }
    
    return 0;
}
