#pragma once
#include "afxcmn.h"
#include "MyListCtrl.h"
#include "afxwin.h"

// CPagePDU dialog

class CPagePDU : public CDialog
{
	DECLARE_DYNAMIC(CPagePDU)

public:
	CPagePDU(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPagePDU();

// Dialog Data
	enum { IDD = IDD_DLG_PDU };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg void OnLvnItemchangedListOutlet(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangingListOutlet(NMHDR *pNMHDR, LRESULT *pResult);

	CMyListCtrl			m_cListOutlet;
	CStringArray		m_cszOutletNames;

	void CollectOutletNmsInfo();
	void UpdateOutletNamesInfo();

};