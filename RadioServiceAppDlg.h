
// RadioServiceAppDlg.h : header file
//

#pragma once


// CRadioServiceAppDlg dialog
class CRadioServiceAppDlg : public CDialogEx
{
// Construction
public:
	CWinThread *XferThread;
	bool bLooping;
	int loops;
	CRadioServiceAppDlg(CWnd* pParent = NULL);	// standard constructor
	struct device_data{
		UINT16 start_freq;
		UINT16 end_freq;
		UINT8 attenua;
	};
	device_data getting_data();

	struct cur_time{
		char st[13]; //14:25:36.423
	};
	cur_time get_cur_time();

// Dialog Data
	enum { IDD = IDD_RADIOSERVICEAPP_DIALOG };

	double m_dZoomFactor;  // Коэффициент масштабирования
	CPoint m_ScrollPos;    // Позиция прокрутки (в пикселях)
	CSize m_TotalSize;     // Общий размер виртуального пространства
	void UpdateScrollBars();



	struct FrequencyData {
		double frequency;
		double amplitude;
	};
	std::vector<FrequencyData> m_frequencyData;
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
};
