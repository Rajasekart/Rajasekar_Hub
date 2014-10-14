#include "stdafx.h"
#include "MyListCtrl.h"
#include "MyEdit.h"
#include "General.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{
	m_ReadOnlyColumnsList.RemoveAll();
	m_strValidEditCtrlChars.Empty();
	m_dwEditCtrlStyle = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_NOHIDESEL;
	m_dwDropDownCtrlStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
	m_nCurrColumnNum = 0;
	bEditMode = FALSE;
	pMyEdit = NULL;
}

CMyListCtrl::~CMyListCtrl()
{
	CMyEdit::DeleteInstance();  
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	//}}AFX_MSG_MAP
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers

CMyEdit* CMyListCtrl::ShowMyEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection)
{
	// Create an in-place edit control
	CMyEdit* pMyEdit = CMyEdit::GetInstance();

	m_nCurrColumnNum = iColumnIndex;

	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);

	pMyEdit->ShowEditCtrl(m_dwEditCtrlStyle, obCellRect, this, 0, iRowIndex, iColumnIndex, m_strValidEditCtrlChars, rstrCurSelection);

	if (iColumnIndex == COL_OUTLET_NAM)
	{
		pMyEdit->SetLimitText(15);
	}
	else if (COL_OUTLET_VOL == iColumnIndex)
	{
		pMyEdit->SetLimitText(6);
	}
	else if (COL_OUTLET_FRQ == iColumnIndex)
	{
		pMyEdit->SetLimitText(2);
	}

	return pMyEdit;
}

void CMyListCtrl::OnHScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if (GetFocus() != this)
	{
		SetFocus();
	}

	CListCtrl::OnHScroll(iSBCode, iPos, pScrollBar);
}

void CMyListCtrl::OnVScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if (GetFocus() != this)
	{
		SetFocus();
	}

	CListCtrl::OnVScroll(iSBCode, iPos, pScrollBar);
}

void CMyListCtrl::OnLButtonDown(UINT iFlags, CPoint obPoint) 
{
	// TODO: Add your message handler code here and/or call default

	int iColumnIndex = -1;
	int iRowIndex = -1;

	if (bEditMode)
	{
		if (pMyEdit)
		{
			pMyEdit->OnKillFocus(this);
			//pMyEdit->SendMessage(WM_KEYDOWN, VK_RETURN, 0);
		}
		//return;
	}

	// Get the current column and row
	if (!HitTestEx(obPoint, &iRowIndex, &iColumnIndex))
	{
		return;
	}

	CListCtrl::OnLButtonDown(iFlags, obPoint);
	
	// If column is not read only then
	// If the SHIFT or CTRL key is down call the base class
	// Check the high bit of GetKeyState to determine whether SHIFT or CTRL key is down
	if ((GetKeyState(VK_SHIFT) & 0x80) || (GetKeyState(VK_CONTROL) & 0x80))
	{
		return;
	}

	// Get the current selection before creating the in place combo box
	CString strCurSelection = GetItemText(iRowIndex, iColumnIndex);

	if (-1 != iRowIndex)
	{
		UINT flag = LVIS_FOCUSED;
		
		if ((GetItemState(iRowIndex, flag ) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				if (!IsReadOnly(iColumnIndex))
				{
					//CMyEdit* pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					bEditMode = TRUE;
					pMyEdit->ShowWindow(TRUE);
				}
			}
		}
	}  
}

void CMyListCtrl::OnPressEnterKey()
{
	int			iColumnIndex = OUTLET_NM_ID;
	int			iRowIndex = -1;
	CString		strCurSelection;
	UINT		flag = 0;

	iRowIndex = this->GetSelectionMark();

	if (-1 != iRowIndex)
	{
		strCurSelection = GetItemText(iRowIndex, iColumnIndex);

		UINT flag = LVIS_FOCUSED;
		
		if ((GetItemState(iRowIndex, flag ) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				if (!IsReadOnly(iColumnIndex))
				{
					//CMyEdit* pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					bEditMode = TRUE;
				}
			}
		}
	}  
}

bool CMyListCtrl::HitTestEx(CPoint &obPoint, int* pRowIndex, int* pColumnIndex) const
{
	if (!pRowIndex || !pColumnIndex)
	{
		return false;
	}

	// Get the row index
	*pRowIndex = HitTest(obPoint, NULL);

	if (pColumnIndex)
	{
		*pColumnIndex = 0;
	}

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{
		return false;
	}

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	int iColumnCount = pHeader->GetItemCount();

	// Get bounding rect of item and check whether obPoint falls in it.
	CRect obCellRect;
	GetItemRect(*pRowIndex, &obCellRect, LVIR_BOUNDS);
	
	if (obCellRect.PtInRect(obPoint))
	{
		// Now find the column
		for (*pColumnIndex = 0; *pColumnIndex < iColumnCount; (*pColumnIndex)++)
		{
			int iColWidth = GetColumnWidth(*pColumnIndex);
			
			if (obPoint.x >= obCellRect.left && obPoint.x <= (obCellRect.left + iColWidth))
			{
				return true;
			}
			obCellRect.left += iColWidth;
		}
	}
	return false;
}

void CMyListCtrl::SetReadOnlyColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ReadOnlyColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet) 
	{
		m_ReadOnlyColumnsList.AddTail(iColumnIndex); 
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet) 
	{
		m_ReadOnlyColumnsList.RemoveAt(Pos); 
	}
}

bool CMyListCtrl::IsReadOnly(int iColumnIndex)
{
	if (m_ReadOnlyColumnsList.Find(iColumnIndex))
	{
		return true;
	}
	
	return false;
}

void CMyListCtrl::CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect)
{
	GetItemRect(iRowIndex, &robCellRect, LVIR_BOUNDS);
	
	CRect rcClient;
	GetClientRect(&rcClient);

	if (robCellRect.right > rcClient.right) 
	{
		robCellRect.right = rcClient.right;
	}

	ScrollToView(iColumnIndex, robCellRect); 
}

void CMyListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	CString			cszLoad;
	CString			cszOrgTxt;

	cszLoad.Empty();
	cszLoad = pDispInfo->item.pszText;

	if (cszLoad.IsEmpty()) 
	{
		if (m_nCurrColumnNum == COL_OUTLET_NAM)
		{
			cszLoad = m_cszOrgOutletNm.ElementAt(pDispInfo->item.iItem);
		}
		else if (m_nCurrColumnNum == COL_OUTLET_VOL)
		{
			cszLoad = m_cszOrgOutletVolt.ElementAt(pDispInfo->item.iItem);
		}
		else if (m_nCurrColumnNum == COL_OUTLET_FRQ)
		{
			cszLoad = m_cszOrgOutletFreq.ElementAt(pDispInfo->item.iItem);
		}
	}

	if ((m_nCurrColumnNum == COL_OUTLET_VOL) || (m_nCurrColumnNum == COL_OUTLET_FRQ))
	{
		cszOrgTxt.Empty();
		if (m_nCurrColumnNum == COL_OUTLET_VOL)
		{
			cszOrgTxt = m_cszOrgOutletVolt.ElementAt(pDispInfo->item.iItem);
		}
		else if (m_nCurrColumnNum == COL_OUTLET_FRQ)
		{
			cszOrgTxt = m_cszOrgOutletFreq.ElementAt(pDispInfo->item.iItem);
		}

		if (!cszOrgTxt.CompareNoCase(STR_NOT_APP))
		{
			cszLoad = STR_NOT_APP;
		}
	}
	
	// Update the item text with the new text
	SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, cszLoad);

	GetParent()->SendMessage(WM_VALIDATE, GetDlgCtrlID(), (LPARAM)pDispInfo); 

	bEditMode = FALSE;
	pMyEdit = NULL;
	
	*pResult = 0;
}

void CMyListCtrl::SetValidEditCtrlCharacters(CString &rstrValidCharacters)
{
	m_strValidEditCtrlChars = rstrValidCharacters;
}

void CMyListCtrl::EnableHScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_HSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_HSCROLL;
	}	
}

void CMyListCtrl::EnableVScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_VSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_VSCROLL;
	}
}

void CMyListCtrl::ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& robCellRect)
{
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);

	int iColumnWidth = GetColumnWidth(iColumnIndex);

	// Get the column iOffset
	int iOffSet = 0;
	for (int iIndex_ = 0; iIndex_ < iColumnIndex; iIndex_++)
	{
		iOffSet += GetColumnWidth(iIndex_);
	}

	// If x1 of cell rect is < x1 of ctrl rect or
	// If x1 of cell rect is > x1 of ctrl rect or **Should not ideally happen**
	// If the width of the cell extends beyond x2 of ctrl rect then
	// Scroll

	CSize obScrollSize(0, 0);

	if (((iOffSet + robCellRect.left) < rcClient.left) || 
		((iOffSet + robCellRect.left) > rcClient.right))
	{
		obScrollSize.cx = iOffSet + robCellRect.left;
	}
	else if ((iOffSet + robCellRect.left + iColumnWidth) > rcClient.right)
	{
		obScrollSize.cx = iOffSet + robCellRect.left + iColumnWidth - rcClient.right;
	}

	Scroll(obScrollSize);
	robCellRect.left -= obScrollSize.cx;
	
	// Set the width to the column width
	robCellRect.left += iOffSet;
	robCellRect.right = robCellRect.left + iColumnWidth;
}

void CMyListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (IsReadOnly(pDispInfo->item.iSubItem))
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

BOOL CMyListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN)
		{
			if (!bEditMode)
			{
				OnPressEnterKey();
				pMsg->wParam = NULL;
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

void CMyListCtrl::OnKillFocus(CWnd* pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
}


/////////////////////////////////////////////////////////////
// LIST CTRL - 2
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrlR

CMyListCtrlR::CMyListCtrlR()
{
	m_ReadOnlyColumnsList.RemoveAll();
	m_strValidEditCtrlChars.Empty();
	m_dwEditCtrlStyle = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_NOHIDESEL;
	m_dwDropDownCtrlStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
	m_nCurrColumnNum = 0;
	bEditMode = FALSE;
	pMyEdit = NULL;
}

CMyListCtrlR::~CMyListCtrlR()
{
	CMyEdit::DeleteInstance();  
}


BEGIN_MESSAGE_MAP(CMyListCtrlR, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrlR)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	//}}AFX_MSG_MAP
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrlR message handlers

CMyEditR* CMyListCtrlR::ShowMyEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection)
{
	// Create an in-place edit control
	CMyEditR* pMyEdit = CMyEditR::GetInstance();

	m_nCurrColumnNum = iColumnIndex;

	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);

	pMyEdit->ShowEditCtrl(m_dwEditCtrlStyle, obCellRect, this, 0, iRowIndex, iColumnIndex, m_strValidEditCtrlChars, rstrCurSelection);

	if (iColumnIndex == COL_OUTLET_NAM)
	{
		pMyEdit->SetLimitText(15);
	}
	else if (COL_OUTLET_VOL == iColumnIndex)
	{
		pMyEdit->SetLimitText(6);
	}
	else if (COL_OUTLET_FRQ == iColumnIndex)
	{
		pMyEdit->SetLimitText(2);
	}

	return pMyEdit;
}

void CMyListCtrlR::OnHScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if (GetFocus() != this)
	{
		SetFocus();
	}

	CListCtrl::OnHScroll(iSBCode, iPos, pScrollBar);
}

void CMyListCtrlR::OnVScroll(UINT iSBCode, UINT iPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default

	if (GetFocus() != this)
	{
		SetFocus();
	}

	CListCtrl::OnVScroll(iSBCode, iPos, pScrollBar);
}

void CMyListCtrlR::OnLButtonDown(UINT iFlags, CPoint obPoint) 
{
	// TODO: Add your message handler code here and/or call default

	int iColumnIndex = -1;
	int iRowIndex = -1;

	if (bEditMode)
	{
		if (pMyEdit)
		{
			pMyEdit->OnKillFocus(this);
			//pMyEdit->SendMessage(WM_KEYDOWN, VK_RETURN, 0);
		}
		//return;
	}

	// Get the current column and row
	if (!HitTestEx(obPoint, &iRowIndex, &iColumnIndex))
	{
		return;
	}

	CListCtrl::OnLButtonDown(iFlags, obPoint);
	
	// If column is not read only then
	// If the SHIFT or CTRL key is down call the base class
	// Check the high bit of GetKeyState to determine whether SHIFT or CTRL key is down
	if ((GetKeyState(VK_SHIFT) & 0x80) || (GetKeyState(VK_CONTROL) & 0x80))
	{
		return;
	}

	// Get the current selection before creating the in place combo box
	CString strCurSelection = GetItemText(iRowIndex, iColumnIndex);

	if (-1 != iRowIndex)
	{
		UINT flag = LVIS_FOCUSED;
		
		if ((GetItemState(iRowIndex, flag ) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				if (!IsReadOnly(iColumnIndex))
				{
					//CMyEditR* pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					bEditMode = TRUE;
					pMyEdit->ShowWindow(TRUE);
				}
			}
		}
	}  
}

void CMyListCtrlR::OnPressEnterKey()
{
	int			iColumnIndex = OUTLET_NM_ID;
	int			iRowIndex = -1;
	CString		strCurSelection;
	UINT		flag = 0;

	iRowIndex = this->GetSelectionMark();

	if (-1 != iRowIndex)
	{
		strCurSelection = GetItemText(iRowIndex, iColumnIndex);

		UINT flag = LVIS_FOCUSED;
		
		if ((GetItemState(iRowIndex, flag ) & flag) == flag)
		{
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				if (!IsReadOnly(iColumnIndex))
				{
					//CMyEditR* pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					pMyEdit = ShowMyEdit(iRowIndex, iColumnIndex, strCurSelection);
					bEditMode = TRUE;
				}
			}
		}
	}  
}

bool CMyListCtrlR::HitTestEx(CPoint &obPoint, int* pRowIndex, int* pColumnIndex) const
{
	if (!pRowIndex || !pColumnIndex)
	{
		return false;
	}

	// Get the row index
	*pRowIndex = HitTest(obPoint, NULL);

	if (pColumnIndex)
	{
		*pColumnIndex = 0;
	}

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{
		return false;
	}

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	int iColumnCount = pHeader->GetItemCount();

	// Get bounding rect of item and check whether obPoint falls in it.
	CRect obCellRect;
	GetItemRect(*pRowIndex, &obCellRect, LVIR_BOUNDS);
	
	if (obCellRect.PtInRect(obPoint))
	{
		// Now find the column
		for (*pColumnIndex = 0; *pColumnIndex < iColumnCount; (*pColumnIndex)++)
		{
			int iColWidth = GetColumnWidth(*pColumnIndex);
			
			if (obPoint.x >= obCellRect.left && obPoint.x <= (obCellRect.left + iColWidth))
			{
				return true;
			}
			obCellRect.left += iColWidth;
		}
	}
	return false;
}

void CMyListCtrlR::SetReadOnlyColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ReadOnlyColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet) 
	{
		m_ReadOnlyColumnsList.AddTail(iColumnIndex); 
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet) 
	{
		m_ReadOnlyColumnsList.RemoveAt(Pos); 
	}
}

bool CMyListCtrlR::IsReadOnly(int iColumnIndex)
{
	if (m_ReadOnlyColumnsList.Find(iColumnIndex))
	{
		return true;
	}
	
	return false;
}

void CMyListCtrlR::CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect)
{
	GetItemRect(iRowIndex, &robCellRect, LVIR_BOUNDS);
	
	CRect rcClient;
	GetClientRect(&rcClient);

	if (robCellRect.right > rcClient.right) 
	{
		robCellRect.right = rcClient.right;
	}

	ScrollToView(iColumnIndex, robCellRect); 
}

void CMyListCtrlR::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	CString			cszLoad;
	CString			cszOrgTxt;

	cszLoad.Empty();
	cszLoad = pDispInfo->item.pszText;

	if (cszLoad.IsEmpty()) 
	{
		if (m_nCurrColumnNum == COL_OUTLET_NAM)
		{
			cszLoad = m_cszOrgOutletNm.ElementAt(pDispInfo->item.iItem);
		}
		else if (m_nCurrColumnNum == COL_OUTLET_VOL)
		{
			cszLoad = m_cszOrgOutletVolt.ElementAt(pDispInfo->item.iItem);
		}
		else if (m_nCurrColumnNum == COL_OUTLET_FRQ)
		{
			cszLoad = m_cszOrgOutletFreq.ElementAt(pDispInfo->item.iItem);
		}
	}

	if ((m_nCurrColumnNum == COL_OUTLET_VOL) || (m_nCurrColumnNum == COL_OUTLET_FRQ))
	{
		cszOrgTxt.Empty();
		if (m_nCurrColumnNum == COL_OUTLET_VOL)
		{
			cszOrgTxt = m_cszOrgOutletVolt.ElementAt(pDispInfo->item.iItem);
		}
		else if (m_nCurrColumnNum == COL_OUTLET_FRQ)
		{
			cszOrgTxt = m_cszOrgOutletFreq.ElementAt(pDispInfo->item.iItem);
		}

		if (!cszOrgTxt.CompareNoCase(STR_NOT_APP))
		{
			cszLoad = STR_NOT_APP;
		}
	}
	
	// Update the item text with the new text
	SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, cszLoad);

	GetParent()->SendMessage(WM_VALIDATE, GetDlgCtrlID(), (LPARAM)pDispInfo); 

	bEditMode = FALSE;
	pMyEdit = NULL;
	
	*pResult = 0;
}

void CMyListCtrlR::SetValidEditCtrlCharacters(CString &rstrValidCharacters)
{
	m_strValidEditCtrlChars = rstrValidCharacters;
}

void CMyListCtrlR::EnableHScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_HSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_HSCROLL;
	}	
}

void CMyListCtrlR::EnableVScroll(bool bEnable /*= true*/)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_VSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_VSCROLL;
	}
}

void CMyListCtrlR::ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& robCellRect)
{
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);

	int iColumnWidth = GetColumnWidth(iColumnIndex);

	// Get the column iOffset
	int iOffSet = 0;
	for (int iIndex_ = 0; iIndex_ < iColumnIndex; iIndex_++)
	{
		iOffSet += GetColumnWidth(iIndex_);
	}

	// If x1 of cell rect is < x1 of ctrl rect or
	// If x1 of cell rect is > x1 of ctrl rect or **Should not ideally happen**
	// If the width of the cell extends beyond x2 of ctrl rect then
	// Scroll

	CSize obScrollSize(0, 0);

	if (((iOffSet + robCellRect.left) < rcClient.left) || 
		((iOffSet + robCellRect.left) > rcClient.right))
	{
		obScrollSize.cx = iOffSet + robCellRect.left;
	}
	else if ((iOffSet + robCellRect.left + iColumnWidth) > rcClient.right)
	{
		obScrollSize.cx = iOffSet + robCellRect.left + iColumnWidth - rcClient.right;
	}

	Scroll(obScrollSize);
	robCellRect.left -= obScrollSize.cx;
	
	// Set the width to the column width
	robCellRect.left += iOffSet;
	robCellRect.right = robCellRect.left + iColumnWidth;
}

void CMyListCtrlR::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (IsReadOnly(pDispInfo->item.iSubItem))
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}

BOOL CMyListCtrlR::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_RETURN)
		{
			if (!bEditMode)
			{
				OnPressEnterKey();
				pMsg->wParam = NULL;
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

void CMyListCtrlR::OnKillFocus(CWnd* pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
}
