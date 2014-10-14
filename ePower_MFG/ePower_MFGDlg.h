// ePower_MFGDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MyListCtrl.h"
#include "PopUp.h"
#include "MACAddrEdit.h"

typedef union 
{
	INT32	nValue;
	float	fValue;
}IEE_CONV;

// CePower_MFGDlg dialog
class CePower_MFGDlg : public CDialog
{
// Construction
public:
	CePower_MFGDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_EPOWER_MFG_DIALOG };

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

	CMyListCtrl				m_cListCtrlLeft;
	CMyListCtrlR			m_cListCtrlRight;
	CEdit					m_cEditModelNm;
	CEdit 					m_cEditSerialNum;
	CEdit 					m_cEditManufacDate;
	CEdit 					m_cEditHostNm;
	CMACAddrEdit			m_cEditMACAddr;
	CEdit 					m_cEditStaggerOnDelay;
	CEdit 					m_cEditStaggerOffDelay;
	CEdit 					m_cEditFirmVers;
	CEdit 					m_cEditBaseUnit;
	CEdit 					m_cEditInputConn;
	CComboBox				m_cComboModelNum;
	CPopUp					*m_cPopUpWnd;

	BOOL					m_bGreen;
	int						m_nPosition;
	int						m_nDeviceType;
	UINT					m_nDivValue;
	CStringArray			m_cszSplCharsList;
	CString					m_cszBasePath;
	CString					m_cszCurrPath;
	CString					m_cszPDUAddr;
	CStringArray			m_cszBaseUnitNms;
	CStringArray			m_csz3PhSubUnitNms;
	CStringArray			m_csz1PhSubUnitNms;
	CStringArray			m_csz3PhInptConnNms;
	CByteArray				m_by3PhInpConnType;
	CStringArray			m_csz1PhInptConnNms;
	CByteArray				m_by1PhInpConnType;
	CByteArray				m_byMonitorOnly;
	CByteArray				m_byFullControl;
	CByteArray				m_byTotalOutlets;
	CByteArray				m_byTotalBanks;
	CByteArray				m_byOutletsPerBank;
	CString					m_cszGenRandNum;
	CStringArray			m_cszPidValues;
	CStringArray			m_cszVidValues;
	PHASE_INFO				m_stPhaseInfo;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedGenerate();
	afx_msg void OnCbnSelchangeModelNum();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnNMCustomdrawListLOutletNm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawListROutletNm(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL WriteCSVFile();
	CString GetYearNo();
	void LoadSplChars();
	void RefreshPDUList();
	void LoadModelSubNum();
	void ReadFirmwareVer();
	BOOL WriteStaticInfo();
	BOOL LoadVidPidValues();
	void GenerateModelNum();
	BOOL LoadAllPhaseInfo();
	BOOL GenerateSerialNum();
	BOOL ReadCSVDeviceType();
	BOOL GenerateManufacDate();
	void GenerateSubModelNum();
	void ShowPopupDlg(BYTE byType);
	CString GenerateSerialRandNum();
	BOOL PduEnumerateDevice(BOOL bWrLog);
	BOOL CheckBufferWithSplChar(CString cszBuffer);
	CString GetWeekNo(int year, int month, int day);
	CString RemoveSplCharFrmBuffer(CString cszBuffer);
	BOOL WriteLogDetails(CString cszSection, CString cszData);
	void GenerateOutletNamesInfo(UINT nTotal, BYTE byAccessFlag);
	BOOL PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog);
	BOOL PduSysNmWrite(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum);
	BOOL PduStaticInfoWriteAndRead(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum = 0);

	CComboBox m_cComboModelNumSub;
	CComboBox m_cComboModelNumEnd;
	afx_msg void OnCbnSelchangeModelSubNum();
	afx_msg void OnBnClickedReadPdu();
	afx_msg void OnEnChangeStaggerOffDelay();
	afx_msg void OnEnChangeStaggerOnDelay();
	CButton m_cButReadPDU;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CComboBox m_cComboDevice;
	afx_msg void OnCbnSelchangeDevice();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnEnChangeEditModelNm();
	afx_msg void OnEnChangeEditSerNum();
	afx_msg void OnEnChangeEditManuDate();
	afx_msg void OnEnChangeEditHostNm();
	afx_msg void OnEnChangeEditMacAddr();
	CEdit m_cEditSysNm;
	afx_msg void OnBnClickedBtnRefresh();
	CButton m_cButWrite;
};
