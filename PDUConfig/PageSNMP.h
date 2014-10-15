#pragma once
#include "afxwin.h"
#include "afxcmn.h"


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

	void EnableSNMPTrapSettings(BOOL bEnabled);
	CEdit m_cEditReadCmnty;
	CEdit m_cWriteCmnty;
	CIPAddressCtrl m_cReceIPAddr;
	CEdit m_cEditSnmpTrapPortNum;
};
