// pduapi.h : main header file for the pduapi DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CpduapiApp
// See pduapi.cpp for the implementation of this class
//

class CpduapiApp : public CWinApp
{
public:
	CHAR					szNodeName[1024];

	
	
	CArray<CWinThread *, CWinThread *> m_AlertThreadArray;
		
	CpduapiApp();

	BOOL StopAllAlertEvent();
	BOOL StartOutletAlertThrd(BYTE byPDUPos, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, UINT nIniFilePos);

// Overrides
public:
	virtual BOOL InitInstance();
//	void RegisterForDeviceNotifications();
	DECLARE_MESSAGE_MAP()
};
