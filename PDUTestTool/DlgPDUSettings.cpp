// DlgPDUSettings.cpp : implementation file
//

#include "stdafx.h"
#include "PDUTestTool.h"
#include "DlgPDUSettings.h"


// CDlgPDUSettings dialog

IMPLEMENT_DYNAMIC(CDlgPDUSettings, CDialog)

CDlgPDUSettings::CDlgPDUSettings(CString cszIniPath, BOOL bDisable, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPDUSettings::IDD, pParent)
{
	m_cszIniFilePath.Empty();
	m_cszIniFilePath = cszIniPath;

	m_bDisableOCBids = bDisable;
}

CDlgPDUSettings::~CDlgPDUSettings()
{
}

void CDlgPDUSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TOT_BANKS, m_cComboTotBanks);
	DDX_Control(pDX, IDC_COMBO_TOT_OUTLETS_PER_BANK, m_cComboTotOutletsPerBank);
	DDX_Control(pDX, IDC_COMBO_OCB1, m_cComboOCB1);
	DDX_Control(pDX, IDC_COMBO_OCB2, m_cComboOCB2);
	DDX_Control(pDX, IDC_COMBO_OCB3, m_cComboOCB3);
	DDX_Control(pDX, IDC_COMBO_OCB4, m_cComboOCB4);
	DDX_Control(pDX, IDC_COMBO_OCB5, m_cComboOCB5);
	DDX_Control(pDX, IDC_COMBO_OCB6, m_cComboOCB6);
}


BEGIN_MESSAGE_MAP(CDlgPDUSettings, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_TOT_BANKS, &CDlgPDUSettings::OnCbnSelchangeComboTotBanks)
	ON_CBN_SELCHANGE(IDC_COMBO_TOT_OUTLETS_PER_BANK, &CDlgPDUSettings::OnCbnSelchangeComboTotOutletsPerBank)
	ON_BN_CLICKED(IDOK, &CDlgPDUSettings::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_STAG_ON, &CDlgPDUSettings::OnEnChangeEditStagOn)
	ON_EN_CHANGE(IDC_EDIT_STAG_OFF, &CDlgPDUSettings::OnEnChangeEditStagOff)
	ON_BN_CLICKED(IDC_BUTTON_BROWS, &CDlgPDUSettings::OnBnClickedButtonBrows)
END_MESSAGE_MAP()


// CDlgPDUSettings message handlers

BOOL CDlgPDUSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	UINT		nNum = 0;
	CString		cszLoad;

	m_cComboTotBanks.ResetContent();
	m_cComboTotOutletsPerBank.ResetContent();
	
	for (nNum = 1; nNum <= 12; nNum++)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%d Bank(s)"), nNum);
		m_cComboTotBanks.AddString(cszLoad);
	}

	for (nNum = 1; nNum <= 48; nNum++)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%d Outlet(s)"), nNum);
		m_cComboTotOutletsPerBank.AddString(cszLoad);
	}

	m_cComboTotBanks.SetCurSel(0);
	m_cComboTotOutletsPerBank.SetCurSel(0);

	if (m_bDisableOCBids)
	{
		m_cComboOCB1.EnableWindow(FALSE);
		m_cComboOCB2.EnableWindow(FALSE);
		m_cComboOCB3.EnableWindow(FALSE);
		m_cComboOCB4.EnableWindow(FALSE);
		m_cComboOCB5.EnableWindow(FALSE);
		m_cComboOCB6.EnableWindow(FALSE);
	}

	ReadIniFile();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPDUSettings::OnCbnSelchangeComboTotBanks()
{
	// TODO: Add your control notification handler code here
	UINT		nNum = 0;
	CString		cszLoad;

	m_cComboOCB1.ResetContent();
	m_cComboOCB2.ResetContent();
	m_cComboOCB3.ResetContent();
	m_cComboOCB4.ResetContent();
	m_cComboOCB5.ResetContent();
	m_cComboOCB6.ResetContent();

	// m_cComboTotBanks.GetCurSel() + 1
	for (nNum = 0; nNum <= 12; nNum++)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%d"), nNum);

		if (!nNum)
		{
			cszLoad.Empty();
			cszLoad = _T("-");
		}
		
		m_cComboOCB1.AddString(cszLoad);
		m_cComboOCB2.AddString(cszLoad);
		m_cComboOCB3.AddString(cszLoad);
		m_cComboOCB4.AddString(cszLoad);
		m_cComboOCB5.AddString(cszLoad);
		m_cComboOCB6.AddString(cszLoad);
	}

	m_cComboOCB1.SetCurSel(0);
	m_cComboOCB2.SetCurSel(0);
	m_cComboOCB3.SetCurSel(0);
	m_cComboOCB4.SetCurSel(0);
	m_cComboOCB5.SetCurSel(0);
	m_cComboOCB6.SetCurSel(0);

	OnCbnSelchangeComboTotOutletsPerBank();
}

void CDlgPDUSettings::OnCbnSelchangeComboTotOutletsPerBank()
{
	// TODO: Add your control notification handler code here
	int			nTotal = 0;
	CString		cszLoad;

	if ((m_cComboTotBanks.GetCurSel() != -1) && (m_cComboTotOutletsPerBank.GetCurSel() != -1))
	{
		nTotal = ((m_cComboTotBanks.GetCurSel() + 1) * (m_cComboTotOutletsPerBank.GetCurSel() + 1));
	}

	cszLoad.Empty();
	cszLoad.Format(_T("%d Outlet(s)"), nTotal);
	SetDlgItemText(IDC_STATIC_TOT_OUTLETS, cszLoad);
}

void CDlgPDUSettings::ReadIniFile()
{
	UINT		nValue = 0;
	TCHAR		szBuffer[1024] = {0};
	CString		cszLoad;

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_TOT_BANKS, 0, m_cszIniFilePath);
	m_cComboTotBanks.SetCurSel(nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_TOT_OUTLETS_PB, 0, m_cszIniFilePath);
	m_cComboTotOutletsPerBank.SetCurSel(nValue);

	OnCbnSelchangeComboTotBanks();

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL1, 0, m_cszIniFilePath);
	m_cComboOCB1.SetCurSel(nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL2, 0, m_cszIniFilePath);
	m_cComboOCB2.SetCurSel(nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL3, 0, m_cszIniFilePath);
	m_cComboOCB3.SetCurSel(nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL4, 0, m_cszIniFilePath);
	m_cComboOCB4.SetCurSel(nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL5, 0, m_cszIniFilePath);
	m_cComboOCB5.SetCurSel(nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL6, 0, m_cszIniFilePath);
	m_cComboOCB6.SetCurSel(nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_STAGGER_ON_DELAY, 0, m_cszIniFilePath);
	SetDlgItemInt(IDC_EDIT_STAG_ON, nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_STAGGER_OFF_DELAY, 0, m_cszIniFilePath);
	SetDlgItemInt(IDC_EDIT_STAG_OFF, nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_FINAL_RELASE, 0, m_cszIniFilePath);
	if (nValue)
	{
		((CButton *)GetDlgItem(IDC_RADIO_OFF))->SetCheck(BST_UNCHECKED);
		((CButton *)GetDlgItem(IDC_RADIO_ON))->SetCheck(BST_CHECKED);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_RADIO_ON))->SetCheck(BST_UNCHECKED);
		((CButton *)GetDlgItem(IDC_RADIO_OFF))->SetCheck(BST_CHECKED);
	}

	cszLoad.Empty();
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(STR_SEC_PDU_SETTINGS, STR_LOG_FILE_PATH, _T(""), szBuffer, sizeof(szBuffer), m_cszIniFilePath);
	cszLoad = szBuffer;
	
	if (cszLoad.IsEmpty())
	{
		cszLoad = theApp.m_cszLogFolderPath;
	}

	SetDlgItemText(IDC_EDIT_LOG_PATH, cszLoad);
}

void CDlgPDUSettings::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int				nValue = 0;
	CString			cszLoad;

	nValue = m_cComboTotBanks.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_TOT_BANKS, cszLoad, m_cszIniFilePath);

	nValue = m_cComboTotOutletsPerBank.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_TOT_OUTLETS_PB, cszLoad, m_cszIniFilePath);

	nValue = m_cComboOCB1.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL1, cszLoad, m_cszIniFilePath);

	nValue = m_cComboOCB2.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL2, cszLoad, m_cszIniFilePath);

	nValue = m_cComboOCB3.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL3, cszLoad, m_cszIniFilePath);

	nValue = m_cComboOCB4.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL4, cszLoad, m_cszIniFilePath);

	nValue = m_cComboOCB5.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL5, cszLoad, m_cszIniFilePath);

	nValue = m_cComboOCB6.GetCurSel();
	
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL6, cszLoad, m_cszIniFilePath);

	nValue = GetDlgItemInt(IDC_EDIT_STAG_ON);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_STAGGER_ON_DELAY, cszLoad, m_cszIniFilePath);

	nValue = GetDlgItemInt(IDC_EDIT_STAG_OFF);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_STAGGER_OFF_DELAY, cszLoad, m_cszIniFilePath);

	nValue = 0;
	if (((CButton *)GetDlgItem(IDC_RADIO_ON))->GetCheck() == BST_CHECKED)
	{
		nValue = 1;
	}

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_FINAL_RELASE, cszLoad, m_cszIniFilePath);

	cszLoad.Empty();
	GetDlgItemText(IDC_EDIT_LOG_PATH, cszLoad);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_LOG_FILE_PATH, cszLoad, m_cszIniFilePath);

	OnOK();
}

void CDlgPDUSettings::OnEnChangeEditStagOn()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_STAG_ON);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_DELAY_SECS_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_STAG_ON, "");
		GetDlgItem(IDC_EDIT_STAG_ON)->SetFocus();
	}
}

void CDlgPDUSettings::OnEnChangeEditStagOff()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_STAG_OFF);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_DELAY_SECS_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_STAG_OFF, "");
		GetDlgItem(IDC_EDIT_STAG_OFF)->SetFocus();
	}
}

void CDlgPDUSettings::OnBnClickedButtonBrows()
{
	// TODO: Add your control notification handler code here
	CString		cszLoad;

	/*
	CFileDialog FileDlg(TRUE, NULL, NULL, 0, NULL);

	if (IDOK == FileDlg.DoModal())
	{
		cszLoad.Empty();
		cszLoad = FileDlg.GetPathName();
		AfxMessageBox(cszLoad);
	}
	*/

	BROWSEINFO bi = {0};
	bi.lpszTitle = _T("Select a folder path to store the log file");
	LPITEMIDLIST pidl = SHBrowseForFolder (&bi);
	if (pidl != 0)
	{
		// get the name of the folder
		TCHAR path[MAX_PATH];
		SHGetPathFromIDList (pidl, path);

		cszLoad.Empty();
		cszLoad = path;

		// free memory used
		IMalloc * imalloc = 0;
		if (SUCCEEDED(SHGetMalloc (&imalloc)))
		{
			imalloc->Free (pidl);
			imalloc->Release ();
		}

		SetDlgItemText(IDC_EDIT_LOG_PATH, cszLoad);
	}

}
