#pragma once
#include "afxwin.h"


// SelectDirDlg 对话框

class SelectDirDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SelectDirDlg)

public:
	SelectDirDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SelectDirDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SETPATH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditSrcDir;
	CEdit m_EditDstDir;
	CString m_strSrcDir;
	CString m_strDstDir;
	afx_msg void OnEnSetfocusEditSrcPath();
	afx_msg void OnEnSetfocusEditDstPath();
	CButton m_ButtonOK;
	CEdit m_EditScanTime;
	int m_nScanTime;
	bool m_bRecursive;
	static bool m_1v1Images; // 图像1对1比较
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioRecursive();
	afx_msg void OnBnClickedRadioFile();
};
