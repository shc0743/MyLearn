#include "hook.x64.h"
#include <map>
#include <string>

// https://www.cnblogs.com/LyShark/p/13653394.html

using OldCodes_t = struct { BYTE code[12]; };
using ModuleName_t = std::wstring;//WCHAR[MAX_PATH + 2];
using FuncName_t = std::string;//WCHAR[256];
static std::map<ModuleName_t, std::map<FuncName_t, OldCodes_t>*> OldCodes;

//static BYTE OldCode[12] = 
//	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static BYTE HookCode[12] = 
	{ 0x48, 0xB8, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xFF, 0xE0 };

BOOL ApiInlineHook(PCWSTR lpModule, PCSTR lpFuncName, PVOID lpFunction) {
	if (!lpModule || !lpFuncName || !lpFunction) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	HMODULE hMod = GetModuleHandle(lpModule);
	if (!hMod) return FALSE;
	DWORD_PTR FuncAddress = (UINT64)GetProcAddress(hMod, lpFuncName);
	if (!FuncAddress) return FALSE;
	DWORD OldProtect = 0; BOOL bRet = TRUE;

	if (VirtualProtect((LPVOID)FuncAddress, 12, PAGE_EXECUTE_READWRITE, &OldProtect)) {
		BYTE oldcd[12]{ 0 };
		memcpy(oldcd, (LPVOID)FuncAddress, 12);                       // ����ԭʼ������ָ��
		std::map<FuncName_t, OldCodes_t>* pOldMap = nullptr;          // ԭ�е�std::map
		try {
			pOldMap = (OldCodes.at(lpModule));                        // ���Բ���
		}
		catch (...) {
			pOldMap = new std::map<FuncName_t, OldCodes_t>;
			// û���ҵ��ͷ���һ��
			// [TODO] ����� ApiInlineUnHook Ҫ delete
		}
		std::pair<FuncName_t, OldCodes_t> mypair;                     // ��Ҫ����pOldMap������
		mypair.first = lpFuncName;                                    // function name
		memcpy(mypair.second.code, oldcd, 12);                        // binary code
		pOldMap->insert(mypair);                                      // insert to pOldMap

		OldCodes.insert(std::make_pair(lpModule, pOldMap));           // 

		*(PINT64)(HookCode + 2) = (UINT64)lpFunction;                 // ���90Ϊָ����ת��ַ
	} else bRet = FALSE;
	memcpy((LPVOID)FuncAddress, &HookCode, sizeof(HookCode));         // ����Hook����ָ��
	VirtualProtect((LPVOID)FuncAddress, 12, OldProtect, &OldProtect); // ��ԭ�ڴ汣��

	return bRet;
}

BOOL ApiInlineUnHook(PCWSTR lpModule, PCSTR lpFuncName) {
	if (!lpModule || !lpFuncName) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	HMODULE hMod = GetModuleHandle(lpModule);
	if (!hMod) return FALSE;
	UINT64 FuncAddress = (UINT64)GetProcAddress(hMod, lpFuncName);
	if (!FuncAddress) return FALSE;
	DWORD OldProtect = 0; BOOL bRet = TRUE;
	if (VirtualProtect((LPVOID)FuncAddress, 12, PAGE_EXECUTE_READWRITE, &OldProtect)) {
		try {
			memcpy((LPVOID)FuncAddress, OldCodes.at(lpModule)->at(lpFuncName).code, 12);
			auto& sub1 = OldCodes.at(lpModule);
			sub1->erase(lpFuncName);
			if (sub1->empty()) {
				OldCodes.erase(lpModule);
				delete &sub1;                  // delete object
			}
		}
		catch (...) {
			bRet = FALSE;
		}
	} else bRet = FALSE;
	VirtualProtect((LPVOID)FuncAddress, 12, OldProtect, &OldProtect);
	return bRet;
}

#if 0
int WINAPI MyMessageBoxW(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	// ���Ȼָ�Hook����
	UnHook(L"user32.dll", "MessageBoxW");
	int ret = MessageBoxW(0, L"hello lyshark", lpCaption, uType);

	// ���ý�����,�ٴιҹ�
	Hook(L"user32.dll", "MessageBoxW", (PROC)MyMessageBoxW);
	return ret;
}
#endif

