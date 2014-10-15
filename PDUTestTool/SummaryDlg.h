#pragma once
#include "afxcmn.h"


// CSummaryDlg dialog

class CSummaryDlg : public CDialog
{
	DECLARE_DYNAMIC(CSummaryDlg)

public:
	CSummaryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSummaryDlg();

// Dialog Data
	enum { IDD = IDD_DLG_SUMM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CFont		m_cMedFont;
	CFont		m_cMed1Font;
	BYTE		m_byTotBanks;
	BYTE		m_bySelBanks[6];
	BYTE		m_byTotOutlets;
	BYTE		m_byOutletResult[100];
	BYTE		m_byOutletsPerBank;
	BOOL		m_bChangeOCB;
	BOOL		m_bChangeOCB_Result;
	BOOL		m_bRelayResult;
	CListCtrl	m_cListCtrlTestSumm;

	afx_msg void OnNMCustomdrawListTestSumm(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL PrintBufferInTestInfoList(CString cszBuffer, CString cszResult, int nTxtColor);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
