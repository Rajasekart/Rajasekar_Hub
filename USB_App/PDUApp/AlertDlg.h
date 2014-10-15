#pragma once


// CAlertDlg dialog

class CAlertDlg : public CDialog
{
	DECLARE_DYNAMIC(CAlertDlg)

public:
	CAlertDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAlertDlg();

	PDU_OUTLET_ALERT_INFO		m_stPduOutletAlrtInfo;

// Dialog Data
	enum { IDD = IDD_DLG_ALERT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedChkVirCirBrkEnb();
	afx_msg void OnBnClickedChkHighCurrEnb();
	afx_msg void OnBnClickedChkLowCurrEnb();

	void GetUserAlertSettings();
	void SetDefaultValues(PDU_OUTLET_ALERT_INFO stPduOutletAlrtInfo);

protected:
	virtual void OnCancel();
};
