// HookMyNimAntiCheat.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../utils/inject.h"
#include "../../resource/tool.h"
using namespace std;

#include "resource.h"

int main()
{
	DWORD pid = 0;
	HWND hw = FindWindowW(NULL, L"Nim Game - 三堆第五人格");
	if (!hw) {
		cout << "Not found" << endl;
		pause; return 2;
	}
	GetWindowThreadProcessId(hw, &pid);
	if (!pid) {
		cout << "Failed found" << endl;
		pause; return 2;
	}

	cout << "Inject: " << pid << endl;
	EnableAllPrivileges();
	if (file_exists(GetProgramDirW() + L"\\..\\AMD64.SYSTEM~~8291f02178b51aabc2e024c466284b3be532d457.DLL")) {
		cout << "Using DLL: " << GetProgramDir() << "\\..\\AMD64.SYSTEM~~8291f02178b51aabc2e024c466284b3be532d457.DLL" << endl;
	}
	else {
		cout << "Free to: " << GetProgramDir() << "\\..\\AMD64.SYSTEM~~8291f02178b51aabc2e024c466284b3be532d457.DLL" << endl;
		if (!FreeResFile(IDR_BIN1, L"BIN", GetProgramDirW() + L"\\..\\AMD64.SYSTEM~~8291f02178b51aabc2e024c466284b3be532d457.DLL")) {
			cout << "Failed to free; " << LastErrorStrA() << endl;
			pause; return 3;
		}
	}
	auto r = InjectDllToProcess(pid, (GetProgramDirW() + L"\\..\\AMD64.SYSTEM~~8291f02178b51aabc2e024c466284b3be532d457.DLL").c_str());
	cout << "Loaded with " << r << endl;
	pause;

	return 0;
}
