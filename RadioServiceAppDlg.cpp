
// RadioServiceAppDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "RadioServiceApp.h"
//#include "RadioServiceAppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ConsoleDebug.h"

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRadioServiceAppDlg dialog



CRadioServiceAppDlg::CRadioServiceAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRadioServiceAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRadioServiceAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRadioServiceAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL,  &CRadioServiceAppDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_INIT, &CRadioServiceAppDlg::OnBnClickedInitialization)
	ON_BN_CLICKED(IDC_PUSK,  &CRadioServiceAppDlg::OnBnClickedPusk)
END_MESSAGE_MAP()


// CRadioServiceAppDlg message handlers

BOOL CRadioServiceAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	CreateConsole();
	InitUsb();
	GetDlgItem(IDC_START_FREQ)->SetWindowTextW(_T("580"));
	GetDlgItem(IDC_END_FREQ)->SetWindowTextW(_T("750"));
	((CComboBox*)GetDlgItem(IDC_COMBO_ATTENU))->SelectString(0,_T("0 dB"));
	XferThread = NULL;
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRadioServiceAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRadioServiceAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRadioServiceAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRadioServiceAppDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CRadioServiceAppDlg::OnBnClickedInitialization(){
	SendInitUsbPacket();
	RecvInitUsbPacket();
	SendEmptyUsbPacket();
	RecvInitUsbPacket();
}
/*
0 dB - 3F
5 dB - 35
10 dB - 2B 
15 dB - 21
20 dB- 17
25 dB - 0D
30 dB - 03
*/
void CRadioServiceAppDlg::OnBnClickedPusk(){


	if (XferThread)
	{
		bLooping = false;
		GetDlgItem(IDC_PUSK)->SetWindowTextW(_T("старт"));
		return;
	}
	//printf("OnBnClickedPusk()\n");
	UINT16 start_freq;
	UINT16 end_freq;
	UINT8 attenua;

	int cur_sel_atten;
	BOOL lpTrans = FALSE;

	start_freq = GetDlgItemInt(IDC_START_FREQ, &lpTrans, FALSE);
	printf("start_freq: %d ", start_freq);

	end_freq = GetDlgItemInt(IDC_END_FREQ, &lpTrans, FALSE);
	printf("end_freq: %d ", end_freq);

	cur_sel_atten = GetDlgItemInt(IDC_COMBO_ATTENU, &lpTrans, FALSE);
	if (cur_sel_atten == 0)
	    { attenua = 0x3F;}
	else if (cur_sel_atten == 5)
	    { attenua = 0x35;}
	else if (cur_sel_atten == 10)
	    { attenua = 0x2B;}
	else if (cur_sel_atten == 15)
	    { attenua = 0x21;}
	else if (cur_sel_atten == 20)
	    { attenua = 0x17;}
	else if (cur_sel_atten == 25)
	    { attenua = 0x0D;}
	else if (cur_sel_atten == 30)
	    { attenua = 0x03;}
	else{
		printf("unknown attenua selection, set to 0 dB ");
		attenua = 0x3F;
	}

	printf("attenua: 0x%02x\n", attenua);
	//_tprintf(_T("%s\n"), (LPCTSTR)cur_sel);
	SendSetupUsbPacket(start_freq, end_freq, attenua);

	bLooping = true;
	XferThread = AfxBeginThread(XferLoop, this);

	GetDlgItem(IDC_PUSK)->SetWindowTextW(_T("стоп"));
}
