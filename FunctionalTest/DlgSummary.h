#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "General.h"

// CDlgSummary dialog

class CDlgSummary : public CDialog
{
	DECLARE_DYNAMIC(CDlgSummary)

public:
	CDlgSummary(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSummary();

// Dialog Data
	enum { IDD = IDD_DLG_SUMMARY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_cStaticResult;
	CListCtrl m_cListCtrlResult;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMCustomdrawListSummary(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	BOOL		m_bTestResult;
	CFont		m_cMedFont;
	BYTE		m_byTotalBanks;
	BYTE		m_byTotalOutlets;
	BYTE		m_byResult[100];

	BOOL PrintBufferInTestInfoList(CString cszBuffer, CString cszResult, int nTxtColor);
};
