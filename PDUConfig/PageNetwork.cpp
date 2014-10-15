// PageNetwork.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PageNetwork.h"


// CPageNetwork dialog

IMPLEMENT_DYNAMIC(CPageNetwork, CDialog)

CPageNetwork::CPageNetwork(CWnd* pParent /*=NULL*/)
	: CDialog(CPageNetwork::IDD, pParent)
{
}

CPageNetwork::~CPageNetwork()
{
}

void CPageNetwork::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_DHCP, m_cChkDHCP);
	DDX_Control(pDX, IDC_IPADD, m_cIPAddrCtrl);
	DDX_Control(pDX, IDC_SUBNET_MASK, m_cSubnetMsk);
	DDX_Control(pDX, IDC_GATEWAY_IP, m_cGateway);
	DDX_Control(pDX, IDC_PRIM_DNS, m_cPrimDNS);
	DDX_Control(pDX, IDC_SEC_DNS, m_cSecDNS);
}


BEGIN_MESSAGE_MAP(CPageNetwork, CDialog)
	ON_BN_CLICKED(IDC_CHECK_DHCP, &CPageNetwork::OnBnClickedDHCP)
END_MESSAGE_MAP()


// CPageNetwork message handlers

BOOL CPageNetwork::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->wParam == VK_RETURN)
	{
		pMsg->wParam = NULL;
	}
	else if (pMsg->wParam == VK_ESCAPE)
	{
		pMsg->wParam = NULL;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CPageNetwork::OnBnClickedDHCP()
{
	// TODO: Add your control notification handler code here
	if (m_cChkDHCP.GetCheck() == BST_CHECKED)
	{
		EnableManualIP(FALSE);
	}
	else
	{
		EnableManualIP(TRUE);
	}
}

BOOL CPageNetwork::EnableManualIP(BOOL bEnabled)
{
	BOOL			bRet = TRUE;

	GetDlgItem(IDC_STATIC_IP_FRAME)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_IP_ADD)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_SUBNET_MASK)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_DEF_GATEWAY)->EnableWindow(bEnabled);
	GetDlgItem(IDC_IPADD)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SUBNET_MASK)->EnableWindow(bEnabled);
	GetDlgItem(IDC_GATEWAY_IP)->EnableWindow(bEnabled);

	GetDlgItem(IDC_STATIC_DNS_SER_ADDR)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_PRIM_DNS)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_SEC_DNS)->EnableWindow(bEnabled);
	GetDlgItem(IDC_PRIM_DNS)->EnableWindow(bEnabled);
	GetDlgItem(IDC_SEC_DNS)->EnableWindow(bEnabled);

	return bRet;
}

BOOL CPageNetwork::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString				cszLoad;

	m_cChkDHCP.SetCheck(BST_CHECKED);
	OnBnClickedDHCP();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
