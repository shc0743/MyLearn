// RichEditDemo.cpp : 定义应用程序的入口点。
//

#include <Windows.h>
#include <Richedit.h>
#include "framework.h"
#include "RichEditDemo.h"
#include "resource.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。
	if (!LoadLibrary(TEXT("msftedit.dll"))) {
		MessageBoxW(NULL, L"Cannot find runtime library msftedit.dll", 0, MB_ICONHAND);
		return GetLastError();
	}

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_RICHEDITDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RICHEDITDEMO));

	MSG msg;

	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RICHEDITDEMO));
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_RICHEDITDEMO);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


// https://docs.microsoft.com/en-us/windows/win32/controls/printing-rich-edit-controls

// hwnd is the HWND of the rich edit control.
// hdc is the HDC of the printer. This value can be obtained for the 
// default printer as follows:
//
//     PRINTDLG pd = { sizeof(pd) };
//     pd.Flags = PD_RETURNDC | PD_RETURNDEFAULT;
//
//     if (PrintDlg(&pd))
//     {
//        HDC hdc = pd.hDC;
//        ...
//     }

BOOL PrintRTF(HWND hwnd, HDC hdc)
{
	DOCINFO di = { sizeof(di) };

	if (!StartDoc(hdc, &di))
	{
		return FALSE;
	}

	int cxPhysOffset = GetDeviceCaps(hdc, PHYSICALOFFSETX);
	int cyPhysOffset = GetDeviceCaps(hdc, PHYSICALOFFSETY);

	int cxPhys = GetDeviceCaps(hdc, PHYSICALWIDTH);
	int cyPhys = GetDeviceCaps(hdc, PHYSICALHEIGHT);

	// Create "print preview". 
	SendMessage(hwnd, EM_SETTARGETDEVICE, (WPARAM)hdc, cxPhys / 2);

	FORMATRANGE fr;

	fr.hdc = hdc;
	fr.hdcTarget = hdc;

	// Set page rect to physical page size in twips.
	fr.rcPage.top = 0;
	fr.rcPage.left = 0;
	fr.rcPage.right = MulDiv(cxPhys, 1440, GetDeviceCaps(hdc, LOGPIXELSX));
	fr.rcPage.bottom = MulDiv(cyPhys, 1440, GetDeviceCaps(hdc, LOGPIXELSY));

	// Set the rendering rectangle to the pintable area of the page.
	fr.rc.left = cxPhysOffset;
	fr.rc.right = cxPhysOffset + cxPhys;
	fr.rc.top = cyPhysOffset;
	fr.rc.bottom = cyPhysOffset + cyPhys;

	SendMessage(hwnd, EM_SETSEL, 0, (LPARAM)-1);          // Select the entire contents.
	SendMessage(hwnd, EM_EXGETSEL, 0, (LPARAM)&fr.chrg);  // Get the selection into a CHARRANGE.

	BOOL fSuccess = TRUE;

	// Use GDI to print successive pages.
	while (fr.chrg.cpMin < fr.chrg.cpMax&& fSuccess)
	{
		fSuccess = StartPage(hdc) > 0;

		if (!fSuccess) break;

		int cpMin = (int)SendMessage(hwnd, EM_FORMATRANGE, TRUE, (LPARAM)&fr);

		if (cpMin <= fr.chrg.cpMin)
		{
			fSuccess = FALSE;
			break;
		}

		fr.chrg.cpMin = cpMin;
		fSuccess = EndPage(hdc) > 0;
	}

	SendMessage(hwnd, EM_FORMATRANGE, FALSE, 0);

	if (fSuccess)
	{
		EndDoc(hdc);
	}

	else

	{
		AbortDoc(hdc);
	}

	return fSuccess;

}

DWORD CALLBACK EditStreamCallback(DWORD_PTR dwCookie,
	LPBYTE lpBuff,
	LONG cb,
	PLONG pcb)
{
	HANDLE hFile = (HANDLE)dwCookie;

	if (ReadFile(hFile, lpBuff, cb, (DWORD*)pcb, NULL))
	{
		return 0;
	}

	return -1;
}

BOOL FillRichEditFromFile(HWND hwnd, LPCTSTR pszFile)
{
	BOOL fSuccess = FALSE;

	HANDLE hFile = CreateFile(pszFile, GENERIC_READ,
		FILE_SHARE_READ, 0, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		EDITSTREAM es = { 0 };

		es.pfnCallback = EditStreamCallback;
		es.dwCookie = (DWORD_PTR)hFile;

		if (SendMessage(hwnd, EM_STREAMIN, SF_RTF, (LPARAM)&es) && es.dwError == 0)
		{
			fSuccess = TRUE;
		}

		CloseHandle(hFile);
	}

	return fSuccess;

}


//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: {
		HWND hwndEdit = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""),
			ES_MULTILINE | WS_VISIBLE | WS_CHILD |
			WS_TABSTOP | WS_HSCROLL | WS_VSCROLL,
			0, 0, 1, 1,
			hWnd, NULL, hInst, NULL);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)hwndEdit);

		break;
	}

	case WM_SIZE:
	case WM_SIZING:
	{
		LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
		HWND pwnd = reinterpret_cast<HWND>(ptr);
		RECT rc{};
		GetClientRect(hWnd, &rc);
		SetWindowPos(pwnd, 0, 0, 0,
			rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId)
			{
			case ID_32771:
				ShellExecuteW(hWnd, L"open", L"https://docs.microsoft.com/en-us/"
					"windows/win32/controls/about-rich-edit-controls"
					"#rich-edit-shortcut-keys", NULL, NULL, SW_NORMAL);
				break;
			case ID_FILE_OPEN: {
				WCHAR file[2048]{};
				OPENFILENAMEW fn{};
				fn.lStructSize = sizeof(fn);
				fn.hwndOwner = NULL;
				fn.lpstrFile = file;
				fn.lpstrFile[0] = L'\0';
				fn.nMaxFile = MAX_PATH;
				fn.lpstrFilter = L"RTF Files (*.rtf)\0*.rtf\0All Files\0*.*\0";
				fn.nFilterIndex = 1;
				fn.lpstrFileTitle = NULL;
				fn.nMaxFileTitle = 0;
				fn.lpstrInitialDir = NULL;
				fn.lpstrTitle = L"Choose files to open";
				fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (!GetOpenFileNameW(&fn)) {
					break;
				}
				FillRichEditFromFile(HWND(GetWindowLongPtr(hWnd, GWLP_USERDATA)), file);
			}
				break;
			case ID_MENU_PRINT: {
				PRINTDLG pd = { sizeof(pd) };
				pd.Flags = PD_RETURNDC | PD_RETURNDEFAULT;

				if (PrintDlg(&pd))
				{
					HDC hdc = pd.hDC;
					PrintRTF(HWND(GetWindowLongPtr(hWnd, GWLP_USERDATA)), hdc);
				}
			}
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此处添加使用 hdc 的任何绘图代码...
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
