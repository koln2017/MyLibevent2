
// tcpTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "tcpTest.h"
#include "tcpTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#pragma comment(lib, "TcpCommLibD.lib")
#else
#pragma comment(lib, "TcpCommLibR.lib")
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


// CtcpTestDlg 对话框



CtcpTestDlg::CtcpTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TCPTEST_DIALOG, pParent)
	, m_pTcpServer(NULL)
	, m_pTcpClient(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CtcpTestDlg::~CtcpTestDlg()
{
	DeleteTcpCommLib();
}

void CtcpTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RECV, m_editRecv);
	DDX_Control(pDX, IDC_EDIT_SEND, m_editSend);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cmbType);
}

BEGIN_MESSAGE_MAP(CtcpTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CtcpTestDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CtcpTestDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CtcpTestDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &CtcpTestDlg::OnBnClickedButtonNew)
END_MESSAGE_MAP()


// CtcpTestDlg 消息处理程序

BOOL CtcpTestDlg::OnInitDialog()
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

	m_cmbType.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CtcpTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CtcpTestDlg::OnPaint()
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
HCURSOR CtcpTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CtcpTestDlg::TcpcbFun(CBTYPE_EN enType, void *pThis, void *pBuf)
{
	CtcpTestDlg *pDlg = (CtcpTestDlg*)pThis;
	switch (enType)
	{
	case TCP_CLIENT_CONNECT:
	{
		int nFd = *(int*)pBuf;
		int i = 0;
	}
	break;
	case TCP_CLIENT_DISCONNECT:
	{
		int nFd = *(int*)pBuf;
		int i = 0;
	}
	break;
	case TCP_SERVER_CONNECT:
	{
		int nFd = *(int*)pBuf;
		int i = 0;
	}
	break;
	case TCP_SERVER_DISCONNECT:
	{
		int nFd = *(int*)pBuf;
		int i = 0;
	}
	break;
	case TCP_READ_DATA:
	{
		DATA_PACKAGE_T *pData = (DATA_PACKAGE_T*)pBuf;
		if (NULL != pDlg->m_pTcpServer)
		{
			pDlg->m_pTcpServer->Send(pData->pSendID, pData->pData, pData->nLen);
		}
		if (NULL != pDlg->m_pTcpClient)
		{
			pDlg->m_pTcpClient->Send(pData->pSendID, pData->pData, pData->nLen);
		}
		//USES_CONVERSION;
		//CString str;
		//pDlg->m_editRecv.GetWindowText(str);
		//if (_T("") != str)
		//{
		//	str.AppendFormat(_T("\r\n"));
		//}
		//str.AppendFormat(A2W((char*)pData->pData));
		//pDlg->m_editRecv.SetWindowText(str);
	}
	break;
	default:
		break;

	}
}


void CtcpTestDlg::OnBnClickedButtonStart()
{
	int nSel = m_cmbType.GetCurSel();
	if (0 == nSel)
	{
		m_pTcpServer->Start();
	}
	else
	{
		m_pTcpClient->Start();
	}
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
}


void CtcpTestDlg::OnBnClickedButtonStop()
{
	int nSel = m_cmbType.GetCurSel();
	if (0 == nSel)
	{
		m_pTcpServer->Stop();
	}
	else
	{
		m_pTcpClient->Stop();
	}
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
}


void CtcpTestDlg::OnBnClickedButtonSend()
{
	USES_CONVERSION;
	CString str;
	m_editSend.GetWindowText(str);
	int nSel = m_cmbType.GetCurSel();
	if (0 == nSel)
	{
		//TCHAR *pBuf = (LPTSTR)(LPCTSTR)str;
		//m_pTcpServer->Send(NULL, (const unsigned char*)W2A(pBuf), str.GetLength());
	}
	else
	{
		TCHAR *pBuf = (LPTSTR)(LPCTSTR)str;
		m_pTcpClient->Send(NULL, (const unsigned char*)W2A(pBuf), str.GetLength());
	}
}


void CtcpTestDlg::OnBnClickedButtonNew()
{
	CBFUN_PARAM_T param;
	param.pIP = "10.21.38.21";
	param.nPort = 8888;
	param.pThis = this;
	param.cbFun = TcpcbFun;
	int nSel = m_cmbType.GetCurSel();
	if (0 == nSel)
	{
		m_pTcpServer = CreateTcpCommLib(0, param);
		m_pTcpServer->Init();
	}
	else
	{
		m_pTcpClient = CreateTcpCommLib(1, param);
		m_pTcpClient->Init();
	}
	GetDlgItem(IDC_BUTTON_NEW)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	m_cmbType.EnableWindow(FALSE);
}
