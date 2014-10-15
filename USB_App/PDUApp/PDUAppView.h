// PDUAppView.h : interface of the CPDUAppView class
//


#pragma once

#include "TreeListView.h"

class CPDUAppView : public CTreeListView
{
protected: // create from serialization only
	CPDUAppView();
	DECLARE_DYNCREATE(CPDUAppView)

// Attributes
public:

	CImageList		m_cImages;
	HINSTANCE		m_hPDUAPIDLL;
	BYTE			byTotalPDUDevice;							// Total PDU Devices connected to the system
	HTREEITEM		m_hActiveItem;

	CWinThread		*m_pThread;
	BOOL			m_bThreadExit;
	UINT			m_nPollSecs;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct
	


// Implementation
public:
	virtual ~CPDUAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void LoadAPIDLL();
	void ListAllPDU();
	BOOL GetNDisplayPDU();
	void CloseAllThreads();
	void InitiateAlertEvent();
	void GetNDisplayOutletData();
	BOOL UpdateBankPDUReadings();
	void UpdateValuesInRightView();
	void GetNDisplayBanksOutlets();
	void SetPollingSecs(UINT nPoll);
	void StartBaseThread(UINT nPoll);
	void OnOutletOn(HTREEITEM hCurrItem);
	void OnOutletOff(HTREEITEM hCurrItem);
	void SendRelayStatus(BYTE byRelayStatus);
	void OnOutletReboot(HTREEITEM hCurrItem);
	BOOL OnTreeItemSelection(UINT nTreeItemEvent);
	HTREEITEM FindParentPDUItem(HTREEITEM hAnyChild);
	CString GetExactOutletNm(CString cszPDUDevice, UINT nBankNum, UINT nOutletNum);
	BOOL GetAllUIPositionNAddress(HTREEITEM hChildTreeItem, PCURR_SEL_DETAILS pCurrSelInfo, BYTE byNeed);
	BOOL UpdateOutletStatus(CString cszPDUDevice, UINT nBankNum, UINT nOCBNum, UINT nOutletNum, UINT nRealOutlet, UINT nAlertType);


protected:
// Generated message map functions
	DECLARE_MESSAGE_MAP()

	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);

public:
	afx_msg void OnDestroy();
	afx_msg void OnSendON();
	afx_msg void OnSendOFF();
	afx_msg void OnSendREBOOT();
	afx_msg void OnOutletRefresh();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint ptMousePos);

};

