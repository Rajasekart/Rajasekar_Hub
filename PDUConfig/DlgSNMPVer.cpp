// DlgSNMPVer.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "DlgSNMPVer.h"


// CDlgSNMPVer dialog

IMPLEMENT_DYNAMIC(CDlgSNMPVer, CDialog)

CDlgSNMPVer::CDlgSNMPVer(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSNMPVer::IDD, pParent)
{

}

CDlgSNMPVer::~CDlgSNMPVer()
{
}

void CDlgSNMPVer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SNMP_V3_USER, m_cEditSNMPV3User);
	DDX_Control(pDX, IDC_COMBO_AUTH_LEVEL, m_cComboAuthLevel);
	DDX_Control(pDX, IDC_EDIT_AUTH_PASSKEY, m_cEditAuthPassKey);
	DDX_Control(pDX, IDC_EDIT_CONF_AUTH_PASSKEY, m_cEditConfAuthPassKey);
	DDX_Control(pDX, IDC_EDIT_PRIV_PASSKEY, m_cEditPrivPassKey);
	DDX_Control(pDX, IDC_EDIT_PRIV_CONF_PASSKEY, m_cEditPrivConfPasskey);
	DDX_Control(pDX, IDC_EDIT_V3READ_COMNTY, m_cEditV3ReadComm);
	DDX_Control(pDX, IDC_EDIT_V3WRITE_COMNTY, m_cEditV3WriteComm);
}


BEGIN_MESSAGE_MAP(CDlgSNMPVer, CDialog)
	ON_BN_CLICKED(IDC_RADIO_SNMP_V2C, &CDlgSNMPVer::OnBnClickedRadioSnmpV2c)
	ON_BN_CLICKED(IDC_RADIO_SNMP_V3, &CDlgSNMPVer::OnBnClickedRadioSnmpV3)
	ON_CBN_SELCHANGE(IDC_COMBO_AUTH_LEVEL, &CDlgSNMPVer::OnCbnSelchangeComboAuthLevel)
END_MESSAGE_MAP()


// CDlgSNMPVer message handlers

BOOL CDlgSNMPVer::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CButton		*cRadioSNMPV3 = (CButton *)GetDlgItem(IDC_RADIO_SNMP_V3);
	CButton		*cRadioSNMPV2 = (CButton *)GetDlgItem(IDC_RADIO_SNMP_V2C);

	if (cRadioSNMPV2)
	{
		cRadioSNMPV2->SetCheck(BST_UNCHECKED);
	}

	if (cRadioSNMPV3)
	{
		cRadioSNMPV3->SetCheck(BST_CHECKED);
		OnBnClickedRadioSnmpV3();
	}

	m_cEditV3ReadComm.SetLimitText(7);
	m_cEditV3WriteComm.SetLimitText(7);
	m_cEditAuthPassKey.SetLimitText(15);
	m_cEditConfAuthPassKey.SetLimitText(15);
	m_cEditPrivPassKey.SetLimitText(15);
	m_cEditPrivConfPasskey.SetLimitText(15);

	FillAuthLevels();
	m_cComboAuthLevel.SetCurSel(0);
	OnCbnSelchangeComboAuthLevel();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgSNMPVer::OnBnClickedRadioSnmpV2c()
{
	// TODO: Add your control notification handler code here
	UpdateSNMPVerUI(FALSE);
}

void CDlgSNMPVer::OnBnClickedRadioSnmpV3()
{
	// TODO: Add your control notification handler code here
	UpdateSNMPVerUI(TRUE);
}

void CDlgSNMPVer::UpdateSNMPVerUI(BOOL bEnable)
{
	if (bEnable)
	{
		////// SNMP V3
		GetDlgItem(IDC_STATIC_SNMPV3_READ)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SNMPV3_WRITE)->ShowWindow(SW_HIDE);
		m_cEditV3ReadComm.ShowWindow(SW_HIDE);
		m_cEditV3WriteComm.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_SNMP_FRAME)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_SNMP_V3_USER_CAP)->ShowWindow(SW_SHOWNORMAL);
		m_cEditSNMPV3User.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_AUTH_LEVEL)->ShowWindow(SW_SHOWNORMAL);
		m_cComboAuthLevel.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_SNMP_AUTH)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_AUTH_PROTOCOL)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_AUTH_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditAuthPassKey.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_CONF_AUTH_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditConfAuthPassKey.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_SNMPV3_PRIVACY)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_PRIV_PROT)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_PRIV_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditPrivPassKey.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_CONF_PRIV_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditPrivConfPasskey.ShowWindow(SW_SHOWNORMAL);

		OnCbnSelchangeComboAuthLevel();
	}
	else
	{
		GetDlgItem(IDC_STATIC_SNMP_FRAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SNMP_V3_USER_CAP)->ShowWindow(SW_HIDE);
		m_cEditSNMPV3User.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_AUTH_LEVEL)->ShowWindow(SW_HIDE);
		m_cComboAuthLevel.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SNMP_AUTH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_AUTH_PROTOCOL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_AUTH_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditAuthPassKey.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CONF_AUTH_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditConfAuthPassKey.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SNMPV3_PRIVACY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PRIV_PROT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PRIV_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditPrivPassKey.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CONF_PRIV_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditPrivConfPasskey.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_SNMPV3_READ)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_SNMPV3_WRITE)->ShowWindow(SW_SHOWNORMAL);
		m_cEditV3ReadComm.ShowWindow(SW_SHOWNORMAL);
		m_cEditV3WriteComm.ShowWindow(SW_SHOWNORMAL);
	}
}

void CDlgSNMPVer::FillAuthLevels()
{
	m_cComboAuthLevel.ResetContent();

	m_cComboAuthLevel.AddString(_T("No auth No priv"));
	m_cComboAuthLevel.AddString(_T("auth No priv"));
	m_cComboAuthLevel.AddString(_T("auth priv"));
}

void CDlgSNMPVer::OnCbnSelchangeComboAuthLevel()
{
	// TODO: Add your control notification handler code here
	if (m_cComboAuthLevel.GetCurSel() == 0)
	{
		GetDlgItem(IDC_STATIC_SNMP_AUTH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_AUTH_PROTOCOL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_AUTH_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditAuthPassKey.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CONF_AUTH_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditConfAuthPassKey.ShowWindow(SW_HIDE);

		GetDlgItem(IDC_STATIC_SNMPV3_PRIVACY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PRIV_PROT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PRIV_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditPrivPassKey.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CONF_PRIV_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditPrivConfPasskey.ShowWindow(SW_HIDE);
	}
	else if (m_cComboAuthLevel.GetCurSel() == 1)
	{
		GetDlgItem(IDC_STATIC_SNMP_AUTH)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_AUTH_PROTOCOL)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_AUTH_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditAuthPassKey.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_CONF_AUTH_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditConfAuthPassKey.ShowWindow(SW_SHOWNORMAL);

		GetDlgItem(IDC_STATIC_SNMPV3_PRIVACY)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PRIV_PROT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PRIV_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditPrivPassKey.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_CONF_PRIV_PASSKEY)->ShowWindow(SW_HIDE);
		m_cEditPrivConfPasskey.ShowWindow(SW_HIDE);
	}
	else if (m_cComboAuthLevel.GetCurSel() == 2)
	{
		GetDlgItem(IDC_STATIC_SNMP_AUTH)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_AUTH_PROTOCOL)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_AUTH_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditAuthPassKey.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_CONF_AUTH_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditConfAuthPassKey.ShowWindow(SW_SHOWNORMAL);

		GetDlgItem(IDC_STATIC_SNMPV3_PRIVACY)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_PRIV_PROT)->ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_PRIV_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditPrivPassKey.ShowWindow(SW_SHOWNORMAL);
		GetDlgItem(IDC_STATIC_CONF_PRIV_PASSKEY)->ShowWindow(SW_SHOWNORMAL);
		m_cEditPrivConfPasskey.ShowWindow(SW_SHOWNORMAL);
	}
}

void CDlgSNMPVer::SelectControl(BYTE byV3)
{
	CButton		*cRadioSNMPV3 = (CButton *)GetDlgItem(IDC_RADIO_SNMP_V3);
	CButton		*cRadioSNMPV2 = (CButton *)GetDlgItem(IDC_RADIO_SNMP_V2C);

	if (byV3)
	{
		if (cRadioSNMPV2)
		{
			cRadioSNMPV2->SetCheck(BST_UNCHECKED);
		}

		if (cRadioSNMPV3)
		{
			cRadioSNMPV3->SetCheck(BST_CHECKED);
			OnBnClickedRadioSnmpV3();
		}
	}
	else
	{
		if (cRadioSNMPV2)
		{
			cRadioSNMPV2->SetCheck(BST_CHECKED);
			OnBnClickedRadioSnmpV2c();
		}

		if (cRadioSNMPV3)
		{
			cRadioSNMPV3->SetCheck(BST_UNCHECKED);
		}
	}
}

BOOL CDlgSNMPVer::GetV3CheckStatus()
{
	CButton		*cRadioSNMPV3 = (CButton *)GetDlgItem(IDC_RADIO_SNMP_V3);

	if (cRadioSNMPV3)
	{
		if (cRadioSNMPV3->GetCheck() == BST_CHECKED)
			return TRUE;
	}

	return FALSE;
}
