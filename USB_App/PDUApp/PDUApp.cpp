// PDUApp.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "General.h"
#include "PDUApp.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDUAppApp

BEGIN_MESSAGE_MAP(CPDUAppApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CPDUAppApp::OnAppAbout)
END_MESSAGE_MAP()


// CPDUAppApp construction

CPDUAppApp::CPDUAppApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	pLeftView = NULL;
	pRightView = NULL;
	pBottomView = NULL;
}


// The one and only CPDUAppApp object

CPDUAppApp theApp;


// CPDUAppApp initialization

BOOL CPDUAppApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
#if 0
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
#endif

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
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
	cszLoad.LoadString(IDS_STR_APP_TITLE);

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
#ifdef _APP_32BIT
		hMapObjectSingleInst = CreateFileMapping((HANDLE)(UINT_PTR) 0xFFFFFFFF,// Use Paging File
  												NULL,                // No Security Attributes
  												PAGE_READWRITE,      // Read/Write Access
  												0,                   // Size: High 32-BITS
  												sizeof(TEST),			// Size: Low 32-BITS
  												cszLoad);  	// Name Of Map Object
#elif defined _APP_64BIT
		hMapObjectSingleInst = CreateFileMapping(((HANDLE)(LONG_PTR) - 1),// Use Paging File
  												NULL,                // No Security Attributes
  												PAGE_READWRITE,      // Read/Write Access
  												0,                   // Size: High 32-BITS
  												sizeof(TEST),			// Size: Low 32-BITS
  												cszLoad);  	// Name Of Map Object
#endif

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


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	HICON			hIcon;

	hIcon = LoadIcon(IDI_ICON_PDU);
	AfxGetMainWnd()->SendMessage(WM_SETICON, TRUE, (LPARAM)hIcon);

	return TRUE;
}


// CPDUAppApp message handlers




// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString			cszLoad;

	cszLoad.Empty();
	cszLoad.LoadString(IDR_MAINFRAME);
	SetWindowText(cszLoad);

	SetDlgItemText(IDC_STATIC_MAIN_TXT, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_COPY_TXT);
	SetDlgItemText(IDC_STATIC_COPY_TXT, cszLoad);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// App command to run the dialog
void CPDUAppApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CPDUAppApp message handlers

int CPDUAppApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	PPDU_OUTLET_INFO		pCurrOUTLETInfo = NULL;
	PPDU_BANK_INFO			pCurrBANKInfo = NULL;
	PPDU_TREE_DET			pCurrPDUInfo = NULL;

	pCurrPDUInfo = gpHeadPDUInfo;

	while (pCurrPDUInfo)
	{
		if (!pCurrPDUInfo->pNextPDUInfo)
			break;

		pCurrPDUInfo = pCurrPDUInfo->pNextPDUInfo;
	}

	while (pCurrPDUInfo)
	{
		if (pCurrPDUInfo->pBankInfo)
		{
			// Here starts BANK
			pCurrBANKInfo = pCurrPDUInfo->pBankInfo;

			while (pCurrBANKInfo)
			{
				if (!pCurrBANKInfo->pNextBankInfo)
					break;

				pCurrBANKInfo = pCurrBANKInfo->pNextBankInfo;
			}

			while (pCurrBANKInfo)
			{
				if (pCurrBANKInfo->pOutletInfo)
				{
					// Here starts OUTLET
					pCurrOUTLETInfo = pCurrBANKInfo->pOutletInfo;

					while (pCurrOUTLETInfo)
					{
						if (!pCurrOUTLETInfo->pNextOutletInfo)
							break;

						pCurrOUTLETInfo = pCurrOUTLETInfo->pNextOutletInfo;
					}

					while (pCurrOUTLETInfo->pPrevOutletInfo)
					{
						pCurrOUTLETInfo = pCurrOUTLETInfo->pPrevOutletInfo;

						Free(pCurrOUTLETInfo->pNextOutletInfo);
						pCurrOUTLETInfo->pNextOutletInfo = NULL;
					}

					if (pCurrOUTLETInfo)
					{
						Free(pCurrOUTLETInfo);
						pCurrOUTLETInfo = NULL;
					}

				} // OUTLETS

				if (pCurrBANKInfo->pPrevBankInfo)
				{
					pCurrBANKInfo = pCurrBANKInfo->pPrevBankInfo;

					Free(pCurrBANKInfo->pNextBankInfo);
					pCurrBANKInfo->pNextBankInfo = NULL;
				}
				else
				{
					Free(pCurrBANKInfo);
					pCurrBANKInfo = NULL;
				}

			} // while BANKS

		} // if BANKS

		if (pCurrPDUInfo->pPrevPDUInfo)
		{
			pCurrPDUInfo = pCurrPDUInfo->pPrevPDUInfo;

			Free(pCurrPDUInfo->pNextPDUInfo);
			pCurrPDUInfo->pNextPDUInfo = NULL;
		}
		else
		{
			Free(pCurrPDUInfo);
			pCurrPDUInfo = NULL;
		}

	} // while PDU

	gpNewPDUInfo = NULL;
	gpCurrPDUInfo = NULL;
	gpHeadPDUInfo = NULL;
	gpNewBankInfo = NULL;

	return CWinApp::ExitInstance();
}

