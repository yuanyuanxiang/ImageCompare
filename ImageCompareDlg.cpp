
// ImageCompareDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageCompare.h"
#include "ImageCompareDlg.h"
#include "afxdialogex.h"
#include "SelectDirDlg.h"

#include <windows.h>
#include <io.h>
#include "config.h"

#include <iosfwd>
#include "Resource.h"
using namespace std;

// @see https://github.com/yuanyuanxiang/public
// 请将此项目克隆
#include "../public/fileop.h"

ofstream out;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Scalar getSSIM(const cv::Mat &i1, const cv::Mat &i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	int d = CV_32F;
	Mat I1, I2;
	i1.convertTo(I1, d);
	i2.convertTo(I2, d);
	Mat I2_2 = I2.mul(I2);
	Mat I1_2 = I1.mul(I1);

	Mat I1_I2 = I1.mul(I2);
	Mat mu1, mu2;
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);
	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);
	Mat sigma1_2, sigma2_2, sigma12;
	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;
	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;
	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;
	Mat t1, t2, t3;
	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);
	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);
	Mat ssim_map;
	divide(t3, t1, ssim_map);

	Scalar mssim = mean(ssim_map);

	return mssim;
}

// 比较2幅图像，得到psnr和ssim.
void Compare(const cv::Mat &m1, const cv::Mat &m2, double &psnr, double &ssim) {
	clock_t t = clock();
	psnr = cv::PSNR(m1, m2);
	Scalar s = getSSIM(m1, m2);
	ssim = (s.val[0] + s.val[1] + s.val[2]) / 3.0;
	TRACE("===> Compare using: %d ms.\n", clock() - t);
}

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


// CImageCompareDlg 对话框



CImageCompareDlg::CImageCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IMAGECOMPARE_DIALOG, pParent)
	, m_nCurPic(0)
	, m_nTotalPic(0)
	, m_fPSNR(0)
	, m_fSSIM(0)
	, m_strSrcName(_T(""))
	, m_strDstName(_T(""))
{
	padding_w = 0;
	padding_h = 0;
	m_isScan = false;
	m_showWarning = true;
	char s[_MAX_PATH];
	GetPrivateProfileStringA("settings", "src_dir", "", s, sizeof(s), "./settings.ini");
	m_strSrcDir = s;
	GetPrivateProfileStringA("settings", "dst_dir", "", s, sizeof(s), "./settings.ini");
	m_strDstDir = s;
	padding_w = GetPrivateProfileIntA("settings", "pad_w", 0, "./settings.ini");
	padding_h = GetPrivateProfileIntA("settings", "pad_h", 0, "./settings.ini");
	m_recursive = GetPrivateProfileIntA("settings", "recursive", 0, "./settings.ini");
	m_nSleep = GetPrivateProfileIntA("settings", "timer", 1000, "./settings.ini");
	m_nSleep = max(m_nSleep, 10);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_SRC, m_PicSrc);
	DDX_Control(pDX, IDC_PIC_DST, m_PicDst);
	DDX_Control(pDX, IDC_EDIT_PSNR, m_EditPSNR);
	DDX_Control(pDX, IDC_EDIT_SSIM, m_EditSSIM);
	DDX_Text(pDX, IDC_EDIT_CURRENT, m_nCurPic);
	DDX_Text(pDX, IDC_EDIT_TOTAL, m_nTotalPic);
	DDX_Text(pDX, IDC_EDIT_PSNR, m_fPSNR);
	DDX_Text(pDX, IDC_EDIT_SSIM, m_fSSIM);
	DDX_Control(pDX, IDC_PREV, m_ButtonPrev);
	DDX_Control(pDX, IDC_NEXT, m_ButtonNext);
	DDX_Control(pDX, IDC_EDIT_SRC_NAME, m_EditSrcName);
	DDX_Text(pDX, IDC_EDIT_SRC_NAME, m_strSrcName);
	DDX_Control(pDX, IDC_EDIT_DST_NAME, m_EditDstName);
	DDX_Text(pDX, IDC_EDIT_DST_NAME, m_strDstName);
	DDX_Control(pDX, IDC_EDIT_CURRENT, m_EditCur);
	DDX_Control(pDX, IDC_EDIT_TOTAL, m_EditTotal);
	DDX_Control(pDX, IDC_BUTTON_SCAN, m_ButtonScan);
}

BEGIN_MESSAGE_MAP(CImageCompareDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(ID_FILE_SET, &CImageCompareDlg::OnFileSet)
	ON_COMMAND(ID_FILE_QUIT, &CImageCompareDlg::OnFileQuit)
	ON_BN_CLICKED(IDC_PREV, &CImageCompareDlg::OnBnClickedPrev)
	ON_BN_CLICKED(IDC_NEXT, &CImageCompareDlg::OnBnClickedNext)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, &CImageCompareDlg::OnBnClickedButtonScan)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_STN_DBLCLK(IDC_PIC_SRC, &CImageCompareDlg::OnStnDblclickPicSrc)
	ON_STN_DBLCLK(IDC_PIC_DST, &CImageCompareDlg::OnStnDblclickPicDst)
END_MESSAGE_MAP()


// CImageCompareDlg 消息处理程序

BOOL CImageCompareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
	m_PicSrc.GetWindowRect(&m_rtSrc);
	ScreenToClient(&m_rtSrc);
	m_PicDst.GetWindowRect(&m_rtDst);
	ScreenToClient(&m_rtDst);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CImageCompareDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CImageCompareDlg::OnPaint()
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
		if (!m_src.IsNull()&&!m_dst.IsNull())
		{
			CPaintDC dc(this);
			m_src.Draw(dc, m_rtSrc, Gdiplus::InterpolationModeBilinear);
			m_dst.Draw(dc, m_rtDst, Gdiplus::InterpolationModeBilinear);
		}
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImageCompareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 从 CImage创建Mat
cv::Mat CreateMat(const CImage &m_src) {
	int bpp = m_src.GetBPP();
	assert(bpp == 24 || bpp == 32);
	int type = bpp == 24 ? CV_8UC3 : CV_8UC4;
	uchar *pHead = (uchar*)m_src.GetBits() + (m_src.GetHeight() - 1)*m_src.GetPitch();
	cv::Mat m(m_src.GetHeight(), m_src.GetWidth(), type, pHead);
	return m;
}

bool CImageCompareDlg::Load(int cur, bool showWarning) {
	m_src.Destroy();
	const char *image1 = m_srcList.at(cur-1).c_str();
	if (S_OK != m_src.Load(CString(image1))) {
		if(showWarning)MessageBox(_T("Load src image failed."), _T("Warning"),
			MB_OK | MB_ICONWARNING);
		return false;
	}
	m_dst.Destroy();
	const char *image2 = m_dstList.at(cur-1).c_str();
	if (S_OK != m_dst.Load(CString(image2))) {
		if (showWarning)MessageBox(_T("Load dst image failed."), _T("Warning"),
			MB_OK | MB_ICONWARNING);
		return false;
	}

	m_nCurPic = cur;
	m_strSrcName = image1;
	m_strDstName = image2;
	cv::Mat m1 = CreateMat(m_src);
	cv::Mat b = CreateMat(m_dst);
	cv::Mat m2 = b(cv::Rect(0, 0, b.cols - padding_w, b.rows - padding_h));
	if (m1.rows != m2.rows || m1.cols != m2.cols || m1.type() != m2.type())
	{
		UpdateData(FALSE);
		Invalidate(TRUE);
		if (showWarning)
			MessageBox(_T("Size/Type of images don't match."), _T("Warning"),
				MB_OK | MB_ICONWARNING);
		return false;
	} else {
		Compare(m1, m2, m_fPSNR, m_fSSIM);
		if (out.is_open())
			out << image1 << " vs. " << image2 << "|"
			<< m_fPSNR << "," << m_fSSIM << "\n";
		UpdateData(FALSE);
		Invalidate(TRUE);
		return true;
	}
}

// 加载第一张图
void CImageCompareDlg::LoadFirst(CString src_, CString dst_) {
	m_fPSNR = m_fSSIM = 0;
	m_nCurPic = m_nTotalPic = 0;
	DWORD a1 = GetFileAttributes(src_);
	DWORD a2 = GetFileAttributes(dst_);
	if (a1 != a2)
	{
		MessageBox(_T("Wrong file attributes."), _T("Warning"),
			MB_OK | MB_ICONWARNING);
		return;
	}
	USES_CONVERSION;
	const char *src = W2A(src_);
	const char *dst = W2A(dst_);
	m_strSrcDir = src;
	m_strDstDir = dst;
	m_srcList = (a1&FILE_ATTRIBUTE_DIRECTORY) ?
		getFilesByDir(m_strSrcDir, ".*", m_recursive) :
		FileList(1, src);
	m_dstList = (a2&FILE_ATTRIBUTE_DIRECTORY) ?
		getFilesByDir(m_strDstDir, ".*", m_recursive) :
		FileList(1, dst);
	if (m_srcList.empty() || m_dstList.empty())
	{
		MessageBox(_T("Directory is empty."), _T("Warning"),
			MB_OK | MB_ICONWARNING);
		return;
	}
	if (m_srcList.size() != m_dstList.size())
	{
		MessageBox(_T("Wrong num of pictures."), _T("Warning"),
			MB_OK | MB_ICONWARNING);
		m_srcList.clear();
		m_dstList.clear();
		return;
	}
	m_nTotalPic = m_srcList.size();
	m_ButtonScan.EnableWindow(m_nTotalPic > 2);
	UpdateData(FALSE);
	Load(1);
	WritePrivateProfileStringA("settings", "src_dir", m_strSrcDir.c_str(), "./settings.ini");
	WritePrivateProfileStringA("settings", "dst_dir", m_strDstDir.c_str(), "./settings.ini");
	char buf[32];
	sprintf_s(buf, "%d", m_recursive ? 1 : 0);
	WritePrivateProfileStringA("settings", "recursive", buf, "./settings.ini");
}

void CImageCompareDlg::OnFileSet()
{
	if (m_isScan)return;

	SelectDirDlg dlg;
	CString s1 = CString((m_strSrcDir.c_str()));
	CString s2 = CString((m_strDstDir.c_str()));
	dlg.m_strSrcDir = s1; dlg.m_strDstDir = s2;
	dlg.m_nScanTime = m_nSleep; dlg.m_bRecursive = m_recursive;
	bool first_run = m_src.IsNull();
	if (dlg.DoModal() != IDOK)
		return;
	if (m_nSleep!= dlg.m_nScanTime){
		m_nSleep = dlg.m_nScanTime;
		char buf[32];
		sprintf_s(buf, "%d", m_nSleep);
		WritePrivateProfileStringA("settings", "timer", buf, "./settings.ini");
	}
	if ((m_recursive == dlg.m_bRecursive 
		&& s1 == dlg.m_strSrcDir && s2 == dlg.m_strDstDir) && !first_run) {
		return;
	}

	m_recursive = dlg.m_bRecursive;

	LoadFirst(dlg.m_strSrcDir, dlg.m_strSrcDir);
}


void CImageCompareDlg::OnFileQuit()
{
	if (m_isScan)return;

	SendMessage(WM_CLOSE, 0, 0);
}


void CImageCompareDlg::OnBnClickedPrev()
{
	if (m_nCurPic > 1)
	{
		Load(m_nCurPic - 1);
	}
}


void CImageCompareDlg::OnBnClickedNext()
{
	if (m_nCurPic < m_nTotalPic)
	{
		Load(m_nCurPic + 1);
	}
}


void CImageCompareDlg::OnBnClickedButtonScan()
{
	if (m_isScan)return;
	m_isScan = true;

	time_t t = time(0);
	char path[_MAX_PATH];
	strftime(path, sizeof(path), "./%Y-%m-%d %H-%M-%S.txt", localtime(&t));
	out.open(path);
	if (out.is_open())
	{
		m_showWarning = false;
		m_ButtonPrev.EnableWindow(FALSE);
		m_ButtonNext.EnableWindow(FALSE);
		SetTimer(1, m_nSleep, NULL);
	}else {
		MessageBox(_T("Scan pictures failed."), _T("Warning"),
			MB_OK | MB_ICONWARNING);
		m_isScan = false;
	}
}


void CImageCompareDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		if (m_nCurPic < m_nTotalPic) {
			OnBnClickedNext();
		}
		else
		{
			KillTimer(nIDEvent);
			m_isScan = false;
			m_ButtonPrev.EnableWindow(TRUE);
			m_ButtonNext.EnableWindow(TRUE);
			if (out.is_open())
				out.close();
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CImageCompareDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	if (out.is_open())
		out.close();
}


BOOL CImageCompareDlg::OnEraseBkgnd(CDC* pDC)
{
	if (!m_src.IsNull() && !m_dst.IsNull())
	{
		// === 
		//rtSrc, rtDst
		// === 
		CRect rect;
		GetClientRect(&rect);
		pDC->PatBlt(0, 0, rect.Width(), m_rtSrc.top, PATCOPY);
		pDC->PatBlt(0, m_rtSrc.bottom, rect.Width(), rect.bottom-m_rtSrc.bottom, PATCOPY);
		return TRUE;
	}

	return CDialogEx::OnEraseBkgnd(pDC);
}

BOOL CImageCompareDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && 
		(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN))
	{
		if (pMsg->wParam == VK_RETURN && m_nCurPic 
			&& !m_isScan && GetFocus() == &m_EditCur){
			int cur = m_nCurPic;
			UpdateData(TRUE);
			if (1<=m_nCurPic && m_nCurPic<= m_nTotalPic)
			{
				Load(m_nCurPic);
			}else{
				m_nCurPic = cur;
				UpdateData(FALSE);
			}
		}
		return TRUE;
	}
	// 上一张图
	if (pMsg->message == WM_KEYDOWN && 
		(pMsg->wParam == VK_LEFT || pMsg->wParam == VK_UP || pMsg->wParam == VK_PRIOR))
	{
		if (!m_isScan && GetFocus() != &m_EditCur)
		{
			OnBnClickedPrev();
			return TRUE;
		}
	}
	// 下一张图
	if (pMsg->message == WM_KEYDOWN && 
		(pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_NEXT))
	{
		if (!m_isScan && GetFocus() != &m_EditCur)
		{
			OnBnClickedNext();
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

// 双击选择src
void CImageCompareDlg::OnStnDblclickPicSrc()
{
	if (m_srcList.size() <= 1)
	{
		CFileDialog dlg(TRUE);
		if (dlg.DoModal() == IDOK) {
			SelectDirDlg::m_1v1Images = true;
			CString path = dlg.GetPathName();
			LoadFirst(path, m_dst.IsNull() ? path : CString(m_strDstDir.c_str()));
		}
	}
}

// 双击选择dst
void CImageCompareDlg::OnStnDblclickPicDst()
{
	if (m_srcList.size() == 1 && m_dstList.size() <= 1)
	{
		CFileDialog dlg(TRUE);
		if (dlg.DoModal() == IDOK) {
			CString path = dlg.GetPathName();
			LoadFirst(CString(m_strSrcDir.c_str()), path);
		}
	}
}
