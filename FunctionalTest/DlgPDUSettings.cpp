// DlgPDUSettings.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "FunctionalTest.h"
#include "DlgPDUSettings.h"


// CDlgPDUSettings dialog

IMPLEMENT_DYNAMIC(CDlgPDUSettings, CDialog)

CDlgPDUSettings::CDlgPDUSettings(CString cszIniPath, CString cszLogPath, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPDUSettings::IDD, pParent)
{
	m_cszIniFilePath.Empty();
	m_cszIniFilePath = cszIniPath;

	m_cszLogFilePath.Empty();
	m_cszLogFilePath = cszLogPath;
}

CDlgPDUSettings::~CDlgPDUSettings()
{
}

void CDlgPDUSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgPDUSettings, CDialog)
	ON_BN_CLICKED(IDC_BTN_BROWSE, &CDlgPDUSettings::OnBnClickedBtnBrowse)
	ON_BN_CLICKED(IDOK, &CDlgPDUSettings::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_STAGGER_ON_DELAY, &CDlgPDUSettings::OnEnChangeEditStaggerOnDelay)
	ON_EN_CHANGE(IDC_EDIT_STAGGER_OFF_DELAY, &CDlgPDUSettings::OnEnChangeEditStaggerOffDelay)
END_MESSAGE_MAP()


// CDlgPDUSettings message handlers

void CDlgPDUSettings::OnBnClickedBtnBrowse()
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

void CDlgPDUSettings::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	int				nValue = 0;
	CString			cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_ON_DELAY);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nValue);
	WritePrivateProfileString(STR_SEC_PDU_SETTINGS, STR_STAGGER_ON_DELAY, cszLoad, m_cszIniFilePath);

	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_OFF_DELAY);

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

BOOL CDlgPDUSettings::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	ReadIniFile();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgPDUSettings::ReadIniFile()
{
	UINT		nValue = 0;
	TCHAR		szBuffer[1024] = {0};
	CString		cszLoad;

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_STAGGER_ON_DELAY, 0, m_cszIniFilePath);
	SetDlgItemInt(IDC_EDIT_STAGGER_ON_DELAY, nValue);

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_STAGGER_OFF_DELAY, 0, m_cszIniFilePath);
	SetDlgItemInt(IDC_EDIT_STAGGER_OFF_DELAY, nValue);

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
		cszLoad = m_cszLogFilePath;
	}

	SetDlgItemText(IDC_EDIT_LOG_PATH, cszLoad);
}
void CDlgPDUSettings::OnEnChangeEditStaggerOnDelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_ON_DELAY);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad = _T("Staggering delay should between 0 to 5000 MilliSeconds.");
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_STAGGER_ON_DELAY, "");
		GetDlgItem(IDC_EDIT_STAGGER_ON_DELAY)->SetFocus();
	}
}

void CDlgPDUSettings::OnEnChangeEditStaggerOffDelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_OFF_DELAY);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad = _T("Staggering delay should between 0 to 5000 MilliSeconds.");
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_STAGGER_OFF_DELAY, "");
		GetDlgItem(IDC_EDIT_STAGGER_OFF_DELAY)->SetFocus();
	}
}
