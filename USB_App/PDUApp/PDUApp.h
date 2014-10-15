// PDUApp.h : main header file for the PDUApp application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPDUAppApp:
// See PDUApp.cpp for the implementation of this class
//
#include "General.h"
#include "PDUAppView.h"
#include "DlgRight.h"
#include "DlgBtm.h"

class CPDUAppApp : public CWinApp
{
public:
	CPDUAppApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	CPDUAppView			*pLeftView;
	CDlgRight			*pRightView;
	CDlgBtm				*pBottomView;

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()


};

extern CPDUAppApp theApp;