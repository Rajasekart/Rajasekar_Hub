// FunctionalTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FunctionalTest.h"
#include "FunctionalTestDlg.h"
#include "DlgPDUSettings.h"
#include "DlgSummary.h"

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


// CFunctionalTestDlg dialog




CFunctionalTestDlg::CFunctionalTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFunctionalTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_cszLogFilePath.Empty();

	m_bGreen = FALSE;
	m_bProcessStarted = FALSE;
	memset(&m_stPduSettings, 0, sizeof(PDU_SETTINGS));

	m_cszPidValues.RemoveAll();
	m_cszVidValues.RemoveAll();
	memset(&m_stCSDevInfo, 0, sizeof(CS_DEV_INFO));
	memset(&byOutletResult, 0, sizeof(byOutletResult));
}

void CFunctionalTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_PDU_REFRESH, m_cButPduRefresh);
	DDX_Control(pDX, IDC_BTN_PDU_SETTINGS, m_cBtnPduSettings);
	DDX_Control(pDX, IDC_LIST_FUNC_TEST_LOG, m_cLstFuncTestLog);
	DDX_Control(pDX, IDC_BTN_START, m_cButStart);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_cButClear);
	DDX_Control(pDX, IDC_BTN_SAVE_LOG_INFO, m_cButSaveLogInfo);
	DDX_Control(pDX, IDC_STATIC_BIG_TXT, m_cStaticBgTxt);
}

BEGIN_MESSAGE_MAP(CFunctionalTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_PDU_REFRESH, &CFunctionalTestDlg::OnBnClickedBtnPduRefresh)
	ON_BN_CLICKED(IDC_BTN_PDU_SETTINGS, &CFunctionalTestDlg::OnBnClickedBtnPduSettings)
	ON_BN_CLICKED(IDC_BTN_START, &CFunctionalTestDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CFunctionalTestDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_SAVE_LOG_INFO, &CFunctionalTestDlg::OnBnClickedBtnSaveLogInfo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_FUNC_TEST_LOG, &CFunctionalTestDlg::OnNMCustomdrawListFuncTestLog)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CFunctionalTestDlg message handlers

BOOL CFunctionalTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	TCHAR					szNodeName[1024] = {0};
	DWORD					dwStyle = 0;

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

	memset(szNodeName, 0, sizeof(szNodeName));
	GetModuleFileName(NULL, szNodeName, sizeof(szNodeName));
	ASSERT(lstrlen(szNodeName));
	*(strrchr(szNodeName, '\\') + 1) = '\0';

	m_cszPDUAddr.Empty();
	m_cszIniFilePath.Empty();
	m_cszIniFilePath = szNodeName;
	m_cszIniFilePath += STR_PDU_UNITS;

	m_cszDevLogFile.Empty();
	m_cszDevLogFile = szNodeName;
	m_cszDevLogFile += STR_LOG_FILE_NM;

	m_cszLogFilePath.Empty();
	m_cszLogFilePath = szNodeName;

	dwStyle = m_cLstFuncTestLog.GetExtendedStyle();
	m_cLstFuncTestLog.SetExtendedStyle(dwStyle | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_cLstFuncTestLog.InsertColumn(0, _T(""), LVCFMT_LEFT);
	m_cLstFuncTestLog.SetColumnWidth(0, 572);

	GetDlgItem(IDC_STATIC_BIG_TXT)->SetFont(&m_cMedFont);

	RefreshPDUList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFunctionalTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFunctionalTestDlg::OnPaint()
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
HCURSOR CFunctionalTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CFunctionalTestDlg::GetRelayText(BYTE byRelayState)
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

void CFunctionalTestDlg::RefreshPDUList()
{
	WORD						wPid = 0;
	WORD						wVid = 0;
	BYTE						byErrCode = 0;
	CString						cszLoad;
	BOOL 						bRetStatus = FALSE;

	m_bGreen = FALSE;
	cszLoad.Empty();

	if (!LoadVidPidValues())
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to find the Config file CS_Device_Units.ini.");
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return;
	}

	if (PduEnumerateDevice(TRUE))
	{
		m_bGreen = TRUE;
		SetDlgItemText(IDC_STATIC_PDU_STATUS, _T("PDU Device found"));
		m_cButStart.EnableWindow(TRUE);
	}
	else
	{
		m_cButStart.EnableWindow(FALSE);
		SetDlgItemText(IDC_STATIC_PDU_STATUS, _T("No PDU device found"));
		//if (!bSilent)
		//{
		//	cszLoad = _T("No PDU device connected to the system. Press 'Refresh' button again, after connected the PDU device.");
		//	AfxMessageBox(cszLoad, MB_ICONSTOP | MB_OK);
		//}
	}
}


void CFunctionalTestDlg::OnBnClickedBtnPduRefresh()
{
	// TODO: Add your control notification handler code here
	RefreshPDUList();
}

void CFunctionalTestDlg::OnBnClickedBtnPduSettings()
{
	// TODO: Add your control notification handler code here
	CDlgPDUSettings			cPduSettings(m_cszIniFilePath, m_cszLogFilePath);

	cPduSettings.DoModal();
}

void CFunctionalTestDlg::OnBnClickedBtnStart()
{
	CString				cszLoad;

	if (m_bProcessStarted)
	{
		if (IDNO == AfxMessageBox(_T("Do you want to stop the process?"), MB_ICONINFORMATION | MB_YESNO))
		{
			return;
		}
	}

	m_bProcessStarted = !m_bProcessStarted;

	cszLoad.Empty();
	if (m_bProcessStarted)
	{
		OnBnClickedBtnClear();

		cszLoad = _T("&STOP");
		m_cButStart.SetWindowText(cszLoad);

		StartProcess();

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

BOOL CFunctionalTestDlg::StartProcess()
{
	// TODO: Add your control notification handler code here
	INT					nCount = 0;
	float				fCurrentOFF = 0.0;
	float				fVoltageOFF = 0.0;
	float				fCurrentON = 0.0;
	float				fVoltageON = 0.0;
	BOOL				bTestFail = FALSE;
	BOOL				bResult = FALSE;
	BYTE				byOCB = 0;
	BYTE				byRealOutlet = 0;
	BYTE				byErrorCode = 0;
	BYTE				byTotalOutlets = 0;
	BYTE				byCurrState = 0;
	BYTE				byTotalBanks = 0;
	BYTE				byOutletsPerBank = 0;
	BYTE				byNum = 0;
	BYTE				byOutlet = 0;
	BYTE				byBank = 0;
	BYTE				byErrCode = 0;
	BYTE				byDataBuffer[66] = {0};
	CString				cszLoad;
	CString				cszBuffer;
	CStringArray		cszErrLogs;

	if (!m_bProcessStarted)
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to continue... Process stopped by user.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	RefreshOurApp();

	memset(&m_stCSDevInfo, 0, sizeof(CS_DEV_INFO));

	if (m_cszPDUAddr.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to find the PDU Device. Please press the Refresh button again to detect the PDU device.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);
		return FALSE;
	}

	RefreshOurApp();

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

	RefreshOurApp();

	if (!m_bProcessStarted)
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to continue... Process stopped by user.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	for (byNum = PDU_MODEL_NM; byNum <= PDU_MANUFAC_DATE; byNum++)
	{
		RefreshOurApp();

		if (!m_bProcessStarted)
		{
			cszLoad.Empty();
			cszLoad = _T("Unable to continue... Process stopped by user.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			return FALSE;
		}

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byNum, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

		cszBuffer.Empty();
		cszBuffer.Format(_T("%s"), byDataBuffer);
		cszBuffer.TrimLeft();
		cszBuffer.TrimRight();

		RefreshOurApp();

		if (!m_bProcessStarted)
		{
			cszLoad.Empty();
			cszLoad = _T("Unable to continue... Process stopped by user.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			return FALSE;
		}

		cszLoad.Empty();
		if (byNum == PDU_MODEL_NM)
		{
			cszLoad = _T("Model Name :");
		}
		else if (byNum == PDU_SERIAL_NUM)
		{
			memcpy(m_stCSDevInfo.bySerialNum, cszBuffer.GetBuffer(cszBuffer.GetLength()), cszBuffer.GetLength());

			cszLoad = _T("Serial Number :");

			m_cszSerialNum.Empty();
			m_cszSerialNum = cszBuffer;
		}
		else if (byNum == PDU_MODEL_NUM)
		{
			memcpy(m_stCSDevInfo.byModelNum, cszBuffer.GetBuffer(cszBuffer.GetLength()), cszBuffer.GetLength());

			cszLoad = _T("Model Number :");
		}
		else if (byNum == PDU_MANUFAC_DATE)
		{
			cszLoad = _T("Manufacturing Date :");
		}

		RefreshOurApp();

		if (!m_bProcessStarted)
		{
			cszLoad.Empty();
			cszLoad = _T("Unable to continue... Process stopped by user.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			return FALSE;
		}

		PrintBufferInTestInfoList(cszLoad);
		PrintBufferInTestInfoList(cszBuffer, BLUE_TEXT);
	}

	// ============= Display Firmware version ===============================
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_FIRMWARE_VERSION, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	m_cszPDUAddr.ReleaseBuffer();

	cszBuffer.Empty();
	cszBuffer.Format(_T("%s"), byDataBuffer);
	cszLoad.Empty();
	cszLoad = _T("Firmware Version :");

	RefreshOurApp();

	if (!m_bProcessStarted)
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to continue... Process stopped by user.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	PrintBufferInTestInfoList(cszLoad);
	PrintBufferInTestInfoList(cszBuffer, BLUE_TEXT);

	// ============= Display Total Outlets, Banks, Outlets per Bank, Phase & Input Connection ===============================
	for (byNum = PDU_TOTAL_OUTLETS; byNum <= PDU_PHASE_INP_CONN_TYPE; byNum++)
	{
		RefreshOurApp();

		if (!m_bProcessStarted)
		{
			cszLoad.Empty();
			cszLoad = _T("Unable to continue... Process stopped by user.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			return FALSE;
		}

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byNum, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

		cszBuffer.Empty();
		cszBuffer.Format(_T("%u"), byDataBuffer[0]);
		cszLoad.Empty();
		if (byNum == PDU_TOTAL_OUTLETS)
		{
			m_stCSDevInfo.byTotOutlets = byDataBuffer[0];

			cszLoad = _T("Total Outlets :");
		}
		else if (byNum == PDU_TOTAL_BANKS)
		{
			m_stCSDevInfo.byTotBanks = byDataBuffer[0];

			cszLoad = _T("Total Banks :");
		}
		else if (byNum == PDU_OUTLETS_PER_BANK)
		{
			m_stCSDevInfo.byOutletsPerBank = byDataBuffer[0];

			cszLoad = _T("Outlets Per Bank :");
		}
		else if (byNum == PDU_PHASE_INP_CONN_TYPE)
		{
			cszBuffer.Empty();
			cszBuffer.Format(_T("%u"), byDataBuffer[0]);
			cszLoad.Empty();
			cszLoad = _T("Phase Type :");

			PrintBufferInTestInfoList(cszLoad);
			PrintBufferInTestInfoList(cszBuffer, BLUE_TEXT);

			cszBuffer.Empty();
			cszBuffer.Format(_T("%c"), byDataBuffer[1]);
			cszLoad.Empty();
			cszLoad = _T("Input Connection Type :");

		}

		PrintBufferInTestInfoList(cszLoad);
		PrintBufferInTestInfoList(cszBuffer, BLUE_TEXT);

		RefreshOurApp();

		if (!m_bProcessStarted)
		{
			cszLoad.Empty();
			cszLoad = _T("Unable to continue... Process stopped by user.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			return FALSE;
		}
	}

	RefreshOurApp();

	if (!m_bProcessStarted)
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to continue... Process stopped by user.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);

		return FALSE;
	}

	if ((!m_stCSDevInfo.byTotOutlets) || (!m_stCSDevInfo.byOutletsPerBank))
	{
		cszLoad.Empty();
		cszLoad = _T("Device is not configured using CyberSwitching Manufacturing Tool. Close this application, please configure and then re-launch this application again.");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);
		AfxMessageBox(cszLoad, MB_ICONSTOP);

		return FALSE;
	}

	byOCB = 1;

	for (byBank = 1; byBank <= m_stCSDevInfo.byTotBanks; byBank++)
	{
		RefreshOurApp();

		if (!m_bProcessStarted)
		{
			cszLoad.Empty();
			cszLoad = _T("Unable to continue... Process stopped by user.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			return FALSE;
		}

		byRealOutlet = 1;

		for (byOutlet = 1; byOutlet <= m_stCSDevInfo.byOutletsPerBank; byOutlet++)
		{
			nCount++;

			bTestFail = FALSE;

			cszErrLogs.RemoveAll();

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			cszLoad.Empty();
			cszLoad = _T("Functional testing Relay :");
			cszBuffer.Empty();
			cszBuffer.Format(_T("Bank %u"), byBank);

			PrintBufferInTestInfoList(cszLoad);
			PrintBufferInTestInfoList(cszBuffer, BLUE_TEXT);
			RefreshOurApp();

			// Start Outlet
			cszLoad.Empty();
			cszLoad = _T("Functional testing Relay :");
			cszBuffer.Empty();
			cszBuffer.Format(_T("Outlet %u"), byOutlet);

			PrintBufferInTestInfoList(cszLoad);
			PrintBufferInTestInfoList(cszBuffer, BLUE_TEXT);
			RefreshOurApp();

			m_cStaticBgTxt.SetWindowText(_T("Fetching current relay state..."));

			RefreshOurApp();

			cszLoad.Empty();
			cszLoad.Format(_T("Fetching the Outlet %u current relay state..."), byOutlet);
			PrintBufferInTestInfoList(cszLoad);

			RefreshOurApp();

			byCurrState = RELAY_UNKNOWN;
			bResult = PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byRealOutlet, &byCurrState, &byErrorCode, TRUE);
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
	
			m_cStaticBgTxt.SetWindowText(cszLoad);

			if (byCurrState != RELAY_OPEN)
			{
				// NEED TO OFF THE OUTLET ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				cszLoad.Empty();
				cszLoad.Format(_T("Turning power OFF the Outlet %u..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);

				m_cStaticBgTxt.SetWindowText(cszLoad);

				RefreshOurApp();

				bResult = PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byRealOutlet, RELAY_OPEN, m_stPduSettings.dwStaggerOffDelay, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();

				RefreshOurApp();

				//Sleep(3000);
				WaitingText(3);
			}

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

			byCurrState = RELAY_UNKNOWN;
			bResult = PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byRealOutlet, &byCurrState, &byErrorCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();

			if (byCurrState != RELAY_OPEN)
			{
				bTestFail = TRUE;
				byOutletResult[nCount] = 0;

				cszLoad.Empty();
				cszLoad.Format(_T("Unable to power OFF the Outlet %u..."), byOutlet);
				cszErrLogs.Add(cszLoad);
			}

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
			cszLoad = _T("Getting the Current and Voltage value...");
			PrintBufferInTestInfoList(cszLoad);

			m_cStaticBgTxt.SetWindowText(cszLoad);

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			fCurrentOFF = 0.0;
			fVoltageOFF = 0.0;
			GetCurrVolt(&fCurrentOFF, &fVoltageOFF, byOCB, byRealOutlet);

			// NEED TO ON THE OUTLET ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			cszLoad.Empty();
			cszLoad.Format(_T("Turning power ON the Outlet %u..."), byOutlet);
			PrintBufferInTestInfoList(cszLoad);

			m_cStaticBgTxt.SetWindowText(cszLoad);

			RefreshOurApp();

			bResult = PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byRealOutlet, RELAY_CLOSE, m_stPduSettings.dwStaggerOnDelay, &byErrorCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();

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

			byCurrState = RELAY_UNKNOWN;
			bResult = PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byRealOutlet, &byCurrState, &byErrorCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();

			if (byCurrState != RELAY_CLOSE)
			{
				bTestFail = TRUE;
				byOutletResult[nCount] = 0;

				cszLoad.Empty();
				cszLoad.Format(_T("Unable to power ON the Outlet %u..."), byOutlet);
				cszErrLogs.Add(cszLoad);
			}

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
			cszLoad = _T("Getting the Current and Voltage value...");
			PrintBufferInTestInfoList(cszLoad);

			m_cStaticBgTxt.SetWindowText(cszLoad);

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			fCurrentON = 0.0;
			fVoltageON = 0.0;
			GetCurrVolt(&fCurrentON, &fVoltageON, byOCB, byRealOutlet);

			if (m_stPduSettings.byFinalRelease)
			{
				cszLoad.Empty();
				cszLoad.Format(_T("Final Release - Turning power ON the Outlet %u..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);
				m_cStaticBgTxt.SetWindowText(cszLoad);
								
				bResult = PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byRealOutlet, RELAY_CLOSE, m_stPduSettings.dwStaggerOnDelay, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();
			}
			else
			{
				cszLoad.Empty();
				cszLoad.Format(_T("Final Release - Turning power OFF the Outlet %u..."), byOutlet);
				PrintBufferInTestInfoList(cszLoad);
				m_cStaticBgTxt.SetWindowText(cszLoad);
								
				bResult = PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byOCB, byRealOutlet, RELAY_OPEN, m_stPduSettings.dwStaggerOffDelay, &byErrorCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();
			}

			RefreshOurApp();

			cszLoad.Empty();
			cszLoad = _T("------------------------------------------------------------------------");
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			cszLoad.Empty();
			cszLoad.Format(_T("Current & Voltage value taken when the outlet %u is in OFF state"), byOutlet);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			cszLoad.Empty();
			cszLoad.Format(_T("Current value : %.9f A"), fCurrentOFF);
			PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);

			m_cStaticBgTxt.SetWindowText(cszLoad);

			cszLoad.Empty();
			cszLoad.Format(_T("Voltage value : %.9f V"), fVoltageOFF);
			PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);

			m_cStaticBgTxt.SetWindowText(cszLoad);

			cszLoad.Empty();
			cszLoad.Format(_T("Current & Voltage value taken when the outlet %u is in ON state"), byOutlet);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			cszLoad.Empty();
			cszLoad.Format(_T("Current value : %.9f A"), fCurrentON);
			PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);

			m_cStaticBgTxt.SetWindowText(cszLoad);

			cszLoad.Empty();
			cszLoad.Format(_T("Voltage value : %.9f V"), fVoltageON);
			PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);

			m_cStaticBgTxt.SetWindowText(cszLoad);

			cszLoad.Empty();
			cszLoad = _T("------------------------------------------------------------------------");
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			if (100 > fVoltageOFF)
			{
				bTestFail = TRUE;
				byOutletResult[nCount] = 0;

				cszLoad.Empty();
				cszLoad.Format(_T("Voltage value is less than the value 100 when the Outlet %u is in OFF state..."), byOutlet);
				cszErrLogs.Add(cszLoad);
			}

			if (100 > fVoltageON)
			{
				bTestFail = TRUE;
				byOutletResult[nCount] = 0;

				cszLoad.Empty();
				cszLoad.Format(_T("Voltage value is less than the value 100 when the Outlet %u is in ON state..."), byOutlet);
				cszErrLogs.Add(cszLoad);
			}
			
			if (fCurrentOFF >= fCurrentON)
			{
				bTestFail = TRUE;
				byOutletResult[nCount] = 0;

				cszLoad.Empty();
				cszLoad.Format(_T("Current value taken when the Outlet %u is in ON state is less than the Current value taken when the Outlet %u is in OFF state..."), byOutlet, byOutlet);
				cszErrLogs.Add(cszLoad);
			}

			if (!bTestFail)
			{
				byOutletResult[nCount] = 1;
			}

			////////////////// ERROR LOG AREA ////////////////////////////////////////////////
			cszLoad.Empty();
			cszLoad = _T("------------------------------------------------------------------------");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			cszLoad.Empty();
			cszLoad = _T("ERROR LOG :");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);

			if (cszErrLogs.GetCount())
			{
				for (int ne = 0; ne < cszErrLogs.GetCount(); ne++)
				{
					cszLoad.Empty();
					cszLoad = cszErrLogs.ElementAt(ne);
					PrintBufferInTestInfoList(cszLoad, RED_TEXT);
				}
			}
			else
			{
				cszLoad.Empty();
				cszLoad = _T("no error(s)");
				PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);
			}

			cszLoad.Empty();
			cszLoad = _T("------------------------------------------------------------------------");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);
			//////////////////////////////////////////////

			byRealOutlet++;

			if ((byRealOutlet > 4) && (byOutlet < m_stCSDevInfo.byOutletsPerBank))
			{
				byOCB++;
				byRealOutlet = 1;
			}

		} // for byOutlet

		byOCB++;

	} // for byBank

#if 1		// LOG PART
	nCount = 1;
	for (byNum = 1; byNum <= m_stCSDevInfo.byTotBanks; byNum++)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("Bank %u"), byNum);

		PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

		for (byOutlet =  1; byOutlet <= m_stCSDevInfo.byOutletsPerBank; byOutlet++)
		{
			bResult = TRUE;
			if (byOutletResult[nCount] == 0)
			{
				bResult = FALSE;
			}

			cszLoad.Empty();
			cszLoad.Format(_T("Outlet %u"), byOutlet);
			cszLoad += _T("      ");
			if (bResult)
			{
				cszLoad += _T("PASS");
				PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);
			}
			else
			{
				cszLoad += _T("FAIL");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);
			}

			nCount++;
		}
	}
#endif

	CDlgSummary		cSummaryDlg;

	cSummaryDlg.m_byTotalBanks = m_stCSDevInfo.byTotBanks;
	cSummaryDlg.m_byTotalOutlets = m_stCSDevInfo.byOutletsPerBank;
	cSummaryDlg.m_bTestResult = TRUE;
	for (byNum = 1; byNum <= m_stCSDevInfo.byTotOutlets; byNum++)
	{
		if (byOutletResult[byNum] == 0)
		{
			cSummaryDlg.m_bTestResult = FALSE;
		}

		cSummaryDlg.m_byResult[byNum] = byOutletResult[byNum];
	}

	cszLoad.Empty();
	cszLoad = _T("Process completed successfully.");
	if (cSummaryDlg.m_bTestResult)
	{
		cszLoad += _T(" Functional Test Status - PASS");
		PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);
		cszLoad.Empty();
		cszLoad = _T("Functional Test Status - PASS");
	}
	else
	{
		cszLoad += _T(" Functional Test Status - FAIL");
		PrintBufferInTestInfoList(cszLoad, RED_TEXT);
		cszLoad.Empty();
		cszLoad = _T("Functional Test Status - FAIL");
	}

	m_cStaticBgTxt.SetWindowText(cszLoad);

	cSummaryDlg.DoModal();

	OnBnClickedBtnSaveLogInfo();

	return TRUE;
}

BOOL CFunctionalTestDlg::GetCurrVolt(float *fCurrent, float *fVoltage, BYTE byBank, BYTE byOutlet)
{
	BOOL				bLog = TRUE;
	BYTE				byErrCode = 0;
	CString				cszLoad;
	DWORD				dwByteWritten = 0;
	DWORD				NumberOfBytesRead = 0;
	BYTE				InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};
	union IntFloat		val; 
	/* -----------------------------------------------------------------------
		Send the Pass through Command to get VOLTAGE
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byBank;
	InputPacketBuff[3] = TIME_STAMP_ID;			// Time Stamp
	InputPacketBuff[4] = VOLTAGE_ID;				// Voltage
	InputPacketBuff[5] = byOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
	
	PduWriteReadBuffer(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), InputPacketBuff, &dwByteWritten, FALSE, &byErrCode, bLog);
	m_cszPDUAddr.ReleaseBuffer();

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	*fVoltage = val.f;

	/* -----------------------------------------------------------------------
		Send the Pass through Command to get CURRENT
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byBank;
	InputPacketBuff[3] = TIME_STAMP_ID;			// Time Stamp
	InputPacketBuff[4] = CURRENT_ID;				// CURRENT
	InputPacketBuff[5] = byOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
		
	PduWriteReadBuffer(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), InputPacketBuff, &dwByteWritten, FALSE, &byErrCode, bLog);
	m_cszPDUAddr.ReleaseBuffer();

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	*fCurrent = val.f;

	return TRUE;
}

void CFunctionalTestDlg::WaitingText(int nSleep)
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

void CFunctionalTestDlg::DeleteLastRow()
{
	int			nRow = 0;

	nRow = m_cLstFuncTestLog.GetItemCount();
	if (nRow > 0)
	{
		m_cLstFuncTestLog.DeleteItem(nRow - 1);
	}
}


void CFunctionalTestDlg::OnBnClickedBtnClear()
{
	// TODO: Add your control notification handler code here
	m_cLstFuncTestLog.DeleteAllItems();
}

void CFunctionalTestDlg::OnBnClickedBtnSaveLogInfo()
{
	// TODO: Add your control notification handler code here
	int						nNum = 0;
	int						nTotal = 0;
	CFileFind				cFinder;
	CFile					cLogFile;
	CString					cszLoad;
	CFileException			cFileEx;
	CString					cszLogFilePath;

	if (m_cszLogFolderPath.IsEmpty())
	{
		m_cszLogFolderPath = m_cszLogFilePath;
	}

	cszLogFilePath = GetLogPathWithDate(m_cszLogFolderPath);

	if (cFinder.FindFile(cszLogFilePath))
	{
		cFinder.Close();
	}
	else
	{
		if (!::CreateDirectory(cszLogFilePath, NULL))
		{
			AfxMessageBox(_T("Please select a valid folder path in PDU Settings dialog"), MB_ICONSTOP);
			return;
		}
	}

	cszLogFilePath += '\\';
	//cszLogFilePath += STR_PDU_TEST_LOG;		NEED TO CHECKKKKKKKKKKKKKKKKKKKKKKk
	cszLogFilePath += m_cszSerialNum;
	cszLogFilePath += _T("_");
	cszLogFilePath = GetLogPathWithDate(cszLogFilePath);
	cszLogFilePath += _T(".txt");

	if (cFinder.FindFile(cszLogFilePath))
	{
		cFinder.Close();
		::DeleteFile(cszLogFilePath);
	}

	cLogFile.Open(cszLogFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &cFileEx);

	nTotal = m_cLstFuncTestLog.GetItemCount();

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		cszLoad.Empty();
		cszLoad = m_cLstFuncTestLog.GetItemText(nNum, 0);
		cszLoad += STR_NEXT_LINE;
		cLogFile.Write(cszLoad, cszLoad.GetLength());
	}
	
	cszLoad.Empty();
	cszLoad.Format(_T("Successfully created the log file in this path %s \nDo you wish to open the log file in notepad..."), cszLogFilePath);
	if (IDYES == AfxMessageBox(cszLoad, MB_YESNO | MB_ICONINFORMATION))
	{
		ShellExecute(NULL, _T("open"), cszLogFilePath, NULL, NULL, SW_SHOWNORMAL);
	}
}

void CFunctionalTestDlg::RefreshOurApp()
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

BOOL CFunctionalTestDlg::PduStaticInfoWriteAndRead(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum)
{
	BOOL									bRet = FALSE;
	CHAR									byLocalBuf[MAX_PATH] = {0};
	BYTE									InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};
	DWORD									dwByteWritten = 0;
	DWORD									NumberOfBytesRead = 0;
	CString				 					szTmpstring;
	BYTE									byCount = 0;
	CString									cszLoad;
	CString									cszTemp;

	if (bWrite)
	{
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Write Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.

		// ********** WRITE *****************
		InputPacketBuff[1] = PDU_STATIC_WRITE;
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byInfoSize;		// Time Stamp

		for (byCount = 0; byCount < byInfoSize; byCount++)
		{
			InputPacketBuff[4+byCount] =  *(byInfoStr + byCount);
		}
 
		PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, TRUE, byErrCode, bWrLog);
		bRet = TRUE;
	}
	else
	{
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Read Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.

		// ********** READ *****************
		InputPacketBuff[1] = PDU_STATIC_READ;		//for Read Static Information
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byOutletNum;

		PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bWrLog);
		if (byInfoID == InputPacketBuff[1])
		{
	/*		if ((byInfoID != PDU_MAC_ADDRS) && (byInfoID != PDU_STAGGER_ON_DELAY) && (byInfoID != PDU_STAGGER_OFF_DELAY))
			{*/
				/* -----------------------------------------------------------------------
					Copy the Buffer
				----------------------------------------------------------------------- */
				for (byCount = 0; byCount < InputPacketBuff[2]; byCount++)
				{
					*(byInfoStr + byCount) =  *(InputPacketBuff + (3 + byCount));
				}
		/*	}
			else if (byInfoID == PDU_MAC_ADDRS)
			{
				for (byCount = 0; byCount <= 8; byCount++)
				{
					*(byInfoStr + byCount) =  *(InputPacketBuff+(2+byCount));
				}
			}
			else if ((byInfoID == PDU_STAGGER_ON_DELAY) || (byInfoID == PDU_STAGGER_OFF_DELAY))
			{
				for (byCount = 0; byCount <= 6; byCount++)
				{
					*(byInfoStr + byCount) =  *(InputPacketBuff+(2+byCount));
				}
			}*/
		}
		else
		{
			//CONFIRMLOG(bySection, "Data", ("%s",  "BadData"), szLogPath, bLog);
		}
	}

	/*cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);
	CONFIRMLOG(bySection, "HexFrequency", ("%s", cszLoad), szLogPath, bLog);*/

	return bRet;
}

BOOL CFunctionalTestDlg::PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog)
{
	BOOL			bRet = TRUE;
	CString			cszLoad;
	UINT			nNumPass = 0;
	BYTE			byBufferCpy[MAX_PDU_BUFF_SIZE] = {0};
	TCHAR			bySection[MAX_PATH] = {0};
	HANDLE			hMyPDUHandle = INVALID_HANDLE_VALUE;

	if (bWrLog)
	{
		WriteLogDetails(STR_INIT_WRITE_CMD, _T("Initiating the Write Command..."));
	}

	/* -----------------------------------------------------------------------
		Create the WriteHandle to Send the Command.
	----------------------------------------------------------------------- */
	hMyPDUHandle = CreateFile(byPduAdd, 
							 GENERIC_READ | GENERIC_WRITE, 
							 FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, 
							 OPEN_EXISTING, 
							 0, 
							 NULL);

	if (hMyPDUHandle == INVALID_HANDLE_VALUE)
	{
		if (bWrLog)
		{
			WriteLogDetails(STR_DEVICE_HANDLE, _T("Unable to create device handle..."));
		}
		*byErrCode = 0x02;

		bRet = FALSE;
		return bRet;
	}
	else
	{
		if (bWrLog)
		{
			WriteLogDetails(STR_DEVICE_HANDLE, _T("Device handle created successfully..."));
		}
	}

	nNumPass = 0;

	memcpy(byBufferCpy, byDataBuffer, MAX_PDU_BUFF_SIZE);

	do
	{
		if (hMyPDUHandle != INVALID_HANDLE_VALUE)
		{
			WriteFile(hMyPDUHandle, byBufferCpy, MAX_PDU_BUFF_SIZE, dwRWBytes, 0);
			
			if (bWrLog)
			{
				cszLoad.Empty();
				cszLoad.Format(_T("DataWritten %d bytes"), *dwRWBytes);
				WriteLogDetails(STR_DEV_DATA_WRTN_BYTES, cszLoad);
			}
		}

		if (bWriteOnly)
		{
			break;
		}
		
		*dwRWBytes = 0;
		memset(byDataBuffer, 0, MAX_PDU_BUFF_SIZE);
		
		if (hMyPDUHandle != INVALID_HANDLE_VALUE)
		{
			bRet = ReadFile(hMyPDUHandle, byDataBuffer, MAX_PDU_BUFF_SIZE, dwRWBytes, 0);

			if (bWrLog)
			{
				cszLoad.Empty();
				cszLoad.Format(_T("DataRead %d bytes"), *dwRWBytes);
				WriteLogDetails(STR_DEV_DATA_READ_BYTES, cszLoad);
			}
		}

		nNumPass++;

		if ((byDataBuffer[1] == 0xFF) && (byDataBuffer[2] == 0xEE) && (nNumPass >= 3))
			break;

	} while((byDataBuffer[1] == 0xFF) && (byDataBuffer[2] == 0xEE));

	if (bWrLog)
	{
		WriteLogDetails(STR_CLOSE_DEV_HAND, _T("Closing device handle..."));
	}

	if (hMyPDUHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMyPDUHandle);
	}

	return bRet;

}

BOOL CFunctionalTestDlg::LoadVidPidValues()
{
	BOOL			bFoundOk = TRUE;
	UINT			nNum = 0;
	UINT			nTotal = 0;
	CString			cszLoad;
	CString			cszSection;
	CString			cszKey;
	CString			cszEpowerFile;
	TCHAR			szBuffer[MAX_PATH] = {0};

	m_cszPidValues.RemoveAll();
	m_cszVidValues.RemoveAll();

	cszEpowerFile.Empty();
	cszEpowerFile = m_cszIniFilePath;

	cszSection.Empty();
	cszSection = STR_DEVICE_DETAILS;

	nTotal = GetPrivateProfileInt(cszSection, STR_TOTAL, 0, cszEpowerFile);

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		// VID Values
		cszKey.Empty();
		cszKey.Format(STR_VID_NO, nNum);

		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cszVidValues.Add(cszLoad);

		// PID Values
		cszKey.Empty();
		cszKey.Format(STR_PID_NO, nNum);

		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cszPidValues.Add(cszLoad);
	}

	if (!nTotal)
	{
		bFoundOk = FALSE;
	}

	return bFoundOk;
}

BOOL CFunctionalTestDlg::PduEnumerateDevice(BOOL bWrLog)
{
	BOOL										bPduFound = FALSE;
	TCHAR										byDevPath[MAX_PATH] = {0};
	BOOL										bPidVidOk = FALSE;
	ULONG										Length = 0;
	UINT										nNum = 0;
	int											MemberIndex = 0;
	HIDD_ATTRIBUTES								Attributes;
	SP_DEVICE_INTERFACE_DATA					devInfoData;
	HANDLE										hDevInfo = INVALID_HANDLE_VALUE;
	LONG										Result = 0;	
	PSP_DEVICE_INTERFACE_DETAIL_DATA			detailData = NULL;
	GUID										HidGuid;
	ULONG										Required = 0;
	CString										cszLoad;
	CString										cszPid;
	CString										cszVid;
	HANDLE										DeviceHandle = INVALID_HANDLE_VALUE;

/* -----------------------------------------------------------------------
	API function: HidD_GetHidGuid	Get the GUID for all system HIDs. Returns: the GUID in HidGuid.
	----------------------------------------------------------------------- */

	HidD_GetHidGuid(&HidGuid);	
	
/* -----------------------------------------------------------------------
	API function: SetupDiGetClassDevs
	Returns: a handle to a device information set for all installed devices.
	Requires: the GUID returned by GetHidGuid.
	----------------------------------------------------------------------- */
	
	hDevInfo = SetupDiGetClassDevs(&HidGuid, 
									NULL, 
									NULL, 
									DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

	if (hDevInfo == INVALID_HANDLE_VALUE)
		return 0;

	devInfoData.cbSize = sizeof(devInfoData);

/* -----------------------------------------------------------------------
	API function: SetupDiEnumDeviceInterfaces
	On return, MyDeviceInterfaceData contains the handle to a
	SP_DEVICE_INTERFACE_DATA structure for a detected device.

	Requires:
	The DeviceInfoSet returned in SetupDiGetClassDevs.
	The HidGuid returned in GetHidGuid.
	An index to specify a device.
	----------------------------------------------------------------------- */

	if (bWrLog)
	{
		WriteLogDetails(STR_DEVICE_DETECT, _T("Detecting PDU device..."));
	}

	MemberIndex = 0;

	for(MemberIndex = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData); ++MemberIndex)
	{						
		//A device has been detected, so get more information about it.

	/* -----------------------------------------------------------------------
		API function: SetupDiGetDeviceInterfaceDetail
		Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
		containing information about a device.
		To retrieve the information, call this function twice.
		The first time returns the size of the structure in Length.
		The second time returns a pointer to the data in DeviceInfoSet.
		Requires:
		A DeviceInfoSet returned by SetupDiGetClassDevs
		The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
		
		The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
		This application doesn't retrieve or use the structure.			
		If retrieving the structure, set 
		MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
		and pass the structure's address.
		----------------------------------------------------------------------- */
		
		//Get the Length value.
		//The call will return with a "buffer too small" error which can be ignored.

		Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);

		//Allocate memory for the hDevInfo structure, using the returned Length.

		detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
		
		//Set cbSize in the detailData structure.

		detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		//Call the function again, this time passing it the returned buffer size.

		Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, 
												  &devInfoData, 
												  detailData, 
												  Length, 
												  &Required, 
												  NULL);

		// Open a handle to the device.
		// To enable retrieving information about a system mouse or keyboard,
		// don't request Read or Write access for this handle.

 	/* -----------------------------------------------------------------------
		API function: CreateFile
		Returns: a handle that enables reading and writing to the device.

		Requires:
		The DevicePath in the detailData structure
		returned by SetupDiGetDeviceInterfaceDetail.
		----------------------------------------------------------------------- */
		if (bWrLog)
		{
			WriteLogDetails(STR_DEVICE_PATH, detailData->DevicePath);
		}

		DeviceHandle = CreateFile(detailData->DevicePath, 
									0, 
									FILE_SHARE_READ|FILE_SHARE_WRITE, 
									(LPSECURITY_ATTRIBUTES)NULL,
									OPEN_EXISTING, 
									0, 
									NULL);

		if (DeviceHandle != INVALID_HANDLE_VALUE)
		{
			if (bWrLog)
			{
				WriteLogDetails(STR_DEVICE_HANDLE, _T("Device handle created successfully..."));
			}
		}
		else
		{
			if (bWrLog)
			{
				WriteLogDetails(STR_DEVICE_HANDLE, _T("Unable to create device handle..."));
			}
			continue;
		}

 	/* -----------------------------------------------------------------------
		API function: HidD_GetAttributes
		Requests information from the device.
		Requires: the handle returned by CreateFile.
		Returns: a HIDD_ATTRIBUTES structure containing
		the Vendor ID, Product ID, and Product Version Number.
		Use this information to decide if the detected device is
		the one we're looking for.
		----------------------------------------------------------------------- */

		//Set the Size to the number of bytes in the structure.

		Attributes.Size = sizeof(Attributes);

		Result = HidD_GetAttributes(DeviceHandle, &Attributes);

		if (bWrLog)
		{
			cszLoad.Empty();
			cszLoad.Format(_T("0x%04X"), Attributes.VendorID);
			WriteLogDetails(STR_DEVICE_VID, cszLoad);

			cszLoad.Empty();
			cszLoad.Format(_T("0x%04X"), Attributes.ProductID);
			WriteLogDetails(STR_DEVICE_PID, cszLoad);
		}

		bPidVidOk = FALSE;
		for (nNum = 0; nNum < (UINT)m_cszVidValues.GetSize(); nNum++)
		{
			cszPid.Empty();
			cszPid.Format(_T("0x%04X"), Attributes.ProductID);

			cszVid.Empty();
			cszVid.Format(_T("0x%04X"), Attributes.VendorID);

			if ((!m_cszVidValues.ElementAt(nNum).CompareNoCase(cszVid)) && (!m_cszPidValues.ElementAt(nNum).CompareNoCase(cszPid)))
			{
				bPidVidOk = TRUE;
			}
		}

		if (bPidVidOk) //((Attributes.VendorID == wVid) && (Attributes.ProductID == wPid))
		{
			//Both the Vendor ID and Product ID match.
			if (bWrLog)
			{
				WriteLogDetails(STR_PID_VID, _T("Found"));
			}

		/* -----------------------------------------------------------------------
			Device Path is needed to create the device handle.
			----------------------------------------------------------------------- */
			m_cszPDUAddr.Empty();
			m_cszPDUAddr = detailData->DevicePath;

			//memset(byDevPath, PDU_BUFF_NULL, sizeof(byDevPath));
			//lstrcpy(byDevPath, detailData->DevicePath);
			//GetDeviceCapabilities(byDevPath, bLog, DeviceHandle);

			if (bWrLog)
			{
				WriteLogDetails(STR_CLOSE_DEV_HAND, _T("Closing device handle..."));
			}

			CloseHandle(DeviceHandle);
			DeviceHandle = INVALID_HANDLE_VALUE;

			bPduFound = TRUE;

			break;		// Exit the loop after proper device detection...
	 	} 
		else
		{
			bPduFound = FALSE;
			//The Vendor ID doesn't match.

			if (bWrLog)
			{
				WriteLogDetails(STR_PID_VID, _T("Mismatch"));
				WriteLogDetails(STR_CLOSE_DEV_HAND, _T("Closing device handle..."));
			}

			CloseHandle(DeviceHandle);
			DeviceHandle = INVALID_HANDLE_VALUE;
		}

		//Free the memory used by the detailData structure (no longer needed).
		free(detailData);

	} //for loop

	if (DeviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(DeviceHandle);
		DeviceHandle = INVALID_HANDLE_VALUE;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bPduFound;
}

BOOL CFunctionalTestDlg::WriteLogDetails(CString cszSection, CString cszData)
{
	BOOL			bRet = TRUE;

	if ((!m_cszDevLogFile.IsEmpty()) && (!cszSection.IsEmpty()) && (!cszData.IsEmpty()))
	{
		WritePrivateProfileString(STR_PDU_LOG, cszSection, cszData, m_cszDevLogFile);
	}

	return bRet;
}

BOOL CFunctionalTestDlg::PduSwitchOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE byCurrState, DWORD dwDelaySecs, BYTE *byErrCode, BOOL bLog)
{
	BOOL									bRet = FALSE;
	BOOL									bWriteMode = FALSE;
	BYTE									byType = 0;
	CHAR									bySection[MAX_PATH] = {0};
	CHAR									byLocalBuf[MAX_PATH] = {0};
	BYTE									byOCBStateBuff[MAX_PDU_BUFF_SIZE] = {0};		//Allocate a memory buffer equal to our endpoint size + 1
	DWORD									dwByteWritten = 0;
	CString									szTmpstring;


	/* -----------------------------------------------------------------------
		Make Section name for the Log.
	----------------------------------------------------------------------- */
	
	memcpy(byLocalBuf, byPduadd, strlen(byPduadd));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_OCBSTATE, szTmpstring, byOCB, byOutlet);

	/* -----------------------------------------------------------------------
		Send the Pass through Command to devcie
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	memset(byOCBStateBuff, 0, sizeof(byOCBStateBuff));
	byOCBStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	byOCBStateBuff[1] = RELAY_STATE;
	byOCBStateBuff[2] = byOCB;
	byOCBStateBuff[3] = byOutlet;
	byOCBStateBuff[4] = byCurrState;
	byOCBStateBuff[5] = 0x01;


	Sleep(dwDelaySecs);

	if (byCurrState == RELAY_REBOOT)
	{
		bWriteMode = TRUE;
	}

	PduWriteReadBuffer(byPduadd, byOCBStateBuff, &dwByteWritten, bWriteMode, byErrCode, bLog);

	if ((byOCBStateBuff[1] == byOutlet) && (byOCBStateBuff[2] == byCurrState))
	{
		bRet = TRUE;
		WriteLogDetails(bySection, _T("OCB STATE CHANGED -- PASS"));
	}
	else 
	{
		bRet = FALSE;
		WriteLogDetails(bySection, _T("OCB STATE CHANGED -- FAIL"));
	}

	return bRet;
}

BOOL CFunctionalTestDlg::PduGetOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog)
{
	BOOL								bRet = FALSE;
	CHAR								bySection[MAX_PATH] = {0};
	CHAR								byLocalBuf[MAX_PATH] = {0};
	BYTE								byStateBuff[MAX_PDU_BUFF_SIZE] = {0};		
	DWORD								dwByteWritten = 0;
	CString								szTmpstring;

	/* -----------------------------------------------------------------------
		Make Section name for the Log.
	----------------------------------------------------------------------- */
	
	memcpy(byLocalBuf, byPduadd, strlen(byPduadd));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_OUTLET_STATE, szTmpstring, byOCB, byOutlet);

	/* -----------------------------------------------------------------------
		Send the Pass through Command to devcie
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	memset(byStateBuff, 0, sizeof(byStateBuff));
	byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	byStateBuff[1] = RELAY_STATE;
	byStateBuff[2] = byOCB;
	byStateBuff[3] = byOutlet;
	byStateBuff[4] = RELAY_STATUS;
	
	PduWriteReadBuffer(byPduadd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);

	if (byStateBuff[1] == byOutlet)
	{
		bRet = TRUE;
		*byStateValue = byStateBuff[2];

		WriteLogDetails(bySection, _T("GET OUTLET STATE -- PASS"));
	}
	else if ((byStateBuff[1] == PDU_BUFF_END) && (byStateBuff[2] == PDU_BUFF_ERR))
	{
		bRet = FALSE;
		*byStateValue = RELAY_UNKNOWN;
		WriteLogDetails(bySection, _T("GET OUTLET STATE -- FAIL"));
	}

	return bRet;
}

CString CFunctionalTestDlg::GetLogPathWithDate(CString cszLogPath)
{
	CString			cszLoad;
	CString			cszValue;
	CTime			cTm(CTime::GetCurrentTime());

	cszValue = cszLogPath;

	cszLoad.Empty();
	cszLoad.Format(_T("%04d%02d%02d"), cTm.GetYear(), cTm.GetMonth(), cTm.GetDay());
	cszValue += cszLoad;

	return cszValue;
}

BOOL CFunctionalTestDlg::ReadSettingsFile()
{
	int				nValue = 0;
	TCHAR			szBuffer[1024] = {0};

	memset(&m_stPduSettings, 0, sizeof(PDU_SETTINGS));

	if (m_cszIniFilePath.IsEmpty())
		return FALSE;

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
		m_cszLogFolderPath = m_cszLogFilePath;
	}

	return TRUE;
}
void CFunctionalTestDlg::OnNMCustomdrawListFuncTestLog(NMHDR *pNMHDR, LRESULT *pResult)
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

BOOL CFunctionalTestDlg::PrintBufferInTestInfoList(CString cszBuffer, int nTxtColor)
{
	int					nRow = 0;
	LVITEM				lvItem = {0};

	nRow = m_cLstFuncTestLog.GetItemCount();

	memset(&lvItem, 0, sizeof(LVITEM));
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = nRow;
	lvItem.pszText = cszBuffer.GetBuffer(cszBuffer.GetLength());
	cszBuffer.ReleaseBuffer();
	lvItem.cchTextMax = cszBuffer.GetLength();
	lvItem.iSubItem = 0;
	lvItem.lParam = nTxtColor;
	nRow = m_cLstFuncTestLog.InsertItem(&lvItem);

	if (nRow == MINUS_ONE)
		return FALSE;

	nRow = m_cLstFuncTestLog.GetItemCount();
	if (nRow > 0)
	{
		m_cLstFuncTestLog.EnsureVisible((nRow - 1), FALSE);
	}
	
	return TRUE;
}

HBRUSH CFunctionalTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PDU_STATUS)
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
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}
