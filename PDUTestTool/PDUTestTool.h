// PDUTestTool.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include <wtypes.h>
#include <initguid.h>

extern "C" {

#include "hidsdi.h"
#include <setupapi.h>
#include <dbt.h>
}

#include "General.h"

// CPDUTestToolApp:
// See PDUTestTool.cpp for the implementation of this class
//

class CPDUTestToolApp : public CWinApp
{
public:
	CPDUTestToolApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	CString					m_cszPDUAddr;
	CStringArray			m_cszPidValues;
	CStringArray			m_cszVidValues;
	CString					m_cszDevLogFile;
	CString					m_cszIniFilePath;
	CString					m_cszLogFolderPath;

	BOOL LoadVidPidValues();
	BOOL PduEnumerateDevice(BOOL bWrLog);
	CString GetLogPathWithDate(CString cszLogPath);
	BOOL WriteLogDetails(CString cszSection, CString cszData);
	BOOL PduResetOCB(CHAR *byPduadd, BYTE byCurrOCB, BYTE *byErrCode, BOOL bLog);
	BYTE PduGetHardwareModel(CHAR *byPduadd, BYTE byOCB, BYTE *byErrCode, BOOL bLog);
	BOOL PduChangeOCBid(CHAR *byPduadd, BYTE byCurrOCB, BYTE byNewOCB,BYTE *byErrCode, BOOL bLog);
	BOOL PduGetOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog);
	BOOL PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog);
	BOOL PduSwitchOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE byCurrState, DWORD dwDelaySecs, BYTE *byErrCode, BOOL bLog);

	DECLARE_MESSAGE_MAP()
};

extern CPDUTestToolApp theApp;