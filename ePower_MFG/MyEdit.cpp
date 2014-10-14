#include "stdafx.h"
#include "MyEdit.h"
#include "General.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEdit

CMyEdit* CMyEdit::m_pMyEdit = NULL;  

CMyEdit::CMyEdit()
{
	m_iRowIndex= -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;
	m_strValidChars.Empty();
}

CMyEdit::~CMyEdit()
{
}

BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//{{AFX_MSG_MAP(CMyEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

void CMyEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here

	// Get the text in the edit ctrl
	CString strEdit;
	GetWindowText(strEdit);

	// Send Notification to parent of edit ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iRowIndex;
	dispinfo.item.iSubItem = m_iColumnIndex;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_strWindowText) : LPTSTR((LPCTSTR)strEdit);
	dispinfo.item.cchTextMax = m_bESC ? m_strWindowText.GetLength() : strEdit.GetLength();
	
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	PostMessage(WM_CLOSE);
}

void CMyEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	if (COL_OUTLET_NAM == m_iColumnIndex)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}
	else if (COL_OUTLET_VOL == m_iColumnIndex)
	{
		// VOLTAGE
		if (!((nChar < '0' || nChar > '9') && nChar != '.' && nChar != 8)) //Is not a number or backspace
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
		}
	}
	else if (COL_OUTLET_FRQ == m_iColumnIndex)
	{
		// FREQUENCY
		if (!((nChar < '0' || nChar > '9') && nChar != 8)) //Is not a number or backspace
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
		}
	}

#if 0
 	if ((m_strValidChars.IsEmpty()) || ((-1 != m_strValidChars.Find(static_cast<TCHAR> (nChar))) || 
		(nChar == VK_BACK) || (nChar == CTRL_C) || (nChar == CTRL_V) || (nChar == CTRL_X)))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
#endif
}

BOOL CMyEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN == pMsg->message && (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

int CMyEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	ShowWindow(SW_SHOW);
	SetWindowText(m_strWindowText);
	SetSel(0, -1);
	SetFocus();
	
	  
	return 0;
}

CMyEdit* CMyEdit::GetInstance()
{
	if(m_pMyEdit == NULL)
	{
		m_pMyEdit = new CMyEdit;
	}
	return m_pMyEdit;
}

void CMyEdit::DeleteInstance()
{
	delete m_pMyEdit;
	m_pMyEdit = NULL;
}

BOOL CMyEdit::ShowEditCtrl(DWORD dwStyle, const RECT &rCellRect, CWnd* pParentWnd, UINT uiResourceID, int iRowIndex, int iColumnIndex, CString& strValidChars, CString& rstrCurSelection)
{
	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_strValidChars = strValidChars;
	m_strWindowText = rstrCurSelection;
	m_bESC = FALSE;

	if (NULL == m_pMyEdit->m_hWnd) 
	{
		return m_pMyEdit->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}	

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMyEditR

CMyEditR* CMyEditR::m_pMyEdit = NULL;  

CMyEditR::CMyEditR()
{
	m_iRowIndex= -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;
	m_strValidChars.Empty();
}

CMyEditR::~CMyEditR()
{
}

BEGIN_MESSAGE_MAP(CMyEditR, CEdit)
	//{{AFX_MSG_MAP(CMyEditR)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEditR message handlers

void CMyEditR::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here

	// Get the text in the edit ctrl
	CString strEdit;
	GetWindowText(strEdit);

	// Send Notification to parent of edit ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iRowIndex;
	dispinfo.item.iSubItem = m_iColumnIndex;
	dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_strWindowText) : LPTSTR((LPCTSTR)strEdit);
	dispinfo.item.cchTextMax = m_bESC ? m_strWindowText.GetLength() : strEdit.GetLength();
	
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	PostMessage(WM_CLOSE);
}

void CMyEditR::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	if (COL_OUTLET_NAM == m_iColumnIndex)
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}
	else if (COL_OUTLET_VOL == m_iColumnIndex)
	{
		// VOLTAGE
		if (!((nChar < '0' || nChar > '9') && nChar != '.' && nChar != 8)) //Is not a number or backspace
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
		}
	}
	else if (COL_OUTLET_FRQ == m_iColumnIndex)
	{
		// FREQUENCY
		if (!((nChar < '0' || nChar > '9') && nChar != 8)) //Is not a number or backspace
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
			return;
		}
	}

#if 0
 	if ((m_strValidChars.IsEmpty()) || ((-1 != m_strValidChars.Find(static_cast<TCHAR> (nChar))) || 
		(nChar == VK_BACK) || (nChar == CTRL_C) || (nChar == CTRL_V) || (nChar == CTRL_X)))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	else
	{
		MessageBeep(MB_ICONEXCLAMATION);
		return;
	}
#endif
}

BOOL CMyEditR::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN == pMsg->message && (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}

int CMyEditR::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	ShowWindow(SW_SHOW);
	SetWindowText(m_strWindowText);
	SetSel(0, -1);
	SetFocus();
	
	  
	return 0;
}

CMyEditR* CMyEditR::GetInstance()
{
	if(m_pMyEdit == NULL)
	{
		m_pMyEdit = new CMyEditR;
	}
	return m_pMyEdit;
}

void CMyEditR::DeleteInstance()
{
	delete m_pMyEdit;
	m_pMyEdit = NULL;
}

BOOL CMyEditR::ShowEditCtrl(DWORD dwStyle, const RECT &rCellRect, CWnd* pParentWnd, UINT uiResourceID, int iRowIndex, int iColumnIndex, CString& strValidChars, CString& rstrCurSelection)
{
	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_strValidChars = strValidChars;
	m_strWindowText = rstrCurSelection;
	m_bESC = FALSE;

	if (NULL == m_pMyEdit->m_hWnd) 
	{
		return m_pMyEdit->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}	

	return TRUE;
}
