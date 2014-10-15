// DlgStaticInfo.cpp : implementation file
//

#include "stdafx.h"
#include "PDUApp.h"
#include "DlgStaticInfo.h"


// CDlgStaticInfo dialog

IMPLEMENT_DYNAMIC(CDlgStaticInfo, CDialog)

CDlgStaticInfo::CDlgStaticInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStaticInfo::IDD, pParent)
{
	memset(&stPDUStaticInfo, 0, sizeof(PDU_STATIC_INTO));
}

CDlgStaticInfo::~CDlgStaticInfo()
{
}

void CDlgStaticInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BANK_OUTLETS, m_cListBankCtrl);
}


BEGIN_MESSAGE_MAP(CDlgStaticInfo, CDialog)
END_MESSAGE_MAP()


// CDlgStaticInfo message handlers

BOOL CDlgStaticInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString				cszLoad;
	CString				cszTemp;

	cszTemp.Empty();
	cszTemp.LoadString(IDS_STR_DLG_STATIC_INFO);

	cszLoad.Empty();
	cszLoad.Format(cszTemp, cszPDUName);
	SetWindowText(cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_EXIT);
	SetDlgItemText(IDCANCEL, cszLoad);

	m_cListBankCtrl.InsertColumn(0, _T("BANK NUMBER"), LVCFMT_CENTER, 200, -1);
	m_cListBankCtrl.InsertColumn(1, _T("TOTAL OUTLETS"), LVCFMT_CENTER, 200, -1);

	DisplayAllStaticValues();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgStaticInfo::DisplayAllStaticValues()
{
	int					nPos = 0;
	UINT				nNum = 0;
	CString				cszLoad;
	CString				cszTemp;

	cszLoad.Empty();
	cszLoad.Format(_T("%s"), stPDUStaticInfo.byModelNm);
	SetDlgItemText(IDC_EDIT_MNM, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%s"), stPDUStaticInfo.byModelNo);
	SetDlgItemText(IDC_EDIT_MNO, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%s"), stPDUStaticInfo.bySerialNo);
	SetDlgItemText(IDC_EDIT_SNO, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%s"), stPDUStaticInfo.byFirmwareVer);
	SetDlgItemText(IDC_EDIT_FVERS, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%s"), stPDUStaticInfo.byManfDate);
	SetDlgItemText(IDC_EDIT_ICR, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%lu"), stPDUStaticInfo.dwStaggOnDelay);
	SetDlgItemText(IDC_EDIT_RV, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%lu"), stPDUStaticInfo.dwStaggOffDelay);
	SetDlgItemText(IDC_EDIT_RP, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPDUStaticInfo.nTotalOutlets);
	SetDlgItemText(IDC_EDIT_NOO, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPDUStaticInfo.nTotalBanks);
	SetDlgItemText(IDC_EDIT_NOB, cszLoad);

	cszLoad.Empty();
	cszLoad.Format(_T("%d Phase - %c Type"), stPDUStaticInfo.nPhaseType, stPDUStaticInfo.byInpConnType);
	SetDlgItemText(IDC_EDIT_NOI, cszLoad);

	for (nNum = 0; nNum < stPDUStaticInfo.nTotalBanks; nNum++)
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BANK_NM);

		cszLoad.Empty();
		cszLoad.Format(cszTemp, nNum + 1);

		nPos = m_cListBankCtrl.InsertItem(nNum, cszLoad);
		
		cszLoad.Empty();
		cszLoad.Format(_T("%d"), stPDUStaticInfo.nOutletsPerBank);
		m_cListBankCtrl.SetItemText(nPos, 1, cszLoad);
	}
	
	return TRUE;
}
