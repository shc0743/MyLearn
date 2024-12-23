﻿#pragma once


// CDlgMyInfo 对话框

class CDlgMyInfo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMyInfo)

public:
	CDlgMyInfo(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgMyInfo();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MYINFO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonInfoInit();
	afx_msg void OnBnClickedButtonInfsave();
};
