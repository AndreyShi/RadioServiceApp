
// RadioServiceAppDlg.cpp : implementation file
//

#include "stdafx.h"
//#include "RadioServiceApp.h"
//#include "RadioServiceAppDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#include "ConsoleDebug.h"
//#include "RadioServiceAppDlg.h"

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
	ptr_usb_data = NULL;
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
CRadioServiceAppDlg::device_data CRadioServiceAppDlg::getting_data(){

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

CRadioServiceAppDlg::cur_time CRadioServiceAppDlg::get_cur_time(){
	SYSTEMTIME st;
	cur_time res;

	GetLocalTime(&st);  // Получаем локальное время
	sprintf(res.st,"%02d:%02d:%02d.%03d",
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return res;
}

void CRadioServiceAppDlg::save_hex_buffer_to_file(const unsigned char *buffer, size_t buffer_size, const char *filename) {
	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		perror("Failed to open file");
		return;
	}

	for (size_t i = 0; i < buffer_size; i++) {
		// Печатаем байт в HEX формате с ведущим нулём
		fprintf(file, "%02x", buffer[i]);

		// Добавляем пробел после каждого байта, кроме последнего в строке
		if ((i + 1) % 16 != 0 && i + 1 != buffer_size) {
			fputc(' ', file);
		}

		// Переход на новую строку после каждых 16 байтов
		if ((i + 1) % 16 == 0 && i + 1 != buffer_size) {
			fputc('\n', file);
		}
	}

	fclose(file);
}

void CRadioServiceAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRadioServiceAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//ON_WM_HSCROLL()
	//ON_WM_VSCROLL()
	//ON_WM_MOUSEWHEEL()
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

void CRadioServiceAppDlg::OnTimer(UINT_PTR nIdEvent){
	if (nIdEvent == ID_TIMER_USB_RECIEVE_TIMEOUT_EXPIRES){
		if (usb_timeout == TRUE){
			printf("USB TIMEOUT\n");
			usb_timeout = FALSE;
		}
	}
	else{
		printf("timer event:%d %s\n", nIdEvent, get_cur_time().st);
	}

	KillTimer(nIdEvent);
}

void CRadioServiceAppDlg::StartUsbTimer(){
	usb_timeout = TRUE;
	SetTimer(ID_TIMER_USB_RECIEVE_TIMEOUT_EXPIRES, 1000, NULL);
}
void CRadioServiceAppDlg::EndUsbTimer(){
	usb_timeout = FALSE;
	KillTimer(ID_TIMER_USB_RECIEVE_TIMEOUT_EXPIRES);
}
void CRadioServiceAppDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	Invalidate();  // Принудительная перерисовка при изменении размера
	//UpdateScrollBars();
}
// CRadioServiceAppDlg message handlers

void CRadioServiceAppDlg::UpdateScrollBars()
{

}

void CRadioServiceAppDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CRadioServiceAppDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CRadioServiceAppDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


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

	fftw_complex *in, *out;
	fftw_plan p;
	int N = 1024;

	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* N);

	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);

	printf("FFTW works!\n");

	fftw_destroy_plan(p);
	fftw_free(in);
	fftw_free(out);

	//m_dZoomFactor = 1.0;
	//m_ScrollPos = CPoint(0, 0);
	//m_TotalSize = CSize(800, 600); // Начальный размер виртуального пространства
	//UpdateScrollBars();
	//calculate_fft(this);

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
		//CDialogEx::OnPaint();
		//берем отрисовку всего фона на себя, остальный контролы кнопки и тд сами себя рисуют
		//GraphHandlerSin(this);
		//GraphHandlerSinDoubleBuffer(this);
		//GraphHandlerSinDoubleBufferScroll(this);
		if (XferThread)
		{
			printf("XferThread still work\n");
			CDialogEx::OnPaint();
		}
		else
		    { GraphHandler_fft(this);}
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
	//fft30_5[157]
	//m_frequencyData[78].amplitude = m_frequencyData[78].amplitude + 5.0F;
	//printf("m_frequencyData[78].amplitude :%f\n",	m_frequencyData[78].amplitude);
	//Invalidate();
	//calculate_fft(this);
	//printf("curr time: %s\n", get_cur_time().st);
	//SetTimer(2, 1000, NULL);
	// Пример буфера (0x00 до 0xFF)
	//unsigned char buffer[256];
	//for (int i = 0; i < 256; i++) {
	//	buffer[i] = (unsigned char)i;
	//}

	 //Сохраняем в файл
	//save_hex_buffer_to_file(buffer, sizeof(buffer), "output.txt");
	//printf("File saved successfully.\n");
	//return;

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
	loops = ceilf((float)(dt.end_freq - dt.start_freq) / 2);
	for (int i = 0; i < loops; i++){
		UINT8 buf[1024] = { 0 };
		read_usb_async(0x82, buf, 1024);
	}
#endif

#ifndef THREAD_TEST
	if (XferThread)
	{
		bLooping = false;
		GetDlgItem(IDC_PUSK)->SetWindowTextW(_T("старт"));
		return;
	}
	SetupUsbOUT_init();
	dt = getting_data();
	SetupUsbOUT_settings(dt);

	bLooping = true;
	XferThread = AfxBeginThread(XferLoop, this);
	GetDlgItem(IDC_PUSK)->SetWindowTextW(_T("стоп"));
	//printf("we exit from OnBnClickedPusk()\n");
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
