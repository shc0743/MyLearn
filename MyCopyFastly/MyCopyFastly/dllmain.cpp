// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <windows.h>
#include <string>
using namespace std;

PVOID pBufferA = NULL, pBufferB = NULL;
constexpr SIZE_T default_buffer_size = 128 * 1024 * 1024;
DWORD buffer_size = 0;

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DWORD dwSize = default_buffer_size;
		WCHAR BufferSizeStr[16]{};
		GetEnvironmentVariableW(L"BufferSize", BufferSizeStr, 16);
		if (BufferSizeStr[0] != 0) {
			dwSize = _wtoi(BufferSizeStr);
		}
		::buffer_size = dwSize;
		pBufferA = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		pBufferB = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
		if (!pBufferA || !pBufferB) {
			if (pBufferA) VirtualFree(pBufferA, 0, MEM_RELEASE);
			if (pBufferB) VirtualFree(pBufferB, 0, MEM_RELEASE);
			return FALSE;
		}
	}
		break;
	case DLL_PROCESS_DETACH:
		if (pBufferA) VirtualFree(pBufferA, 0, MEM_RELEASE);
		if (pBufferB) VirtualFree(pBufferB, 0, MEM_RELEASE);
		break;
	default:;
	}
	return TRUE;
}



#pragma region utils
string ws2s(const wstring wstr) {
	string result;
	size_t len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
		(int)(wstr.size()), NULL, 0, NULL, NULL);
	if (len < 0) return result;
	char* buffer = new char[len + 1];
	if (buffer == NULL) return result;
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
		(int)(wstr.size()), buffer, (int)len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}
wstring& str_replace(std::wstring& strBase,
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
ULONGLONG MyGetFileSizeW(wstring filename) {
	GET_FILEEX_INFO_LEVELS fInfoLevelId = GetFileExInfoStandard;
	WIN32_FILE_ATTRIBUTE_DATA FileInformation = { 0 };
	// 通过winapi获取文件属性
	BOOL bGet = GetFileAttributesExW(filename.c_str(), fInfoLevelId, &FileInformation);
	if (bGet) {
		_fsize_t nFileSizeLow = FileInformation.nFileSizeLow;
		_fsize_t nFileSizeHigh = FileInformation.nFileSizeHigh;
		ULARGE_INTEGER szFile{};
		szFile.HighPart = nFileSizeHigh; // 超大文件才需要
		szFile.LowPart = nFileSizeLow;
		return szFile.QuadPart;
	}
	return 0;
}

#pragma endregion

struct CFInfoStruct
{
	PCWSTR src, dest;
	HANDLE hFile;
	PVOID buffer;
	size_t write_actual_size;
	bool success;
	DWORD result;
};
DWORD WINAPI r(PVOID pInfo) {
	CFInfoStruct* info = (CFInfoStruct*)pInfo;
	DWORD readed = 0;
	info->success = ReadFile(info->hFile, info->buffer, buffer_size, &readed, NULL);
	info->result = readed;
	return readed;
}
DWORD WINAPI w(PVOID pInfo) {
	CFInfoStruct* info = (CFInfoStruct*)pInfo;
	DWORD written = 0;
	info->success = WriteFile(info->hFile, info->buffer, 
		DWORD(info->write_actual_size ? (std::min)((size_t)buffer_size, info->write_actual_size) : buffer_size),
		&written, NULL);
	info->result = written;
	return written;
}

bool CopyFastly(PCWSTR srcFile, PCWSTR destFile, PCWSTR lpSzPrintFormat) {
	HANDLE hSrc = CreateFileW(srcFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hSrc == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	ULONGLONG srcSize = MyGetFileSizeW(srcFile);

	HANDLE hDest = CreateFileW(destFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, GetFileAttributesW(srcFile) & (~FILE_ATTRIBUTE_ENCRYPTED), NULL);
	if (hDest == INVALID_HANDLE_VALUE) {
		CloseHandle(hSrc);
		return FALSE;
	}

	bool failed = false;

#pragma warning(disable: 4114)
	// main logic...
	signed signed signed signed signed signed
		long long 
		signed signed 
	signed signed signed signed signed signed
		signed signed 
		signed signed 
	signed signed signed signed signed signed
		signed signed 
		signed signed 
	signed 
	signed 
	signed signed signed signed 
	signed signed signed signed signed 
	signed 
	signed signed signed signed 
	signed 
	signed signed signed signed signed signed signed signed signed 
	signed 
	signed signed signed signed signed signed signed signed signed 
	signed signed signed signed signed signed signed signed 
	signed 
		current_chunk = -1;
	ULONGLONG proceeded = 0, last_chunk_read_size = 0;
	bool shouldWriteAndBreak = false;
	while (true) {
		++current_chunk;
		if (lpSzPrintFormat) {
			wstring str = lpSzPrintFormat;
			str_replace(str, L"{size}", to_wstring(srcSize));
			str_replace(str, L"{written}", to_wstring(proceeded));
			wstring p = to_wstring((double(proceeded) / double(srcSize)) * 100);
			if (p.length() > 5/*12.34*/) p.erase(5);
			str_replace(str, L"{percentage}", p);
			printf_s("%s", ws2s(str).c_str());
		}
		if (failed || shouldWriteAndBreak) break;

		PVOID
			read_buffer = (current_chunk % 2) ? pBufferB : pBufferA,
			writ_buffer = (current_chunk % 2) ? pBufferA : pBufferB;

		// for Read
		CFInfoStruct info_r{
			.src = srcFile,
			.dest = destFile,
			.hFile = hSrc,
			.buffer = read_buffer
		};
		HANDLE hThreadR = NULL;
		if (!shouldWriteAndBreak) {
			hThreadR = CreateThread(NULL, 0, r, &info_r, CREATE_SUSPENDED, 0);
			if (!hThreadR) {
				failed = true;
				break;
			}
		}

		// for Write
		CFInfoStruct info_w{
			.src = srcFile,
			.dest = destFile,
			.hFile = hDest,
			.buffer = writ_buffer,
			.write_actual_size = last_chunk_read_size
		};
		HANDLE hThreadW = NULL;
		if (current_chunk > 0) {
			hThreadW = CreateThread(NULL, 0, w, &info_w, CREATE_SUSPENDED, 0);
			if (!hThreadW) {
				failed = true;
				if (hThreadR) {
#pragma warning(push)
#pragma warning(disable: 6258)
					TerminateThread(hThreadR, 0);
#pragma warning(pop)
					CloseHandle(hThreadR);
				}
				break;
			}
		}

		HANDLE threads[]{ hThreadR,hThreadW };
		if (hThreadR) ResumeThread(hThreadR);
		if (hThreadW) ResumeThread(hThreadW);
		WaitForMultipleObjects((hThreadR && hThreadW) ? 2 : 1, threads, TRUE, INFINITE);

		DWORD read_result = 0, write_result = 0;
		if (hThreadR) GetExitCodeThread(hThreadR, &read_result);
		if (hThreadW) GetExitCodeThread(hThreadW, &write_result);

		if (hThreadR) CloseHandle(hThreadR);
		if (hThreadW) CloseHandle(hThreadW);

		if ((info_w.result == 0 || !info_w.success) && current_chunk > 0) {
			// cannot write
			failed = true;
		}
		last_chunk_read_size = info_r.result;
		if (info_r.result == 0 || !info_r.success) {
			// ended, 读取结束。
			shouldWriteAndBreak = true;
		}
		proceeded += write_result;
	}


	CloseHandle(hSrc);
	CloseHandle(hDest);
	if (failed) return false;
	return true;
}


