// colorLstCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "PDUApp.h"
#include "colorLstCtrl.h"


// CcolorLstCtrl

IMPLEMENT_DYNAMIC(CcolorLstCtrl, CListCtrl)

CcolorLstCtrl::CcolorLstCtrl()
{
	m_colRow1 = RGB(255, 255, 255);
	m_colRow2 = RGB(250, 250, 250);
}

CcolorLstCtrl::~CcolorLstCtrl()
{
}


BEGIN_MESSAGE_MAP(CcolorLstCtrl, CListCtrl)
	ON_WM_ERASEBKGND()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CcolorLstCtrl::OnNMCustomdraw)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()


// CcolorLstCtrl message handlers

BOOL CcolorLstCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	CRect		rect;
	POINT		mypoint;  
	int			nNum = 0;
	int			nHeight = 0;
	CBrush		brush0(m_colRow1);
	CBrush		brush1(m_colRow2);
	
	GetClientRect(rect);
	nHeight = GetCountPerPage();
	pDC->FillRect(&rect, &brush1);

	for (nNum = 0; nNum <= nHeight; nNum++)
	{
		GetItemPosition(nNum, &mypoint);
		rect.top = mypoint.y;
		GetItemPosition(nNum + 1, &mypoint);
		rect.bottom = mypoint.y;
		pDC->FillRect(&rect, nNum %2 ? &brush1 : &brush0);
	}

	brush0.DeleteObject();
	brush1.DeleteObject();
	
	return FALSE;

	//return CListCtrl::OnEraseBkgnd(pDC);
}

void CcolorLstCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
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
			lplvcd->clrText = RGB(0,0,0);

			//int nRowData = (int)GetItemData(lplvcd->nmcd.dwItemSpec);
			//if (nRowData)
			//{
			//	lplvcd->clrText = RGB(255, 0, 0);
			//}

			// If you want the sub items the same as the item,
			// set *pResult to CDRF_NEWFONT
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}

		// Modify sub item text and/or background
		case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{
			if (dwRow %2)
			{
				lplvcd->clrTextBk = m_colRow2;
			}
			else
			{
				lplvcd->clrTextBk = m_colRow1;
			}

			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
}

void CcolorLstCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
