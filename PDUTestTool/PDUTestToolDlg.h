// PDUTestToolDlg.h : header file
//

#pragma once

#include "General.h"
#include "afxwin.h"
#include "afxcmn.h"

// CPDUTestToolDlg dialog
class CPDUTestToolDlg : public CDialog
{
// Construction
public:
	CPDUTestToolDlg(CWnd* pParent = NULL);	// standard constructor
	~CPDUTestToolDlg();
// Dialog Data
	enum { IDD = IDD_PDUTESTTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	BYTE					m_byUserAns;
	CFont					m_cBigFont;
	CFont					m_cMedFont;
	CFont					m_cMed1Font;
	BOOL					m_bGreen;
	BOOL					m_bProcessStarted;
	CString					m_cszLogFolderPath;
	CString					m_cszLogFilePath;
	CString					m_cszIniFilePath;
	PDU_SETTINGS			m_stPduSettings;
	CString					m_cszPDUAddr;

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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedButtonPduSettings();
	afx_msg void OnBnClickedButtonPduRefresh();

	BOOL StartPDUTesting();
	BOOL ReadSettingsFile();
	void RefreshOurApp();
	void EnableUserButtons(BOOL bValue);
	CString GetRelayText(BYTE byRelayState);
	void RefreshPDUList(BOOL bSilent = FALSE);
	void UpdateBigText(BYTE byValue, CString cszText, BOOL bLeaveEmpty = FALSE);
	void ViewTestCtrls(BOOL bEnable, BOOL bProcess = FALSE);
	BOOL PrintBufferInTestInfoList(CString cszBuffer, int nTxtColor = 0);
	void WaitingText(int nSleep);
	void DeleteLastRow();

	CButton m_cButPass;
	CButton m_cButFail;
	CButton m_cButStart;
	CButton m_cButSaveLog;
	CButton m_cButPDUSetting;
	CListCtrl m_cListCtrlTest;
	afx_msg void OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonPass();
	CEdit m_cEditSysID;
	CStatic m_cStaticBigCap;
	CStatic m_cStaticBigTxt;
	afx_msg void OnBnClickedButtonFail();
	afx_msg void OnBnClickedButtonSaveLog();
	afx_msg void OnBnClickedButtonClear();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
