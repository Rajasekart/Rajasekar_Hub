// PageEmail.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PageEmail.h"


// CPageEmail dialog

IMPLEMENT_DYNAMIC(CPageEmail, CDialog)

CPageEmail::CPageEmail(CWnd* pParent /*=NULL*/)
	: CDialog(CPageEmail::IDD, pParent)
{

}

CPageEmail::~CPageEmail()
{
}

void CPageEmail::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_ENABLE_EMAIL, m_cChkEmail);
	DDX_Control(pDX, IDC_EDIT_SMTP_SERVER, m_cEditSMTPServer);
	DDX_Control(pDX, IDC_EDIT_SMTP_PORT, m_cEditSMTPPortNum);
	DDX_Control(pDX, IDC_EDIT_EMAIL_USER_NM, m_cEditSMTPSendrUserNm);
	DDX_Control(pDX, IDC_EDIT_EMAIL_PASSWORD, m_cEditSMTPSendrPasswd);
	DDX_Control(pDX, IDC_EDIT_RECE_EMAIL_ID, m_cEditSMTPRecevrEmailId);
}


BEGIN_MESSAGE_MAP(CPageEmail, CDialog)
	ON_BN_CLICKED(IDC_CHECK_ENABLE_EMAIL, &CPageEmail::OnBnClickedCheckEnableEmail)
END_MESSAGE_MAP()


// CPageEmail message handlers

BOOL CPageEmail::PreTranslateMessage(MSG* pMsg)
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

void CPageEmail::OnBnClickedCheckEnableEmail()
{
	// TODO: Add your control notification handler code here
	if (m_cChkEmail.GetCheck() == BST_CHECKED)
	{
		EnableEmailSettings(TRUE);
	}
	else
	{
		EnableEmailSettings(FALSE);
	}
}

void CPageEmail::EnableEmailSettings(BOOL bEnabled)
{
	GetDlgItem(IDC_STATIC_SMTP_SERVER)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_SMTP_PORT)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_USER_NAME)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_PASSWORD)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_RECEIVER_EMAIL_ID)->EnableWindow(bEnabled);

	GetDlgItem(IDC_EDIT_SMTP_SERVER)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_SMTP_PORT)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_EMAIL_USER_NM)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_EMAIL_PASSWORD)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_RECE_EMAIL_ID)->EnableWindow(bEnabled);
}

BOOL CPageEmail::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_cChkEmail.SetCheck(BST_CHECKED);
	OnBnClickedCheckEnableEmail();

	m_cEditSMTPServer.SetLimitText(39);
	m_cEditSMTPPortNum.SetLimitText(4);
	m_cEditSMTPSendrUserNm.SetLimitText(49);
	m_cEditSMTPSendrPasswd.SetLimitText(39);
	m_cEditSMTPRecevrEmailId.SetLimitText(49);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
