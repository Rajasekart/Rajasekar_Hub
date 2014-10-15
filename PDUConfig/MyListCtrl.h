#if !defined(AFX_MYLISTCTRL_H__2464747__INCLUDED_)
#define AFX_MYLISTCTRL_H__2464747__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class CMyEdit;

// User define message 
// This message is posted to the parent
// The message can be handled to make the necessary validations, if any
#define WM_VALIDATE		WM_USER + 0x7FFD

// User define message 
// This message is posted to the parent
// The message should be handled to spcify the items to the added to the combo
#define WM_SET_ITEMS	WM_USER + 0x7FFC

class CMyListCtrl : public CListCtrl
{
public:
	
// Implementation

	// Constructor
	CMyListCtrl();

	// Destructor
	virtual ~CMyListCtrl();

	BOOL			bEditMode;
	CMyEdit*		pMyEdit;

	// Sets/Resets the column which support the in place edit control
	void SetReadOnlyColumns(int iColumnIndex, bool bSet = true);

	// Sets the valid characters for the edit ctrl
	void SetValidEditCtrlCharacters(CString& rstrValidCharacters);

	// Sets the vertical scroll
	void EnableVScroll(bool bEnable = true);

	// Sets the horizontal scroll
	void EnableHScroll(bool bEnable = true);

	// Pass to edit mode while pressing the enter key
	void OnPressEnterKey();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	//}}AFX_VIRTUAL

protected:

// Methods
	// Generated message map functions
	//{{AFX_MSG(CMyListCtrl)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
			
// Implementation

	// Returns the row & column index of the column on which mouse click event has occured
	bool HitTestEx(CPoint& rHitPoint, int* pRowIndex, int* pColumnIndex) const;

	// Creates and displays the in place edit control
	CMyEdit* ShowMyEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection);

	// Calculates the cell rect
	void CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect);

	// Checks whether column is read only
	bool IsReadOnly(int iColumnIndex);

	// Scrolls the list ctrl to bring the in place ctrl to the view
	void ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& obCellRect);

// Attributes
	
	// List of columns that are read only
	CList<int, int> m_ReadOnlyColumnsList;

	// Valid characters
	CString m_strValidEditCtrlChars;

	// The window style of the in place edit ctrl
	DWORD m_dwEditCtrlStyle;

	// The window style of the in place combo ctrl
	DWORD m_dwDropDownCtrlStyle;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CStringArray	m_cszOrgOutletNm;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__2464747__INCLUDED_)
