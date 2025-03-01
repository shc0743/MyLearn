#include <Windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>
#include <string>
using namespace std;

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "comdlg32.lib")

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// 控制台输出处理函数
void ConsoleOutput(const wchar_t *message)
{
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE)
        {
            DWORD written;
            WriteConsoleW(hConsole, message, wcslen(message), &written, nullptr);
            WriteConsoleW(hConsole, L"\n", 1, &written, nullptr);
        }
        FreeConsole();
    }
}

// 通用文件对话框封装
bool GetFilePath(bool openDialog, wchar_t *buffer, const wchar_t *title)
{
    OPENFILENAMEW ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_EXPLORER | (openDialog ? OFN_PATHMUSTEXIST : OFN_OVERWRITEPROMPT);
    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.lpstrDefExt = L"";
    return openDialog ? GetOpenFileNameW(&ofn) : GetSaveFileNameW(&ofn);
}

// 显示任务对话框
void ShowResultDialog(bool success, const wchar_t *details)
{
    TASKDIALOGCONFIG tdc = {0};
    const TASKDIALOG_BUTTON buttons[] = {{IDOK, L"确定"}};

    tdc.cbSize = sizeof(tdc);
    tdc.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION;
    tdc.pszWindowTitle = L"创建符号链接";
    tdc.pszMainIcon = success ? TD_INFORMATION_ICON : TD_ERROR_ICON;
    tdc.pszMainInstruction = success ? L"符号链接已创建。" : L"无法完成操作。";
    tdc.pszContent = details;
    tdc.cButtons = _countof(buttons);
    tdc.pButtons = buttons;

    TaskDialogIndirect(&tdc, nullptr, nullptr, nullptr);
}

bool IsRunAsAdmin() //判断是否管理员模式
{
	BOOL bElevated = false;
	HANDLE hToken = NULL;
	// Get current process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return false;
	TOKEN_ELEVATION tokenEle = {0};
	DWORD dwRetLen = 0;
	// Retrieve token elevation information
	if (GetTokenInformation(hToken, TokenElevation,
		&tokenEle, sizeof(tokenEle), &dwRetLen))
	{
		if (dwRetLen == sizeof(tokenEle))
		{
			bElevated = tokenEle.TokenIsElevated;
		}
	}
	CloseHandle(hToken);
	return bElevated;
}

// 主程序逻辑
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
    // 初始化通用控件
    INITCOMMONCONTROLSEX icc = {sizeof(icc), ICC_STANDARD_CLASSES};
    InitCommonControlsEx(&icc);

    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    bool consoleMode = (argc == 3); // 两个参数模式

    wchar_t sourcePath[MAX_PATH] = {0};
    wchar_t targetPath[MAX_PATH] = {0};

    // 参数处理逻辑
    if (argc < 2)
    {
        if (!GetFilePath(true, sourcePath, L"选择源文件/目录"))
            return 1;
        if (!GetFilePath(false, targetPath, L"选择符号链接保存位置"))
            return 1;
    }
    else if (argc == 2)
    {
        wcscpy_s(sourcePath, argv[1]);
        if (!GetFilePath(false, targetPath, L"选择符号链接保存位置"))
            return 1;
    }
    else if (argc == 3)
    {
        wcscpy_s(sourcePath, argv[1]);
        wcscpy_s(targetPath, argv[2]);
    }
    else
    {
        if (consoleMode)
            ConsoleOutput(L"错误：参数数量不正确");
        else
            ShowResultDialog(false, L"参数数量不正确");
        return 1;
    }

    // 验证源文件
    DWORD srcAttrib = GetFileAttributesW(sourcePath);
    if (srcAttrib == INVALID_FILE_ATTRIBUTES)
        srcAttrib = 0;
        
    // 创建符号链接
    DWORD flags = (srcAttrib & FILE_ATTRIBUTE_DIRECTORY) ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0;
    flags |= SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE;
    bool success = CreateSymbolicLinkW(targetPath, sourcePath, flags);

    // 结果处理
    if (success)
    {
        if (consoleMode)
            ConsoleOutput(L"符号链接创建成功");
        else
            ShowResultDialog(true, L"符号链接已成功创建");
    }
    else
    {
        if (!IsRunAsAdmin()) {
            wstring cl = (L"\""s + sourcePath + L"\" \"" + targetPath + L"\"");
            SHELLEXECUTEINFOW sei = {sizeof(sei)};
            sei.lpFile = argv[0];
            sei.lpParameters = cl.c_str();
            sei.lpVerb = L"runas";
            sei.fMask = SEE_MASK_NOCLOSEPROCESS;
            if(ShellExecuteExW(&sei)){
                DWORD success = -1;
                WaitForSingleObject(sei.hProcess, INFINITE);
                GetExitCodeProcess(sei.hProcess, (DWORD*)&success);
                CloseHandle(sei.hProcess);
                if (success == 0) {
                    if (consoleMode)
                        ConsoleOutput(L"符号链接创建成功");
                    else
                        ShowResultDialog(true, L"符号链接已成功创建");
                }
                return success;
            }
        }

        DWORD err = GetLastError();
        wchar_t details[256];
        swprintf_s(details, L"错误代码: 0x%08lX", err);

        if (consoleMode)
        {
            wchar_t consoleMsg[512];
            swprintf_s(consoleMsg, L"创建失败 - %s", details);
            ConsoleOutput(consoleMsg);
        }
        else
        {
            ShowResultDialog(false, details);
        }
    }

    LocalFree(argv);
    return success ? 0 : 1;
}