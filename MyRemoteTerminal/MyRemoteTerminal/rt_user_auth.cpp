#include "rt_user_auth.h"
#include "mycryp.h"
#include "../../resource/tool.h"
#include "str_table.h"
#include "svc.h"


rt_userinfo* gSrvUsers;
static unsigned long long nSrvUserCount = 0;
unsigned long long MAX_USERS_COUNT = 1024;



size_t rt_GetUserCount() {
	return nSrvUserCount;
}

bool rt_srv_load_users(HANDLE* huf, HANDLE* hufm) {
	HANDLE& gSvcUserFile = *huf;

	WCHAR ufl[256]{};
	if (!LoadStringW(NULL, IDS_STRING_SRV_USERFILE_LOCATION, ufl, 256))
		return false;

	if (!file_exists(ufl)) {
		gSvcUserFile = CreateFileW(ufl, FILE_ALL_ACCESS, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!gSvcUserFile || gSvcUserFile == INVALID_HANDLE_VALUE) {
			return false;
		}

		unsigned long long val = 0;
		DWORD cb = sizeof(unsigned long long);
		WriteFile(gSvcUserFile, &val, cb, &cb, NULL);
		val = MAX_USERS_COUNT; cb = sizeof(unsigned long long);
		WriteFile(gSvcUserFile, &val, cb, &cb, NULL);

		CloseHandle(gSvcUserFile);
	}

	gSvcUserFile = CreateFileW(ufl, FILE_ALL_ACCESS, 0, 0,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!gSvcUserFile || gSvcUserFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	SetHandleInformation(gSvcUserFile, HANDLE_FLAG_PROTECT_FROM_CLOSE,
		HANDLE_FLAG_PROTECT_FROM_CLOSE);

	DWORD size = sizeof(unsigned long long), size2 = size;
	if (!ReadFile(gSvcUserFile, &nSrvUserCount, size, &size2, NULL)) return false;
	if (size2 != size) {
		CloseHandle(gSvcUserFile);
		gSvcUserFile = NULL;
		SetLastError(ERROR_INVALID_DATA);
		return false;
	}
	if (!ReadFile(gSvcUserFile, &MAX_USERS_COUNT, size, &size2, NULL)) return false;
	if (size2 != size) {
		CloseHandle(gSvcUserFile);
		gSvcUserFile = NULL;
		SetLastError(ERROR_INVALID_DATA);
		return false;
	}
	if (nSrvUserCount > MAX_USERS_COUNT) nSrvUserCount = MAX_USERS_COUNT;

	const unsigned long long mapMaxSize = MAX_USERS_COUNT * sizeof(rt_userinfo);
	*hufm = MyCreateMemmap(
		DWORD(mapMaxSize & 0x00000000FFFFFFFF),
		DWORD(((mapMaxSize & 0xFFFFFFFF00000000) >> 32) & 0x00000000FFFFFFFF)
	);
	if (!(*hufm)) {
		CloseHandle(gSvcUserFile);
		gSvcUserFile = NULL;
		return false;
	}
	gSrvUsers = (rt_userinfo*)MapViewOfFile(*hufm,
		FILE_MAP_ALL_ACCESS, 0, 0, DWORD(mapMaxSize));
	if (!gSrvUsers) {
		CloseHandle(*hufm);
		*hufm = NULL;
		CloseHandle(gSvcUserFile);
		gSvcUserFile = NULL;
		return false;
	}

	size = sizeof(rt_userinfo);
	rt_userinfo buffer2;
	for (unsigned long long i = 0; i < nSrvUserCount; ++i) {
		size2 = size;
		if (ReadFile(gSvcUserFile, &buffer2, size, &size2, NULL)) {
			memcpy(gSrvUsers + i, &buffer2, size);
		}
		else {
			nSrvUserCount = i;
			break;
		}
	}


	return true;
}

rt_userinfo rt_LogonUser(std::wstring username, std::wstring password_enc) {
	using namespace std;

	for (unsigned long long i = 0; i < nSrvUserCount; ++i) {
		if (gSrvUsers[i].username == username &&
			gSrvUsers[i].valid && gSrvUsers[i].enabled &&
			gSrvUsers[i].password_encrypted == s2ws(rt_sha256(ws2s(password_enc)))
		) {
			return gSrvUsers[i];
		}
	}

	return rt_userinfo();
}



