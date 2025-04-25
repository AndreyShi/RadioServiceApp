
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
	device_data getting_data();
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
	afx_msg void OnSize(UINT nType, int cx, int cy);
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
