#pragma once
#include "afxwin.h"


// CDlgPDUSettings dialog

class CDlgPDUSettings : public CDialog
{
	DECLARE_DYNAMIC(CDlgPDUSettings)

public:
	CDlgPDUSettings(CString cszIniPath, CString cszLogPath, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPDUSettings();

// Dialog Data
	enum { IDD = IDD_DLG_PDU_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	CString			m_cszIniFilePath;
	CString			m_cszLogFilePath;

public:

	afx_msg void OnBnClickedBtnBrowse();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	void ReadIniFile();

	afx_msg void OnEnChangeEditStaggerOnDelay();
	afx_msg void OnEnChangeEditStaggerOffDelay();
};
