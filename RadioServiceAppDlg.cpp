
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
#include "RadioServiceAppDlg.h"

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
/*
0 dB - 3F
5 dB - 35
10 dB - 2B
15 dB - 21
20 dB- 17
25 dB - 0D
30 dB - 03
*/
device_data CRadioServiceAppDlg::getting_data(){

	device_data dt = { 0 };

	int cur_sel_atten;
	BOOL lpTrans = FALSE;

	dt.start_freq = GetDlgItemInt(IDC_START_FREQ, &lpTrans, FALSE);
	printf("start_freq: %d ", dt.start_freq);

	dt.end_freq = GetDlgItemInt(IDC_END_FREQ, &lpTrans, FALSE);
	printf("end_freq: %d ", dt.end_freq);

	cur_sel_atten = GetDlgItemInt(IDC_COMBO_ATTENU, &lpTrans, FALSE);
	if (cur_sel_atten == 0)
	{
		dt.attenua = 0x3F;
	}
	else if (cur_sel_atten == 5)
	{
		dt.attenua = 0x35;
	}
	else if (cur_sel_atten == 10)
	{
		dt.attenua = 0x2B;
	}
	else if (cur_sel_atten == 15)
	{
		dt.attenua = 0x21;
	}
	else if (cur_sel_atten == 20)
	{
		dt.attenua = 0x17;
	}
	else if (cur_sel_atten == 25)
	{
		dt.attenua = 0x0D;
	}
	else if (cur_sel_atten == 30)
	{
		dt.attenua = 0x03;
	}
	else{
		printf("unknown attenua selection, set to 0 dB ");
		dt.attenua = 0x3F;
	}

	printf("attenua: 0x%02x\n", dt.attenua);

	return dt;
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
	ON_BN_CLICKED(IDOK, &CRadioServiceAppDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_USB, &CRadioServiceAppDlg::OnBnClickedButtonWriteUsb)
	ON_BN_CLICKED(IDC_BUTTON_READ_SYNC, &CRadioServiceAppDlg::OnBnClickedButtonReadSync)
	ON_BN_CLICKED(IDC_BUTTON_READ_ASYNC, &CRadioServiceAppDlg::OnBnClickedButtonReadAsync)
	ON_BN_CLICKED(IDC_BUTTON_Abort, &CRadioServiceAppDlg::OnBnClickedButtonAbort)
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
	GetDlgItem(IDC_BUTTON_READ_ASYNC)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_READ_SYNC)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_WRITE_USB)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Abort)->ShowWindow(FALSE);
	GetDlgItem(IDC_INIT)->ShowWindow(FALSE);
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
	SetupUsbOUT_init();
}

void CRadioServiceAppDlg::OnBnClickedPusk(){

#ifdef TESTING
	reset_pipe(0x82);
	SetupUsbIN(0xc0, 0xab, 0, 0, 64);
	for (int i = 0; i < 8; i++){
		SetupUsbOUT_init();
		SetupUsbIN(0xc0, 0xae, 0, 0, 64);
	}
	SetupUsbOUT_empty();
	Sleep(300);
	abort_pipe(0x82);

	device_data dt = getting_data();

	SetupUsbOUT_settings(dt);
	loops = ceilf((float)(dt.end_freq - dt.start_freq) / 20);
	for (int i = 0; i < loops; i++){
		UINT8 buf[1024] = { 0 };
		read_usb_async(0x82, buf, 1024);
	}
#endif

#ifndef THREAD_TEST
	if (XferThread)
	{
		bLooping = false;
		GetDlgItem(IDC_PUSK)->SetWindowTextW(_T("стоп"));
		return;
	}
	SetupUsbOUT_init();
	device_data dt = getting_data();
	SetupUsbOUT_settings(dt);

	bLooping = true;
	loops = ceilf((float)(dt.end_freq - dt.start_freq) / 20);
	XferThread = AfxBeginThread(XferLoop, this);
	GetDlgItem(IDC_PUSK)->SetWindowTextW(_T("старт"));
#endif
}


void CRadioServiceAppDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CRadioServiceAppDlg::OnBnClickedButtonWriteUsb()
{
	// TODO: Add your control notification handler code here
	UCHAR buf[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	write_usb(0x02, buf, 10);
}


void CRadioServiceAppDlg::OnBnClickedButtonReadSync()
{
	// TODO: Add your control notification handler code here
	UINT8 buf[10] = { 0 };
	read_usb_sync(0x86, buf, 10);
}


void CRadioServiceAppDlg::OnBnClickedButtonReadAsync()
{
	// TODO: Add your control notification handler code here
	UINT8 buf[10] = { 0 };
	read_usb_async(0x86, buf, 10);
}


void CRadioServiceAppDlg::OnBnClickedButtonAbort()
{
	// TODO: Add your control notification handler code here
	abort_pipe(0x82);
}
