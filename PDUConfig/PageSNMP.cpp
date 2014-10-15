// PageSNMP.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PageSNMP.h"


// CPageSNMP dialog

IMPLEMENT_DYNAMIC(CPageSNMP, CDialog)

CPageSNMP::CPageSNMP(CWnd* pParent /*=NULL*/)
	: CDialog(CPageSNMP::IDD, pParent)
{

}

CPageSNMP::~CPageSNMP()
{
}

void CPageSNMP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SNMP_TRAP, m_cChkSNMPTrap);
	DDX_Control(pDX, IDC_EDIT_READ_COMNTY, m_cEditReadCmnty);
	DDX_Control(pDX, IDC_EDIT_WRITE_COMNTY, m_cWriteCmnty);
	DDX_Control(pDX, IDC_IPADD_TRAP_IP, m_cReceIPAddr);
	DDX_Control(pDX, IDC_EDIT_SNMP_TRAP_PORT, m_cEditSnmpTrapPortNum);
}


BEGIN_MESSAGE_MAP(CPageSNMP, CDialog)
	ON_BN_CLICKED(IDC_CHECK_SNMP_TRAP, &CPageSNMP::OnBnClickedCheckSnmpTrap)
END_MESSAGE_MAP()


// CPageSNMP message handlers

BOOL CPageSNMP::PreTranslateMessage(MSG* pMsg)
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

void CPageSNMP::OnBnClickedCheckSnmpTrap()
{
	// TODO: Add your control notification handler code here
	if (m_cChkSNMPTrap.GetCheck() == BST_CHECKED)
	{
		EnableSNMPTrapSettings(TRUE);
	}
	else
	{
		EnableSNMPTrapSettings(FALSE);
	}
}

void CPageSNMP::EnableSNMPTrapSettings(BOOL bEnabled)
{
	GetDlgItem(IDC_STATIC_RECEIVER_IP_ADD)->EnableWindow(bEnabled);
	GetDlgItem(IDC_IPADD_TRAP_IP)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_PORT)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_SNMP_TRAP_PORT)->EnableWindow(bEnabled);
}

BOOL CPageSNMP::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_cChkSNMPTrap.SetCheck(BST_CHECKED);
	OnBnClickedCheckSnmpTrap();

	m_cEditReadCmnty.SetLimitText(7);
	m_cWriteCmnty.SetLimitText(7);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
