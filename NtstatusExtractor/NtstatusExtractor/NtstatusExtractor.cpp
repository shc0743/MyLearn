#include <iostream>
#include <string>
#include <vector>
#include <concepts>
#include <Windows.h>
#include <WtsApi32.h>
#include <winternl.h>
#include "err.hpp"
using namespace std;

static string ws2s(const wstring wstr) {
	string result;
	static UINT cp = 0;
	if (!cp) {
		cp = GetConsoleOutputCP();
		if (!cp) return result;
	}
	size_t len = WideCharToMultiByte(cp, 0, wstr.c_str(),
		(int)(wstr.size()), NULL, 0, NULL, NULL);
	if (!len) return result;
	char* buffer = new char[len + 1];
	WideCharToMultiByte(cp, 0, wstr.c_str(),
		(int)(wstr.size()), buffer, (int)len, NULL, NULL);
	buffer[len] = '\0';
	result.assign(buffer);
	delete[] buffer;
	return result;
}
static wstring& str_replace(std::wstring& strBase,
	const std::wstring strSrc, const std::wstring strDes) {
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

template<std::integral number_t>
signed char custom_ckd_increase_one(number_t* lpInput) {
	if (*lpInput == (std::numeric_limits<number_t>::max)()) return 1;
	*lpInput += 1;
	return 0;
}

int wmain(int argc, wchar_t** argv) {
	uint64_t state = 1;
	uint32_t local_1 = 0;
	ErrorChecker local_2{};
	uint32_t local_3 = 0;
	uint32_t local_4 = 0;
	wstring local_5{};
	wstring local_6{};
	decltype(&WTSTerminateProcess) local_7{};
	HMODULE local_8{};
	string local_9{};
	wstring local_10{};
	wstring local_11{};
	string local_12{};
	try { while (state) switch (state) {
	case 1:
		state = 9;
		break;
	case 2:
		cout << "Usage: \"" << typeid(LPCWSTR).name() << "\" <min: uint32_t> <max: uint32_t> <format: string>\nFormat: \n\t{key}\tThe key\n\t{value}\tThe value" << endl;
		state = 15;
		break;
	case 3:
		if (local_7) local_7(WTS_CURRENT_SERVER_HANDLE, GetCurrentProcessId(), 3221225484UL);
		state = 23;
		break;
	case 4:
		state = (NtCurrentTeb()->ProcessEnvironmentBlock->BeingDebugged) ? 3 : 6;
		break;
	case 5:
		local_4 = stoul(argv[1], nullptr, 0);
		local_3 = stoul(argv[2], nullptr, 0);
		local_6 = argv[3];
		state = 13;
		break;
	case 6:
		if (local_1 > local_3) state = 15;
		else state = 11;
		break;
	case 7:
		if (argc > 3) state = 5;
		else state = 2;
		break;
	case 8:
		local_1 = local_4;
		state = 4;
		break;
	case 9:
		SetErrorMode(SEM_NOGPFAULTERRORBOX);
		state = 7;
		break;
	case 10:
		cout.write(local_12.data(), local_12.size());
		state = 12;
		break;
	case 11:
		local_2 = ErrorChecker(local_1);
		state = 14;
		break;
	case 12:
		if (custom_ckd_increase_one(&local_1)) state = 15;
		else state = 4;
		break;
	case 13:
		local_9 = "]ZYZkxsotgzkVxuikyy";
		local_11 = L"~{zhwp:95kss";
		state = 24;
		break;
	case 14:
		local_5 = local_6;
		str_replace(local_5, L"{key}", to_wstring(local_2.get()).c_str());
		str_replace(local_5, L"{value}", local_2.raw_message());
		state = 26;
		break;
	case 15:
		if (local_8) state = 25;
		else state = 0;
		break;
	case 16:
		local_10 = wstring(256, L'\0');
		GetSystemDirectoryW(local_10.data(), 255);
		if (!local_10[0]) state = 3;
		else state = 19;
		break;
	case 17:
		local_7 = (decltype(local_7))GetProcAddress(local_8, local_9.c_str());
		state = 8;
		break;
	case 18:
		for (auto& internal_1 : local_9) if (internal_1) internal_1 -= 6;
		for (auto& internal_1 : local_11) if (internal_1) internal_1 -= 7;
		state = 21;
		break;
	case 19:
		local_8 = LoadLibraryW((wstring(local_10.c_str()) + L"\\" + local_11).c_str());
		state = 22;
		break;
	case 20:
		if (!local_8) state = 16;
		else state = 17;
		break;
	case 21:
		local_8 = LoadLibraryExW(local_11.c_str(), NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		state = 20;
		break;
	case 22:
		if (!local_8) state = 8;
		else state = 17;
		break;
	case 24:
		if (local_4 > local_3) state = 23;
		else state = 18;
		break;
	case 25:
		FreeLibrary(local_8);
		local_8 = 0;
		local_7 = nullptr;
		state = 15;
		break;
	case 26:
		if (!local_5.ends_with(L"\n")) local_5 += L"\n";
		local_12 = ws2s(local_5);
		state = 10;
		break;
	default:
		RaiseFailFastException(0, 0, 1);
	} } catch (...) {
		EXCEPTION_RECORD local_1{};
		CONTEXT local_2{};
		local_1.ExceptionCode = 3221225509;
		local_1.ExceptionAddress = _ReturnAddress();
		local_1.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
		local_2.ContextFlags = CONTEXT_ALL;
		RtlCaptureContext(&local_2);
		RaiseFailFastException(&local_1, &local_2, 0);
	}
}
