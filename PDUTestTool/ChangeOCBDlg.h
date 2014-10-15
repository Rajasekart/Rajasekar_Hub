#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "General.h"

// CChangeOCBDlg dialog

class CChangeOCBDlg : public CDialog
{
	DECLARE_DYNAMIC(CChangeOCBDlg)

public:
	CChangeOCBDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChangeOCBDlg();

// Dialog Data
	enum { IDD = IDD_DLG_OCB_CHANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	BOOL					m_bGreen;
	CString					m_cszLogFolderPath;
	CString					m_cszLogFilePath;
	CString					m_cszIniFilePath;
	BOOL					m_bProcessStarted;
	CString					m_cszPDUAddr;
	PDU_SETTINGS			m_stPduSettings;

	BOOL ChangeOCBID();
	void DeleteLastRow();
	void RefreshOurApp();
	BOOL ReadSettingsFile();
	void WaitingText(int nSleep);
	void ViewTestCtrls(BOOL bEnable);
	CString GetRelayText(BYTE byRelayState);
	void RefreshPDUList(BOOL bSilent = FALSE);
	BOOL PrintBufferInTestInfoList(CString cszBuffer, int nTxtColor = 0);


	CButton m_cButChange;
	virtual BOOL OnInitDialog();
	CListCtrl m_cListCtrlOCB;
	afx_msg void OnBnClickedButChange();
	afx_msg void OnBnClickedButtonPduSetgs();
	afx_msg void OnBnClickedButtonSaveLog();
	afx_msg void OnBnClickedButtonCls();
};
