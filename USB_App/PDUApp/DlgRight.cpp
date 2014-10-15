// DlgRight.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "PDUApp.h"
#include "DlgRight.h"


// CDlgRight

IMPLEMENT_DYNCREATE(CDlgRight, CFormView)

CDlgRight::CDlgRight()
	: CFormView(CDlgRight::IDD)
{
	m_bStartPoll = FALSE;
	m_nPollingSecs = 5;

	m_pEditBkBrush = NULL;
	m_pEditBkBrush = new CBrush(RGB(160, 160, 160));

	//m_pNewBkBrush = NULL;
	//m_pNewBkBrush = new CBrush(RGB(0, 0, 0));

	VERIFY(m_MediumFont.CreateFont(16,
									0,
									0,
									0,
									FW_BOLD,
									0,
									0,
									0,
									ANSI_CHARSET,
									OUT_DEFAULT_PRECIS,
									CLIP_DEFAULT_PRECIS,
									CLEARTYPE_QUALITY,
									FIXED_PITCH|FF_DONTCARE,
									STR_COUNT_DOWN_FONT_NAME));

	VERIFY(m_BigFont.CreateFont(22,
								0,
								0,
								0,
								FW_BOLD,
								0,
								0,
								0,
								ANSI_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								CLEARTYPE_QUALITY,
								FIXED_PITCH|FF_DONTCARE,
								STR_COUNT_DOWN_FONT_NAME));
}

CDlgRight::~CDlgRight()
{
	if (m_pEditBkBrush)
	{
		m_pEditBkBrush->DeleteObject();
		delete m_pEditBkBrush;
		m_pEditBkBrush = NULL;
	}

	/*if (m_pNewBkBrush)
	{
		m_pNewBkBrush->DeleteObject();
		delete m_pNewBkBrush;
		m_pNewBkBrush = NULL;
	}*/
}

void CDlgRight::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgRight, CFormView)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUT_START_STOP, &CDlgRight::OnBnClickedButStartStop)
	ON_EN_KILLFOCUS(IDC_EDIT_SECS, &CDlgRight::OnEnKillfocusEditSecs)
	ON_BN_CLICKED(IDC_BUT_SET, &CDlgRight::OnBnClickedButSet)
END_MESSAGE_MAP()


// CDlgRight diagnostics

#ifdef _DEBUG
void CDlgRight::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDlgRight::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDlgRight message handlers

HBRUSH CDlgRight::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	pDC->SetBkColor(RGB(160, 160, 160));
	pDC->SetTextColor(RGB(0, 0, 0));

	if ((pWnd->GetDlgCtrlID() == IDC_EDIT_MOD_NM) || (pWnd->GetDlgCtrlID() == IDC_EDIT_MOD_NO) || (pWnd->GetDlgCtrlID() == IDC_EDIT_SER_NO) || (pWnd->GetDlgCtrlID() == IDC_EDIT_FIRM_VER))
	{
		//pDC->SetBkColor(RGB(0, 0, 0));
		pDC->SetTextColor(RGB(128, 0, 0));
		//return (HBRUSH)(m_pNewBkBrush->GetSafeHandle());
	}

	return (HBRUSH)(m_pEditBkBrush->GetSafeHandle());
}

void CDlgRight::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class

	CString			cszLoad;
	CStringArray	cszStrArr;
	CPDUAppApp		*pApp = (CPDUAppApp *)AfxGetApp();

	if (pApp)
	{
		pApp->pRightView = this;
	}

	GetDlgItem(IDC_STATIC_BG_TEXT)->SetFont(&m_BigFont);

	GetDlgItem(IDC_STATIC_OUTLET_VAL)->SetFont(&m_MediumFont);

	GetDlgItem(IDC_STATIC_CURRENT)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_STATIC_VOLTAGE)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_STATIC_FREQ)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_STATIC_APPPOWER)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_STATIC_REALPOWER)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_STATIC_POWFAC)->SetFont(&m_MediumFont);

	GetDlgItem(IDC_ST_CURR_VAL)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_ST_VOL_VAL)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_ST_FREQ_VAL)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_ST_APPPOW_VAL)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_ST_REALPOW_VAL)->SetFont(&m_MediumFont);
	GetDlgItem(IDC_ST_POWFAC_VAL)->SetFont(&m_MediumFont);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_POLL_CONF);
	SetDlgItemText(IDC_STATIC_POLL_CONF, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_POLL_INTER);
	SetDlgItemText(IDC_STATIC_POLL, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SECS);
	SetDlgItemText(IDC_STATIC_SECS, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SET);
	SetDlgItemText(IDC_BUT_SET, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_START);
	SetDlgItemText(IDC_BUT_START_STOP, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_BG_TEXT);
	SetDlgItemText(IDC_STATIC_BG_TEXT, cszLoad);

	((CEdit *)GetDlgItem(IDC_EDIT_SECS))->SetLimitText(3);
	SetDlgItemInt(IDC_EDIT_SECS, 5);

	UpdateAllValues(cszStrArr, ITEM_NONE);

}

void CDlgRight::OnBnClickedButStartStop()
{
	// TODO: Add your control notification handler code here
	CString				cszLoad;
	CPDUAppApp			*pApp = (CPDUAppApp *)AfxGetApp();

	m_bStartPoll = !m_bStartPoll;

	cszLoad.Empty();
	if (m_bStartPoll)
	{
		m_nPollingSecs = GetDlgItemInt(IDC_EDIT_SECS);

		if ((pApp) && (pApp->pLeftView))
		{
			pApp->pLeftView->StartBaseThread(m_nPollingSecs);
		}

		cszLoad.LoadString(IDS_STR_STOP);
	}
	else
	{
		if ((pApp) && (pApp->pLeftView))
		{
			pApp->pLeftView->CloseAllThreads();
		}

		cszLoad.LoadString(IDS_STR_START);
	}

	SetDlgItemText(IDC_BUT_START_STOP, cszLoad);
}

void CDlgRight::OnEnKillfocusEditSecs()
{
	// TODO: Add your control notification handler code here
	UINT			nPollSecs = 0;
	CString			cszLoad;
	
	nPollSecs = GetDlgItemInt(IDC_EDIT_SECS);
	if (nPollSecs < 5)
	{
		nPollSecs = 5;
		SetDlgItemInt(IDC_EDIT_SECS, nPollSecs);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MIN_POLL_SEC_GRT_5SECS_MESS);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
	}
	else if (nPollSecs > 900)
	{
		nPollSecs = 900;
		SetDlgItemInt(IDC_EDIT_SECS, nPollSecs);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MAX_POLL_SEC);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
	}
}

void CDlgRight::OnBnClickedButSet()
{
	// TODO: Add your control notification handler code here
	CPDUAppApp	*pApp = (CPDUAppApp *)AfxGetApp();

	if ((pApp) && (pApp->pLeftView))
	{
		m_nPollingSecs = GetDlgItemInt(IDC_EDIT_SECS);
		pApp->pLeftView->SetPollingSecs(m_nPollingSecs);
	}
}

void CDlgRight::UpdateAllValues(CStringArray &cszValuesArray, BYTE byItemLevel)
{
	BOOL			bStaticShow = FALSE;
	BOOL			bShow = FALSE;
	BOOL			bShowRem = FALSE;
	CString			cszLoad;

	SetDlgItemText(IDC_STATIC_OUTLET_VAL, _T(""));

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_VOLT_NM);
	SetDlgItemText(IDC_STATIC_VOLTAGE, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_CURR_NM);
	SetDlgItemText(IDC_STATIC_CURRENT, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_FREQ_NM);
	SetDlgItemText(IDC_STATIC_FREQ, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_APP_POW_NM);
	SetDlgItemText(IDC_STATIC_APPPOWER, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_REAL_POW_NM);
	SetDlgItemText(IDC_STATIC_REALPOWER, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_POW_FAC_NM);
	SetDlgItemText(IDC_STATIC_POWFAC, cszLoad);

	if (byItemLevel == CHILD_OUTLET)
	{
		if (cszValuesArray.GetSize() < 7)
			return;

		SetDlgItemText(IDC_STATIC_OUTLET_VAL, cszValuesArray.ElementAt(0));

		SetDlgItemText(IDC_ST_CURR_VAL, cszValuesArray.ElementAt(1));
		SetDlgItemText(IDC_ST_VOL_VAL, cszValuesArray.ElementAt(2));
		SetDlgItemText(IDC_ST_POWFAC_VAL, cszValuesArray.ElementAt(3));
		SetDlgItemText(IDC_ST_FREQ_VAL, cszValuesArray.ElementAt(4));
		SetDlgItemText(IDC_ST_APPPOW_VAL, cszValuesArray.ElementAt(5));
		SetDlgItemText(IDC_ST_REALPOW_VAL, cszValuesArray.ElementAt(6));
		
		bShowRem = bShow = TRUE;
		bStaticShow = FALSE;
	}
	else if (byItemLevel == PARENT_PDU)
	{
		if (cszValuesArray.GetSize() < 5)
		{
			bShow = FALSE;
			bShowRem = FALSE;
		}
		else
		{
			cszLoad.Empty();
			cszLoad.LoadString(IDS_STR_MODEL_NM);
			SetDlgItemText(IDC_STATIC_VOLTAGE, cszLoad);

			cszLoad.Empty();
			cszLoad.LoadString(IDS_STR_MODEL_NUM);
			SetDlgItemText(IDC_STATIC_CURRENT, cszLoad);

			cszLoad.Empty();
			cszLoad.LoadString(IDS_STR_SERIAL_NUM);
			SetDlgItemText(IDC_STATIC_FREQ, cszLoad);

			cszLoad.Empty();
			cszLoad.LoadString(IDS_STR_FIRM_VERS);
			SetDlgItemText(IDC_STATIC_APPPOWER, cszLoad);

			SetDlgItemText(IDC_STATIC_OUTLET_VAL, cszValuesArray.ElementAt(0));

			SetDlgItemText(IDC_EDIT_MOD_NM, cszValuesArray.ElementAt(1));
			SetDlgItemText(IDC_EDIT_MOD_NO, cszValuesArray.ElementAt(3));
			SetDlgItemText(IDC_EDIT_SER_NO, cszValuesArray.ElementAt(2));
			SetDlgItemText(IDC_EDIT_FIRM_VER, cszValuesArray.ElementAt(4));
			
			bShow = TRUE;
			bStaticShow = TRUE;
			bShowRem = FALSE;
		}
	}

	GetDlgItem(IDC_ST_CURR_VAL)->ShowWindow(bShowRem);
	GetDlgItem(IDC_ST_VOL_VAL)->ShowWindow(bShowRem);
	GetDlgItem(IDC_ST_FREQ_VAL)->ShowWindow(bShowRem);
	GetDlgItem(IDC_ST_APPPOW_VAL)->ShowWindow(bShowRem);
	GetDlgItem(IDC_ST_REALPOW_VAL)->ShowWindow(bShowRem);
	GetDlgItem(IDC_ST_POWFAC_VAL)->ShowWindow(bShowRem);

	GetDlgItem(IDC_STATIC_CURRENT)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_VOLTAGE)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_FREQ)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_APPPOWER)->ShowWindow(bShow);
	GetDlgItem(IDC_STATIC_REALPOWER)->ShowWindow(bShowRem);
	GetDlgItem(IDC_STATIC_POWFAC)->ShowWindow(bShowRem);

	GetDlgItem(IDC_EDIT_MOD_NM)->ShowWindow(bStaticShow);
	GetDlgItem(IDC_EDIT_MOD_NO)->ShowWindow(bStaticShow);
	GetDlgItem(IDC_EDIT_SER_NO)->ShowWindow(bStaticShow);
	GetDlgItem(IDC_EDIT_FIRM_VER)->ShowWindow(bStaticShow);


	GetDlgItem(IDC_STATIC_BG_TEXT)->ShowWindow(!bShow);
}
