#include "main.h"
using namespace std;

#pragma comment(lib, "ShlwApi.lib")

t_Item* __stdcall mLoadConfig(std::wstring szFile, t_Item* buffer, t_ItemCount* countAndMaxBufferSize) {
	t_ItemCount buf_size = *countAndMaxBufferSize;

	HANDLE hFile = CreateFileW(szFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile || hFile == INVALID_HANDLE_VALUE) {
		return nullptr;
	}

	DWORD nSize = 0, nRealSize = 0;

	auto ver = nMyVersion, ver2 = ver;

	// check version
	if (!ReadFile(hFile, &ver2, sizeof(ver), &nRealSize, NULL)) {
		CloseHandle(hFile);
		return nullptr;
	}
	if (ver < ver2) {
		CloseHandle(hFile);
		SetLastError(ERROR_FILE_NOT_SUPPORTED);
		return nullptr;
	}

	// read count
	if (!ReadFile(hFile, countAndMaxBufferSize, sizeof(t_ItemCount), &nRealSize, NULL)) {
		*countAndMaxBufferSize = buf_size;
		CloseHandle(hFile);
		return nullptr;
	}

	// alloc memory
	if (buf_size == 0) {
		buffer = (t_Item*)calloc(*countAndMaxBufferSize, sizeof(t_Item));
		if (!buffer) {
			CloseHandle(hFile);
			SetLastError(errno);
			return nullptr;
		}
	}
	else if (!buffer) {
		CloseHandle(hFile);
		SetLastError(ERROR_INVALID_PARAMETER);
		return nullptr;
	}

	// read items
	t_ItemCount maxCount = *countAndMaxBufferSize;
	nSize = sizeof(t_Item);
	for (t_ItemCount i = 0; i < maxCount; ++i) {
		if ((!ReadFile(hFile, buffer + i, nSize, &nRealSize, NULL))
			|| nSize != nRealSize
		) {
			(*countAndMaxBufferSize) -= 1;
			continue;
		}

	}


	CloseHandle(hFile);

	return buffer;
}

bool __stdcall mSaveConfig(std::wstring szFile, t_Item* buffer, t_ItemCount count) {
	HANDLE hFile = CreateFileW(szFile.c_str(), GENERIC_WRITE, 0, 0,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!hFile || hFile == INVALID_HANDLE_VALUE) return false;

	DWORD n = 0, b = n;

	auto ver = nMyVersion;
	if (!WriteFile(hFile, &ver, sizeof(ver), &b, NULL)) {
		CloseHandle(hFile);
		return false;
	}
	if (!WriteFile(hFile, &count, sizeof(count), &b, NULL)) {
		CloseHandle(hFile);
		return false;
	}
	n = sizeof(t_Item);
	for (size_t i = 0; i < count; ++i) {
		if (!WriteFile(hFile, buffer + i, n, &b, NULL)) {
			CloseHandle(hFile);
			return false;
		}
		if (n != b) {
			CloseHandle(hFile);
			CloseHandle(CreateFileW(szFile.c_str(), GENERIC_WRITE, 0, 0,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)); // reset file
			DeleteFileW(szFile.c_str());
			SetLastError(ERROR_DATA_CHECKSUM_ERROR);
			return false;
		}
	}

	return CloseHandle(hFile);
}

bool __stdcall mFreeConfig(t_Item*_) {
	if (!_) return false;
	__try {
		free (_);
		return true;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return false;
	}
}

bool __stdcall mRegisterRestartManager(std::wstring szConfigFile) {
	std::wstring c, f;
	WCHAR p[MAX_PATH + 1]{};
	DWORD n = MAX_PATH;
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) return false;
	if (!GetUserProfileDirectoryW(hToken, p, &n)) return false;
	CloseHandle(hToken);
	PathAppendW(p, (L".ProcessRestartData.pid=" + to_wstring(GetCurrentProcessId())
		+ L",uuid=" + s2ws(GenerateUUIDWithoutDelim()) + L".dat").c_str());
	f = p;
	{
		wfstream fp(f, ios::out | ios::binary);
		if (!fp) return false;
		SetFileAttributesW(p, FILE_ATTRIBUTE_HIDDEN);
		//fp.write(szConfigFile.c_str(), szConfigFile.length() * sizeof(WCHAR));
		fp << szConfigFile;
	}

	c = L"\"" + GetProgramDirW() + L"\" start \"" + szConfigFile +
		L"\" /restartByRestartManager --data=\"" + f + L"\"";

	auto result = RegisterApplicationRestart(c.c_str(), 0);
	if (S_OK != result) {
		SetLastError(result);
		return false;
	}
	return true;
}

int __stdcall mRunAutostart(t_Item* item, t_ItemCount count, t_FileVersion ver) {
	return 0;
}




