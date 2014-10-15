// FunctionalTestDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "General.h"

#include <wtypes.h>
#include <initguid.h>

extern "C" {

#include "hidsdi.h"
#include <setupapi.h>
#include <dbt.h>
}

union IntFloat {  __int32 i;  float f;  };

// CFunctionalTestDlg dialog
class CFunctionalTestDlg : public CDialog
{
// Construction
public:
	CFunctionalTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FUNCTIONALTEST_DIALOG };

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
	CButton m_cButPduRefresh;
	CButton m_cBtnPduSettings;
	CListCtrl m_cLstFuncTestLog;
	CButton m_cButStart;
	CButton m_cButClear;
	CButton m_cButSaveLogInfo;

	afx_msg void OnBnClickedBtnPduRefresh();
	afx_msg void OnBnClickedBtnPduSettings();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnSaveLogInfo();

	BOOL					m_bGreen;
	CString					m_cszPDUAddr;
	CStringArray			m_cszPidValues;
	CStringArray			m_cszVidValues;
	CString					m_cszIniFilePath;
	CString					m_cszDevLogFile;
	CString					m_cszLogFolderPath;
	CString					m_cszLogFilePath;
	PDU_SETTINGS			m_stPduSettings;
	CS_DEV_INFO				m_stCSDevInfo;
	CString					m_cszSerialNum;
	BOOL					m_bProcessStarted;
	BYTE					byOutletResult[100];
	CFont					m_cMedFont;

	BOOL StartProcess();
	void DeleteLastRow();
	void RefreshOurApp();
	void RefreshPDUList();
	BOOL LoadVidPidValues();
	BOOL ReadSettingsFile();
	void WaitingText(int nSleep);
	BOOL PduEnumerateDevice(BOOL bWrLog);
	CString GetRelayText(BYTE byRelayState);
	CString GetLogPathWithDate(CString cszLogPath);
	BOOL WriteLogDetails(CString cszSection, CString cszData);
	BOOL PrintBufferInTestInfoList(CString cszBuffer, int nTxtColor = 0);
	BOOL GetCurrVolt(float *fCurrent, float *fVoltage, BYTE byBank, BYTE byOutlet);
	BOOL PduGetOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog);
	BOOL PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog);
	BOOL PduSwitchOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE byCurrState, DWORD dwDelaySecs, BYTE *byErrCode, BOOL bLog);
	BOOL PduStaticInfoWriteAndRead(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum = 0);

	afx_msg void OnNMCustomdrawListFuncTestLog(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CStatic m_cStaticBgTxt;
};
