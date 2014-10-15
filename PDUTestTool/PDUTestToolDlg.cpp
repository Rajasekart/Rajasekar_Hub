// PDUTestToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDUTestTool.h"
#include "DlgPDUSettings.h"
#include "PDUTestToolDlg.h"
#include "SummaryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CPDUTestToolDlg dialog




CPDUTestToolDlg::CPDUTestToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPDUTestToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_bGreen = FALSE;
	m_bProcessStarted = FALSE;
	memset(&m_stPduSettings, 0, sizeof(PDU_SETTINGS));

	m_cszIniFilePath.Empty();
	m_cszIniFilePath = theApp.m_cszIniFilePath;

	m_cszLogFolderPath.Empty();
	m_cszLogFilePath.Empty();
}

void CPDUTestToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_PASS, m_cButPass);
	DDX_Control(pDX, IDC_BUTTON_FAIL, m_cButFail);
	DDX_Control(pDX, IDC_BUTTON_START, m_cButStart);
	DDX_Control(pDX, IDC_BUTTON_SAVE_LOG, m_cButSaveLog);
	DDX_Control(pDX, IDC_BUTTON_PDU_SETTINGS, m_cButPDUSetting);
	DDX_Control(pDX, IDC_LIST1, m_cListCtrlTest);
	DDX_Control(pDX, IDC_EDIT_SYS_ID, m_cEditSysID);
	DDX_Control(pDX, IDC_STATIC_BIG_TITLE, m_cStaticBigCap);
	DDX_Control(pDX, IDC_STATIC_CURR_TIT, m_cStaticBigTxt);
}

BEGIN_MESSAGE_MAP(CPDUTestToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_PDU_SETTINGS, &CPDUTestToolDlg::OnBnClickedButtonPduSettings)
	ON_BN_CLICKED(IDC_BUTTON_PDU_REFRESH, &CPDUTestToolDlg::OnBnClickedButtonPduRefresh)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &CPDUTestToolDlg::OnNMCustomdrawList1)
	ON_BN_CLICKED(IDC_BUTTON_START, &CPDUTestToolDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_PASS, &CPDUTestToolDlg::OnBnClickedButtonPass)
	ON_BN_CLICKED(IDC_BUTTON_FAIL, &CPDUTestToolDlg::OnBnClickedButtonFail)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_LOG, &CPDUTestToolDlg::OnBnClickedButtonSaveLog)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CPDUTestToolDlg::OnBnClickedButtonClear)
END_MESSAGE_MAP()


// CPDUTestToolDlg message handlers

BOOL CPDUTestToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD			dwStyle = 0;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	RefreshPDUList(TRUE);

	VERIFY(m_cBigFont.CreateFont(22,
							 	0,
							 	0,
							 	0,
							 	FW_EXTRABOLD,
							 	0,
							 	0,
							 	0,
							 	ANSI_CHARSET,
							 	OUT_DEFAULT_PRECIS,
							 	CLIP_DEFAULT_PRECIS,
							 	DEFAULT_QUALITY,
							 	DEFAULT_PITCH|FF_DONTCARE,
							 	_T("ARIAL")));

	VERIFY(m_cMedFont.CreateFont(20,
							 	0,
							 	0,
							 	0,
							 	FW_EXTRABOLD,
							 	0,
							 	0,
							 	0,
							 	ANSI_CHARSET,
							 	OUT_DEFAULT_PRECIS,
							 	CLIP_DEFAULT_PRECIS,
							 	DEFAULT_QUALITY,
							 	DEFAULT_PITCH|FF_DONTCARE,
							 	_T("ARIAL")));

	VERIFY(m_cMed1Font.CreateFont(15,
							 	0,
							 	0,
							 	0,
							 	FW_EXTRABOLD,
							 	0,
							 	0,
							 	0,
							 	ANSI_CHARSET,
							 	OUT_DEFAULT_PRECIS,
							 	CLIP_DEFAULT_PRECIS,
							 	DEFAULT_QUALITY,
							 	DEFAULT_PITCH|FF_DONTCARE,
							 	_T("ARIAL")));

	dwStyle = m_cListCtrlTest.GetExtendedStyle();
	m_cListCtrlTest.SetExtendedStyle(dwStyle | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_cListCtrlTest.InsertColumn(0, _T(""), LVCFMT_LEFT);
	m_cListCtrlTest.SetColumnWidth(0, 648);

	m_cEditSysID.SetWindowText(_T("001"));

	GetDlgItem(IDC_STATIC_BIG_TITLE)->SetFont(&m_cBigFont);
	GetDlgItem(IDC_STATIC_CURR_TIT)->SetFont(&m_cMedFont);
	GetDlgItem(IDC_STATIC_YES_TXT)->SetFont(&m_cMed1Font);
	GetDlgItem(IDC_STATIC_NO_TXT)->SetFont(&m_cMed1Font);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

CPDUTestToolDlg::~CPDUTestToolDlg()
{
}

void CPDUTestToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPDUTestToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPDUTestToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CPDUTestToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PDU_DETECT)
		{
			if (m_bGreen)
			{
				pDC->SetTextColor(RGB(0, 128, 128)); // GR
			}
			else
			{
				pDC->SetTextColor(RGB(128, 0, 0)); // RED
			}
		}
		else if (pWnd->GetDlgCtrlID() == IDC_BUTTON_PASS)
		{
			pDC->SetTextColor(RGB(0, 128, 128)); // GR
		}
		else if (pWnd->GetDlgCtrlID() == IDC_BUTTON_FAIL)
		{
			pDC->SetTextColor(RGB(128, 0, 0)); // RED
		}
		else if ((pWnd->GetDlgCtrlID() == IDC_STATIC_YES_TXT) || (pWnd->GetDlgCtrlID() == IDC_STATIC_NO_TXT))
		{
			pDC->SetTextColor(RGB(0, 0, 128)); // RED
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CPDUTestToolDlg::OnBnClickedButtonPduSettings()
{
	// TODO: Add your control notification handler code here
	CDlgPDUSettings		cPduSettings(m_cszIniFilePath);

	cPduSettings.DoModal();

	ReadSettingsFile();
}

void CPDUTestToolDlg::RefreshPDUList(BOOL bSilent)
{
	WORD						wPid = 0;
	WORD						wVid = 0;
	BYTE						byErrCode = 0;
	CString						cszLoad;
	BOOL 						bRetStatus = FALSE;

	m_bGreen = FALSE;
	cszLoad.Empty();

	if (!theApp.LoadVidPidValues())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_EPHD_UNIT_ERR_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return;
	}

	if (theApp.PduEnumerateDevice(TRUE))
	{
		m_bGreen = TRUE;
		SetDlgItemText(IDC_STATIC_PDU_DETECT, _T("PDU Device found"));
		ViewTestCtrls(TRUE);

		m_cszPDUAddr.Empty();
		m_cszPDUAddr = theApp.m_cszPDUAddr;
	}
	else
	{
		ViewTestCtrls(FALSE);
		SetDlgItemText(IDC_STATIC_PDU_DETECT, _T("No PDU device found"));
		if (!bSilent)
		{
			cszLoad = _T("No PDU device connected to the system. Press 'Refresh' button again, after connected the PDU device.");
			AfxMessageBox(cszLoad, MB_ICONSTOP | MB_OK);
		}
	}
}

void CPDUTestToolDlg::OnBnClickedButtonPduRefresh()
{
	// TODO: Add your control notification handler code here
	RefreshPDUList();
}



void CPDUTestToolDlg::ViewTestCtrls(BOOL bEnable, BOOL bProcess)
{
	m_cButStart.EnableWindow(bEnable);
	m_cButPass.EnableWindow(bProcess);
	m_cButFail.EnableWindow(bProcess);
}

void CPDUTestToolDlg::OnNMCustomdrawList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;
	DWORD dwRow = (DWORD)lplvcd->nmcd.dwItemSpec;

	switch(lplvcd->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
			return;
		}

		// Modify item text and or background
		case CDDS_ITEMPREPAINT:
		{
			if (lplvcd->nmcd.lItemlParam == GREEN_TEXT)
			{
				lplvcd->clrText = RGB(0, 128, 64);
			}
			else if (lplvcd->nmcd.lItemlParam == RED_TEXT)
			{
				lplvcd->clrText = RGB(166, 0, 0);
			}
			else if (lplvcd->nmcd.lItemlParam == BLUE_TEXT)
			{
				lplvcd->clrText = RGB(66, 66, 255);
			}

			//int nRowData = (int)GetItemData(lplvcd->nmcd.dwItemSpec);
			//if (nRowData)
			//{
			//	lplvcd->clrText = RGB(255, 0, 0);
			//}

			// If you want the sub items the same as the item,
			// set *pResult to CDRF_NEWFONT
			//*pResult = CDRF_NOTIFYSUBITEMDRAW;
			return;
		}

		// Modify sub item text and/or background
		case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:
		{
			lplvcd->clrTextBk = RGB(250, 250, 250);

			*pResult = CDRF_DODEFAULT;
			return;
		}
	}
}

BOOL CPDUTestToolDlg::PrintBufferInTestInfoList(CString cszBuffer, int nTxtColor)
{
	int					nRow = 0;
	LVITEM				lvItem = {0};

	nRow = m_cListCtrlTest.GetItemCount();

	memset(&lvItem, 0, sizeof(LVITEM));
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = nRow;
	lvItem.pszText = cszBuffer.GetBuffer(cszBuffer.GetLength());
	cszBuffer.ReleaseBuffer();
	lvItem.cchTextMax = cszBuffer.GetLength();
	lvItem.iSubItem = 0;
	lvItem.lParam = nTxtColor;
	nRow = m_cListCtrlTest.InsertItem(&lvItem);

	if (nRow == MINUS_ONE)
		return FALSE;

	nRow = m_cListCtrlTest.GetItemCount();
	if (nRow > 0)
	{
		m_cListCtrlTest.EnsureVisible((nRow - 1), FALSE);
	}
	
	return TRUE;
}

void CPDUTestToolDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here
	CString				cszLoad;

	if (m_bProcessStarted)
	{
		if (IDNO == AfxMessageBox(_T("Do you want to stop the process?"), MB_ICONINFORMATION | MB_YESNO))
		{
			return;
		}

		EnableUserButtons(FALSE);
	}

	m_bProcessStarted = !m_bProcessStarted;

	cszLoad.Empty();
	if (m_bProcessStarted)
	{
		OnBnClickedButtonClear();

		cszLoad = _T("&STOP");
		m_cButStart.SetWindowText(cszLoad);

		StartPDUTesting();

		cszLoad = _T("&START");
		m_cButStart.SetWindowText(cszLoad);

		m_bProcessStarted = FALSE;
	}
	else
	{
		cszLoad = _T("&START");
		m_cButStart.SetWindowText(cszLoad);
	}
}

BOOL CPDUTestToolDlg::ReadSettingsFile()
{
	int				nValue = 0;
	TCHAR			szBuffer[1024] = {0};

	memset(&m_stPduSettings, 0, sizeof(PDU_SETTINGS));

	if (m_cszIniFilePath.IsEmpty())
		return FALSE;

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_TOT_BANKS, 0, m_cszIniFilePath);
	m_stPduSettings.byTotalOCB = nValue + 1;

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_TOT_OUTLETS_PB, 0, m_cszIniFilePath);
	m_stPduSettings.byNumOfOutletsPerOCB = nValue + 1;

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL1, 0, m_cszIniFilePath);
	if (nValue)
	{
		m_stPduSettings.byNoOfOCB++;
		m_stPduSettings.byOCBids[0] = nValue;
	}

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL2, 0, m_cszIniFilePath);
	if (nValue)
	{
		m_stPduSettings.byNoOfOCB++;
		m_stPduSettings.byOCBids[1] = nValue;
	}

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL3, 0, m_cszIniFilePath);
	if (nValue)
	{
		m_stPduSettings.byNoOfOCB++;
		m_stPduSettings.byOCBids[2] = nValue;
	}

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL4, 0, m_cszIniFilePath);
	if (nValue)
	{
		m_stPduSettings.byNoOfOCB++;
		m_stPduSettings.byOCBids[3] = nValue;
	}

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL5, 0, m_cszIniFilePath);
	if (nValue)
	{
		m_stPduSettings.byNoOfOCB++;
		m_stPduSettings.byOCBids[4] = nValue;
	}

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_KEY_OCB_SEL6, 0, m_cszIniFilePath);
	if (nValue)
	{
		m_stPduSettings.byNoOfOCB++;
		m_stPduSettings.byOCBids[5] = nValue;
	}

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_STAGGER_ON_DELAY, 0, m_cszIniFilePath);
	m_stPduSettings.dwStaggerOnDelay = nValue;

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_STAGGER_OFF_DELAY, 0, m_cszIniFilePath);
	m_stPduSettings.dwStaggerOffDelay = nValue;

	nValue = GetPrivateProfileInt(STR_SEC_PDU_SETTINGS, STR_FINAL_RELASE, 0, m_cszIniFilePath);
	m_stPduSettings.byFinalRelease = nValue;

	GetPrivateProfileString(STR_SEC_PDU_SETTINGS, STR_LOG_FILE_PATH, _T(""), szBuffer, sizeof(szBuffer), m_cszIniFilePath);
	m_cszLogFolderPath.Empty();
	m_cszLogFolderPath = szBuffer;

	if (m_cszLogFolderPath.IsEmpty())
	{
		m_cszLogFolderPath = theApp.m_cszLogFolderPath;
	}

	return TRUE;
}

void CPDUTestToolDlg::UpdateBigText(BYTE byValue, CString cszText, BOOL bLeaveEmpty)
{
	CString			cszLoad;

	cszLoad.Empty();
	cszLoad.Format(_T("Testing Relay: %u"), byValue);

	if (bLeaveEmpty)
		cszLoad.Empty();

	m_cStaticBigCap.SetWindowText(cszLoad);

	cszLoad.Empty();
	cszLoad = _T("Current Status: ");
	cszLoad += cszText;
	m_cStaticBigTxt.SetWindowText(cszLoad);
}

CString CPDUTestToolDlg::GetRelayText(BYTE byRelayState)
{
	CString			cszLoad;

	cszLoad.Empty();
	switch(byRelayState)
	{
		case RELAY_OPEN:
		{
			cszLoad = _T("RELAY OFF");
			break;
		}

		case RELAY_CLOSE:
		{
			cszLoad = _T("RELAY ON");
			break;
		}

		case RELAY_TRIP:
		{
			cszLoad = _T("RELAY TRIP");
			break;
		}

		case RELAY_SHED:
		{
			cszLoad = _T("RELAY SHED");
			break;
		}

		case RELAY_UNKNOWN:
		default:
		{
			cszLoad = _T("RELAY UNKNOWN");
			break;
		}
	}

	return cszLoad;
}

void CPDUTestToolDlg::EnableUserButtons(BOOL bValue)
{
	m_cButPass.EnableWindow(bValue);
	m_cButFail.EnableWindow(bValue);
}

BOOL CPDUTestToolDlg::StartPDUTesting()
{
	int							nNum = 0;
	BYTE						byCount = 0;
	BYTE						byNum = 0;
	BYTE						byOCB = 0;
	BYTE						byOutlet = 0;
	BYTE						byCurrState = 0;
	BYTE						byErrorCode = 0;
	BOOL						bStatus = TRUE;
	BOOL						bResult = FALSE;
	UINT						nCount = 0;
	CString						cszLoad;
	BYTE						byOutletResult[100] = {0};
	

	if (!m_bProcessStarted)
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to continue... Process stopped by user.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	if (!ReadSettingsFile())
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to read the PDU settings information from settings file... Unable to continue...");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		cszLoad.Empty();
		cszLoad = _T("Testing process terminated...");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	if (!m_stPduSettings.byNoOfOCB)
	{
		cszLoad.Empty();
		cszLoad = _T("Not selected a valid OCD IDs in 'PDU Settings Page'. Click on 'PDU Settings' button and configure it.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	RefreshOurApp();

	if (!m_bProcessStarted)
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to continue... Process stopped by user.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	for (byNum = 0; byNum < m_stPduSettings.byNoOfOCB; byNum++)
	{
		byOCB = m_stPduSettings.byOCBids[byNum];

		RefreshOurApp();

		if (!m_bProcessStarted)
		{
			cszLoad.Empty();
			cszLoad = _T("Unable to continue... Process stopped by user.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			return FALSE;
		}

		for (byOutlet = 1; byOutlet <= m_stPduSettings.byNumOfOutletsPerOCB; byOutlet++)
		{
			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			cszLoad.Empty();
			cszLoad.Format(_T("Testing Relay : Bank %u"), byOCB);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			RefreshOurApp();

			// Start Outlet
			cszLoad.Empty();
			cszLoad.Format(_T("Testing Relay : Outlet %u"), byOutlet);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			UpdateBigText(byOutlet, _T("Fetching current relay state..."));

			RefreshOurApp();

			cszLoad.Empty();
			cszLoad.Format(_T("Fetching the Outlet %u current relay state..."), byOutlet);
			PrintBufferInTestInfoList(cszLoad);

			RefreshOurApp();

			//bResult = (*fnGetOutletState) ((CHAR *)m_stPduAddress.byPduAdd[0], byOCB, byOCB, byOutlet, byOutlet, &byCurrState, &byErrorCode, TRUE);
			bResult = theApp.PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, &byCurrState, &byErrorCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			cszLoad.Empty();
			cszLoad.Format(_T("Outlet %u present relay state is %s"), byOutlet, GetRelayText(byCurrState));
			PrintBufferInTestInfoList(cszLoad);
			UpdateBigText(byOutlet, cszLoad);

			cszLoad.Empty();
			cszLoad.Format(_T("Rebooting the Outlet %u..."), byOutlet);
			PrintBufferInTestInfoList(cszLoad);
			UpdateBigText(byOutlet, cszLoad);

			//bResult = (*fnSetOutletState) ((CHAR *)m_stPduAddress.byPduAdd[0], byOCB, byOCB, byOutlet, byOutlet, RELAY_REBOOT, &byErrorCode, TRUE);
			bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, RELAY_REBOOT, m_stPduSettings.dwStaggerOnDelay, &byErrorCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();

			RefreshOurApp();

			//Sleep(6000);
			WaitingText(6);

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}
	
			//Sleep(2000);	// Wait for reboot process

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			nCount = 0;
			m_byUserAns = UNKNOWN_ANS;
			EnableUserButtons(TRUE);
			while (TRUE)
			{
				RefreshOurApp();

				if (!m_bProcessStarted)
				{
					cszLoad.Empty();
					cszLoad = _T("Unable to continue... Process stopped by user.");
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);

					return FALSE;
				}

				nCount++;

				cszLoad.Empty();
				cszLoad.Format(_T("Testing Cycle %d"), nCount);
				PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

				cszLoad.Empty();
				cszLoad.Format(_T("Turning power OFF the Outlet %u..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);
				UpdateBigText(byOutlet, cszLoad);

				RefreshOurApp();
				
				//bResult = (*fnSetOutletState) ((CHAR *)m_stPduAddress.byPduAdd[0], byOCB, byOCB, byOutlet, byOutlet, RELAY_OPEN, &byErrorCode, TRUE);
				bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, RELAY_OPEN, m_stPduSettings.dwStaggerOffDelay, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();

#if 0
				cszLoad.Empty();
				if (!bResult)
				{
					cszLoad.Format(_T("Turning power OFF the Outlet %u call FAILED..."), byOutlet);
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);
				}
				else
				{
					cszLoad.Format(_T("Turning power OFF the Outlet %u call PASSED..."), byOutlet);
					PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);
				}
#endif

				RefreshOurApp();

				//Sleep(3000);
				WaitingText(3);

				if (!m_bProcessStarted)
				{
					cszLoad.Empty();
					cszLoad = _T("Unable to continue... Process stopped by user.");
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);

					return FALSE;
				}

				cszLoad.Empty();
				cszLoad.Format(_T("Fetching the Outlet %u current relay state..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);

				RefreshOurApp();

				bResult = theApp.PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, &byCurrState, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();

				cszLoad.Empty();
				cszLoad.Format(_T("Outlet %u present relay state is %s"), byOutlet, GetRelayText(byCurrState));
				PrintBufferInTestInfoList(cszLoad);

				RefreshOurApp();

				if (!m_bProcessStarted)
				{
					cszLoad.Empty();
					cszLoad = _T("Unable to continue... Process stopped by user.");
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);

					return FALSE;
				}

				cszLoad.Empty();
				cszLoad.Format(_T("Turning power ON the Outlet %u..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);
				UpdateBigText(byOutlet, cszLoad);

				RefreshOurApp();
				
				//bResult = (*fnSetOutletState) ((CHAR *)m_stPduAddress.byPduAdd[0], byOCB, byOCB, byOutlet, byOutlet, RELAY_CLOSE, &byErrorCode, TRUE);
				bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, RELAY_CLOSE, m_stPduSettings.dwStaggerOnDelay, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();

#if 0
				cszLoad.Empty();
				if (!bResult)
				{
					cszLoad.Format(_T("Turning power ON the Outlet %u call FAILED..."), byOutlet);
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);
				}
				else
				{
					cszLoad.Format(_T("Turning power ON the Outlet %u call PASSED..."), byOutlet);
					PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);
				}
#endif
				
				RefreshOurApp();

				//Sleep(3000);
				WaitingText(3);

				if (!m_bProcessStarted)
				{
					cszLoad.Empty();
					cszLoad = _T("Unable to continue... Process stopped by user.");
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);

					return FALSE;
				}

				cszLoad.Empty();
				cszLoad.Format(_T("Fetching the Outlet %u current relay state..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);

				RefreshOurApp();

				bResult = theApp.PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, &byCurrState, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();

				RefreshOurApp();

				if (!m_bProcessStarted)
				{
					cszLoad.Empty();
					cszLoad = _T("Unable to continue... Process stopped by user.");
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);

					return FALSE;
				}

				cszLoad.Empty();
				cszLoad.Format(_T("Outlet %u present relay state is %s"), byOutlet, GetRelayText(byCurrState));
				PrintBufferInTestInfoList(cszLoad);
		
				cszLoad.Empty();
				cszLoad = _T("Pausing 2 seconds for the next cycle...");
				PrintBufferInTestInfoList(cszLoad);
				UpdateBigText(byOutlet, cszLoad);

				RefreshOurApp();

				if (!m_bProcessStarted)
				{
					cszLoad.Empty();
					cszLoad = _T("Unable to continue... Process stopped by user.");
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);

					return FALSE;
				}

				for (nNum = 0; nNum < 10; nNum++)
				{
					Sleep(200);

					RefreshOurApp();

					if (!m_bProcessStarted)
					{
						cszLoad.Empty();
						cszLoad = _T("Unable to continue... Process stopped by user.");
						PrintBufferInTestInfoList(cszLoad, RED_TEXT);

						return FALSE;
					}

					if (m_byUserAns)
						break;
				}

				RefreshOurApp();

				if (!m_bProcessStarted)
				{
					cszLoad.Empty();
					cszLoad = _T("Unable to continue... Process stopped by user.");
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);

					return FALSE;
				}

				if (m_byUserAns)
					break;
			}

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			cszLoad.Empty();
			if (m_byUserAns == YES_ANS)
			{
				cszLoad.Format(_T("User input for testing the Outlet %u is YES"), byOutlet);
				PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);
				byOutletResult[byCount] = 1;
			}
			else if (m_byUserAns == NO_ANS) 
			{
				cszLoad.Format(_T("User input for testing the Outlet %u is NO"), byOutlet);
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);
				byOutletResult[byCount] = 0;
			}
			else if (m_byUserAns == UNKNOWN_ANS) 
			{
				cszLoad.Format(_T("User input for testing the Outlet %u is UNKNOWN"), byOutlet);
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);
				byOutletResult[byCount] = 0;
			}

			RefreshOurApp();

			byCount++;

			if (m_stPduSettings.byFinalRelease)
			{
				cszLoad.Empty();
				cszLoad.Format(_T("Final Release - Turning power ON the Outlet %u..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);
				UpdateBigText(byOutlet, cszLoad);
				
				//bResult = (*fnSetOutletState) ((CHAR *)m_stPduAddress.byPduAdd[0], byOCB, byOCB, byOutlet, byOutlet, RELAY_CLOSE, &byErrorCode, TRUE);
				bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, RELAY_CLOSE, m_stPduSettings.dwStaggerOnDelay, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();
			}
			else
			{
				cszLoad.Empty();
				cszLoad.Format(_T("Final Release - Turning power OFF the Outlet %u..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);
				UpdateBigText(byOutlet, cszLoad);
				
				//bResult = (*fnSetOutletState) ((CHAR *)m_stPduAddress.byPduAdd[0], byOCB, byOCB, byOutlet, byOutlet, RELAY_OPEN, &byErrorCode, TRUE);
				bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byOutlet, RELAY_OPEN, m_stPduSettings.dwStaggerOffDelay, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();
			}

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}
		}
	}

	CSummaryDlg		cSummDlg;

	cSummDlg.m_byTotBanks = m_stPduSettings.byNoOfOCB;
	cSummDlg.m_byTotOutlets = m_stPduSettings.byNoOfOCB * m_stPduSettings.byNumOfOutletsPerOCB;
	cSummDlg.m_byOutletsPerBank = m_stPduSettings.byNumOfOutletsPerOCB;
	for (byNum = 0; byNum < m_stPduSettings.byNoOfOCB; byNum++)
	{
		cSummDlg.m_bySelBanks[byNum] = m_stPduSettings.byOCBids[byNum];
	}

	cSummDlg.m_bRelayResult = TRUE;
	for (byNum = 0; byNum < cSummDlg.m_byTotOutlets; byNum++)
	{
		if (byOutletResult[byNum] == 0)
		{
			cSummDlg.m_bRelayResult = FALSE;
		}

		cSummDlg.m_byOutletResult[byNum] = byOutletResult[byNum];
	}

	cszLoad.Empty();
	cszLoad = _T("Process completed successfully.");
	if (cSummDlg.m_bRelayResult)
	{
		cszLoad += _T(" Relay Test Status - PASS");
		PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);
		cszLoad.Empty();
		cszLoad = _T("Relay Test Status - PASS");
	}
	else
	{
		cszLoad += _T(" Relay Test Status - FAIL");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);
		cszLoad.Empty();
		cszLoad = _T("Relay Test Status - FAIL");
	}

	UpdateBigText(0, cszLoad, TRUE);

	cSummDlg.DoModal();

	return bStatus;
}

void CPDUTestToolDlg::OnBnClickedButtonPass()
{
	// TODO: Add your control notification handler code here
	EnableUserButtons(FALSE);
	m_byUserAns = YES_ANS;
}

void CPDUTestToolDlg::OnBnClickedButtonFail()
{
	// TODO: Add your control notification handler code here
	EnableUserButtons(FALSE);
	m_byUserAns = NO_ANS;
}

void CPDUTestToolDlg::OnBnClickedButtonSaveLog()
{
	// TODO: Add your control notification handler code here
	int						nNum = 0;
	int						nTotal = 0;
	CFileFind				cFinder;
	CFile					cLogFile;
	CString					cszLoad;
	CFileException			cFileEx;

	if (m_cszLogFolderPath.IsEmpty())
	{
		m_cszLogFolderPath = theApp.m_cszLogFolderPath;
	}

	m_cszLogFilePath = theApp.GetLogPathWithDate(m_cszLogFolderPath);

	if (cFinder.FindFile(m_cszLogFilePath))
	{
		cFinder.Close();
	}
	else
	{
		if (!::CreateDirectory(m_cszLogFilePath, NULL))
		{
			AfxMessageBox(_T("Please select a valid folder path in PDU Settings dialog"), MB_ICONSTOP);
			return;
		}
	}

	m_cszLogFilePath += '\\';
	m_cszLogFilePath += STR_PDU_TEST_LOG;

	if (cFinder.FindFile(m_cszLogFilePath))
	{
		cFinder.Close();
		::DeleteFile(m_cszLogFilePath);
	}

	cLogFile.Open(m_cszLogFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &cFileEx);

	nTotal = m_cListCtrlTest.GetItemCount();

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		cszLoad.Empty();
		cszLoad = m_cListCtrlTest.GetItemText(nNum, 0);
		cszLoad += STR_NEXT_LINE;
		cLogFile.Write(cszLoad, cszLoad.GetLength());
	}
	
	cszLoad.Empty();
	cszLoad.Format(_T("Successfully created the log file in this path %s"), m_cszLogFilePath);
	AfxMessageBox(cszLoad);
}

void CPDUTestToolDlg::RefreshOurApp()
{
	MSG				msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (!::IsDialogMessage(this->m_hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void CPDUTestToolDlg::OnBnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	m_cListCtrlTest.DeleteAllItems();
}

BOOL CPDUTestToolDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->wParam == VK_RETURN)
	{
		pMsg->wParam = NULL;
	}
	else if (pMsg->wParam == VK_ESCAPE)
	{
		pMsg->wParam = NULL;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CPDUTestToolDlg::WaitingText(int nSleep)
{
	int				nNum = 0;
	CString			cszLoad;

	for (nNum = 0; nNum < nSleep; nNum++)
	{
		Sleep(1000);

		if (nNum)
		{
			DeleteLastRow();
		}

		cszLoad.Empty();
		cszLoad.Format(_T("Please wait for %d second(s)..."), (nSleep - nNum));
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);
		RefreshOurApp();
	}
}

void CPDUTestToolDlg::DeleteLastRow()
{
	int			nRow = 0;

	nRow = m_cListCtrlTest.GetItemCount();
	if (nRow > 0)
	{
		m_cListCtrlTest.DeleteItem(nRow - 1);
	}
}
