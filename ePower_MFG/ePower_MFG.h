// ePower_MFG.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CePower_MFGApp:
// See ePower_MFG.cpp for the implementation of this class
//

class CePower_MFGApp : public CWinApp
{
public:
	CePower_MFGApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CePower_MFGApp theApp;