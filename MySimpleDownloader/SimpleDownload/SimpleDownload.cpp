// SimpleDownload.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../../resource/tool.h"
using namespace std;


#include <windows.h>  
#include <wininet.h>  
#include <iostream>  
#include "wizard.user.h"

#pragma comment(lib, "wininet.lib")  
#pragma comment(lib, "MyProgressWizardLib64.lib") 

#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup") 

HANDLE ggfile;
wstring gsFileName;
void myexit() {
	CloseHandle(ggfile);
	DeleteFileW(gsFileName.c_str());
	ExitProcess(ERROR_CANCELLED);
}
constexpr size_t buffer_size = 256 * 1024 * 1024; // 256MB
void CALLBACK InternetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
DWORD download_main(wstring url, wstring filename, wstring ua, bool slient) {
	// 预先创建进度条，提升用户体验
	HMPRGOBJ hObj = CreateMprgObject();
	HMPRGWIZ hWiz = CreateMprgWizard(hObj, MPRG_CREATE_PARAMS{
		.szTitle = filename.c_str(),
		.max = size_t(-1),
		//.max = dwFileSize ? dwFileSize : size_t(-1),
	});
	if (!slient) OpenMprgWizard(hWiz);
	SetMprgWizAttribute(hWiz, MPRG_WIZARD_EXTENSIBLE_ATTRIBUTES::WizAttrCancelHandler,
		(LONG_PTR)(void*)myexit);

	SetMprgWizardText(hWiz, L"Establishing connection, please wait...");

	HINTERNET hInternet = InternetOpenW(ua.c_str(), INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInternet == NULL) {
		return GetLastError();
	}

	HINTERNET hRequest = InternetOpenUrlW(hInternet,
		url.c_str(),
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
		std::cerr << "[WARN]  Cannot query file size. Maybe the server doesn't provide Content-Length header.\n";
	}
	if (dwFileSize) {
		auto data = GetModifiableMprgWizardData(hWiz);
		data->max = dwFileSize;
		UpdateMprgWizard(hWiz);

		wstring sz = L"Downloading... 0/" +
			to_wstring(dwFileSize) + L" bytes (0%)";
		SetMprgWizardText(hWiz, sz.c_str());
	}

	// 打开文件用于写入
	gsFileName = filename;
	ggfile = CreateFileW(filename.c_str(), GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (!ggfile || ggfile == INVALID_HANDLE_VALUE) {
		DWORD dwErr = GetLastError();
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hInternet);
		return dwErr;
	}

	// 设置回调函数以接收下载进度通知  
	InternetSetStatusCallbackW(hRequest, InternetCallback);

	// MessageBoxW(0, L"1", 0, 0);

	 // 开始下载文件  
	char* buffer = new char[buffer_size];
	DWORD bytesRead = 0, bytesWrite = 0;
	wstring sz;
	wchar_t* ccsz = new wchar_t[8192];
	__int64 totalBytesRead = 0; // 使用64位整数来存储总读取字节数，以支持大文件  
	while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
		WriteFile(ggfile, buffer, bytesRead, &bytesWrite, NULL);

		totalBytesRead += bytesRead;
		// 在这里更新下载进度  
		int percentage = static_cast<int>((totalBytesRead * 100.0) / dwFileSize);
		//std::cout << "Download progress: " << percentage << "%" << std::endl;
		SetMprgWizardValue(hWiz, totalBytesRead, false);
		sz = L"Downloading... " + to_wstring(totalBytesRead) + L"/" +
			to_wstring(dwFileSize) + L" bytes (" + to_wstring(percentage) + L"%)";
		wcscpy_s(ccsz, 8192, sz.c_str());
		SetMprgWizardText(hWiz, ccsz, false);
	}

	// 关闭文件和连接  
	CloseHandle(ggfile);
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hInternet);
	DeleteMprgObject(hObj);
	delete[] buffer;
	delete[] ccsz;

	return 0;
}

// 回调函数，用于接收下载进度通知  
void CALLBACK InternetCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength) {
	return;
}




int main()
{
	CmdLineW cl(GetCommandLineW());
	wstring url, file, userAgent = L"Windows (c) WinINet Platform Downloader";
	cl.getopt(L"url", url);
	cl.getopt(L"file", file);
	cl.getopt(L"user-agent", userAgent);
	bool slient = cl.getopt(L"slient") || cl.getopt(L"silent");
	if (url.empty() || file.empty())
	{
		if (cl.argc() >= 3) {
			url = cl[1], file = cl[2];
		}
		if (url.empty() || file.empty())
			return 87;
	}

	InitMprgComponent();
	return (int)download_main(url, file, userAgent, slient);

	return 0;
}
