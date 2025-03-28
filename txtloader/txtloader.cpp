#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <ShObjIdl.h>
#include <Shlwapi.h>
using namespace std;

#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma region utility
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
wstring& str_replace(std::wstring& strBase,
	const std::wstring strSrc,const std::wstring strDes){
	wstring::size_type pos = 0;
	wstring::size_type srcLen = strSrc.size();
	wstring::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while ((pos != wstring::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, (pos + desLen));
	}
	return strBase;
}
// Translate a error-code such as GetLastError() to std::wstring
std::wstring ErrorCodeToStringW(DWORD ErrorCode);
// Translate a error-code such as GetLastError() to std::string
std::string  ErrorCodeToStringA(DWORD ErrorCode);
inline std::wstring LastErrorStrW() {
    auto error = GetLastError();
    auto result = ErrorCodeToStringW(error);
    SetLastError(error);
    return result;
}
#pragma endregion

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // printf("Usage: %s <file> [arguments...]\n", argv[0]);
        TaskDialog(NULL, NULL, L"Error: Invalid Parameter", L"Error: Invalid Parameter", 
            L"Usage: [loader|:s] <file> [arguments...]\n\nIf argv[0] is :s, then all "
            "TaskDialog will be blocked. Use the following syntax to do it:\n"
            "loader.exe loader.exe :s", TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, NULL);
        return ERROR_INVALID_PARAMETER;
    }

	bool showTaskDialog = true;
	if (argv[0] == ":s"s) showTaskDialog = false;

    // 转换为宽字符路径（Windows API 推荐使用宽字符）
    wstring exePath = s2ws(argv[1]);

    // 构建命令行参数（格式："exe_path arg1 arg2..."）
    // wchar_t cmdLine[32768] {};
    wstring cmdLine;
    // wcscpy_s(cmdLine, MAX_PATH * 2, L"app");

    if (argc > 2) {
        for (int i = 2; i < argc; i++) {
            wstring argWide = s2ws(argv[i]);
            if (i != 2) cmdLine += L" ";

            // 转义双引号
            bool shouldQuote = argWide.find_first_of(L" ") != wstring::npos || argWide.find_first_of(L"\t") != wstring::npos;
            if (argWide.find(L"\"") != wstring::npos) {
                argWide = str_replace(argWide, L"\"", L"\"\"");
                shouldQuote = true;
            }

            // 参数中包含空格或制表符时，用双引号括起来
            if (shouldQuote) {
                cmdLine += L"\"";
                cmdLine += argWide;
                cmdLine += L"\"";
            } else {
                cmdLine += argWide;
            }
        }
    }

    // 创建 Job 对象（用于强制终止子进程）
    HANDLE hJob = CreateJobObjectW(NULL, NULL);
    if (hJob == NULL) {
        printf("Error: Failed to create job object (Error Code: %lu)\n", GetLastError());
        return -1;
    }

    // 设置 Job 对象属性：Loader 退出时强制终止子进程
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo = {0};
    jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jobInfo, sizeof(jobInfo))) {
        printf("Error: Failed to set job object limits (Error Code: %lu)\n", GetLastError());
        CloseHandle(hJob);
        return -2;
    }

    // 初始化 STARTUPINFO 和 PROCESS_INFORMATION
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    // 使用 CreateProcessW 直接运行文件（无视扩展名）
	wchar_t* cl = new wchar_t[cmdLine.size() + 1];
	wcscpy(cl, cmdLine.c_str());
    BOOL success = CreateProcessW(
        exePath.c_str(),        // 可执行文件路径（即使扩展名是 .txt 也能运行）
        cl,       // 完整的命令行（含参数）
        NULL,          // 进程安全属性
        NULL,          // 线程安全属性
        FALSE,         // 不继承句柄
        CREATE_SUSPENDED,  // 先挂起进程，以便关联 Job 对象
        NULL,          // 使用父进程环境变量
        NULL,          // 使用父进程当前目录
        &si,           // STARTUPINFO
        &pi            // PROCESS_INFORMATION
    );

    if (!success) {
		DWORD ec = GetLastError();
        if (showTaskDialog) TaskDialog(NULL, NULL, L"Error: Failed to Execute File", LastErrorStrW().c_str(),
            (L"Error Code: " + to_wstring(ec)).c_str(), TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, NULL);
        CloseHandle(hJob);
        delete[] cl;
        return ec;
    }

    // 将子进程关联到 Job 对象
    if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
		DWORD ec = GetLastError();
		if (showTaskDialog) TaskDialog(NULL, NULL, L"Error: Failed to Assign Process to Job Object", LastErrorStrW().c_str(),
			(L"Error Code: " + to_wstring(ec)).c_str(), TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, NULL);
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hJob);
        delete[] cl;
        return ec;
    }

    // 恢复子进程运行（之前是 CREATE_SUSPENDED）
    ResumeThread(pi.hThread);

    printf("Child process PID: %lu\n", pi.dwProcessId);

    // 等待子进程退出
    printf("Waiting for child process to exit...\n");
    WaitForSingleObject(pi.hProcess, INFINITE);

    // 获取子进程退出码
    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    // printf("Child process exited with code: %lu\n", exitCode);

    // 关闭句柄（防止泄漏）
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hJob);
	delete[] cl;

    return exitCode;
}



#pragma region util
wstring ErrorCodeToStringW(DWORD ErrorCode)
{
    typedef LPWSTR str_t;
    str_t LocalAddress = NULL;
    if (NULL == ([&] {
        __try {
            if (!FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_IGNORE_INSERTS |
                FORMAT_MESSAGE_FROM_SYSTEM, NULL,
                ErrorCode, 0, (str_t)&LocalAddress, 0, NULL)) {
                return (str_t)NULL;
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER) { return (str_t)NULL; }
        return LocalAddress;
        })()) {
        SetLastError(ErrorCode);
        return L"";
    }
    LocalAddress[wcslen((str_t)LocalAddress) - 1] = 0;
    wstring szText = LocalAddress;
    LocalFree((HLOCAL)LocalAddress);
    SetLastError(ErrorCode);
    return szText;
}
#pragma endregion
