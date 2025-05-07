
// RadioServiceAppDlg.h : header file
//

#pragma once
#define ID_TIMER_USB_RECIEVE_TIMEOUT_EXPIRES 100

// CRadioServiceAppDlg dialog
class CRadioServiceAppDlg : public CDialogEx
{
// Construction
public:
	CWinThread *XferThread;
	bool bLooping;
	bool usb_timeout;
	void StartUsbTimer();
	void EndUsbTimer();
	CRadioServiceAppDlg(CWnd* pParent = NULL);	// standard constructor
	struct device_data{
		UINT16 start_freq;
		UINT16 end_freq;
		UINT8 attenua;
	};
	device_data getting_data();
	device_data dt;
	uint8_t* ptr_usb_data;
	size_t usbbytescount;
	bool data_is_processing;

	const wchar_t* p_str = L"старт";
	bool m_lastMouseInGraph; // Был ли курсор в области графика в предыдущий раз

	struct cur_time{
		char st[13]; //14:25:36.423
	};
	cur_time get_cur_time();
	void save_hex_buffer_to_file(const unsigned char *buffer, size_t buffer_size, const char *filename);
	void update_graph_on_display(BOOL bErase);

// Dialog Data
	enum { IDD = IDD_RADIOSERVICEAPP_DIALOG };

	double m_visibleMinFreq;  // Минимальная частота видимого диапазона
	double m_visibleMaxFreq;  // Максимальная частота видимого диапазона



	struct FrequencyData {
		double frequency;
		double amplitude;
	};
	std::vector<FrequencyData> m_frequencyData;
	std::vector<FrequencyData> m_frequencyData_display;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIdEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedInitialization();
	afx_msg void OnBnClickedPusk();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonWriteUsb();
	afx_msg void OnBnClickedButtonReadSync();
	afx_msg void OnBnClickedButtonReadAsync();
	afx_msg void OnBnClickedButtonAbort();
	afx_msg void OnEnChangeEditStartFreq();
	afx_msg void OnEnChangeEditEndFreq();
};
