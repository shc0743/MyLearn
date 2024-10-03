// MyRecreateFile_ChangeCreationTime.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include "../../resource/tool.h"
using namespace std;



/*
the utility functions are generated with https://yiyan.baidu.com
*/
// 结构用于存储文件路径和对应的创建时间  
class FileTimeInfo {public:
	std::wstring filePath;
	FILETIME creationTime{};

	// 提供比较操作以用于std::sort  
	bool operator<(const FileTimeInfo& other) const {
		return CompareFileTime(&creationTime, &other.creationTime) < 0;
	}
};
bool GetFileList(PCWSTR szDirName, std::vector<std::wstring>& output) {
	wstring szFindDirName = szDirName + L"\\*"s; // 符合FindFirstFileW规范
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(szFindDirName.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		// 无法打开目录，可能是因为路径无效或没有权限等  
		return false;
	}

	do {
		// 跳过目录本身和"."和".."项  
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			wcscmp(findFileData.cFileName, L".") != 0 &&
			wcscmp(findFileData.cFileName, L"..") != 0) {

			// 构建完整的文件路径  
			std::wstring fullPath = szDirName;
			if (fullPath.back() != L'\\') {
				fullPath += L"\\";
			}
			fullPath += findFileData.cFileName;

			// 将文件路径添加到输出向量中  
			output.push_back(fullPath);
		}
	} while (FindNextFileW(hFind, &findFileData));

	//// 检查FindNextFile是否因为到达文件末尾而返回FALSE  
	//if (GetLastError() != ERROR_NO_MORE_FILES) {
	//	// 如果不是文件末尾，则可能存在错误  
	//	FindClose(hFind);
	//	return false;
	//}

	// 关闭文件句柄  
	FindClose(hFind);
	return true;
}
// 函数用于获取文件创建时间并排序文件列表  
bool SortFileListByCreationTime(std::vector<std::wstring>& fileList) {
	// 创建一个FileTimeInfo的vector来存储文件路径和创建时间  
	std::vector<FileTimeInfo> fileTimeInfos;
	fileTimeInfos.reserve(fileList.size());

	// 遍历fileList并获取每个文件的创建时间  
	for (const auto& filePath : fileList) {
		FILETIME ftCreate, ftAccess, ftWrite;
		HANDLE hFile = CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			// 无法打开文件，可能是权限问题或其他错误  
			return false;
		}

		if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
			// 获取文件时间失败  
			CloseHandle(hFile);
			return false;
		}

		CloseHandle(hFile);

		// 将文件路径和创建时间添加到fileTimeInfos中  
		fileTimeInfos.emplace_back();
		fileTimeInfos.back().filePath = filePath;
		fileTimeInfos.back().creationTime = ftCreate;
	}

	// 使用std::sort对fileTimeInfos进行排序  
	std::sort(fileTimeInfos.begin(), fileTimeInfos.end());

	// 将排序后的文件路径重新放入fileList中  
	fileList.clear();
	for (const auto& info : fileTimeInfos) {
		fileList.push_back(info.filePath);
	}

	return true;
}
inline bool getYNInput() {
	char input;
	do {
		input = _getch(); // 获取用户输入  

		// 判断输入是否为 'y' 或 'N'  
		if (input == 'y' || input == 'Y') {
			puts("y");
			return true;
		}
		else if (input == 'N' || input == 'n') {
			puts("N");
			return false;
		}

		// 如果输入无效，打印警告
		_putch('\a'); // 打印警告音  

	} while (true); // 循环直到输入有效  
}


int main()
{
	std::cout << "请输入目录: ";
	wchar_t dir[512]{};
	wcin.getline(dir, 512);
	vector<wstring> files;
	if (!GetFileList(dir, files)) {
		cerr << "遇到错误。" << LastErrorStrA() << endl; pause
		return 1;
	}
	cout << "正在排序... ";
	if (!SortFileListByCreationTime(files)) {
		cerr << "遇到错误。" << LastErrorStrA() << endl;
		cout << "您想要忽略这些异常并继续吗? (y/N) ";
		if (!getYNInput()) return 1;
	}
	cout << "完成。" << endl;
	cout << string(30, '=') << endl;
	cout << "** 以下是排序后的文件列表:" << endl;
	DWORD written = 0;
	HANDLE so = GetStdHandle(STD_OUTPUT_HANDLE);
	for (auto& i : files) {
		WriteConsoleW(so, i.c_str(), (DWORD)i.length(), &written, 0);
		WriteConsoleW(so, L"\n", 1, &written, 0);
	}
	cout << "是否批准这些更改? (y/N) ";
	if (!getYNInput()) return ERROR_CANCELLED;
	for (auto& i : files) {
		wcout << L"正在屏蔽原文件... ";
		WriteConsoleW(so, i.c_str(), (DWORD)i.length(), &written, 0);
		WriteConsoleW(so, L"\r", 1, &written, 0);
		MoveFileW(i.c_str(), (i + L".tmp").c_str());
	}
	cout << "原文件已屏蔽，请在就绪后继续应用更改" << endl;
	cout << "是否应用这些更改? (y/N) ";
	if (!getYNInput()) return ERROR_CANCELLED;
	for (auto& i : files) {
		wcout << L"正在创建文件... ";
		WriteConsoleW(so, i.c_str(), (DWORD)i.length(), &written, 0);
		WriteConsoleW(so, L"\r", 1, &written, 0);
		FlushFileBuffers(so);
		CopyFileW((i + L".tmp").c_str(), i.c_str(), FALSE);
		Sleep(1000);
		DeleteFileW((i + L".tmp").c_str());
	}
	cout << ErrorCodeToStringA(0) << endl;
	pause;

	return 0;
}



