// Demo_Spacefill.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <Windows.h>
#include "../../resource/tool.h"
using namespace std;
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "Rpcrt4.lib")

DWORD CALLBACK func() {
	HANDLE hThread = CreateThread(0, 0, [](PVOID)->DWORD {
		return 0;
	}, 0, 0, 0);
	if (hThread) {
		DWORD v = 0;
		WaitForSingleObject(hThread, INFINITE);
		GetExitCodeThread(hThread, &v);
		return v;
	}
	return GetLastError();
}

int main()
{
	std::cout << "Disk (don't input \":\"): ";
	wchar_t buffer[64]{};
	char bufferc[64]{};
	DWORD written = 0;
	//ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &written, 0);
	cin.getline(bufferc, 10);
	wcscpy_s(buffer, s2ws(bufferc).c_str());
	if (!buffer[0]) return ERROR_CANCELLED;

	HANDLE hFile = NULL;
	size_t index = 0;
	wstring path = buffer;
	path += L":\\";
	SetCurrentDirectoryW(path.c_str());

	cout << "Create data directory... ";
	wstring dir = GenerateUUIDW();
	CreateDirectoryW(dir.c_str(), NULL);
	if (!SetCurrentDirectoryW((path + dir).c_str())) {
		cerr << "Error: " << GetLastError() << endl;
		wcin.getline(buffer, 1);
		return -1;
	}
	cout << "OK. " << endl;
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), dir.c_str(), (DWORD)dir.length(), &written, 0);
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", 1, &written, 0);

	cout << "Init memory buffer... ";
	constexpr size_t DATA_SIZE = static_cast<size_t>(64) * 1024 * 1024;
	constexpr size_t CHUNK_SIZE = static_cast<size_t>(8) * 1024 * 1024 * 1024;
	BYTE* data = new BYTE[DATA_SIZE];
	ZeroMemory(data, sizeof(BYTE) * DATA_SIZE);
	cout << "OK. " << endl;
	//pause;

	DWORD toWrite = (DWORD)DATA_SIZE;
	written = 0;
	size_t totalWritten;
	bool succ = false;

	while (1) {
		totalWritten = 0;
		wstring filename = to_wstring(++index);
		hFile = CreateFileW(filename.c_str(), GENERIC_ALL, FILE_SHARE_READ,
			0, CREATE_ALWAYS, 0, 0);
		if (!hFile || hFile == INVALID_HANDLE_VALUE) break;

		//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), filename.c_str(), (DWORD)filename.length(), &written, 0);
		cout << "Writing file: " << ws2s(path + dir + L"\\" + filename) << " \r";

		succ = WriteFile(hFile, data, toWrite, &written, 0);
		if (!succ || (toWrite != written)) { // no enough space
			const auto lambda1 = [&] (DWORD towr) {
				do {
					succ = WriteFile(hFile, data, towr, &written, 0);
					if (towr != written) break;
				} while (succ);
			};
			succ = true;
			lambda1(1024 * 1024);
			lambda1(64 * 1024);
			lambda1(1024);
			lambda1(64);
			lambda1(1);
			CloseHandle(hFile);
			break;
		}

		totalWritten += written;

		do {
			succ = WriteFile(hFile, data, toWrite, &written, 0);
			if (!succ || (toWrite != written)) {
				break;
			}
			totalWritten += written;
			cout << "Writing file: " << ws2s(path + dir + L"/" + filename) <<
				" | " << long long(double(totalWritten)/1048576) << "MiB written "
				"   \r";
		} while (totalWritten < CHUNK_SIZE);
		CloseHandle(hFile);
		cout << "Written file  " << ws2s(path + dir + L"/" + filename) << endl;

	}

	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\nDone.\n", 6, &written, 0);
	written = func();
	delete[] data; data = nullptr;
	written = func();
	cout << "OK. Do you want to delete them? (y/N) ";
	written = func();
	//ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &written, 0);
	char cch = _getch();
	written = func();
	_putch('\n');
	if (cch == L'y' || cch == L'Y') {
		cout << "Thank you! Deleting... \r";
		(void)GetWindowsDirectoryW(buffer, 64);
		SetCurrentDirectoryW(buffer);
		FileDeleteTreeW(path + dir);
		cout << "Successfully deleted these files." << endl;
	}
	written = func();
	//ReadConsoleW(GetStdHandle(STD_INPUT_HANDLE), buffer, 1, &written, 0);

	pause

	return 0;
}


