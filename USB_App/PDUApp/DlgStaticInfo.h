#pragma once
#include "afxcmn.h"


// CDlgStaticInfo dialog

class CDlgStaticInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgStaticInfo)

public:
	CDlgStaticInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgStaticInfo();

// Dialog Data
	enum { IDD = IDD_DLG_STATIC_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CString					cszPDUName;
	PDU_STATIC_INTO			stPDUStaticInfo;


	BOOL DisplayAllStaticValues();

	CListCtrl m_cListBankCtrl;
};
