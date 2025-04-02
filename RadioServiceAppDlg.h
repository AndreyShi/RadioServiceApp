
// RadioServiceAppDlg.h : header file
//

#pragma once

typedef struct cntrl_data{
	UINT16 start_freq;
	UINT16 end_freq;
	UINT8 attenua;
};


// CRadioServiceAppDlg dialog
class CRadioServiceAppDlg : public CDialogEx
{
// Construction
public:
	CWinThread *XferThread;
	bool bLooping;
	CRadioServiceAppDlg(CWnd* pParent = NULL);	// standard constructor
	cntrl_data getting_data();
// Dialog Data
	enum { IDD = IDD_RADIOSERVICEAPP_DIALOG };

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
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedInitialization();
	afx_msg void OnBnClickedPusk();
};
