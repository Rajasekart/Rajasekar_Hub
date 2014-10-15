// DlgBtm.h : interface of the CDlgBtm class
//


#pragma once
#include "colorLstCtrl.h"

class CDlgBtm : public CView
{
protected: // create from serialization only
	CDlgBtm();
	DECLARE_DYNCREATE(CDlgBtm)

// Attributes
public:

	BOOL				m_bLogCreated;
	CcolorLstCtrl		m_LogLstCtrl;
	CWinThread			*m_pThreadBtm;
	BOOL				m_bThreadBtmExit;
	CImageList			m_cImages;
	CToolTipCtrl		m_ToolTip;

// Operations
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CDlgBtm();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();

	void InitToolTip();
	void StartBaseThread();
	void AddCreatedAlertMess();
	void CloseAllThreads();
	CString FindPDUDevice(CString cszPDUAdd);
	int UpdateLogList(void *NotUsed, int argc, char **argv, char **azColName);
	void UpdateAllAlertMess();
	void DisplayLogMessage(BYTE byMessType, CString cszMessage, CString cszDate);
	
	afx_msg void OnDestroy();
	afx_msg void OnLogDelete();
	afx_msg void OnLogSelectall();
	afx_msg void OnLogDeselectall();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
