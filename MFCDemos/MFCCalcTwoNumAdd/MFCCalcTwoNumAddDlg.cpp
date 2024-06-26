﻿
// MFCCalcTwoNumAddDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCCalcTwoNumAdd.h"
#include "MFCCalcTwoNumAddDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CMFCCalcTwoNumAddDlg 对话框



CMFCCalcTwoNumAddDlg::CMFCCalcTwoNumAddDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCCALCTWONUMADD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCCalcTwoNumAddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MTO, m_cbysf);
}

BEGIN_MESSAGE_MAP(CMFCCalcTwoNumAddDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCCalcTwoNumAddDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_C_DLL, &CMFCCalcTwoNumAddDlg::OnBnClickedButtonCDll)
	ON_EN_CHANGE(IDC_EDIT_NUM2, &CMFCCalcTwoNumAddDlg::OnEnChangeEditNum2)
	ON_CBN_SELCHANGE(IDC_COMBO_MTO, &CMFCCalcTwoNumAddDlg::OnCbnSelchangeComboMto)
END_MESSAGE_MAP()


// CMFCCalcTwoNumAddDlg 消息处理程序

BOOL CMFCCalcTwoNumAddDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCCalcTwoNumAddDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCCalcTwoNumAddDlg::OnPaint()
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
HCURSOR CMFCCalcTwoNumAddDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int add(int a, int b) {
	return a + b;
}
int unadd(int a, int b) {
	return a - b;
}
int aadd(int a, int b) {
	return a * b;
}
int unaadd(int a, int b) {
	return a / b;
}



void CMFCCalcTwoNumAddDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_cbysf.GetCurSel();
    CString strCBText;
	m_cbysf.GetLBText( nIndex, strCBText);
	if (strCBText == L"+") SetDlgItemInt(IDC_SHOW_RESULT, add(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
	if (strCBText == L"-") SetDlgItemInt(IDC_SHOW_RESULT, unadd(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
	if (strCBText == L"*") SetDlgItemInt(IDC_SHOW_RESULT, aadd(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
	if (strCBText == L"/") SetDlgItemInt(IDC_SHOW_RESULT, unaadd(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
}


#include<string>
void CMFCCalcTwoNumAddDlg::OnBnClickedButtonCDll()
{
	// TODO: 在此添加控件通知处理程序代码
	using namespace std;
	int nIndex = m_cbysf.GetCurSel();
    CString strCBText;
	m_cbysf.GetLBText( nIndex, strCBText);
	HMODULE dm = LoadLibraryA("calc.dll");
	if (dm == NULL) {
		MessageBox(((wstring)L"无法加载DLL: 方法LoadLibraryA ,错误" + to_wstring(GetLastError())).c_str(), L"错误", MB_ICONERROR); return;
	}
	using FUNADDR=int(*)(int a,int b);   
	FUNADDR dllad=(FUNADDR)GetProcAddress(dm, "add");
	FUNADDR dllunad=(FUNADDR)GetProcAddress(dm, "unadd");
	FUNADDR dllaad=(FUNADDR)GetProcAddress(dm, "aadd");
	FUNADDR dllunaad=(FUNADDR)GetProcAddress(dm, "unaadd");
#define dlladd (dllad)
#define dllunadd (dllunad)
#define dllaadd (dllaad)
#define dllunaadd (dllunaad)
	if (strCBText == L"+") SetDlgItemInt(IDC_SHOW_RESULT, dlladd(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
	if (strCBText == L"-") SetDlgItemInt(IDC_SHOW_RESULT, dllunadd(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
	if (strCBText == L"*") SetDlgItemInt(IDC_SHOW_RESULT, dllaadd(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
	if (strCBText == L"/") SetDlgItemInt(IDC_SHOW_RESULT, dllunaadd(GetDlgItemInt(IDC_EDIT_NUM1),GetDlgItemInt(IDC_EDIT_NUM2)));
}


void CMFCCalcTwoNumAddDlg::OnEnChangeEditNum2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (GetDlgItemInt(IDC_EDIT_NUM2) == 0 && m_cbysf.GetCurSel() == 3) {
		GetDlgItem(IDOK)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_C_DLL)->EnableWindow(0);
	}
	else {
		GetDlgItem(IDOK)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_C_DLL)->EnableWindow(1);
	}
}


void CMFCCalcTwoNumAddDlg::OnCbnSelchangeComboMto()
{
	// TODO: 在此添加控件通知处理程序代码
	if (GetDlgItemInt(IDC_EDIT_NUM2) == 0 && m_cbysf.GetCurSel() == 3) {
		GetDlgItem(IDOK)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_C_DLL)->EnableWindow(0);
	}
	else {
		GetDlgItem(IDOK)->EnableWindow(1);
		GetDlgItem(IDC_BUTTON_C_DLL)->EnableWindow(1);
	}
}
