#pragma once


// CcolorLstCtrl

class CcolorLstCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CcolorLstCtrl)

public:
	CcolorLstCtrl();
	virtual ~CcolorLstCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);

public:
	COLORREF			m_colRow1;
	COLORREF			m_colRow2;
	
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};


