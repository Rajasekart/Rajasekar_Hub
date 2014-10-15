#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "General.h"

// CPageSNMP dialog

class CPageSNMP : public CDialog
{
	DECLARE_DYNAMIC(CPageSNMP)

public:
	CPageSNMP(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageSNMP();

// Dialog Data
	enum { IDD = IDD_DLG_SNMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedCheckSnmpTrap();

	CButton			m_cChkSNMPTrap;
	CIPAddressCtrl	m_cReceIPAddr;
	CEdit			m_cEditSnmpTrapPortNum;
	CDateTimeCtrl	*m_ctrlDateHired;
	CDateTimeCtrl	*m_ctrlTimeHired;
	CButton			m_cBtnEnbNTPDateTime;
	CEdit			m_cEditDateTimeServers;
	CComboBox		m_cComboTimeZone;

	void EnableSNMPTrapSettings(BOOL bEnabled);
	void AddAllTimeZones();
	void CopyDateTime(CString cszDateTimeStr);


	
	afx_msg void OnBnClickedCheckEnbNtpDtTm();

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
