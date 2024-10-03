// ConUI_demo1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <stdio.h>
#include <Windows.h>
using namespace std;

int main(int argc, char* argv[])
{
	// 准备创建进程
	STARTUPINFOW si{
	.cb = sizeof(STARTUPINFOW),
	.dwFlags = STARTF_USESHOWWINDOW,
	.wShowWindow = SW_HIDE,
	}; PROCESS_INFORMATION pi{};
	WCHAR cmd_path[256]{}; DWORD cmd_path_Size = 256;

	// 检查命令行
	if (argc > 1) { // 转发给cmd
		// 先获取cmd.exe的位置
		WCHAR cmdCline[] = L"cmd.exe /c exit";
		if (!CreateProcessW(NULL, cmdCline, 0, 0, 0, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, 0, 0, &si, &pi)) {
			std::cerr << "Failed to find CMD.exe -- ERROR" << GetLastError() << endl;
			return -1;
		}
		QueryFullProcessImageNameW(pi.hProcess, 0, cmd_path, &cmd_path_Size);
		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		if (!cmd_path[0]) {
			std::cerr << "Failed to find CMD.exe -- ERROR" << GetLastError() << endl;
			return -2;
		}

		// 运行转发
		si.wShowWindow = SW_NORMAL;
		wstring cl = GetCommandLineW();
		if (cl.find(L" ") != cl.npos) {
			if (INVALID_FILE_ATTRIBUTES != GetFileAttributesA(argv[1])) { // 文件存在
				cl.insert(cl.find(L" "), L" /c");
			}
		}
		if (!CreateProcessW(cmd_path, (LPWSTR)cl.c_str(), 0, 0, 0, CREATE_SUSPENDED, 0, 0, &si, &pi)) {
			std::cerr << "PROCESS PARAMTER FORWARD FAILED -- ERROR" << GetLastError() << endl;
			return -1;
		}
		DWORD code = DWORD(-1);
		ResumeThread(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &code);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return code; // 返回码转发
	}

	// == 开始= 创建管道
	SECURITY_ATTRIBUTES saAttr{};
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	HANDLE hPipeRead = NULL, hPipeWrite = NULL;
	if (!CreatePipe(&hPipeRead, &hPipeWrite, &saAttr, 0)) {
		std::cerr << "Failed to create PIPE -- ERROR" << GetLastError() << endl;
		return -1;
	}
	// == 结束= 创建管道


	// == 开始= 创建进程
	si.dwFlags |= STARTF_USESTDHANDLES;
	si.hStdOutput = hPipeWrite; // 这里只需要读取cmd的返回结果
	wchar_t szCmdLine[] = L"cmd.exe /c ver";
	if (!CreateProcessW(NULL, szCmdLine, NULL, NULL, TRUE, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, 0, 0, &si, &pi)) {
		std::cerr << "Failed to create process -- ERROR" << GetLastError() << endl;
		return -2;
	}
	// == 结束= 创建进程


	// == 开始= 获取进程信息
	QueryFullProcessImageNameW(pi.hProcess, 0, cmd_path, &cmd_path_Size);
	// == 结束= 获取进程信息

	
	// == 开始= 读取版本信息
	CloseHandle(hPipeWrite); // 关闭不需要的句柄

	char buffer[4096]{};
	DWORD dwRead = sizeof(buffer), dwReaded = 0;

	ResumeThread(pi.hThread);
	(void)ReadFile(hPipeRead, buffer, dwRead, &dwReaded, NULL); // 忽略输出的空行
	if (!ReadFile(hPipeRead, buffer, dwRead, &dwReaded, NULL)) {
		std::cerr << "Failed to read cmd version information -- ERROR" << GetLastError() << endl;
		return -3;
	}
	CloseHandle(hPipeRead); // 关闭不需要的句柄

	if (WaitForSingleObject(pi.hProcess, 10000) == WAIT_TIMEOUT) {
		TerminateProcess(pi.hProcess, ERROR_TIMEOUT);
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	// == 结束= 读取版本信息

	
	// == 开始= 伪造控制台

	SetConsoleTitleW(cmd_path);

	cout << (buffer) << "\n(c) Microsoft Corporation。保留所有权利。" << endl << endl;

	WCHAR cd[1024]{};

	while (1) {
		GetCurrentDirectoryW(1024, cd);
		wcout << cd << L">";
		ZeroMemory(&buffer, sizeof(buffer));
		cin.getline(buffer, 4096); // 最多支持4KB数据

		if (cin.fail() || (
			buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't' &&
			(buffer[4] == 0 || buffer[4] == '\n' || buffer[4] == '\r' || buffer[4] == ' ') // 防止处理大字符串时出现性能问题
			)) break; // 输入exit退出

		system(buffer); // 转发给cmd执行
		cout << endl;
	}


	// == 结束= 伪造控制台




	// 等待
	Sleep(100);

	return 0;
}

