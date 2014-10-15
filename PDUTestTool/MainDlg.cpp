// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDUTestTool.h"
#include "MainDlg.h"
#include "General.h"

// CMainDlg dialog

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
	m_byOption = 0;
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CMainDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_RELAY, &CMainDlg::OnBnClickedButtonRelay)
	ON_BN_CLICKED(IDC_BUTTON_OCB, &CMainDlg::OnBnClickedButtonOcb)
END_MESSAGE_MAP()


// CMainDlg message handlers

void CMainDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CMainDlg::OnBnClickedButtonRelay()
{
	// TODO: Add your control notification handler code here
	m_byOption = OPT_RELAY_TEST;
	OnOK();
}

void CMainDlg::OnBnClickedButtonOcb()
{
	// TODO: Add your control notification handler code here
	m_byOption = OPT_OCB_CHANGE;
	OnOK();
}
