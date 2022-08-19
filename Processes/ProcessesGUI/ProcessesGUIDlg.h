﻿
// ProcessesGUIDlg.h: 头文件
//

#pragma once


// CProcessesGUIDlg 对话框
class CProcessesGUIDlg : public CDialogEx
{
// 构造
public:
	CProcessesGUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDDProcess_tESGUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_main;
	afx_msg void OnBnClickedButtonKillNow();
	afx_msg void OnBnClickedButtonKillCurrent();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnNMRClickListMain(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void On32771();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
