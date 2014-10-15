#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CPageNetwork dialog

class CPageNetwork : public CDialog
{
	DECLARE_DYNAMIC(CPageNetwork)

public:
	CPageNetwork(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageNetwork();

// Dialog Data
	enum { IDD = IDD_DLG_NETWORK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnBnClickedDHCP();

	CIPAddressCtrl			m_cIPAddrCtrl;
	CIPAddressCtrl			m_cSubnetMsk;
	CIPAddressCtrl			m_cGateway;
	CIPAddressCtrl			m_cPrimDNS;
	CIPAddressCtrl			m_cSecDNS;
	CButton					m_cChkDHCP;

	BOOL EnableManualIP(BOOL bEnabled);
};