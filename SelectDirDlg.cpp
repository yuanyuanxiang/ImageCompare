// SelectDirDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageCompare.h"
#include "SelectDirDlg.h"
#include "afxdialogex.h"
#include "Resource.h"


// SelectDirDlg 对话框

bool SelectDirDlg::m_1v1Images = false;

IMPLEMENT_DYNAMIC(SelectDirDlg, CDialogEx)

SelectDirDlg::SelectDirDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SETPATH, pParent)
	, m_strSrcDir(_T(""))
	, m_strDstDir(_T(""))
	, m_nScanTime(0)
	, m_bRecursive(false)
{
}

SelectDirDlg::~SelectDirDlg()
{
}

void SelectDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SRC_PATH, m_EditSrcDir);
	DDX_Control(pDX, IDC_EDIT_DST_PATH, m_EditDstDir);
	DDX_Text(pDX, IDC_EDIT_SRC_PATH, m_strSrcDir);
	DDX_Text(pDX, IDC_EDIT_DST_PATH, m_strDstDir);
	DDX_Control(pDX, IDOK, m_ButtonOK);
	DDX_Control(pDX, IDC_EDIT_SCAN_TIMER, m_EditScanTime);
	DDX_Text(pDX, IDC_EDIT_SCAN_TIMER, m_nScanTime);
	DDV_MinMaxInt(pDX, m_nScanTime, 10, 5000);
}


BEGIN_MESSAGE_MAP(SelectDirDlg, CDialogEx)
	ON_EN_SETFOCUS(IDC_EDIT_SRC_PATH, &SelectDirDlg::OnEnSetfocusEditSrcPath)
	ON_EN_SETFOCUS(IDC_EDIT_DST_PATH, &SelectDirDlg::OnEnSetfocusEditDstPath)
	ON_BN_CLICKED(IDC_RADIO_RECURSIVE, &SelectDirDlg::OnBnClickedRadioRecursive)
	ON_BN_CLICKED(IDC_RADIO_FILE, &SelectDirDlg::OnBnClickedRadioFile)
END_MESSAGE_MAP()


// SelectDirDlg 消息处理程序


void SelectDirDlg::OnEnSetfocusEditSrcPath()
{
	static bool isSelect = false;
	if (isSelect) return;
	isSelect = true;

	CButton *p = (CButton *)GetDlgItem(IDC_RADIO_FILE);
	if (p->GetCheck())
	{
		CFileDialog dlg(TRUE);
		if (dlg.DoModal() == IDOK) {
			m_strSrcDir = dlg.GetPathName();
			UpdateData(FALSE);
		}
	}
	else
	{
		char szPath[MAX_PATH] = { 0 };
		BROWSEINFO bi = { 0 };
		bi.hwndOwner = m_hWnd;
		bi.lpszTitle = _T("Select src dir");
		LPITEMIDLIST lp = SHBrowseForFolder(&bi);
		if (lp && SHGetPathFromIDListA(lp, szPath))
		{
			m_strSrcDir = CString(szPath);
			UpdateData(FALSE);
		}
	}

	m_ButtonOK.SetFocus();
	isSelect = false;
}


void SelectDirDlg::OnEnSetfocusEditDstPath()
{
	static bool isSelect = false;
	if (isSelect) return;
	isSelect = true;

	CButton *p = (CButton *)GetDlgItem(IDC_RADIO_FILE);
	if (p->GetCheck())
	{
		CFileDialog dlg(TRUE);
		if (dlg.DoModal() == IDOK) {
			m_strDstDir = dlg.GetPathName();
			UpdateData(FALSE);
		}
	}
	else {
		char szPath[MAX_PATH] = { 0 };
		BROWSEINFO bi = { 0 };
		bi.hwndOwner = m_hWnd;
		bi.lpszTitle = _T("Select dst dir");
		LPITEMIDLIST lp = SHBrowseForFolder(&bi);
		if (lp && SHGetPathFromIDListA(lp, szPath))
		{
			m_strDstDir = CString(szPath);
			UpdateData(FALSE);
		}
	}

	m_ButtonOK.SetFocus();
	isSelect = false;
}


void SelectDirDlg::OnOK()
{
	CButton *p = (CButton *)GetDlgItem(IDC_RADIO_RECURSIVE);
	if (p->GetSafeHwnd())
		m_bRecursive = p->GetCheck();
	p = (CButton *)GetDlgItem(IDC_RADIO_FILE);
	if (p->GetSafeHwnd())
		m_1v1Images = p->GetCheck();
	CDialogEx::OnOK();
}


BOOL SelectDirDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CButton *p = (CButton *)GetDlgItem(IDC_RADIO_RECURSIVE);
	if (p->GetSafeHwnd())
		p->SetCheck(m_bRecursive);
	p = (CButton *)GetDlgItem(IDC_RADIO_FILE);
	if (p->GetSafeHwnd())
		p->SetCheck(m_1v1Images);

	return TRUE;
}


void SelectDirDlg::OnBnClickedRadioRecursive()
{
	m_bRecursive = !m_bRecursive;
	CButton *p = (CButton *)GetDlgItem(IDC_RADIO_RECURSIVE);
	if (p->GetSafeHwnd())
		p->SetCheck(m_bRecursive);
}


void SelectDirDlg::OnBnClickedRadioFile()
{
	CButton *p = (CButton *)GetDlgItem(IDC_RADIO_FILE);
	BOOL b = p->GetCheck();
	if (p->GetSafeHwnd())
		p->SetCheck(!b);
}
