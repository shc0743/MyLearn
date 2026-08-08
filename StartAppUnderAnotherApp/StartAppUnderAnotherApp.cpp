#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <ShObjIdl.h>
#include <Shlwapi.h>
using namespace std;

#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Advapi32.lib")
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

BOOL EnableAllPrivileges(HANDLE hToken) {
	BOOL bResult = FALSE;
	HANDLE hTokenLocal = nullptr;
	DWORD dwTokenInfoSize = 0;
	PTOKEN_PRIVILEGES pTokenPrivileges = nullptr;

	// 处理令牌句柄
	if (hToken == nullptr) {
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hTokenLocal)) {
			return FALSE;
		}
		hToken = hTokenLocal;
	}

	// 获取所需缓冲区大小
	if (!GetTokenInformation(hToken, TokenPrivileges, nullptr, 0, &dwTokenInfoSize) &&
		GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		goto cleanup;
	}

	// 分配权限信息缓冲区
	pTokenPrivileges = reinterpret_cast<PTOKEN_PRIVILEGES>(malloc(dwTokenInfoSize));
	if (!pTokenPrivileges) {
		goto cleanup;
	}

	// 获取实际权限信息
	if (!GetTokenInformation(hToken, TokenPrivileges, pTokenPrivileges, dwTokenInfoSize, &dwTokenInfoSize)) {
		goto cleanup;
	}

	// 启用所有权限
	bResult = TRUE;
	for (DWORD i = 0; i < pTokenPrivileges->PrivilegeCount; ++i) {
		LUID_AND_ATTRIBUTES& la = pTokenPrivileges->Privileges[i];
		TOKEN_PRIVILEGES tp = { 1, { { la.Luid, SE_PRIVILEGE_ENABLED } } };

		if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr)) {
			bResult = FALSE;
		}
		else if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
			bResult = FALSE;
		}
	}

cleanup:
	if (pTokenPrivileges) free(pTokenPrivileges);
	if (hTokenLocal) CloseHandle(hTokenLocal);
	return bResult;
}

#pragma endregion

int main(int argc, char *argv[]) {
    if (argc < 3) {
        // printf("Usage: %s <file> [arguments...]\n", argv[0]);
        TaskDialog(NULL, NULL, L"Error: Invalid Parameter", L"Error: Invalid Parameter", 
            L"Usage: [loader|:s|:sus] <PPID> <file> [arguments...]\n\nIf argv[0] is :s, then all "
            "TaskDialog will be blocked. Use the following syntax to do it:\n"
            "loader.exe loader.exe :s", TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, NULL);
        return ERROR_INVALID_PARAMETER;
    }

	bool showTaskDialog = true, sus = false;
	if (argv[0] == ":s"s) showTaskDialog = false;
	if (argv[0] == ":sus"s) sus = true;

    // 转换为宽字符路径（Windows API 推荐使用宽字符）
    wstring exePath = s2ws(argv[2]);
    DWORD ppid = (DWORD)std::stoul(argv[1]);

	EnableAllPrivileges(NULL);
    HANDLE hProcess = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, ppid);
    if (!hProcess) {
        DWORD ec = GetLastError();
        if (showTaskDialog) TaskDialog(NULL, NULL, L"Error: Failed to Open Process", LastErrorStrW().c_str(),
            (L"Error Code: " + to_wstring(ec)).c_str(), TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, NULL);
        return ec;
    }

    // 构建命令行参数（格式："exe_path arg1 arg2..."）
    // wchar_t cmdLine[32768] {};
    wstring cmdLine;
    // wcscpy_s(cmdLine, MAX_PATH * 2, L"app");

    if (argc > 3) {
        for (int i = 3; i < argc; i++) {
            wstring argWide = s2ws(argv[i]);
            if (i != 3) cmdLine += L" ";

            bool shouldQuote = argWide.find_first_of(L" ") != wstring::npos || argWide.find_first_of(L"\t") != wstring::npos;
            if (argWide.find(L"\"") != wstring::npos) {
                argWide = str_replace(argWide, L"\"", L"\"\"");
                shouldQuote = true;
            }

            if (shouldQuote) {
                cmdLine += L"\"";
                cmdLine += argWide;
                cmdLine += L"\"";
            } else {
                cmdLine += argWide;
            }
        }
    }

	wchar_t* cl = new wchar_t[cmdLine.size() + 1];
	wcscpy(cl, cmdLine.c_str());

    STARTUPINFOEXW si{ sizeof(si) };
	PROCESS_INFORMATION pi{};
	std::unique_ptr<uint8_t[]> attributeList;

	DWORD flags = CREATE_SUSPENDED | EXTENDED_STARTUPINFO_PRESENT;

	SIZE_T need{};
	InitializeProcThreadAttributeList(0, 1, 0, &need);
	if (need && need < 32768) {
		attributeList = make_unique<uint8_t[]>(need);
		if (InitializeProcThreadAttributeList((PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get(),
			1, 0, &need)) {
			UpdateProcThreadAttribute(
				(PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get(), 0,
				PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
				&hProcess,
				sizeof(HANDLE),
				NULL, NULL
			);
		}
	}

	si.StartupInfo.cb = sizeof(STARTUPINFOEX);
	si.StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	si.StartupInfo.wShowWindow = SW_SHOWNORMAL;
	si.lpAttributeList = PPROC_THREAD_ATTRIBUTE_LIST(attributeList ? attributeList.get() : nullptr);

	if (!CreateProcessW(exePath.c_str(), cl, NULL, NULL, FALSE, flags, NULL, NULL, (LPSTARTUPINFOW)&si, &pi)) {
		if (attributeList) DeleteProcThreadAttributeList((PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get());
		DWORD ec = GetLastError();
		if (showTaskDialog) TaskDialog(NULL, NULL, L"Error: Failed to Execute File", LastErrorStrW().c_str(),
			(L"Error Code: " + to_wstring(ec)).c_str(), TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, NULL);
		delete[] cl;
		return ec;
	}

    printf("%lu\n", pi.dwProcessId);

	if (attributeList) DeleteProcThreadAttributeList((PPROC_THREAD_ATTRIBUTE_LIST)attributeList.get());
	if (!sus) ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	delete[] cl;

    return 0;
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
