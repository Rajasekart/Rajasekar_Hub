// AlertDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDUApp.h"
#include "General.h"
#include "AlertDlg.h"


// CAlertDlg dialog

IMPLEMENT_DYNAMIC(CAlertDlg, CDialog)

CAlertDlg::CAlertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlertDlg::IDD, pParent)
{
	memset(&m_stPduOutletAlrtInfo, 0, sizeof(PDU_OUTLET_ALERT_INFO));
}

CAlertDlg::~CAlertDlg()
{
}

void CAlertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAlertDlg, CDialog)
	ON_BN_CLICKED(IDC_CHK_VIR_CIR_BRK_ENB, &CAlertDlg::OnBnClickedChkVirCirBrkEnb)
	ON_BN_CLICKED(IDC_CHK_HIGH_CURR_ENB, &CAlertDlg::OnBnClickedChkHighCurrEnb)
	ON_BN_CLICKED(IDC_CHK_LOW_CURR_ENB, &CAlertDlg::OnBnClickedChkLowCurrEnb)
END_MESSAGE_MAP()


// CAlertDlg message handlers

BOOL CAlertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString						cszLoad;

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_ALERT_CONF);
	SetWindowText(cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_VCB_NM);
	SetDlgItemText(IDC_FRAME_VIR_CIR_BRK, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_HIGH_CURR_ALRT);
	SetDlgItemText(IDC_FRAME_HIGH_CURR, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_LOW_CURR_ALRT);
	SetDlgItemText(IDC_FRAME_LOW_CURR, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_ENABLED);
	SetDlgItemText(IDC_CHK_VIR_CIR_BRK_ENB, cszLoad);
	SetDlgItemText(IDC_CHK_HIGH_CURR_ENB, cszLoad);
	SetDlgItemText(IDC_CHK_LOW_CURR_ENB, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_THRESHOLD);
	SetDlgItemText(IDC_STATIC_THRES_HOLD_VCB, cszLoad);
	SetDlgItemText(IDC_STATIC_THRES_HOLD_HCA, cszLoad);
	SetDlgItemText(IDC_STATIC_THRES_HOLD_LCA, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_A);
	SetDlgItemText(IDC_STATIC_A_VCB, cszLoad);
	SetDlgItemText(IDC_STATIC_A_HCA, cszLoad);
	SetDlgItemText(IDC_STATIC_A_LCA, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_MAX_GRC_PER);
	SetDlgItemText(IDC_STATIC_MAX_GRC_PER_VCB, cszLoad);
	SetDlgItemText(IDC_STATIC_MAX_GRC_PER_HCA, cszLoad);
	SetDlgItemText(IDC_STATIC_MAX_GRC_PER_LCA, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SEC);
	SetDlgItemText(IDC_STATIC_SEC_VCB, cszLoad);
	SetDlgItemText(IDC_STATIC_SEC_HCA, cszLoad);
	SetDlgItemText(IDC_STATIC_SEC_LCA, cszLoad);

	SetDefaultValues(m_stPduOutletAlrtInfo);
	OnBnClickedChkVirCirBrkEnb();
	OnBnClickedChkHighCurrEnb();
	OnBnClickedChkLowCurrEnb();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAlertDlg::SetDefaultValues(PDU_OUTLET_ALERT_INFO stPduOutletAlrtInfo)
{
	CString			cszLoad;

	// For VCB
	((CButton *)GetDlgItem(IDC_CHK_VIR_CIR_BRK_ENB))->SetCheck(stPduOutletAlrtInfo.bVCBEnabled);

	cszLoad.Empty();
	cszLoad.Format(_T("%.1f"), stPduOutletAlrtInfo.fVCBThreshold);
	SetDlgItemText(IDC_EDIT_THRES_VCB, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPduOutletAlrtInfo.nVCBMaxGrcPer);
	SetDlgItemText(IDC_EDIT_MGP_VCB, cszLoad);

	// For HCA
	((CButton *)GetDlgItem(IDC_CHK_HIGH_CURR_ENB))->SetCheck(stPduOutletAlrtInfo.bHCAEnabled);

	cszLoad.Empty();
	cszLoad.Format(_T("%.1f"), stPduOutletAlrtInfo.fHCAThreshold);
	SetDlgItemText(IDC_EDIT_THRES_HCA, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPduOutletAlrtInfo.nHCAMaxGrcPer);
	SetDlgItemText(IDC_EDIT_MGP_HCA, cszLoad);

	// For LCA
	((CButton *)GetDlgItem(IDC_CHK_LOW_CURR_ENB))->SetCheck(stPduOutletAlrtInfo.bLCAEnabled);

	cszLoad.Empty();
	cszLoad.Format(_T("%.1f"), stPduOutletAlrtInfo.fLCAThreshold);
	SetDlgItemText(IDC_EDIT_THRES_LCA, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPduOutletAlrtInfo.nLCAMaxGrcPer);
	SetDlgItemText(IDC_EDIT_MGP_LCA, cszLoad);

}

void CAlertDlg::OnBnClickedChkVirCirBrkEnb()
{
	// TODO: Add your control notification handler code here
	BOOL		bEnable = FALSE;

	if (((CButton *)GetDlgItem(IDC_CHK_VIR_CIR_BRK_ENB))->GetCheck() == BST_CHECKED)
	{
		bEnable = TRUE;
	}

	GetDlgItem(IDC_STATIC_THRES_HOLD_VCB)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_THRES_VCB)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_A_VCB)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_MAX_GRC_PER_VCB)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_MGP_VCB)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_SEC_VCB)->EnableWindow(bEnable);
}

void CAlertDlg::OnBnClickedChkHighCurrEnb()
{
	// TODO: Add your control notification handler code here
	BOOL		bEnable = FALSE;

	if (((CButton *)GetDlgItem(IDC_CHK_HIGH_CURR_ENB))->GetCheck() == BST_CHECKED)
	{
		bEnable = TRUE;
	}

	GetDlgItem(IDC_STATIC_THRES_HOLD_HCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_THRES_HCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_A_HCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_MAX_GRC_PER_HCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_MGP_HCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_SEC_HCA)->EnableWindow(bEnable);
}

void CAlertDlg::OnBnClickedChkLowCurrEnb()
{
	// TODO: Add your control notification handler code here
	BOOL		bEnable = FALSE;

	if (((CButton *)GetDlgItem(IDC_CHK_LOW_CURR_ENB))->GetCheck() == BST_CHECKED)
	{
		bEnable = TRUE;
	}

	GetDlgItem(IDC_STATIC_THRES_HOLD_LCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_THRES_LCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_A_LCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_MAX_GRC_PER_LCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_EDIT_MGP_LCA)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_SEC_LCA)->EnableWindow(bEnable);
}

void CAlertDlg::GetUserAlertSettings()
{
	CString					cszLoad;

	//--------------------------- VCB Settings -------------------------------------
	m_stPduOutletAlrtInfo.bVCBEnabled = FALSE;
	if (((CButton *)GetDlgItem(IDC_CHK_VIR_CIR_BRK_ENB))->GetCheck() == BST_CHECKED)
	{
		m_stPduOutletAlrtInfo.bVCBEnabled = TRUE;
	}

	cszLoad.Empty();
	GetDlgItemText(IDC_EDIT_THRES_VCB, cszLoad);
	m_stPduOutletAlrtInfo.fVCBThreshold = (float)atof(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	cszLoad.Empty();
	GetDlgItemText(IDC_EDIT_MGP_VCB, cszLoad);
	m_stPduOutletAlrtInfo.nVCBMaxGrcPer = atoi(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();


	//--------------------------- HCA Settings -------------------------------------
	m_stPduOutletAlrtInfo.bHCAEnabled = FALSE;
	if (((CButton *)GetDlgItem(IDC_CHK_HIGH_CURR_ENB))->GetCheck() == BST_CHECKED)
	{
		m_stPduOutletAlrtInfo.bHCAEnabled = TRUE;
	}

	cszLoad.Empty();
	GetDlgItemText(IDC_EDIT_THRES_HCA, cszLoad);
	m_stPduOutletAlrtInfo.fHCAThreshold = (float)atof(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	cszLoad.Empty();
	GetDlgItemText(IDC_EDIT_MGP_HCA, cszLoad);
	m_stPduOutletAlrtInfo.nHCAMaxGrcPer = atoi(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	//--------------------------- LCA Settings -------------------------------------
	m_stPduOutletAlrtInfo.bLCAEnabled = FALSE;
	if (((CButton *)GetDlgItem(IDC_CHK_LOW_CURR_ENB))->GetCheck() == BST_CHECKED)
	{
		m_stPduOutletAlrtInfo.bLCAEnabled = TRUE;
	}

	cszLoad.Empty();
	GetDlgItemText(IDC_EDIT_THRES_LCA, cszLoad);
	m_stPduOutletAlrtInfo.fLCAThreshold = (float)atof(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	cszLoad.Empty();
	GetDlgItemText(IDC_EDIT_MGP_LCA, cszLoad);
	m_stPduOutletAlrtInfo.nLCAMaxGrcPer = atoi(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();
}

void CAlertDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	GetUserAlertSettings();

	CDialog::OnCancel();
}
