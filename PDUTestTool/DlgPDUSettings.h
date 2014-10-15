#pragma once
#include "afxwin.h"


// CDlgPDUSettings dialog

class CDlgPDUSettings : public CDialog
{
	DECLARE_DYNAMIC(CDlgPDUSettings)

public:
	CDlgPDUSettings(CString cszIniPath, BOOL bDisable = FALSE, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPDUSettings();

// Dialog Data
	enum { IDD = IDD_DLG_PDU_SETTINGS };

	BOOL			m_bDisableOCBids;
	CString			m_cszIniFilePath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboTotBanks();
	afx_msg void OnCbnSelchangeComboTotOutletsPerBank();
	afx_msg void OnBnClickedOk();

	CComboBox m_cComboTotBanks;
	CComboBox m_cComboTotOutletsPerBank;
	CComboBox m_cComboOCB1;
	CComboBox m_cComboOCB2;
	CComboBox m_cComboOCB3;
	CComboBox m_cComboOCB4;
	CComboBox m_cComboOCB5;
	CComboBox m_cComboOCB6;

	void ReadIniFile();
	afx_msg void OnEnChangeEditStagOn();
	afx_msg void OnEnChangeEditStagOff();
	afx_msg void OnBnClickedButtonBrows();
};
