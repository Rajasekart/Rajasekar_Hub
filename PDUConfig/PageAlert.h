#pragma once
#include "afxwin.h"
#include "General.h"
#include "NumEdit.h"
// CPageAlert dialog

class CPageAlert : public CDialog
{
	DECLARE_DYNAMIC(CPageAlert)

public:
	CPageAlert(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageAlert();

// Dialog Data
	enum { IDD = IDD_DLG_ALERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedChkVcb();
	afx_msg void OnBnClickedChkLca();
	afx_msg void OnBnClickedChkHca();
	afx_msg void OnCbnSelchangeOutletNumLst();

	int						m_nPrevSel;
	UINT					m_nTotalOutlets;
	BOOL					m_bFirstTime;
	CButton					m_cChkVCB;
	CButton					m_cChkLCA;
	CButton					m_cChkHCA;
	CEdit 					m_cEditVCBMaxGrcPer;
	CNumEdit				m_cEditVCBThresVal;
	CEdit 					m_cEditLCAMaxGrcPer;
	CNumEdit				m_cEditLCAThresVal;
	CEdit 					m_cEditHCAMaxGrcPer;
	CNumEdit				m_cEditHCAThresVal;
	OUTLET_ALERT_INFO		m_stOutletAlertInfo;
	CComboBox				m_cComboOutletNum;


	void SaveAlertInfo();
	void EnableVCBSettings(BOOL bEnable);
	void EnableLCASettings(BOOL bEnable);
	void EnableHCASettings(BOOL bEnable);
	void FillOutletsInCombo(UINT nTotalOutlets);

};
