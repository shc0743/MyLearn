// MyVolumeLocker_con.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <vector>
#include "D:\software\Program\source\git\github.com\shc0743\MyLearn\resource\tool.h"
using namespace std;

int main()
{
	bool run = true;
	vector<HANDLE> vols;
	char vol[2] = {};
	while (run) {
		cout << "Input volume, \"0\" to exit: ";
		vol[0] = _getch();
		_putch(vol[0]);
		puts(":");

		if (vol[0] == '0') break;

		HANDLE hFile = CreateFileW((L"\\\\.\\"s + s2ws(vol) + L":").c_str(), GENERIC_READ | GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			0, OPEN_EXISTING, 0, 0);
		if (!hFile || hFile == INVALID_HANDLE_VALUE) {
			cout << "Error opening volume: " << GetLastError() << ": " << LastErrorStrA() << endl;
			continue;
		}
		vols.push_back(hFile);

		cerr << "Locking volume " << vol << "... ";
		DWORD bytesReturned = 0;
		bool r = DeviceIoControl(
			(HANDLE)hFile,            // handle to a volume
			(DWORD)FSCTL_LOCK_VOLUME,   // dwIoControlCode
			NULL,                        // lpInBuffer
			0,                           // nInBufferSize
			NULL,                        // lpOutBuffer
			0,                           // nOutBufferSize
			(LPDWORD)&bytesReturned,   // number of bytes returned
			NULL                         // OVERLAPPED structure
		);
		if (r)cerr << "Done.\n"; else cerr << "Error.\n";

		if (!r) {
			cerr << "fallback: Dismounting volume " << vol << "... ";
			DWORD bytesReturned = 0;
			bool r = DeviceIoControl(
				(HANDLE)hFile,            // handle to a volume
				(DWORD)FSCTL_DISMOUNT_VOLUME,   // dwIoControlCode
				NULL,                        // lpInBuffer
				0,                           // nInBufferSize
				NULL,                        // lpOutBuffer
				0,                           // nOutBufferSize
				(LPDWORD)&bytesReturned,   // number of bytes returned
				NULL                         // OVERLAPPED structure
			);
			if (r)cerr << "Done.\n"; else cerr << "Error.\n";
		}
	}

	cerr << "Closing handles... ";
	for (auto& i : vols) {
		CloseHandle(i);
	}
	cerr << "Done.\n";
	pause;
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
