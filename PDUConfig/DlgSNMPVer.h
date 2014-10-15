#pragma once
#include "afxwin.h"


// CDlgSNMPVer dialog

class CDlgSNMPVer : public CDialog
{
	DECLARE_DYNAMIC(CDlgSNMPVer)

public:
	CDlgSNMPVer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSNMPVer();

// Dialog Data
	enum { IDD = IDD_DLG_SNMP_VER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_cEditSNMPV3User;
	CComboBox m_cComboAuthLevel;
	CEdit m_cEditAuthPassKey;
	CEdit m_cEditConfAuthPassKey;
	CEdit m_cEditPrivPassKey;
	CEdit m_cEditPrivConfPasskey;
	CEdit m_cEditV3ReadComm;
	CEdit m_cEditV3WriteComm;
	afx_msg void OnBnClickedRadioSnmpV2c();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioSnmpV3();

	void UpdateSNMPVerUI(BOOL bEnable);
	void FillAuthLevels();
	void SelectControl(BYTE byV3);
	BOOL GetV3CheckStatus();




	afx_msg void OnCbnSelchangeComboAuthLevel();
};
