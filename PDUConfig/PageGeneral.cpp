// PageGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PageGeneral.h"


// CPageGeneral dialog

IMPLEMENT_DYNAMIC(CPageGeneral, CDialog)

CPageGeneral::CPageGeneral(CWnd* pParent /*=NULL*/)
	: CDialog(CPageGeneral::IDD, pParent)
{

}

CPageGeneral::~CPageGeneral()
{
}

void CPageGeneral::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_GEN_SYS_NM, m_cEditGenSysNm);
	DDX_Control(pDX, IDC_EDIT_GEN_SYS_LOCTN, m_cEditGenSysLoctn);
	DDX_Control(pDX, IDC_EDIT_GEN_SYS_CONTACT, m_cEditGenSysCont);
	DDX_Control(pDX, IDC_EDIT_GEN_STAGGER_ON_DELAY, m_cEditGenStaggerOnDelay);
	DDX_Control(pDX, IDC_EDIT_GEN_STAGGER_OFF_DELAY, m_cEditGenStaggerOffDelay);
	DDX_Control(pDX, IDC_CHECK_ENB_TELNET_CONN, m_cCheckEnbTelnet);
	DDX_Control(pDX, IDC_CHECK_ENB_HTTP_CONN, m_cCheckEnbHTTP);
	DDX_Control(pDX, IDC_CHECK_ENB_HTTPS_CONN, m_cCheckEnbHTTPS);
}


BEGIN_MESSAGE_MAP(CPageGeneral, CDialog)
	ON_EN_CHANGE(IDC_EDIT_GEN_STAGGER_ON_DELAY, &CPageGeneral::OnEnChangeGenStaggerOnDelay)
	ON_EN_CHANGE(IDC_EDIT_GEN_STAGGER_OFF_DELAY, &CPageGeneral::OnEnChangeGenStaggerOffDelay)
END_MESSAGE_MAP()


// CPageGeneral message handlers

BOOL CPageGeneral::PreTranslateMessage(MSG* pMsg)
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

BOOL CPageGeneral::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_cEditGenSysNm.SetLimitText(15);
	m_cEditGenSysLoctn.SetLimitText(31);
	m_cEditGenSysCont.SetLimitText(31);
	m_cEditGenStaggerOnDelay.SetLimitText(4);
	m_cEditGenStaggerOffDelay.SetLimitText(4);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPageGeneral::OnEnChangeGenStaggerOnDelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_GEN_STAGGER_ON_DELAY);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_DELAY_SECS_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_GEN_STAGGER_ON_DELAY, "");
		m_cEditGenStaggerOnDelay.SetFocus();
	}
}

void CPageGeneral::OnEnChangeGenStaggerOffDelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_GEN_STAGGER_OFF_DELAY);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_DELAY_SECS_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_GEN_STAGGER_OFF_DELAY, "");
		m_cEditGenStaggerOnDelay.SetFocus();
	}
}
