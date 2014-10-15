// PDUConfigDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "General.h"
#include "PageNetwork.h"
#include "PageSNMP.h"
#include "PageEmail.h"
#include "PageGeneral.h"
#include "PagePDU.h"
#include "PageAlert.h"
#include "DlgSNMPVer.h"
#include "afxwin.h"
#include "PopUp.h"

// CPDUConfigDlg dialog
class CPDUConfigDlg : public CDialog
{
// Construction
public:
	CPDUConfigDlg(CWnd* pParent = NULL);	// standard constructor
	~CPDUConfigDlg();
	
// Dialog Data
	enum { IDD = IDD_PDUCONFIG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

	int						m_nFrontPage;
	int						m_nComboSel;
	BYTE					m_byMaxOutlets;
	CTabCtrl				m_cTabConfig;
	CPageNetwork			*m_pPageNetwork;
	CPageSNMP				*m_pPageSNMP;
	CPageEmail				*m_pPageEmail;
	CPageGeneral			*m_pPageGeneral;
	CPagePDU				*m_pPagePDU;
	CPageAlert				*m_pPageAlert;
	CDlgSNMPVer				*m_pPageSNMPVer;
	CString					cszCurrPath;
	CString					cszBasePath;
	CStringArray			m_cszPDUList;
	CStringArray			m_cszPDUNm;
	PDU_CONFIG_INFO			m_stPDUConfigInfo;
	CComboBox				m_cComboPDU;
	CStringArray			m_cszPidValues;
	CStringArray			m_cszVidValues;
	CPopUp					*m_cPopUpWnd;
	float					m_fTimeZones[33];

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedClearAll();
	afx_msg void OnBnClickedFctryReset();
	afx_msg void OnCbnSelchangeComboPm10Pdu();
	afx_msg void OnConfigTabChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnConfigTabChanging(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	BOOL WriteConfigData();
	BOOL LoadVidPidValues();
	BOOL GeneratePDUNames();
	void InitiateTimeZoneValues();
	void ShowPopupDlg(BYTE byType);
	BOOL OpenCurrPage(int nPageNo);
	int GetTimeZonePos(float fValue);
	void PduEnumerateDevice(BOOL bWrLog);
	BOOL ReadPDUSettingsInfo(UINT nPDUNum);
	BOOL UpdateInfoInAllPages(UINT nPageNum);
	BOOL WriteLogDetails(CString cszSection, CString cszData);
	BOOL PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog);
	BOOL PduStaticInfoWriteAndRead(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum = 0);

	CButton m_cButWrite;
};
