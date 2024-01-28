
// MFCSuperWakeupAlarmUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCSuperWakeupAlarmUI.h"
#include "MFCSuperWakeupAlarmUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "../../resource/tool.h"
using namespace std;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCSuperWakeupAlarmUIDlg 对话框



CMFCSuperWakeupAlarmUIDlg::CMFCSuperWakeupAlarmUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCSUPERWAKEUPALARMUI_DIALOG, pParent)
	, myThread_Internal(0)
	, stateIsRunning(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCSuperWakeupAlarmUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, cComboCombox);
	DDX_Control(pDX, IDC_CHECK_LOWPOWER, cCheckLowPower);
}

BEGIN_MESSAGE_MAP(CMFCSuperWakeupAlarmUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCSuperWakeupAlarmUIDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_TOTS, &CMFCSuperWakeupAlarmUIDlg::OnBnClickedButtonTots)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCSuperWakeupAlarmUIDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCSuperWakeupAlarmUIDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CMFCSuperWakeupAlarmUIDlg::OnBnClickedCancel)
	ON_WM_QUERYENDSESSION()
	ON_MESSAGE(MYWM_NOTIFYSHUTDOWNBLOCKING, &CMFCSuperWakeupAlarmUIDlg::OnMywmNotifyshutdownblocking)
END_MESSAGE_MAP()


// CMFCSuperWakeupAlarmUIDlg 消息处理程序

BOOL CMFCSuperWakeupAlarmUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	stateIsRunning = false;
	{
		auto pi = Process.Start_Suspended(L"YKUS");
		if (pi.hProcess && pi.hThread) {
			std::wstring path = Process.GetProcessFullPathById(pi.dwProcessId);
			SetDlgItemTextW(IDC_EDIT1, path.c_str());
			TerminateProcess(pi.hProcess, 0);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}
		else {
			SetDlgItemTextW(IDC_EDIT1, L"<未找到>");
		}
	}
	SetProcessShutdownParameters(0x3FF, 0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCSuperWakeupAlarmUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCSuperWakeupAlarmUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCSuperWakeupAlarmUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


static DWORD WINAPI MyThread(PVOID pObj) {
	CMFCSuperWakeupAlarmUIDlg* dlg = (CMFCSuperWakeupAlarmUIDlg*)pObj;
	if (!dlg) return 87;

	WCHAR ykus[512]{}, tss[32]{}, comText[16]{};
	dlg->GetDlgItemText(IDC_EDIT1, ykus, 512);
	dlg->GetDlgItemText(IDC_EDIT5, tss, 32);
	{
		int cs = dlg->cComboCombox.GetCurSel();
		if (cs >= 0) dlg->cComboCombox.GetLBText(cs, comText);
	}
	bool isLowPowered = !!(BST_CHECKED & dlg->cCheckLowPower.GetState());

	if (ykus[0] == 0 || ykus[0] == L'<' || tss[0] == 0) {
		MessageBox(dlg->GetSafeHwnd(), ErrorCodeToString(87).c_str(), 0, MB_ICONERROR);
		return -1;
	}
	if (comText[0] == 0) {
		if (IDABORT == MessageBox(dlg->GetSafeHwnd(), L"未设置COM。", 0, MB_ICONERROR | MB_ABORTRETRYIGNORE))
			return -1;
	}

	dlg->GetDlgItem(IDOK)->EnableWindow(FALSE);
	dlg->SetDlgItemTextW(IDCANCEL, L"取消 (&E)");
	dlg->stateIsRunning = true;

	SendMessage(dlg->GetSafeHwnd(), MYWM_NOTIFYSHUTDOWNBLOCKING, 1, 0);
	SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);

	time_t currentTime, endTime = atoll(ws2c(tss));
	std::wstring statStr;
	DWORD dwTimeDelay = isLowPowered ? 30000 : 1000;
	do {
		Sleep(dwTimeDelay);
		currentTime = time(0);

		statStr = L"CT=" + std::to_wstring(currentTime) + L",ET=" + std::to_wstring(endTime);
		dlg->SetDlgItemTextW(IDC_EDIT_STAT, statStr.c_str());

		SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
	} while (currentTime < endTime);

	if (currentTime >= endTime) {
		// 打铃
		wstring cRing, cStop;
		cRing = L"\""s + ykus + L"\" " + comText + L" 1 3";
		cStop = L"\""s + ykus + L"\" " + comText + L" 1 2";

		const auto stp = [](wstring& cl) {
			auto pi = Process.Start_HiddenWindow(cl);
			if (pi.hProcess) {
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				if (pi.hThread) CloseHandle(pi.hThread);
			}
		};

		DWORD ret = -1;
		HANDLE hNewThrd = CreateThread(0, 0, [](PVOID pvstr)->DWORD {
			LPCWSTR pstr = (PCWSTR)pvstr;
			int condition;
			s:
			condition = MessageBoxW(0, L"Super Alarm 已启动。\n按下[中止]键，关闭闹铃。",
				L"Super Alarm Service", MB_ICONINFORMATION | MB_ABORTRETRYIGNORE |
				MB_DEFBUTTON2 | MB_SERVICE_NOTIFICATION);
			if (condition != IDABORT) goto s;
			Process.StartOnly_HiddenWindow(pstr);
			return 0;
		}, (void*)cStop.c_str(), 0, 0);
		do {
			SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);

			currentTime = time(0);
			statStr = L"ringing; CT=" + std::to_wstring(currentTime) + L",ET=" + std::to_wstring(endTime);
			dlg->SetDlgItemTextW(IDC_EDIT_STAT, statStr.c_str());
			stp(cRing);
			Sleep(4000);
			currentTime = time(0);
			statStr = L"waiting; CT=" + std::to_wstring(currentTime) + L",ET=" + std::to_wstring(endTime);
			dlg->SetDlgItemTextW(IDC_EDIT_STAT, statStr.c_str());
			stp(cStop);
			Sleep(15000);

			if (!GetExitCodeThread(hNewThrd, &ret)) break;
		} while (STILL_ACTIVE == ret);

		stp(cStop);
	}


	SendMessage(dlg->GetSafeHwnd(), MYWM_NOTIFYSHUTDOWNBLOCKING, 0, 0);
	statStr = L"closed; CT=" + std::to_wstring(currentTime) + L",ET=" + std::to_wstring(endTime);
	dlg->SetDlgItemTextW(IDC_EDIT_STAT, statStr.c_str());
	dlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
	dlg->SetDlgItemTextW(IDCANCEL, L"退出 (&E)");
	dlg->stateIsRunning = false;
	CloseHandle(dlg->myThread_Internal);
	dlg->myThread_Internal = NULL;

	return 0;
}



void CMFCSuperWakeupAlarmUIDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();

	HANDLE hThread = CreateThread(0, 0, MyThread, this, 0, 0);
	if (hThread) {
		//CloseHandle(hThread);
		myThread_Internal = hThread;
		
	}
	else {
		MessageBox(L"无法创建线程", L"Error", MB_ICONERROR);
	}
}


void CMFCSuperWakeupAlarmUIDlg::OnBnClickedButtonTots()
{
	// TODO: 在此添加控件通知处理程序代码

	CString H, M, S;
	GetDlgItemText(IDC_EDIT_h, H);
	GetDlgItemText(IDC_EDIT_m, M);
	GetDlgItemText(IDC_EDIT_s, S);
	unsigned   h, m, s;
	h = atoi(ws2c(H.GetBuffer()));
	m = atoi(ws2c(M.GetBuffer()));
	s = atoi(ws2c(S.GetBuffer()));
	
	struct tm t {};
	time_t currentTime = time(0);
	localtime_s(&t, &currentTime);
	t.tm_hour = h, t.tm_min = m, t.tm_sec = s;

	time_t tt = mktime(&t);
	SetDlgItemText(IDC_EDIT5, std::to_wstring(tt).c_str());
}


void CMFCSuperWakeupAlarmUIDlg::OnBnClickedButton3()
{
	cComboCombox.ResetContent();
	std::wstring ss; HANDLE hFile = 0;
	for (unsigned i = 1; i < 25; ++i) {
		ss = L"COM" + std::to_wstring(i);
		hFile = CreateFileW(ss.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		if (hFile && INVALID_HANDLE_VALUE != hFile) {
			CloseHandle(hFile);
			cComboCombox.InsertString(-1, ss.c_str());
		}
	}
}


void CMFCSuperWakeupAlarmUIDlg::OnBnClickedButton1()
{
	// copied from https://learn.microsoft.com/zh-cn/windows/win32/dlgbox/using-common-dialog-boxes
	// I'm lazy
	// 
	// for Dear Furina, Regina of All Waters, Kindreds, Peoples and Laws

	OPENFILENAME ofn{};       // common dialog box structure
	wchar_t szFile[260]{};       // buffer for file name
	HWND hwnd = GetSafeHwnd();              // owner window
	//HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"YKUS Executable\0YKUS.exe\0Executable\0*.exe\0All Files\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE) {
	/*	hf = CreateFile(ofn.lpstrFile,
			GENERIC_READ,
			0,
			(LPSECURITY_ATTRIBUTES)NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			(HANDLE)NULL);*/
		SetDlgItemText(IDC_EDIT1, ofn.lpstrFile);
	}
}


void CMFCSuperWakeupAlarmUIDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	if (stateIsRunning) {
		if (IDYES != MessageBox(L"确定取消当前操作吗?", L"?", MB_YESNO | MB_ICONWARNING)) return;
		if (myThread_Internal) {
			TerminateThread(myThread_Internal, ERROR_CANCELLED);
			CloseHandle(myThread_Internal);
			auto* dlg = this;
			SendMessage(MYWM_NOTIFYSHUTDOWNBLOCKING, 0, 0);
			dlg->SetDlgItemTextW(IDC_EDIT_STAT, (L"cancelled. CT="s + to_wstring(time(0))).c_str());
			dlg->GetDlgItem(IDOK)->EnableWindow(TRUE);
			dlg->SetDlgItemTextW(IDCANCEL, L"退出 (&E)");
			dlg->stateIsRunning = false;
		}
		return;
	}
	CDialogEx::OnCancel();
}


BOOL CMFCSuperWakeupAlarmUIDlg::OnQueryEndSession()
{
	if (!CDialogEx::OnQueryEndSession())
		return FALSE;

	// TODO:  在此添加专用的查询结束会话代码
	if (stateIsRunning) return false;

	return TRUE;
}


afx_msg LRESULT CMFCSuperWakeupAlarmUIDlg::OnMywmNotifyshutdownblocking(WPARAM wParam, LPARAM lParam)
{
	if (wParam) {
		ShutdownBlockReasonCreate(GetSafeHwnd(),L"至少有一个正在运行的 Super Alarm。");
	}
	else {
		ShutdownBlockReasonDestroy(GetSafeHwnd());
	}
	return 0;
}
