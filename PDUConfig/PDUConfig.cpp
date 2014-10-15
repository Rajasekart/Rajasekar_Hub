// PDUConfig.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PDUConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDUConfigApp

BEGIN_MESSAGE_MAP(CPDUConfigApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPDUConfigApp construction

CPDUConfigApp::CPDUConfigApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPDUConfigApp object

CPDUConfigApp theApp;


// CPDUConfigApp initialization

BOOL CPDUConfigApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	BOOL					fInit = FALSE;
	PTEST					pTest =	NULL;
	CString					cszLoad;
	HANDLE 					hMapObjectSingleInst = NULL;

	cszLoad.Empty();
	cszLoad.LoadString(AFX_IDS_APP_TITLE);

	if ((hMapObjectSingleInst = OpenFileMapping (FILE_MAP_WRITE, FALSE, cszLoad)) != NULL)
	{
		/* -----------------------------------------------------------------------
						Get A Pointer To The File-Mapped Shared Memory.
		------------------------------------------------------------------------- */
		pTest = (PTEST) MapViewOfFile(hMapObjectSingleInst,// Object To Map View Of
										FILE_MAP_WRITE, // Read/Write Access
										0,              // High Offset:  Map From
										0,              // Low Offset:   Beginning
										0);             // Default: Map Entire File
		if (pTest == NULL) 
		{
			return CWinApp::ExitInstance();
		}
		else
		{
			if (pTest->dwInstance)
			{
				UnmapViewOfFile (pTest);
				pTest = NULL;

				CloseHandle(hMapObjectSingleInst);
				hMapObjectSingleInst = NULL;

				return CWinApp::ExitInstance();
			}
		}
	}
	else
	{
		/* -----------------------------------------------------------------------
							Create A Named File Mapping Object.
		------------------------------------------------------------------------- */
		hMapObjectSingleInst = CreateFileMapping((HANDLE)(UINT_PTR) 0xFFFFFFFF,// Use Paging File
  												NULL,                // No Security Attributes
  												PAGE_READWRITE,      // Read/Write Access
  												0,                   // Size: High 32-BITS
  												sizeof(TEST),			// Size: Low 32-BITS
  												cszLoad);  	// Name Of Map Object

		if (hMapObjectSingleInst == NULL) 
		{
			return CWinApp::ExitInstance();
		}

		/* -----------------------------------------------------------------------
						The First Process To Attach Initializes Memory.
		------------------------------------------------------------------------- */
		fInit = (GetLastError() != ERROR_ALREADY_EXISTS);
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																								
		/* -----------------------------------------------------------------------
					Get A Pointer To The File-Mapped Shared Memory.
		------------------------------------------------------------------------- */
		pTest = (PTEST) MapViewOfFile(hMapObjectSingleInst,// Object To Map View Of
										FILE_MAP_WRITE, // Read/Write Access
										0,              // High Offset:  Map From
										0,              // Low Offset:   Beginning
										0);             // Default: Map Entire File
		if (pTest == NULL) 
		{
			return CWinApp::ExitInstance();
		}

		/* -----------------------------------------------------------------------
				Initialize Memory If This Is The First Process.
		------------------------------------------------------------------------- */
		if (fInit)
		{
  			memset (pTest, '\0', sizeof(TEST));
			++pTest->dwInstance ;
		}
	}

	CPDUConfigDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
