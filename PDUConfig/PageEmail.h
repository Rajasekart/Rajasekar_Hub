#pragma once
#include "afxwin.h"


// CPageEmail dialog

class CPageEmail : public CDialog
{
	DECLARE_DYNAMIC(CPageEmail)

public:
	CPageEmail(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageEmail();

// Dialog Data
	enum { IDD = IDD_DLG_EMAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnBnClickedCheckEnableEmail();

	CButton			m_cChkEmail;

	void EnableEmailSettings(BOOL bEnabled);
	CEdit m_cEditSMTPServer;
	CEdit m_cEditSMTPPortNum;
	CEdit m_cEditSMTPSendrUserNm;
	CEdit m_cEditSMTPSendrPasswd;
	CEdit m_cEditSMTPRecevrEmailId;
};
