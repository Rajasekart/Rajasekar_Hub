// SummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDUTestTool.h"
#include "SummaryDlg.h"


// CSummaryDlg dialog

IMPLEMENT_DYNAMIC(CSummaryDlg, CDialog)

CSummaryDlg::CSummaryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSummaryDlg::IDD, pParent)
{
	m_bRelayResult = FALSE;
	m_bChangeOCB = FALSE;
	m_bChangeOCB_Result = TRUE;
}

CSummaryDlg::~CSummaryDlg()
{
}

void CSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TEST_SUMM, m_cListCtrlTestSumm);
}


BEGIN_MESSAGE_MAP(CSummaryDlg, CDialog)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_TEST_SUMM, &CSummaryDlg::OnNMCustomdrawListTestSumm)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSummaryDlg message handlers

BOOL CSummaryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	BYTE			byCount = 0;
	BYTE			byVal = 0;
	BYTE			byNum = 0;
	CString			cszLoad;
	BOOL			bPass = TRUE;

	if (m_bChangeOCB)
	{
		SetWindowText(_T("Change OCB  - Test Summary"));
		SetDlgItemText(IDC_STATIC_RELAY_CAP, _T("Change OCB Status :"));
	}
	else
	{
		SetWindowText(_T("PDU Relay - Test Summary"));
		SetDlgItemText(IDC_STATIC_RELAY_CAP, _T("Relay Test Status :"));
	}

	VERIFY(m_cMedFont.CreateFont(22,
							 	0,
							 	0,
							 	0,
							 	FW_EXTRABOLD,
							 	0,
							 	0,
							 	0,
							 	ANSI_CHARSET,
							 	OUT_DEFAULT_PRECIS,
							 	CLIP_DEFAULT_PRECIS,
							 	DEFAULT_QUALITY,
							 	DEFAULT_PITCH|FF_DONTCARE,
							 	_T("ARIAL")));

	VERIFY(m_cMed1Font.CreateFont(15,
							 	0,
							 	0,
							 	0,
							 	FW_EXTRABOLD,
							 	0,
							 	0,
							 	0,
							 	ANSI_CHARSET,
							 	OUT_DEFAULT_PRECIS,
							 	CLIP_DEFAULT_PRECIS,
							 	DEFAULT_QUALITY,
							 	DEFAULT_PITCH|FF_DONTCARE,
							 	_T("ARIAL")));

	GetDlgItem(IDC_STATIC_RELAY_CAP)->SetFont(&m_cMed1Font);
	GetDlgItem(IDC_STATIC_RESULT)->SetFont(&m_cMedFont);

	if (m_bRelayResult)
	{
		SetDlgItemText(IDC_STATIC_RESULT, _T("TEST PASS"));
	}
	else
	{
		SetDlgItemText(IDC_STATIC_RESULT, _T("TEST FAIL"));
	}

	if (!m_bChangeOCB_Result)
	{
		SetDlgItemText(IDC_STATIC_RESULT, _T("CHANGE FAIL"));
	}

	m_cListCtrlTestSumm.SetExtendedStyle(m_cListCtrlTestSumm.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_cListCtrlTestSumm.InsertColumn(0, _T("PDU Device"), LVCFMT_LEFT, 162);
	m_cListCtrlTestSumm.InsertColumn(1, _T("Relay Test Result"), LVCFMT_LEFT, 146);

	for (byNum = 0; byNum < m_byTotBanks; byNum++)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("Bank %u"), m_bySelBanks[byNum]);

		PrintBufferInTestInfoList(cszLoad, _T(""), BLUE_TEXT);

		for (byVal =  1; byVal <= m_byOutletsPerBank; byVal++)
		{
			bPass = TRUE;
			if (m_byOutletResult[byCount] == 0)
			{
				bPass = FALSE;
			}

			cszLoad.Empty();
			cszLoad.Format(_T("Outlet %u"), byVal);

			if (bPass)
			{
				PrintBufferInTestInfoList(cszLoad, _T("PASS"), GREEN_TEXT);
			}
			else
			{
				PrintBufferInTestInfoList(cszLoad, _T("FAIL"), RED_TEXT);
			}

			byCount++;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSummaryDlg::OnNMCustomdrawListTestSumm(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	DWORD dwRow = (DWORD)lplvcd->nmcd.dwItemSpec;

	switch(lplvcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}

		// Modify item text and or background
		case CDDS_ITEMPREPAINT:
		{
			if (lplvcd->nmcd.lItemlParam == GREEN_TEXT)
			{
				lplvcd->clrText = RGB(0, 128, 64);
			}
			else if (lplvcd->nmcd.lItemlParam == RED_TEXT)
			{
				lplvcd->clrText = RGB(166, 0, 0);
			}
			else if (lplvcd->nmcd.lItemlParam == BLUE_TEXT)
			{
				lplvcd->clrText = RGB(66, 66, 255);
			}

			//int nRowData = (int)GetItemData(lplvcd->nmcd.dwItemSpec);
			//if (nRowData)
			//{
			//	lplvcd->clrText = RGB(255, 0, 0);
			//}

			// If you want the sub items the same as the item,
			// set *pResult to CDRF_NEWFONT
			//*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}

		// Modify sub item text and/or background
		case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{
			lplvcd->clrTextBk = RGB(250, 250, 250);

			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
}

BOOL CSummaryDlg::PrintBufferInTestInfoList(CString cszBuffer, CString cszResult, int nTxtColor)
{
	int					nRow = 0;
	LVITEM				lvItem = {0};

	nRow = m_cListCtrlTestSumm.GetItemCount();

	memset(&lvItem, 0, sizeof(LVITEM));
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = nRow;
	lvItem.pszText = cszBuffer.GetBuffer(cszBuffer.GetLength());
	cszBuffer.ReleaseBuffer();
	lvItem.cchTextMax = cszBuffer.GetLength();
	lvItem.iSubItem = 0;
	lvItem.lParam = nTxtColor;
	nRow = m_cListCtrlTestSumm.InsertItem(&lvItem);

	if (nRow == MINUS_ONE)
		return FALSE;

	m_cListCtrlTestSumm.SetItemText(nRow, 1, cszResult);

	nRow = m_cListCtrlTestSumm.GetItemCount();
	if (nRow > 0)
	{
		m_cListCtrlTestSumm.EnsureVisible((nRow - 1), FALSE);
	}
	
	return TRUE;
}

HBRUSH CSummaryDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
		{
			if (m_bRelayResult)
			{
				pDC->SetTextColor(RGB(0, 128, 128)); // GR
			}
			else
			{
				pDC->SetTextColor(RGB(128, 0, 0)); // RED
			}
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
