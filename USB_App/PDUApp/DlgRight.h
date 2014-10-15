#pragma once



// CDlgRight form view

class CDlgRight : public CFormView
{
	DECLARE_DYNCREATE(CDlgRight)

protected:
	CDlgRight();           // protected constructor used by dynamic creation
	virtual ~CDlgRight();

public:
	enum { IDD = IDD_DLG_RIGHT };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	CBrush				*m_pEditBkBrush;
	//CBrush				*m_pNewBkBrush;
	CFont				m_MediumFont;
	CFont				m_BigFont;
	BOOL				m_bStartPoll;
	UINT				m_nPollingSecs;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual void OnInitialUpdate();
	afx_msg void OnBnClickedButStartStop();
	afx_msg void OnEnKillfocusEditSecs();
	afx_msg void OnBnClickedButSet();

	void UpdateAllValues(CStringArray &cszValuesArray, BYTE byItemLevel);
};


