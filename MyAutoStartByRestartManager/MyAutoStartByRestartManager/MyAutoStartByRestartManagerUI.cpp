// MyAutoStartByRestartManager.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "MyAutoStartByRestartManager.h"

#include "main.h"

using namespace std;

#define MAX_LOADSTRING 256

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HFONT hMainFont;                                // 主窗口字体

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProc_Editor(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

static void __stdcall log(size_t type, wstring text, unsigned long long code = 1);
static void __stdcall log_show();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
					 _In_opt_ HINSTANCE hPrevInstance,
					 _In_ LPWSTR    lpCmdLine,
					 _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。


	CmdLineW cl(GetCommandLineW());

	if (cl.argc() > 1) {
		if (cl[1] == L"/?" || cl[1] == L"--help" || cl[1] == L"help") {
			return MessageBoxW(NULL, (L"Usage:\n    \"" + GetProgramDirW() +
				L"\" [verb] [paramters]\n\nVerbs:\n    "
				"start\t\tStart the file\n"
				"test\t\tTest the file"
				"edit\t\tEdit the file"
				"daemon\t\tInstall the file").c_str(), L"Tips", MB_ICONINFORMATION);
		}
	}

	for (size_t i = 0; i < cl.length(); ++i) {
		if (cl[i] == L"start") {
			try {
				wstring file = cl[i + 1];
				t_ItemCount count = 0;
				t_Item* item = mLoadConfig(file, NULL, &count);
				if (!item) {
					DWORD err = GetLastError();
					log(CriticalError, ErrorCodeToStringW(err), err);
					return err;
				}

				for (t_ItemCount i = 0; i < count; ++i) {
					auto& it = (item[i]);
					if (!it.enabled) continue;
					if (it.progStartType == t_Item::StartType::Shell_Execute) {
						ShellExecuteW(0, L"open", it.program, 0, 0, it.progShowType);
					}
					else {
						STARTUPINFOW si{};
						PROCESS_INFORMATION pi{};

						si.cb = sizeof(si);
						si.dwFlags = STARTF_USESHOWWINDOW;
						si.wShowWindow = it.progShowType;

						if (CreateProcessW(NULL, it.program, 0, 0, 0, CREATE_SUSPENDED,
							0, 0, &si, &pi)) {
							Process.resume(pi.hProcess);

							CloseHandle(pi.hThread);
							CloseHandle(pi.hProcess);
						}
					}
				}

				if (cl[i + 2] == L"/restartByRestartManager") {
					wstring datafile;
					if (1 == cl.getopt(L"data", datafile)) do {
						WCHAR* buffer = new WCHAR[2048];

						wfstream fp(datafile, ios::in | ios::binary);
						if (!fp) break;
						fp.getline(buffer, 2048);
						fp.close();
						DeleteFileW(datafile.c_str());

						Process.StartOnly(L"\"" + GetProgramDirW() + L"\" daemon \"" +
							buffer + L"\"");

						delete[] buffer;

					} while (0);
				}

				return 0;
			}
			catch (...) {
				return ERROR_INVALID_PARAMETER;
			}
		}

		if (cl[i] == L"daemon") {
			try {
				wstring file = cl[i + 1];
				if (!mRegisterRestartManager(file)) {
					MessageBoxW(NULL, LastErrorStrW().c_str(), 0, MB_ICONHAND);
					return GetLastError();
				}
				if(cl.getopt(L"ui"))
					MessageBoxW(NULL, L"Success", L"OK", MB_ICONINFORMATION);

				HWND hw = CreateWindow(_T("#32770"), _T("Daemon Window"),
					0, 0, 0, 1, 1, 0, 0, 0, 0);

				MSG m{};
				while (GetMessage(&m, 0, 0, 0)) {
					TranslateMessage(&m);
					DispatchMessageW(&m);
				}

				return 0;
			}
			catch (...) {
				return ERROR_INVALID_PARAMETER;
			}
		}

	}




	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MYAUTOSTARTBYRESTARTMANAGER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	INITCOMMONCONTROLSEX icex{};           // Structure for control initialization.
	icex.dwSize = sizeof(icex);
	icex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
	if (!InitCommonControlsEx(&icex)) {
		MessageBoxW(NULL, (L"Failed to InitCommonControlsEx! " +
			LastErrorStrW()).c_str(), ERROR, MB_ICONHAND);
		return GetLastError();
	}
	if (!InitInstance (hInstance, nCmdShow))
	{
		MessageBoxW(NULL, LastErrorStrW().c_str(), ERROR, MB_ICONHAND);
		return GetLastError();
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance,
		MAKEINTRESOURCE(IDC_MYAUTOSTARTBYRESTARTMANAGER));

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
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = NULL;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = CreateSolidBrush(RGB(0xF0, 0xF0, 0xF0));
	wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYAUTOSTARTBYRESTARTMANAGER);
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm        = NULL;

	::hMainFont= CreateFont(-14, -7, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, FF_DONTCARE,
		_T("Consolas"));

	auto r = RegisterClassExW(&wcex);

	WCHAR c[256]{};
	LoadStringW(hInst, IDS_STRING_WNDCLASS_EDITOR, c, 256);
	wcex.lpfnWndProc = WndProc_Editor;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = c;
	RegisterClassExW(&wcex);

	return r;
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

   HWND hWnd = CreateWindowExW(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	  0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
	  return FALSE;
   }

   CenterWindow(hWnd);
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

// InitListViewColumns: Adds columns to a list-view control.
// hWndListView:        Handle to the list-view control. 
// Returns TRUE if successful, and FALSE otherwise. 
static BOOL MyListViewAddColumn(HWND hWndListView, int iCol, PCWSTR str,
	int cx = 100, int fmt = LVCFMT_LEFT)
{
	WCHAR szText[256]{};     // Temporary buffer.
	LVCOLUMN lvc{};

	wcscpy_s(szText, str);

	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text,
	// and subitem members of the structure are valid.
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = iCol;
	lvc.pszText = szText;
	lvc.cx = cx;               // Width of column in pixels.

	lvc.fmt = fmt;  // Left-aligned column.

	//// Load the names of the column headings from the string resources.
	//LoadString(g_hInst,
	//	IDS_FIRSTCOLUMN + iCol,
	//	szText,
	//	sizeof(szText) / sizeof(szText[0]));

	// Insert the columns into the list view.
	if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1)
		return FALSE;

	return TRUE;
}

typedef class _WndData {
public:
	_WndData() { ZeroMemory(this, sizeof(*this)); }

	HWND

		_1_1, _1_2, _1_3,
		_2_1,
		_3_1,
		//_4_1,
		_5_1, _5_2, _5_3, _5_4, _5_5,

		END_HWND_LIST;

	bool unsavedChanges;

	t_Item* items;
	t_ItemCount count;
	//PWSTR* textBuffer;
	std::map<t_ItemCount, std::wstring>* textBuffer;
	std::vector<t_Item> asVector;

} WndData;

typedef struct {
	t_Item* ptr;
	WndData* dat;
} myEditorState;

static void ReloadItemData(WndData* data) {
	ListView_DeleteAllItems(data->_3_1);
	if (!data->textBuffer->empty()) data->textBuffer->clear();
	for (t_ItemCount i = 0; i < data->count; ++i) {
		auto& d = data->asVector[i];
		data->textBuffer->insert(make_pair(i, L""));
		data->textBuffer->at(i) = wstring(32, 0);

		LVITEMW it{};
		it.iItem = 0x7FFFFFFF /* 2147483647 */;
		it.mask = LVIF_TEXT | LVIF_STATE;
		it.stateMask = (UINT)-1;
		it.state = 0;
		// cchTextMax is ignored during LVM_SETITEM and LVM_INSERTITEM.
		it.cchTextMax = 0;
		it.iGroupId = I_GROUPIDNONE;

		//No.
		//Name
		//Enabled?
		//File
		//Path

		it.iSubItem = 0;
		wstring sz = to_wstring(i+1);
		it.pszText = (PWSTR)sz.c_str();
		ListView_InsertItem(data->_3_1, &it);

		ListView_SetItemText(data->_3_1, i, 1, d.name);

		ListView_SetItemText(data->_3_1, i, 2,
			PWSTR((d.enabled) ? L"√" : L"x"));

		ListView_SetItemText(data->_3_1, i, 4, d.program);


	}
}

static int CALLBACK MyCompFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	return int(lParam1 - lParam2);
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
	WndData* data = (WndData*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	static WCHAR pub_buffer[2048]{};
	switch (message)
	{
	case WM_CREATE: {
#define MYCTLS_VAR_HWND hWnd
#define MYCTLS_VAR_HINST hInst
#define MYCTLS_VAR_HFONT hMainFont
#include "ctls.h"

		WndData* data = new WndData;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)data);
		memset(data, 0, sizeof(WndData));
		data->textBuffer = new std::map<t_ItemCount, wstring>;

		data->unsavedChanges = false;

		data->_1_1 = text(L"Select a file to edit:");
		data->_1_2 = edit(L"");
		data->_1_3 = button(L"Load", 21, 0, 0, 1, 1, BS_SPLITBUTTON);

		data->_2_1 = text(L"", 0, 0, 1, 1, WS_BORDER);

		data->_3_1 = custom(L"", WC_LISTVIEWW, 0, 0, 1, 1, LVS_REPORT | WS_BORDER);
		EnableWindow(data->_3_1, FALSE);
		MyListViewAddColumn(data->_3_1, 0, L"No.", 50);
		MyListViewAddColumn(data->_3_1, 1, L"Name", 100);
		MyListViewAddColumn(data->_3_1, 2, L"Enabled?", 70);
		MyListViewAddColumn(data->_3_1, 3, L"File", 120);
		MyListViewAddColumn(data->_3_1, 4, L"Path", 260);
		ListView_SortItems(data->_3_1, MyCompFunc, 0);

		//data->_4_1 = text(L"", 0, 0, 1, 1, WS_BORDER);

		data->_5_1 = button(L"New", 22);    // add
		data->_5_2 = button(L"Edit", 23);   // change, query
		data->_5_3 = button(L"Delete", 24); // delete
		data->_5_4 = button(L"Save", 25);
		data->_5_5 = button(L"Install", 26);

		EnableWindow(data->_5_1, FALSE); EnableWindow(data->_5_2, FALSE);
		EnableWindow(data->_5_3, FALSE); EnableWindow(data->_5_4, FALSE);

		DragAcceptFiles(hWnd, TRUE);

		CmdLineW cl(GetCommandLineW());
		if (cl.argc() > 1 && IsFileOrDirectory(cl[1]) == 1) {
			SetWindowTextW(data->_1_2, cl[1].c_str());
		}
		else if (cl.argc() > 2 && cl[1] == L"edit" && IsFileOrDirectory(cl[2]) == 1) {
			SetWindowTextW(data->_1_2, cl[2].c_str());
			SendMessage(hWnd, WM_COMMAND, 21, 0);
		}
		

	}
		break;
	case WM_SIZE:
	case WM_SIZING:
		if (data) {
			RECT rc{};
			GetClientRect(hWnd, &rc);

			long w = rc.right - rc.left, h = rc.bottom - rc.top;

			SetWindowPos(data->_1_1, 0, 10, 12, 160, 20, 0);
			SetWindowPos(data->_1_2, 0, 170, 10, w-255, 25, 0);
			SetWindowPos(data->_1_3, 0, w-75, 10, 70, 25, 0);

			SetWindowPos(data->_2_1, 0, 10, 45, w-15, 1, 0);

			SetWindowPos(data->_3_1, 0, 10, 55, w-15, h-100, 0);

			//SetWindowPos(data->_4_1, 0, 10, h-40, w-15, 1, 0);

			SetWindowPos(data->_5_5, 0, w-65, h-35, 60, 25, 0);
			SetWindowPos(data->_5_4, 0, w-135, h-35, 60, 25, 0);
			SetWindowPos(data->_5_3, 0, w-205, h-35, 60, 25, 0);
			SetWindowPos(data->_5_2, 0, w-275, h-35, 60, 25, 0);
			SetWindowPos(data->_5_1, 0, w-345, h-35, 60, 25, 0);

		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
			case BCN_DROPDOWN:
			{
				NMBCDROPDOWN* pDropDown = (NMBCDROPDOWN*)lParam;
				if (pDropDown->hdr.hwndFrom == data->_1_3)
				{

					// Get screen coordinates of the button.
					POINT pt{};
					pt.x = pDropDown->rcButton.left;
					pt.y = pDropDown->rcButton.bottom;
					ClientToScreen(pDropDown->hdr.hwndFrom, &pt);

					// Create a menu and add items.
					HMENU hSplitMenu = CreatePopupMenu();
					if (data->items) {
						AppendMenu(hSplitMenu, MF_BYPOSITION | MF_GRAYED, 33, L"Unload options");
						AppendMenu(hSplitMenu, MF_SEPARATOR, 0, 0);
						AppendMenu(hSplitMenu, MF_BYPOSITION, 12, L"[&s] Save and unload");
						AppendMenu(hSplitMenu, MF_BYPOSITION, 13, L"[&c] Unload without saving");
						AppendMenu(hSplitMenu, MF_SEPARATOR, 0, 0);
						AppendMenu(hSplitMenu, MF_BYPOSITION, 4, L"[&e] Close menu");

					}
					else {
						AppendMenu(hSplitMenu, MF_BYPOSITION | MF_GRAYED, 33, L"Load options");
						AppendMenu(hSplitMenu, MF_SEPARATOR, 0, 0);
						AppendMenu(hSplitMenu, MF_BYPOSITION, 1, L"[&c] Choose File");
						AppendMenu(hSplitMenu, MF_BYPOSITION, 2, L"[&n] Create New");
						AppendMenu(hSplitMenu, MF_SEPARATOR, 0, 0);
						AppendMenu(hSplitMenu, MF_BYPOSITION, 3, L"[&x] Clear");
						AppendMenu(hSplitMenu, MF_SEPARATOR, 0, 0);
						AppendMenu(hSplitMenu, MF_BYPOSITION, 4, L"[&e] Close menu");
					}

					// Display the menu.
					int r = TrackPopupMenu(hSplitMenu, TPM_LEFTALIGN | TPM_TOPALIGN
						| TPM_RIGHTBUTTON | TPM_RETURNCMD,
						pt.x, pt.y, 0, hWnd, NULL);
					DestroyMenu(hSplitMenu);

					if (r == 0) return 1;
					else if (r == 1) { SendMessageW(hWnd, WM_COMMAND, ID_MENU_FILE_OPEN, 0); }
					else if (r == 2) { SendMessageW(hWnd, WM_COMMAND, ID_MENU_FILE_NEW, 0); }
					else if (r == 3) { SetWindowTextW(data->_1_2, L""); }
					else if (r == 12) { SendMessage(hWnd, WM_COMMAND, ID_MENU_UNLOAD_SAVEANDUNLOAD, 0); }
					else if (r == 13) { SendMessage(hWnd, WM_COMMAND, ID_MENU_UNLOAD_UNLOADWITHOUTSAVING, 0); }

					return TRUE;
				}
			}
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DROPFILES:
	{
		wstring realFile;

		HDROP hDrop = (HDROP)wParam;
		if (NULL != hDrop)
		{
			UINT nDirCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);        // get file count
			if (0 < nDirCount)
			{
				TCHAR szTemp[MAX_PATH + 32]{};

				for (UINT i = 0; i < nDirCount; i++)                        // get file path
				{
					//int nPathLen = DragQueryFile(hDrop, ii, szFilePath, NULL);
					DragQueryFile(hDrop, i, pub_buffer, MAX_PATH);

					//(szFilePath);
					if (nDirCount > 1) {
						int r = 0;
						TaskDialog(hWnd, NULL, L"Drop Files", L"Do you want to load this file?",
							pub_buffer, TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON, 0, &r);
						if (r != IDYES) continue;
					}
					
					realFile = pub_buffer;
					break;
				}
			}
			DragFinish(hDrop);
		}

		SetWindowTextW(data->_1_2, realFile.c_str());
	}
		break;
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId) {
			case ID_MENU_FILE_OPEN:
			{
				if (data->items) {
					return MessageBoxW(hWnd, L"Unable to open", NULL, MB_ICONERROR);
				}
				OPENFILENAMEW fn{};
				fn.lStructSize = sizeof(fn);
				fn.hwndOwner = hWnd;
				fn.lpstrFile = pub_buffer;
				fn.lpstrFile[0] = L'\0';
				fn.nMaxFile = 1023;
				fn.lpstrFilter = L"MyAutoStartByRestartManager config Files "
					"(*.masc)\0*.masc\0All Files\0*.*\0";
				fn.nFilterIndex = 1;
				fn.lpstrFileTitle = NULL;
				fn.nMaxFileTitle = 0;
				fn.lpstrInitialDir = NULL;
				fn.lpstrTitle = L"Choose a file";
				fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (!GetOpenFileNameW(&fn)) {
					return TaskDialog(hWnd, 0, L"Error", (L"Error " +
						to_wstring(CommDlgExtendedError())).c_str(),
						to_wstring(CommDlgExtendedError()).c_str(),
						TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, 0);
				}
				SetWindowTextW(data->_1_2, pub_buffer);
			}
				break;
			case ID_MENU_FILE_NEW:
			case ID_MENU_FILE_SAVEAS:
			{
				if (data->items && wmId == ID_MENU_FILE_NEW) {
					return MessageBoxW(hWnd, L"Unable to open", NULL, MB_ICONERROR);
				}
				if (wmId == ID_MENU_FILE_SAVEAS) 
					GetWindowTextW(data->_1_2, pub_buffer, 2048);
				OPENFILENAMEW fn{};
				fn.lStructSize = sizeof(fn);
				fn.hwndOwner = hWnd;
				fn.lpstrFile = pub_buffer;
				fn.nMaxFile = 1023;
				fn.lpstrFilter = L"MyAutoStartByRestartManager config Files "
					"(*.masc)\0*.masc\0All Files\0*.*\0";
				fn.nFilterIndex = 1;
				fn.lpstrFileTitle = NULL;
				fn.nMaxFileTitle = 0;
				fn.lpstrInitialDir = NULL;
				fn.lpstrTitle = L"Save file dialog";
				fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (!GetSaveFileNameW(&fn)) {
					return TaskDialog(hWnd, 0, L"Error", (L"Error " +
						to_wstring(CommDlgExtendedError())).c_str(),
						to_wstring(CommDlgExtendedError()).c_str(),
						TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, 0);
				}

				if (wmId == ID_MENU_FILE_NEW) {
					if (data->items) {
						mFreeConfig(data->items);
					}
					data->items = NULL;
					data->count = 0;
				}

				if (!mSaveConfig(pub_buffer, data->items, data->count)) {
					return TaskDialog(hWnd, 0, L"Error", LastErrorStrW().c_str(),
						to_wstring(GetLastError()).c_str(),
						TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, 0);
				}
				SetWindowTextW(data->_1_2, pub_buffer);
				TaskDialog(hWnd, 0, L"OK", LastErrorStrW().c_str(), 
					to_wstring(GetLastError()).c_str(), 
					TDCBF_CANCEL_BUTTON, TD_INFORMATION_ICON, 0);
			}
				break;
			case ID_MENU_FILE_SAVE:
			{
				data->unsavedChanges = false;

				mFreeConfig(data->items);
				data->items = (t_Item*)calloc(data->asVector.size(), sizeof(t_Item));
				if (!data->items) {
					MessageBoxW(hWnd, L"Failed to calloc!", L"FATAL ERROR", MB_ICONHAND);
					ExitProcess(-1);
				}
				for (size_t i = 0; i < data->asVector.size(); ++i) {
					data->items[i] = data->asVector[i];
				}

				GetWindowTextW(data->_1_2, pub_buffer, 2048);

				if (!mSaveConfig(pub_buffer, data->items, data->count)) {
					MessageBoxW(hWnd, LastErrorStrW().c_str(), L"ERROR", MB_ICONHAND);
					return false;
				}

				SetWindowTextW(hWnd, (L"[File saved.] "s + szTitle).c_str());
				SetTimer(hWnd, 0xB081, 2000, 0);

			}
				break;
			case ID_MENU_UNLOAD_SAVEANDUNLOAD:
				if (!data || !data->items) {
					break;
				}
				SendMessage(hWnd, WM_COMMAND, ID_MENU_FILE_SAVE, 0);
				SendMessage(hWnd, WM_COMMAND, 21, 0xa0010354);
				break;
			case ID_MENU_UNLOAD_UNLOADWITHOUTSAVING:
				if (!data || !data->items) {
					break;
				}
				if (MessageBoxW(hWnd, L"Unload without saving?", L"Warning",
					MB_ICONWARNING | MB_YESNO) != IDYES) break;
				data->unsavedChanges = false;
				SetWindowTextW(hWnd, szTitle);
				SendMessage(hWnd, WM_COMMAND, 21, 0xa0010354);
				break;
			case 21: 
				if (data) {
					if (data->items) { // Unload
						WCHAR caption[256]{};
						GetWindowTextW(hWnd, caption, 256);
						GetWindowTextW(data->_1_2, pub_buffer, 1024);
						if (lParam != 0xa0010354 && data->unsavedChanges) {
							//if (IDYES != MessageBoxW(hWnd, L"You didn't save your"
							//	" changes.\nStill unload?", caption, MB_ICONWARNING
							//  | MB_YESNO))
							//	return false;
							int btn = IDCANCEL;
							TaskDialog(hWnd, 0, caption, L"Save changes?", pub_buffer,
								TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON,
								TD_WARNING_ICON, &btn);
							if (btn == IDCANCEL) return false;
							if (btn == IDYES)
								SendMessage(hWnd, WM_COMMAND, ID_MENU_FILE_SAVE, 0);
							data->unsavedChanges = false;
						}

						if (!mFreeConfig(data->items)) {
							return MessageBoxW(hWnd, (L"Failed to unload: " +
								LastErrorStrW()).c_str(), 0, MB_ICONHAND);
						}
						data->items = nullptr;
						data->count = 0;
						data->asVector.clear();
						//if (data->textBuffer) free(data->textBuffer);

						EnableWindow(data->_3_1, 0);
						EnableWindow(data->_1_1, 1); EnableWindow(data->_1_2, 1);
						EnableWindow(data->_5_1, 0); EnableWindow(data->_5_2, 0);
						EnableWindow(data->_5_3, 0); EnableWindow(data->_5_4, 0);
						EnableWindow(data->_5_5, 1);

						ListView_DeleteAllItems(data->_3_1);

						DragAcceptFiles(hWnd, TRUE);
						SetWindowTextW(data->_1_3, L"Load");
					}
					else {

						WCHAR szFile[1024]{};
						GetWindowTextW(data->_1_2, szFile, 1023);
						auto ptr = mLoadConfig(szFile, NULL, &(data->count));
						if (!ptr) {
							return TaskDialog(hWnd, 0, L"Error", LastErrorStrW().c_str(),
								to_wstring(GetLastError()).c_str(),
								TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, 0);
						}

						data->items = ptr;
						//data->textBuffer = (PWSTR*)calloc(data->count, sizeof(WCHAR) * 32);

						for (t_ItemCount i = 0; i < data->count; ++i) {
							data->asVector.push_back(data->items[i]);
						}

						EnableWindow(data->_1_1, 0); EnableWindow(data->_1_2, 0);

						DragAcceptFiles(hWnd, FALSE);
						static const auto t = [](PVOID d) -> DWORD {
							WndData* data = (WndData*)d;

							ReloadItemData(data);

							EnableWindow(data->_3_1, 1);
							EnableWindow(data->_5_1, 1); EnableWindow(data->_5_2, 1);
							EnableWindow(data->_5_3, 1); EnableWindow(data->_5_4, 1);
							EnableWindow(data->_5_5, 0);

							SetWindowTextW(data->_1_3, L"Unload");
							return 0;
						};
						if (HANDLE _ = CreateThread(0, 0, t, data, 0, 0))
							CloseHandle(_);


					}
				}
				break;
			case 22:
				if (data) {
					WCHAR c[256]{};
					LoadStringW(hInst, IDS_STRING_WNDCLASS_EDITOR, c, 256);

					//data->itl.push_back(t_Item());
					t_Item* ptr = NULL;
					//try {
					//	ptr = &(data->itl.at(data->itl.size() - 1));
					//	if (!ptr) throw std::exception("Unexpected exception");
					//}
					//catch (std::exception&e) {
					//	MessageBoxA(hWnd, e.what(), NULL, MB_ICONHAND);
					//}
					myEditorState stat{};
					stat.ptr = ptr;
					stat.dat = data;
					
					HWND hw = CreateWindowExW(0, c, L"Default", WS_POPUPWINDOW | 
						WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
						0, 0, 500, 400, hWnd, 0, hInst, &stat);
					if (!hw) {
						//data->itl.pop_back();
						break;
					}
					CenterWindow(hw);
					ShowWindow(hw, SW_NORMAL);

				}
				break;
			case 23:
				if (data) {
					WCHAR c[256]{};
					LoadStringW(hInst, IDS_STRING_WNDCLASS_EDITOR, c, 256);

					int selection = ListView_GetSelectionMark(data->_3_1);
					if (selection < 0) break;
					LVITEMINDEX index{};
					index.iItem = selection;

					do {
						selection = index.iItem;

						myEditorState stat{};
						try {
							stat.ptr = &(data->asVector.at(selection));
							stat.dat = data;
						}
						catch (std::exception& e) {
							MessageBoxA(hWnd, e.what(), "Internal error", MB_ICONHAND);
							break;
						}

						HWND hw = CreateWindowExW(0, c, L"Default", WS_POPUPWINDOW |
							WS_CAPTION | WS_SIZEBOX | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
							0, 0, 500, 400, hWnd, 0, hInst, &stat);
						if (!hw) {
							//data->itl.pop_back();
							break;
						}
						CenterWindow(hw);
						ShowWindow(hw, SW_NORMAL);
					} while (ListView_GetNextItemIndex(data->_3_1,
						&index, LVNI_ALL | LVNI_SELECTED));

				}
				break;
			case 24:
				if (data) {
					int selection = ListView_GetSelectionMark(data->_3_1);
					if (selection < 0) break;
					data->asVector.erase(data->asVector.begin() + selection);
					data->count -= 1;
					SetWindowTextW(hWnd, (L"* "s + szTitle).c_str());
					data->unsavedChanges = true;
					ReloadItemData(data);
				}
				break;
			case 25:
				return SendMessage(hWnd, WM_COMMAND, ID_MENU_FILE_SAVE, 0);
				break;
			case 26:
			{
				GetWindowTextW(data->_1_2, pub_buffer, 1024);

				int a = 0;
				TaskDialog(hWnd, 0, L"Installation", L"Install auto-start?", pub_buffer,
					TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON, 0, &a);
				if (a != IDYES) break;

				Process.StartOnly(L"\"" + GetProgramDirW() + L"\" daemon \"" +
					pub_buffer + L"\" --ui ");
			}
				break;

			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_EXIT:
				SendMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
	case WM_TIMER:
		switch (wParam)
		{
		case 0xB081:
			KillTimer(hWnd, wParam);
			SetWindowTextW(hWnd, szTitle);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
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
	case WM_CLOSE:
	{
		LoadStringW(hInst, IDS_STRING_WNDCLASS_EDITOR, pub_buffer, 2048);
		if (FindWindowEx(NULL, NULL, pub_buffer, (L"Item Editor (Process " +
			to_wstring(GetCurrentProcessId()) + L")").c_str())) {
			MessageBoxW(hWnd, L"You must close all dialogs before closing this.",
				ERROR, MB_ICONERROR);
			return false;
		}
		if (data && data->unsavedChanges) {
			WCHAR caption[256]{};
			GetWindowTextW(hWnd, caption, 256);
			GetWindowTextW(data->_1_2, pub_buffer, 2048);
			int btn = IDCANCEL;
			TaskDialog(hWnd, 0, caption, L"Save changes?", pub_buffer,
				TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON,
				TD_WARNING_ICON, &btn);
			if (btn == IDCANCEL) return false;
			if (btn == IDYES) SendMessage(hWnd, WM_COMMAND, ID_MENU_FILE_SAVE, 0);
		}
	}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		if (data->textBuffer) delete data->textBuffer;
		if (data) delete data;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


typedef class _WndData_Editor {
public:
	_WndData_Editor() { ZeroMemory(this, sizeof(*this)); }


	HWND
		HWNDLIST_BEGIN,

		$1$1, $1$2, $1$3,
		$2$1, $2$2, $2$3, $2$4,
		$3$1,

		$$1, $$2, 


		HWNDLIST_END;

	myEditorState state;
	bool ptr_newal;
} WndData_Editor;
LRESULT WndProc_Editor(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	WndData_Editor& data = *((WndData_Editor*)GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch (msg) {
	case WM_CREATE: {
		SetWindowTextW(hwnd, (L"Item Editor (Process " +
			to_wstring(GetCurrentProcessId()) + L")").c_str());

		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		myEditorState* pState = (myEditorState*)(pCreate->lpCreateParams);
		if (!pState || !pState->dat) {
			return DestroyWindow(hwnd);
		}

		WndData_Editor* data = new WndData_Editor;
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)data);
		memset(data, 0, sizeof(WndData_Editor));
		data->state = *pState;

		if (!data->state.ptr) {
			data->state.ptr = new t_Item;
			data->ptr_newal = true;
		}
		if (data->state.ptr->is_locked) {
			MessageBoxW(GetParent(hwnd), L"Trying to edit a locked item", NULL, MB_ICONHAND);
			return DestroyWindow(hwnd);
		}
		data->state.ptr->is_locked = true;

#define MYCTLS_VAR_HWND hwnd
#define MYCTLS_VAR_HINST hInst
#define MYCTLS_VAR_HFONT hMainFont
#include "ctls.h"

		data->$1$1 = text(L"Item &Name:");
		data->$1$2 = edit(data->state.ptr->name);
		data->$1$3 = button(L"&Enabled", 2, 0, 0, 1, 1, BS_AUTOCHECKBOX);
		
		data->$2$1 = text(L"&Program: ");
		data->$2$2 = custom(L"", WC_COMBOBOXW, 0, 0, 1, 1, CBS_DROPDOWNLIST);
		data->$2$4 = custom(L"", WC_COMBOBOXW, 0, 0, 1, 1, CBS_DROPDOWNLIST);
		data->$2$3 = button(L"Browse...", 3);

		data->$3$1 = edit(data->state.ptr->program, 0, 0, 1, 1,
			WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL
			| ES_MULTILINE | ES_WANTRETURN);

		data->$$1 = button(L"&Save", 5);
		data->$$2 = button(L"&Cancel", 1);

		SendMessage(data->$2$2, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"CreateProcess");
		SendMessage(data->$2$2, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"ShellExecute");
		SendMessage(data->$2$4, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Show: Normal");
		SendMessage(data->$2$4, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Show: Hide");
		SendMessage(data->$2$4, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Show: Show");
		SendMessage(data->$2$4, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Show: Restore");
		SendMessage(data->$2$4, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Show: Minimize");
		SendMessage(data->$2$4, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)L"Show: Maximize");

		SendMessage(data->$2$2, (UINT)CB_SETCURSEL, 
			(data->state.ptr->progStartType == t_Item::StartType::Shell_Execute) ?
			1 : 0, 0
		);
		switch (data->state.ptr->progShowType) {
		case SW_HIDE:
			if(data->ptr_newal)
				SendMessage(data->$2$4, (UINT)CB_SETCURSEL, 0, 0);
			else
				SendMessage(data->$2$4, (UINT)CB_SETCURSEL, 1, 0);
			break;
		case SW_NORMAL:
			SendMessage(data->$2$4, (UINT)CB_SETCURSEL, 0, 0);
			break;
		case SW_MINIMIZE:
			SendMessage(data->$2$4, (UINT)CB_SETCURSEL, 4, 0);
			break;
		case SW_MAXIMIZE:
			SendMessage(data->$2$4, (UINT)CB_SETCURSEL, 5, 0);
			break;
		case SW_RESTORE:
			SendMessage(data->$2$4, (UINT)CB_SETCURSEL, 3, 0);
			break;
		case SW_SHOW:
			SendMessage(data->$2$4, (UINT)CB_SETCURSEL, 2, 0);
			break;
		default:
			break;
		}
		if (data->state.ptr->enabled || data->ptr_newal)
			SendMessage(data->$1$3, BM_SETCHECK, BST_CHECKED, 0);

		DragAcceptFiles(hwnd, 1);


	}
		break;

	case WM_DROPFILES:
	{
		wstring realFile;

		HDROP hDrop = (HDROP)wParam;
		if (NULL != hDrop)
		{
			UINT nDirCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);        // get file count
			if (0 < nDirCount)
			{
				TCHAR szFilePath[MAX_PATH];
				TCHAR szTemp[MAX_PATH + 32]{};

				for (UINT i = 0; i < nDirCount; i++)                        // get file path
				{
					//int nPathLen = DragQueryFile(hDrop, ii, szFilePath, NULL);
					DragQueryFile(hDrop, i, szFilePath, MAX_PATH);

					//(szFilePath);
					if (nDirCount > 1) {
						int r = 0;
						TaskDialog(hwnd, NULL, L"Drop Files", L"Do you want to load this file?",
							szFilePath, TDCBF_YES_BUTTON | TDCBF_CANCEL_BUTTON, 0, &r);
						if (r != IDYES) continue;
					}

					realFile = szFilePath;
					break;
				}
			}
			DragFinish(hDrop);
		}

		SetWindowTextW(data.$3$1, (L"\"" + realFile + L"\"").c_str());
	}
	break;

	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择:
			switch (wmId) {
			case 3:
			{
				WCHAR szFile[1024]{};
				OPENFILENAMEW fn{};
				fn.lStructSize = sizeof(fn);
				fn.hwndOwner = hwnd;
				fn.lpstrFile = szFile;
				fn.lpstrFile[0] = L'\0';
				fn.nMaxFile = 1023;
				fn.lpstrFilter = L"Executable Files "
					"(*.exe)\0*.exe\0All Files\0*.*\0";
				fn.nFilterIndex = 1;
				fn.lpstrFileTitle = NULL;
				fn.nMaxFileTitle = 0;
				fn.lpstrInitialDir = NULL;
				fn.lpstrTitle = L"File Picker";
				fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if (!GetOpenFileNameW(&fn)) {
					return TaskDialog(hwnd, 0, L"Error", (L"Error " +
						to_wstring(CommDlgExtendedError())).c_str(),
						to_wstring(CommDlgExtendedError()).c_str(),
						TDCBF_CANCEL_BUTTON, TD_ERROR_ICON, 0);
				}
				SetWindowTextW(data.$3$1, (L"\""s + szFile + L"\"").c_str());

			}
				break;

			case 1:
				data.state.ptr->is_locked = false;
				DestroyWindow(hwnd);
				break;

			case 5:
			{
				WCHAR tmp[2048]{};

				auto& ptr = data.state.ptr;
				auto ptr_old = ptr;
				ptr->enabled = (SendMessage(data.$1$3, BM_GETSTATE, 0, 0) & BST_CHECKED);
				[&] {
					__try {
						GetWindowTextW(data.$1$2, tmp, 2048);
						wcscpy_s(ptr->name, tmp);
						GetWindowTextW(data.$3$1, tmp, 2048);
						wcscpy_s(ptr->program, tmp);
					}
					__except (EXCEPTION_EXECUTE_HANDLER) {
						MessageBoxW(hwnd, L"Text too long!", NULL, MB_ICONHAND);
					}
				}();
				ptr->progStartType = (SendMessage(data.$2$2, CB_GETCURSEL, 0, 0) != 0)
					? t_Item::StartType::Shell_Execute
					: t_Item::StartType::Create_Process;
				tmp[0] = (WCHAR)SendMessage(data.$2$4, CB_GETCURSEL, 0, 0);
				if (tmp[0] == 0) ptr->progShowType = SW_NORMAL;
				else if (tmp[0] == 1) ptr->progShowType = SW_HIDE;
				else if (tmp[0] == 2) ptr->progShowType = SW_SHOW;
				else if (tmp[0] == 3) ptr->progShowType = SW_RESTORE;
				else if (tmp[0] == 4) ptr->progShowType = SW_MINIMIZE;
				else if (tmp[0] == 5) ptr->progShowType = SW_MAXIMIZE;
				else ptr->progShowType = SW_NORMAL;
				ptr->is_locked = false;
				if (data.ptr_newal)
					data.state.dat->count += 1;
				data.state.dat->asVector.push_back(*ptr);
				data.state.dat->unsavedChanges = true;
				data.state.ptr->is_locked = false;
				for (size_t i = 0, pos = -1; i < data.state.dat->asVector.size(); ++i) {
					if (data.state.dat->asVector[i] == *ptr_old) {
						if (pos != i) {
							pos = i;
							continue;
						}

						data.state.dat->asVector.erase
						(data.state.dat->asVector.begin() + pos);
						break;
					}
				}
				SetWindowTextW(GetParent(hwnd), (L"* "s + szTitle).c_str());

				ReloadItemData(data.state.dat);

				DestroyWindow(hwnd);
			}
				break;

			default:
				return DefWindowProc(hwnd, msg, wParam, lParam);
			}
		}
		break;

	case WM_SIZE:
	case WM_SIZING:
	{
		RECT rc{};
		GetClientRect(hwnd, &rc);

		long w = rc.right - rc.left, h = rc.bottom - rc.top;

		SetWindowPos(data.$1$1,		0, 10, 10, 80, 20, 0);
		SetWindowPos(data.$1$2,		0, 90, 10, w-180, 20, 0);
		SetWindowPos(data.$1$3,		0, w-80, 10, 70, 20, 0);

		SetWindowPos(data.$2$1,		0, 10, 40, 80, 25, 0);
		SetWindowPos(data.$2$2,		0, 90, 40, w-310, 25, 0);
		SetWindowPos(data.$2$4,		0, w-210, 40, 120, 25, 0);
		SetWindowPos(data.$2$3,		0, w-80, 40, 70, 25, 0);

		SetWindowPos(data.$3$1,		0, 10, 75, w-20, h-125, 0);

		SetWindowPos(data.$$1,		0, w-80-80, h-40, 70, 30, 0);
		SetWindowPos(data.$$2,		0, w-80, h-40, 70, 30, 0);


	}
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_CLOSE:
		data.state.ptr->is_locked = false;

		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	case WM_DESTROY:
		if (data.ptr_newal) {
			delete data.state.ptr;
		}
		if (&data) delete &data;
		return DefWindowProc(hwnd, msg, wParam, lParam);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
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
		if (LOWORD(wParam) == IDABORT)
		{
			MessageBoxW(hDlg, L"Whether the launched program has administrator rights"
				" depends on the installation.\nFor example, when installing, "
				"if the installer runs as an administrator, all programs under this"
				" installation will run as an administrator.\n"
				"* This application requires RESTART_APPLICATIONS when shutdown.",
				L"Tips", MB_ICONINFORMATION);
		}
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}





static void __stdcall log(size_t type, wstring text, unsigned long long code) {
	enum tt {
		undefined = 0,
		Critical = 1,
		Error = 2,
		Warning = 3,
		Info = 4,

	} t;

	switch (type)
	{
	case ERROR:
		t = Error;
		break;
	case CriticalError:
		t = Critical;
		break;
	default:
		return;
		break;
	}

	static HWND hInfoWindow = NULL;
	if (!IsWindow(hInfoWindow)) {

	}

	// [TODO]
	MessageBoxW(NULL, text.c_str(), L"Log", (t == Error || t == Critical)
		? MB_ICONHAND : MB_ICONINFORMATION);

}



