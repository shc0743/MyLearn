// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <windows.h>  
#include <wininet.h>  
#include <iostream>
#include "wizard.user.h"
#include <string>
using namespace std;

#pragma comment(lib, "wininet.lib")  
#pragma comment(lib, "MyProgressWizardLib64.lib")  

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



struct MyStruct
{
	PCWSTR url; PCWSTR pszFileToSave;
};
void myexit() {

}
void CALLBACK InternetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
DWORD download_main(PVOID pv) {
	MyStruct* p = (MyStruct*)pv;

	HINTERNET hInternet = InternetOpen(L"FFDownloader", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL) {
		return GetLastError();
	}

	HINTERNET hRequest = InternetOpenUrlW(hInternet,
		L"https://www.gyan.dev/ffmpeg/builds/ffmpeg-release-full.7z",
		NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (hRequest == NULL) {
		DWORD dwErr = GetLastError();
		InternetCloseHandle(hInternet);
		return dwErr;
	}

	// 查询文件大小  
	DWORD dwFileSize = 0;
	DWORD dwSize = sizeof(dwFileSize);
	if (!HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwFileSize, &dwSize, NULL)) {
		;
	}

	// 打开文件用于写入  
	string fname = GenerateUUID() + ".tmp";
	FILE* file = NULL;
	fopen_s(&file, fname.c_str(), "wb");
	if (!file) {
		DWORD dwErr = GetLastError();
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hInternet);
		return dwErr;
	}

	// 设置回调函数以接收下载进度通知  
	InternetSetStatusCallbackW(hRequest, InternetCallback);

	HMPRGOBJ hObj = CreateMprgObject();
	HMPRGWIZ hWiz = CreateMprgWizard(hObj, MPRG_CREATE_PARAMS{
		.szTitle = L"Download FFmpeg",
		.max = dwFileSize ? dwFileSize : size_t(-1),
		});
	OpenMprgWizard(hWiz);
	SetMprgWizAttribute(hWiz, MPRG_WIZARD_EXTENSIBLE_ATTRIBUTES::WizAttrCancelHandler,
		(LONG_PTR)(void*)myexit);

	// MessageBoxW(0, L"1", 0, 0);

	 // 开始下载文件  
	char buffer[4096]{};
	DWORD bytesRead;
	wstring sz;
	__int64 totalBytesRead = 0; // 使用64位整数来存储总读取字节数，以支持大文件  
	while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
		fwrite(buffer, 1, bytesRead, file);
		totalBytesRead += bytesRead;
		// 在这里更新下载进度  
		int percentage = static_cast<int>((totalBytesRead * 100.0) / dwFileSize);
		//std::cout << "Download progress: " << percentage << "%" << std::endl;
		SetMprgWizardValue(hWiz, totalBytesRead);
		sz = L"Downloading... " + to_wstring(totalBytesRead) + L"/" +
			to_wstring(dwFileSize) + L" bytes (" + to_wstring(percentage) + L"%)";
		SetMprgWizardText(hWiz, sz.c_str());
	}

	// 关闭文件和连接  
	fclose(file);
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hInternet);
	DeleteMprgObject(hObj);

	wstring nfn = (L"FFmpeg-" + to_wstring(time(0))) + L".7z";
	MoveFileW(s2wc(fname), nfn.c_str());
	Process.StartOnly(L"explorer /select," + nfn);

	return 0;
}

// 回调函数，用于接收下载进度通知  
void CALLBACK InternetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength) {
	return;
}

int __stdcall download(PCWSTR url, PCWSTR pszFileToSave) {
	return (int)download_main(url, pszFileToSave);
}



