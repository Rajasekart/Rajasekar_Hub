#pragma once
#include "afxwin.h"


// CPageGeneral dialog

class CPageGeneral : public CDialog
{
	DECLARE_DYNAMIC(CPageGeneral)

public:
	CPageGeneral(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPageGeneral();

// Dialog Data
	enum { IDD = IDD_DLG_GENERAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit m_cEditGenSysNm;
	CEdit m_cEditGenSysLoctn;
	CEdit m_cEditGenSysCont;
	virtual BOOL OnInitDialog();
	CEdit m_cEditGenStaggerOnDelay;
	CEdit m_cEditGenStaggerOffDelay;
	afx_msg void OnEnChangeGenStaggerOnDelay();
	afx_msg void OnEnChangeGenStaggerOffDelay();
};
