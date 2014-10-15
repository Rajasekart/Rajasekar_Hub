// PageAlert.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PageAlert.h"


// CPageAlert dialog

IMPLEMENT_DYNAMIC(CPageAlert, CDialog)

CPageAlert::CPageAlert(CWnd* pParent /*=NULL*/)
	: CDialog(CPageAlert::IDD, pParent)
{
	memset(&m_stOutletAlertInfo, 0, sizeof(OUTLET_ALERT_INFO));
	m_bFirstTime = TRUE;
	m_nPrevSel = -1;
}

CPageAlert::~CPageAlert()
{
}

void CPageAlert::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_VCB, m_cChkVCB);
	DDX_Control(pDX, IDC_CHK_LCA, m_cChkLCA);
	DDX_Control(pDX, IDC_CHK_HCA, m_cChkHCA);
	DDX_Control(pDX, IDC_EDIT_VCB_MGP, m_cEditVCBMaxGrcPer);
	DDX_Control(pDX, IDC_EDIT_VCB_THRES_VAL, m_cEditVCBThresVal);
	DDX_Control(pDX, IDC_EDIT_LCA_MGP, m_cEditLCAMaxGrcPer);
	DDX_Control(pDX, IDC_EDIT_LCA_THRES_VAL, m_cEditLCAThresVal);
	DDX_Control(pDX, IDC_EDIT_HCA_MGP, m_cEditHCAMaxGrcPer);
	DDX_Control(pDX, IDC_EDIT_HCA_THRES_VAL, m_cEditHCAThresVal);
	DDX_Control(pDX, IDC_COMBO_OUTLET_NUM_LST, m_cComboOutletNum);
}


BEGIN_MESSAGE_MAP(CPageAlert, CDialog)
	ON_BN_CLICKED(IDC_CHK_VCB, &CPageAlert::OnBnClickedChkVcb)
	ON_BN_CLICKED(IDC_CHK_LCA, &CPageAlert::OnBnClickedChkLca)
	ON_BN_CLICKED(IDC_CHK_HCA, &CPageAlert::OnBnClickedChkHca)
	ON_CBN_SELCHANGE(IDC_COMBO_OUTLET_NUM_LST, &CPageAlert::OnCbnSelchangeOutletNumLst)
END_MESSAGE_MAP()


// CPageAlert message handlers

BOOL CPageAlert::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_cEditVCBMaxGrcPer.SetLimitText(6);
	m_cEditVCBThresVal.SetLimitText(6);
	m_cEditHCAMaxGrcPer.SetLimitText(6);
	m_cEditHCAThresVal.SetLimitText(6);
	m_cEditLCAMaxGrcPer.SetLimitText(6);
	m_cEditLCAThresVal.SetLimitText(6);

	m_cChkVCB.SetCheck(BST_CHECKED);
	OnBnClickedChkVcb();

	m_cChkLCA.SetCheck(BST_CHECKED);
	OnBnClickedChkLca();

	m_cChkHCA.SetCheck(BST_CHECKED);
	OnBnClickedChkHca();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPageAlert::OnBnClickedChkVcb()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == m_cChkVCB.GetCheck())
	{
		EnableVCBSettings(TRUE);
	}
	else
	{
		EnableVCBSettings(FALSE);
	}
}

void CPageAlert::EnableVCBSettings(BOOL bEnable)
{
	GetDlgItem(IDC_STATIC_VCB_MGP)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VCB_MGP)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VCB_SECS)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_VCB_AMPS)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_VCB_THRES_VAL)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_VCB_THRES_VAL)->EnableWindow(bEnable);
}

void CPageAlert::OnBnClickedChkLca()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == m_cChkLCA.GetCheck())
	{
		EnableLCASettings(TRUE);
	}
	else
	{
		EnableLCASettings(FALSE);
	}
}

void CPageAlert::EnableLCASettings(BOOL bEnable)
{
	GetDlgItem(IDC_STATIC_LCA_MGP)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_LCA_MGP)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_LCA_SECS)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_LCA_AMPS)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_LCA_THRES_VAL)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_LCA_THRES_VAL)->EnableWindow(bEnable);
}

void CPageAlert::OnBnClickedChkHca()
{
	// TODO: Add your control notification handler code here
	if (BST_CHECKED == m_cChkHCA.GetCheck())
	{
		EnableHCASettings(TRUE);
	}
	else
	{
		EnableHCASettings(FALSE);
	}
}

void CPageAlert::EnableHCASettings(BOOL bEnable)
{
	GetDlgItem(IDC_STATIC_HCA_MGP)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_HCA_MGP)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_HCA_SECS)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_HCA_AMPS)->EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_HCA_THRES_VAL)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_HCA_THRES_VAL)->EnableWindow(bEnable);
}

void CPageAlert::FillOutletsInCombo(UINT nTotalOutlets)
{
	CString			cszTemp;
	CString			cszLoad;
	UINT			nNum = 0;

	m_cComboOutletNum.ResetContent();

	for (nNum = 1; nNum <= nTotalOutlets; nNum++)
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_OUTLET_NUM);

		cszLoad.Empty();
		cszLoad.Format(cszTemp, nNum);

		m_cComboOutletNum.AddString(cszLoad);
	}

	m_cComboOutletNum.SetCurSel(0);
}
void CPageAlert::OnCbnSelchangeOutletNumLst()
{
	// TODO: Add your control notification handler code here
	int				nOutletNum = 0;
	CString			cszLoad;

	nOutletNum = m_cComboOutletNum.GetCurSel();

	if (nOutletNum == CB_ERR)
		return;

	if (!m_bFirstTime)
	{
		SaveAlertInfo();
	}

	m_nPrevSel = nOutletNum;

	m_bFirstTime = FALSE;

	// Enable VCB
	m_cChkVCB.SetCheck(m_stOutletAlertInfo.byEnbAlertVCB[nOutletNum]);
	OnBnClickedChkVcb();

	// MAX GRACE PERIOD
	cszLoad.Empty();
	cszLoad.Format(_T("%lu"), m_stOutletAlertInfo.dwVCBMaxGracPer[nOutletNum]);
	m_cEditVCBMaxGrcPer.SetWindowText(cszLoad);

	// Threshold Value
	cszLoad.Empty();
	cszLoad.Format(_T("%.2f"), m_stOutletAlertInfo.fVCBThresValue[nOutletNum]);
	m_cEditVCBThresVal.SetWindowText(cszLoad);

	// Enable LCA
	m_cChkLCA.SetCheck(m_stOutletAlertInfo.byEnbAlertLCA[nOutletNum]);
	OnBnClickedChkLca();

	// MAX GRACE PERIOD
	cszLoad.Empty();
	cszLoad.Format(_T("%lu"), m_stOutletAlertInfo.dwLCAMaxGracPer[nOutletNum]);
	m_cEditLCAMaxGrcPer.SetWindowText(cszLoad);

	// Threshold Value
	cszLoad.Empty();
	cszLoad.Format(_T("%.2f"), m_stOutletAlertInfo.fLCAThresValue[nOutletNum]);
	m_cEditLCAThresVal.SetWindowText(cszLoad);

	// Enable HCA
	m_cChkHCA.SetCheck(m_stOutletAlertInfo.byEnbAlertHCA[nOutletNum]);
	OnBnClickedChkHca();

	// MAX GRACE PERIOD
	cszLoad.Empty();
	cszLoad.Format(_T("%lu"), m_stOutletAlertInfo.dwHCAMaxGracPer[nOutletNum]);
	m_cEditHCAMaxGrcPer.SetWindowText(cszLoad);

	// Threshold Value
	cszLoad.Empty();
	cszLoad.Format(_T("%.2f"), m_stOutletAlertInfo.fHCAThresValue[nOutletNum]);
	m_cEditHCAThresVal.SetWindowText(cszLoad);
}

void CPageAlert::SaveAlertInfo()
{
	CString			cszLoad;

	if (m_nPrevSel == CB_ERR)
		return;

	// VCB Enable Status
	m_stOutletAlertInfo.byEnbAlertVCB[m_nPrevSel] = 0;
	if (m_cChkVCB.GetCheck() == BST_CHECKED)
	{
		m_stOutletAlertInfo.byEnbAlertVCB[m_nPrevSel] = 1;
	}

	// MAX GRACE PERIOD
	cszLoad.Empty();
	m_cEditVCBMaxGrcPer.GetWindowText(cszLoad);
	m_stOutletAlertInfo.dwVCBMaxGracPer[m_nPrevSel] = atol((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	// THRESHOLD VALUE
	cszLoad.Empty();
	m_cEditVCBThresVal.GetWindowText(cszLoad);
	m_stOutletAlertInfo.fVCBThresValue[m_nPrevSel] = (float)atof((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();


	// LCA Enable Status
	m_stOutletAlertInfo.byEnbAlertLCA[m_nPrevSel] = 0;
	if (m_cChkLCA.GetCheck() == BST_CHECKED)
	{
		m_stOutletAlertInfo.byEnbAlertLCA[m_nPrevSel] = 1;
	}

	// MAX GRACE PERIOD
	cszLoad.Empty();
	m_cEditLCAMaxGrcPer.GetWindowText(cszLoad);
	m_stOutletAlertInfo.dwLCAMaxGracPer[m_nPrevSel] = atol((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	// THRESHOLD VALUE
	cszLoad.Empty();
	m_cEditLCAThresVal.GetWindowText(cszLoad);
	m_stOutletAlertInfo.fLCAThresValue[m_nPrevSel] = (float)atof((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();


	// HCA Enable Status
	m_stOutletAlertInfo.byEnbAlertHCA[m_nPrevSel] = 0;
	if (m_cChkHCA.GetCheck() == BST_CHECKED)
	{
		m_stOutletAlertInfo.byEnbAlertHCA[m_nPrevSel] = 1;
	}

	// MAX GRACE PERIOD
	cszLoad.Empty();
	m_cEditHCAMaxGrcPer.GetWindowText(cszLoad);
	m_stOutletAlertInfo.dwHCAMaxGracPer[m_nPrevSel] = atol((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	// THRESHOLD VALUE
	cszLoad.Empty();
	m_cEditHCAThresVal.GetWindowText(cszLoad);
	m_stOutletAlertInfo.fHCAThresValue[m_nPrevSel] = (float)atof((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();
}