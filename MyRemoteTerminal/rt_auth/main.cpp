
#include <Windows.h>
#include <string>
#include "../../resource/tool.h"
#include "../MyRemoteTerminal/str_table.h"
#include "../MyRemoteTerminal/mycryp.h"

//#include "resource.h"

using namespace std;



static CHAR auth_act_key[32*2+1]{};
extern HMODULE hInst;


static DWORD WINAPI Thread_AuthServerPipe(PVOID pvsn) {
	WCHAR pszPipeName[256]{};
	LoadStringW(0, IDS_STRING_SRV_AUTHSRV_NAME, pszPipeName, 256);
	std::wstring pipename = pszPipeName;
	str_replace(pipename, L"%ServiceName%", (PCWSTR)pvsn);
	LoadStringW(0, IDS_STRING_APP_UUID, pszPipeName, 256);
	str_replace(pipename, L"%uuid%", pszPipeName);

	while (1) {
		HANDLE hPipe = CreateNamedPipeW(pipename.c_str(),
			PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
			1, 0, 0, 1000, NULL);
		if (hPipe == INVALID_HANDLE_VALUE) return GetLastError();
		// Wait for the client to connect; if it succeeds, 
		// the function returns a nonzero value. If the function
		// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

		BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ?
			TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

		if (fConnected) {
			DWORD t = 0;
			size_t buf_size = 0;
			PSTR buf = NULL, buf2 = NULL;
			string response;
			string key;
			constexpr auto buf2_size = 4098;

			(void)ReadFile(hPipe, &buf_size, sizeof(buf_size), &t, 0);
			if (buf_size == (size_t)-3) {
				response = auth_act_key;
				buf_size = (response.length() + 1) * sizeof(char);
				WriteFile(hPipe, &buf_size, sizeof(buf_size), &t, 0);
				WriteFile(hPipe, response.c_str(), (DWORD)buf_size, &t, 0);
				goto end;
			}
			buf = (PSTR)calloc(1, buf_size);
			if (!buf) goto end;
			buf2 = new char[buf2_size];
			(void)ReadFile(hPipe, buf, (DWORD)buf_size, &t, 0);

			{
				fstream fp("svc_priv.tmp", ios::in);
				if (!fp) {
					delete[] buf2;
					free(buf);
					goto end;
				}
				fp.read(buf2, buf2_size);
				fp.close();

				key = buf2;

				int rl = 0;
				memset(buf2, 0, buf2_size);
				
#if 1
				typedef bool(__stdcall* rt_rsa_decrypt_by_privkey_t)(const char* k,
					const unsigned char* in, size_t in_len,
					unsigned char* out, size_t out_len, int* real_len);
				rt_rsa_decrypt_by_privkey_t rt_rsa_decrypt_by_privkey =
					(rt_rsa_decrypt_by_privkey_t)
					GetProcAddress(hInst, "rt_rsa_decrypt_by_privkey");

#endif


				if (!rt_rsa_decrypt_by_privkey(key.c_str(),
					(unsigned char*)buf, buf_size,
					(unsigned char*)buf2, buf2_size,
					&rl)) {
					//
					void(0);
				}

			}

			if (string(auth_act_key) == buf2) {
				response = GenerateUUIDWithoutDelim();
			}
			else response = "";
			delete[] buf2;
			buf_size = (response.length() + 1) * sizeof(char);
			WriteFile(hPipe, &buf_size, sizeof(buf_size), &t, 0);
			WriteFile(hPipe, response.c_str(), (DWORD)buf_size, &t, 0);

			free(buf);

			end:
			FlushFileBuffers(hPipe);
			DisconnectNamedPipe(hPipe);
			CloseHandle(hPipe);
		}
		else
			// The client could not connect, so close the pipe. 
			CloseHandle(hPipe);
	}

	return 0;
}


int __stdcall rt_AuthorizationServerEntry(PCWSTR lpszServiceName) {

	// init key
	strcpy_s(::auth_act_key, (GenerateUUIDWithoutDelim()).c_str());

	// create main threads

	HANDLE threads[1]{};

	threads[0] = CreateThread(0, 0, Thread_AuthServerPipe, (void*)lpszServiceName, 0, 0);

	WaitForMultipleObjects(1, threads, TRUE, INFINITE);
	
	return 0;
}


unsigned long long rt_GetUserCount() {
	// TODO
	return 0;
}





