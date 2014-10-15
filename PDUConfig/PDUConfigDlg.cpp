// PDUConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PDUConfigDlg.h"
#include <wtypes.h>
#include <initguid.h>

extern "C" {

#include "hidsdi.h"
#include <setupapi.h>
#include <dbt.h>
}

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

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CPDUConfigDlg dialog


CPDUConfigDlg::CPDUConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPDUConfigDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pPageNetwork = NULL;
	m_pPageSNMP = NULL;
	m_pPageEmail = NULL;
	m_pPageGeneral = NULL;
	m_pPagePDU = NULL;
	m_pPageAlert = NULL;
	m_pPageSNMPVer = NULL;

	m_nComboSel = -1;
	m_nFrontPage = 0;
	cszBasePath.Empty();
	cszCurrPath.Empty();
	m_cszPDUNm.RemoveAll();
	m_cszPDUList.RemoveAll();
	memset(&m_stPDUConfigInfo, 0, sizeof(PDU_CONFIG_INFO));

	m_cszPidValues.RemoveAll();
	m_cszVidValues.RemoveAll();

	m_byMaxOutlets = 0;
}

CPDUConfigDlg::~CPDUConfigDlg()
{
	if (m_pPageNetwork)
	{
		m_pPageNetwork->DestroyWindow();
		delete m_pPageNetwork;
		m_pPageNetwork = NULL;
	}

	if (m_pPageSNMP)
	{
		m_pPageSNMP->DestroyWindow();
		delete m_pPageSNMP;
		m_pPageSNMP = NULL;
	}

	if (m_pPageEmail)
	{
		m_pPageEmail->DestroyWindow();
		delete m_pPageEmail;
		m_pPageEmail = NULL;
	}

	if (m_pPageGeneral)
	{
		m_pPageGeneral->DestroyWindow();
		delete m_pPageGeneral;
		m_pPageGeneral = NULL;
	}

	if (m_pPagePDU)
	{
		m_pPagePDU->DestroyWindow();
		delete m_pPagePDU;
		m_pPagePDU = NULL;
	}

	if (m_pPageAlert)
	{
		m_pPageAlert->DestroyWindow();
		delete m_pPageAlert;
		m_pPageAlert = NULL;
	}

	if (m_pPageSNMPVer)
	{
		m_pPageSNMPVer->DestroyWindow();
		delete m_pPageSNMPVer;
		m_pPageSNMPVer = NULL;
	}
}

void CPDUConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_PDU_CONFIG, m_cTabConfig);
	DDX_Control(pDX, IDC_COMBO_PM10_PDU, m_cComboPDU);
	DDX_Control(pDX, IDOK, m_cButWrite);
}

BEGIN_MESSAGE_MAP(CPDUConfigDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PDU_CONFIG, &CPDUConfigDlg::OnConfigTabChanged)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB_PDU_CONFIG, &CPDUConfigDlg::OnConfigTabChanging)
	ON_CBN_SELCHANGE(IDC_COMBO_PM10_PDU, &CPDUConfigDlg::OnCbnSelchangeComboPm10Pdu)
	ON_BN_CLICKED(IDOK, &CPDUConfigDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FAC_RESET, &CPDUConfigDlg::OnBnClickedFctryReset)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_ALL, &CPDUConfigDlg::OnBnClickedClearAll)
END_MESSAGE_MAP()


// CPDUConfigDlg message handlers

BOOL CPDUConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UINT			nNum = 0;
	TCITEM			tcItem = {0};
	TCHAR			szCurrPath[MAX_PATH] = {0};
	CString			cszLoad;
	CFileFind		cFinder;

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
	InitiateTimeZoneValues();

	cszCurrPath.Empty();
	GetModuleFileName(NULL, szCurrPath, sizeof(szCurrPath));
	*(strrchr(szCurrPath, '\\') + 1) = '\0';
	cszCurrPath = szCurrPath;

	cszBasePath.Empty();
	cszBasePath = cszCurrPath;

	cszCurrPath += STR_LOG_FILE_NM;
	if (cFinder.FindFile(cszCurrPath))
	{
		cFinder.Close();
		::DeleteFile(cszCurrPath);
	}

	cszLoad.Empty();
	cszLoad.LoadString(AFX_IDS_APP_TITLE);
	SetWindowText(cszLoad);

	m_pPageNetwork = new CPageNetwork(this);
	m_pPageSNMP = new CPageSNMP(this);
	m_pPageSNMPVer = new CDlgSNMPVer(this);
	m_pPageEmail = new CPageEmail(this);
	m_pPageGeneral = new CPageGeneral(this);
	m_pPagePDU = new CPagePDU(this);
	m_pPageAlert =  new CPageAlert(this);
	
	m_pPageNetwork->Create(IDD_DLG_NETWORK, &m_cTabConfig);
	m_pPageSNMP->Create(IDD_DLG_SNMP, &m_cTabConfig);
	m_pPageSNMPVer->Create(IDD_DLG_SNMP_VER, &m_cTabConfig); 
	m_pPageEmail->Create(IDD_DLG_EMAIL, &m_cTabConfig);
	m_pPageGeneral->Create(IDD_DLG_GENERAL, &m_cTabConfig);
	m_pPagePDU->Create(IDD_DLG_PDU, &m_cTabConfig);
	m_pPageAlert->Create(IDD_DLG_ALERT, &m_cTabConfig);
	
	tcItem.mask = TCIF_TEXT;
	tcItem.iImage = -1;

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_NETWORK_SETTING_TITLE);
	tcItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cTabConfig.InsertItem(ID_PAGE_NETWORK, &tcItem);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SNMP_SETTING_TITLE);
	tcItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cTabConfig.InsertItem(ID_PAGE_SNMP, &tcItem);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SNMP_SETTINGS);
	tcItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cTabConfig.InsertItem(ID_PAGE_SNMP_VERSION, &tcItem);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_EMAIL_SETTING_TITLE);
	tcItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cTabConfig.InsertItem(ID_PAGE_EMAIL, &tcItem);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_GENERAL_SETTING_TITLE);
	tcItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cTabConfig.InsertItem(ID_PAGE_GENERAL, &tcItem);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_PDU_SETTING_TITLE);
	tcItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cTabConfig.InsertItem(ID_PAGE_PDU_SETTING, &tcItem);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_ALERT_SETTING_TITLE);
	tcItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cTabConfig.InsertItem(ID_PAGE_ALERT_SETTING, &tcItem);

	OnConfigTabChanged(NULL, NULL);

	if (!LoadVidPidValues())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_EPHD_UNIT_ERR_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return TRUE;
	}

	PduEnumerateDevice(TRUE);
	if (m_cszPDUList.GetSize() > 0)
	{
		GeneratePDUNames();

		for (nNum = 0; nNum < (UINT)m_cszPDUNm.GetSize(); nNum++)
		{
			m_cComboPDU.AddString(m_cszPDUNm.ElementAt(nNum));
		}

		m_cComboPDU.SetCurSel(0);
		OnCbnSelchangeComboPm10Pdu();
	}
	else
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_PDU_NOT_FOUND_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(0);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPDUConfigDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPDUConfigDlg::OnPaint()
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
HCURSOR CPDUConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPDUConfigDlg::OnConfigTabChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	RECT			rc = {0};

	m_cTabConfig.GetItemRect(0, &rc);

	m_nFrontPage = m_cTabConfig.GetCurSel();

	switch(m_nFrontPage)
	{
		case ID_PAGE_NETWORK:
		{
			m_pPageNetwork->SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			m_pPageNetwork->SetFocus();
		}
		break;

		case ID_PAGE_SNMP:
		{
			m_pPageSNMP->SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			m_pPageSNMP->SetFocus();
		}
		break;

		case ID_PAGE_SNMP_VERSION:
		{
			m_pPageSNMPVer->SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			m_pPageSNMPVer->SetFocus();
		}
		break;

		case ID_PAGE_EMAIL:
		{
			m_pPageEmail->SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			m_pPageEmail->SetFocus();
		}
		break;

		case ID_PAGE_GENERAL:
		{
			m_pPageGeneral->SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			m_pPageGeneral->SetFocus();
		}
		break;

		case ID_PAGE_PDU_SETTING:
		{
			m_pPagePDU->SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			m_pPagePDU->SetFocus();
		}
		break;

		case ID_PAGE_ALERT_SETTING:
		{
			m_pPageAlert->SetWindowPos(NULL, rc.left, rc.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			m_pPageAlert->SetFocus();
		}
		break;
	}

	if (pResult)
		*pResult = 0;
}

void CPDUConfigDlg::OnConfigTabChanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	m_nFrontPage = m_cTabConfig.GetCurSel();

	switch(m_nFrontPage)
	{
		case ID_PAGE_NETWORK:
		{
			m_pPageNetwork->ShowWindow(SW_HIDE);
		}
		break;

		case ID_PAGE_SNMP:
		{
			m_pPageSNMP->ShowWindow(SW_HIDE);
		}
		break;

		case ID_PAGE_SNMP_VERSION:
		{
			m_pPageSNMPVer->ShowWindow(SW_HIDE);
		}
		break;

		case ID_PAGE_EMAIL:
		{
			m_pPageEmail->ShowWindow(SW_HIDE);
		}
		break;

		case ID_PAGE_GENERAL:
		{
			m_pPageGeneral->ShowWindow(SW_HIDE);
		}
		break;

		case ID_PAGE_PDU_SETTING:
		{
			m_pPagePDU->ShowWindow(SW_HIDE);
		}
		break;

		case ID_PAGE_ALERT_SETTING:
		{
			m_pPageAlert->ShowWindow(SW_HIDE);
		}
		break;
	}

	*pResult = 0;
}

BOOL CPDUConfigDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CPDUConfigDlg::PduStaticInfoWriteAndRead(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum)
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
		if (byInfoID <= PDU_MAC_ADDRS)
		{
			InputPacketBuff[1] = PDU_STATIC_WRITE;
		}
		else
		{
			InputPacketBuff[1] = PDU_CONFIG_WRITE;
		}
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
		if ((byInfoID <= PDU_MAC_ADDRS) || (byInfoID == PDU_TOTAL_OUTLETS))
		{
			InputPacketBuff[1] = PDU_STATIC_READ;		//for Read Static Information
		}
		else
		{
			InputPacketBuff[1] = PDU_CONFIG_READ;
		}

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

BOOL CPDUConfigDlg::PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog)
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

void CPDUConfigDlg::PduEnumerateDevice(BOOL bWrLog)
{
	TCHAR										byDevPath[MAX_PATH] = {0};
	ULONG										Length = 0;
	int											MemberIndex = 0;
	UINT										nNum = 0;
	BOOL										bPidVidOk = FALSE;
	HIDD_ATTRIBUTES								Attributes;
	SP_DEVICE_INTERFACE_DATA					devInfoData;
	HANDLE										hDevInfo = INVALID_HANDLE_VALUE;
	LONG										Result = 0;	
	PSP_DEVICE_INTERFACE_DETAIL_DATA			detailData = NULL;
	GUID										HidGuid;
	ULONG										Required = 0;
	WORD										wVid = 0;
	WORD										wPid = 0;
	CString										cszLoad;
	CString										cszPDUAddr;
	CString										cszPid;
	CString										cszVid;
	HANDLE										DeviceHandle = INVALID_HANDLE_VALUE;


	m_cszPDUList.RemoveAll();

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
		return;

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
		API function: HidD_GetAttributes Requests information from the device.
		Requires: the handle returned by CreateFile. 
		Returns: a HIDD_ATTRIBUTES structure containing	the Vendor ID, Product ID, 
		and Product Version Number.	Use this information to decide if the detected 
		device is the one we're looking for.
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

		//wVid = 0x04d8;
		//wPid = 0x003F;
		//wVid = 0x55AA;
		//wPid = 0x8000;

		if (bPidVidOk)		//if ((Attributes.VendorID == wVid) && (Attributes.ProductID == wPid))
		{
			//Both the Vendor ID and Product ID match.
			if (bWrLog)
			{
				WriteLogDetails(STR_PID_VID, _T("Found"));
			}

		/* -----------------------------------------------------------------------
			Device Path is needed to create the device handle.
			----------------------------------------------------------------------- */
			cszPDUAddr.Empty();
			cszPDUAddr = detailData->DevicePath;
			m_cszPDUList.Add(cszPDUAddr);

			//memset(byDevPath, PDU_BUFF_NULL, sizeof(byDevPath));
			//lstrcpy(byDevPath, detailData->DevicePath);
			//GetDeviceCapabilities(byDevPath, bLog, DeviceHandle);

			if (bWrLog)
			{
				WriteLogDetails(STR_CLOSE_DEV_HAND, _T("Closing device handle..."));
			}

			CloseHandle(DeviceHandle);
			DeviceHandle = INVALID_HANDLE_VALUE;
	 	} 
		else
		{
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
}

BOOL CPDUConfigDlg::WriteLogDetails(CString cszSection, CString cszData)
{
	BOOL			bRet = TRUE;

	if ((!cszCurrPath.IsEmpty()) && (!cszSection.IsEmpty()) && (!cszData.IsEmpty()))
	{
		WritePrivateProfileString(STR_PDU_LOG, cszSection, cszData, cszCurrPath);
	}

	return bRet;
}

BOOL CPDUConfigDlg::GeneratePDUNames()
{
	int					nNum = 0;
	BYTE				byErrCode = 0;
	BOOL				bRet = FALSE;
	CString				cszPDUAddress;
	CString				cszPDUNm;
	CString				cszLoad;
	BYTE				byDataBuffer[66] = {0};

	for (nNum = 0; nNum < m_cszPDUList.GetSize(); nNum++)
	{
		cszPDUAddress.Empty();
		cszPDUAddress = m_cszPDUList.ElementAt(nNum);

		cszPDUNm.Empty();

		// -------------------- READ MODEL NUMBER -----------------------------
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_MODEL_NUM, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		cszLoad.Empty();
		cszLoad.Format(_T("%s"), byDataBuffer);

		cszPDUNm = cszLoad;
		cszPDUNm += STR_PDU_MDNM_SRNM_SEP;

		// -------------------- READ SERIAL NUMBER --------------------------
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SERIAL_NUM, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		cszLoad.Empty();
		cszLoad.Format(_T("%s"), byDataBuffer);
		cszPDUNm += cszLoad;

		m_cszPDUNm.Add(cszPDUNm);
	}

	return bRet;
}

void CPDUConfigDlg::OnCbnSelchangeComboPm10Pdu()
{
	// TODO: Add your control notification handler code here
	if (m_cComboPDU.GetCurSel() != m_nComboSel)
	{
		m_nComboSel = m_cComboPDU.GetCurSel();
		if (m_nComboSel != CB_ERR)
		{
			m_byMaxOutlets = 0;
			ReadPDUSettingsInfo(m_nComboSel);
			UpdateInfoInAllPages(m_nFrontPage);
		}
	}
}

BOOL CPDUConfigDlg::ReadPDUSettingsInfo(UINT nPDUNum)
{
	typedef union 
	{
		INT32	nValue;
		float	fValue;
	}IEE_CONV;

	typedef union 
	{
		BYTE	byValue[4];
		float	fAnswer;
	}Float_Conv;

	BOOL				bRet = TRUE;
	UINT				nNum = 0;
	BYTE				byErrCode = 0;
	BYTE				byDataBuffer[66] = {0};
	CString				cszPDUAddress;
	CString				cszLoad;
	IEE_CONV			stIeeConv = {0};
	Float_Conv			stFloatConv = {0};
	NTP_SETTINGS_INFO	stNtpSettings = {0};
	SNMPV3_SETTINGS		stSNMP_V3_Settings = {0};

	cszPDUAddress.Empty();

	if (nPDUNum < (UINT)m_cszPDUList.GetCount())
	{
		cszPDUAddress = m_cszPDUList.ElementAt(nPDUNum);
	}

	memset(&m_stPDUConfigInfo, 0, sizeof(PDU_CONFIG_INFO));

	if (cszPDUAddress.IsEmpty())
		return FALSE;

	// ------------------------ NETWORK SETTINGS ------------------------------------------------------------------------
			
	// ************** READ NETWORK ENBABLE DHCP **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_ENB_DHCP, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.byEnbDHCP = byDataBuffer[0];

	// ************** READ NETWORK IP ADDRESS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_IP_ADDRS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.byIPField[0] = byDataBuffer[0];
	m_stPDUConfigInfo.byIPField[1] = byDataBuffer[1];
	m_stPDUConfigInfo.byIPField[2] = byDataBuffer[2];
	m_stPDUConfigInfo.byIPField[3] = byDataBuffer[3];

	// ************** READ SUBNET MASK **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_SUBNET_MSK, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.bySMField[0] = byDataBuffer[0];
	m_stPDUConfigInfo.bySMField[1] = byDataBuffer[1];
	m_stPDUConfigInfo.bySMField[2] = byDataBuffer[2];
	m_stPDUConfigInfo.bySMField[3] = byDataBuffer[3];

	// ************** READ DEFAULT GATEWAY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_DEF_GATEWAY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.byGWField[0] = byDataBuffer[0];
	m_stPDUConfigInfo.byGWField[1] = byDataBuffer[1];
	m_stPDUConfigInfo.byGWField[2] = byDataBuffer[2];
	m_stPDUConfigInfo.byGWField[3] = byDataBuffer[3];

	// ************** READ PRIMARY DNS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_PRIM_DNS_ADDRS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.byPDNSField[0] = byDataBuffer[0];
	m_stPDUConfigInfo.byPDNSField[1] = byDataBuffer[1];
	m_stPDUConfigInfo.byPDNSField[2] = byDataBuffer[2];
	m_stPDUConfigInfo.byPDNSField[3] = byDataBuffer[3];

	// ************** READ SECONDARY DNS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_SECN_DNS_ADDRS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.bySDNSField[0] = byDataBuffer[0];
	m_stPDUConfigInfo.bySDNSField[1] = byDataBuffer[1];
	m_stPDUConfigInfo.bySDNSField[2] = byDataBuffer[2];
	m_stPDUConfigInfo.bySDNSField[3] = byDataBuffer[3];

	// ------------------------ SNMP SETTINGS ---------------------------------------------------------------------------------------------

	// ************** READ READ COMMUNITY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_READ_COMMUNITY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.bySNMPReadCmn, byDataBuffer, sizeof(m_stPDUConfigInfo.bySNMPReadCmn));

	// ************** READ WRITE COMMUNITY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_WRITE_COMMUNITY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.bySNMPWriteCmn, byDataBuffer, sizeof(m_stPDUConfigInfo.bySNMPWriteCmn));

	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_V3_SETTINGS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(&stSNMP_V3_Settings, byDataBuffer, sizeof(SNMPV3_SETTINGS));
	m_stPDUConfigInfo.bySNMPV3_Enabled = stSNMP_V3_Settings.byEnabled;
	m_stPDUConfigInfo.byAuthLevel = stSNMP_V3_Settings.byAuthLevel;
	memcpy(&m_stPDUConfigInfo.bySNMPV3_UserName, stSNMP_V3_Settings.byUserName, sizeof(stSNMP_V3_Settings.byUserName));
	memcpy(&m_stPDUConfigInfo.byAuthPassword, stSNMP_V3_Settings.byAuthPassword, sizeof(stSNMP_V3_Settings.byAuthPassword));
	m_stPDUConfigInfo.byAuthHashType = stSNMP_V3_Settings.byAuthHashType;
	memcpy(&m_stPDUConfigInfo.byPrivPassword, stSNMP_V3_Settings.byPrivPassword, sizeof(stSNMP_V3_Settings.byPrivPassword));
	m_stPDUConfigInfo.byPrivType = stSNMP_V3_Settings.byPrivType;


	// ------------------------ OTHER SETTINGS ---------------------------------------------------------------------------------------------

	// ************** READ SNMP TRAP ENABLE STATUS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_ENB_SNMP_TRAP, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.byEnbSNMPTrap = byDataBuffer[0];

	// ************** READ SNMP RECEIVER IP ADDRESS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_RECEVR_IP_ADDRS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.bySNMPRecevrField[0] = byDataBuffer[0];
	m_stPDUConfigInfo.bySNMPRecevrField[1] = byDataBuffer[1];
	m_stPDUConfigInfo.bySNMPRecevrField[2] = byDataBuffer[2];
	m_stPDUConfigInfo.bySNMPRecevrField[3] = byDataBuffer[3];

	// ************** READ SNMP TRAP PORT NUM STATUS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_RECEVR_PORT_NUM, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.bySNMPTrapPortNum = byDataBuffer[0];

	////////// NTP SETTINGS
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NTP_SETTINGS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	//memcpy(&stNtpSettings, byDataBuffer, sizeof(NTP_SETTINGS_INFO));
	stNtpSettings.bEnabled = byDataBuffer[0];
	stFloatConv.byValue[0] = byDataBuffer[1];
	stFloatConv.byValue[1] = byDataBuffer[2];
	stFloatConv.byValue[2] = byDataBuffer[3];
	stFloatConv.byValue[3] = byDataBuffer[4];
	stNtpSettings.fNtpZone = stFloatConv.fAnswer;
	m_stPDUConfigInfo.bNtpEnabled = stNtpSettings.bEnabled;
	m_stPDUConfigInfo.fNtpZone = stNtpSettings.fNtpZone;

	/*if (!m_stPDUConfigInfo.bNtpEnabled)
	{*/
		// NEED TO READ THE DATE AND TIME TO SET IT MANUALLY
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NTP_DATE_TIME, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();
		memcpy(m_stPDUConfigInfo.byTimeZoneDateTime, byDataBuffer, sizeof(m_stPDUConfigInfo.byTimeZoneDateTime));
	/*}*/


	// ------------------------ EMAIL SETTINGS --------------------------------------------------------------------------------------------

	// ************** READ EMAIL NOTIFICATION ENABLE STATUS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_ENB_EMAIL_NOTIFY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.byEnbEmailNotify = byDataBuffer[0];

	// ************** READ SMTP SERVER ADDRESS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_SMTP_SERVER_ADDRS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.bySMTPServerAddrs, byDataBuffer, sizeof(m_stPDUConfigInfo.bySMTPServerAddrs));

	// ************** READ SMTP PORT NUMBER **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_SMTP_PORT_NUM, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	//memcpy(m_stPDUConfigInfo.bySMTPPortNum, byDataBuffer, sizeof(m_stPDUConfigInfo.bySMTPPortNum));
	m_stPDUConfigInfo.bySMTPPortNum[0] = byDataBuffer[0];
	m_stPDUConfigInfo.bySMTPPortNum[1] = byDataBuffer[1];
	m_stPDUConfigInfo.bySMTPPortNum[2] = byDataBuffer[2];
	m_stPDUConfigInfo.bySMTPPortNum[3] = byDataBuffer[3];

	// ************** READ USER NAME FOR SENDER EMAIL DETAILS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_USER_NAME, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.byFrmUserName, byDataBuffer, sizeof(m_stPDUConfigInfo.byFrmUserName));

	// ************** READ PASSWORD FOR SENDER EMAIL DETAILS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_PASSWORD, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.byFrmPassword, byDataBuffer, sizeof(m_stPDUConfigInfo.byFrmPassword));

	// ************** READ RECEIVER EMAIL ID **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_RECEVR_EMAIL_ID, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.byRecevrEmailAddrs, byDataBuffer, sizeof(m_stPDUConfigInfo.byRecevrEmailAddrs));


	// ------------------------ GENERAL SETTINGS ------------------------------------------------------------------------------------------

	// ************** READ PDU SYSTEM NAME **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GENERAL_SYSTEM_NAME, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.byGeneralSysNm, byDataBuffer, sizeof(m_stPDUConfigInfo.byGeneralSysNm));

	// ************** READ GENERAL SYSTEM LOCATION **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GENERAL_SYSTEM_LOCATE, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.byGeneralSysLoc, byDataBuffer, sizeof(m_stPDUConfigInfo.byGeneralSysLoc));

	// ************** READ GENERAL SYSTEM CONTACT **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GENERAL_SYSTEM_CONTACT, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	memcpy(m_stPDUConfigInfo.byGeneralSysCont, byDataBuffer, sizeof(m_stPDUConfigInfo.byGeneralSysCont));

	// ************** READ GENERAL STAGGER ON DELAY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_STAGGER_ON_DELAY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	cszLoad.Empty();
	cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
	m_stPDUConfigInfo.dwStaggerOnDelay = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	// ************** READ GENERAL STAGGER OFF DELAY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_STAGGER_OFF_DELAY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	cszLoad.Empty();
	cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
	m_stPDUConfigInfo.dwStaggerOffDelay = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	//	typedef struct
	//{
	//	BYTE	bTelnetEnabled;
	//	BYTE	bHTTPEnabled;
	//	BYTE	bHTTPSEnabled;
	//}NET_SETTINGS;
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NET_SETTINGS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_stPDUConfigInfo.byTelnetEnabled = byDataBuffer[0];
	m_stPDUConfigInfo.byHTTPEnabled = byDataBuffer[1];
	m_stPDUConfigInfo.byHTTPSEnabled = byDataBuffer[2];

	// ------------------------ PDU OUTLET SETTINGS ---------------------------------------------------------------------------------------
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_TOTAL_OUTLETS, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();
	m_byMaxOutlets = byDataBuffer[0];

	for (nNum = 0; nNum < m_byMaxOutlets; nNum++)
	{
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GET_SET_OUTLET_NAME, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		memcpy(m_stPDUConfigInfo.byOutletNm[nNum], byDataBuffer, sizeof(m_stPDUConfigInfo.byOutletNm[nNum]));
	
		// ------------------------ ALERT SETTINGS ---------------------------------------------------------------------------------------------

		// VCB
		// ************** READ VIRTUAL CIRCUIT BREAKER ENABLE STATUS **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_ENB_VCB, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		m_stPDUConfigInfo.byEnbAlertVCB[nNum] = byDataBuffer[0];

		// ************** READ MAX GRACE PERIOD **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_VCB_MAX_GRAC_PER, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		cszLoad.Empty();
		cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
		stIeeConv.nValue = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		m_stPDUConfigInfo.dwVCBMaxGracPer[nNum] = (DWORD)stIeeConv.fValue;
		cszLoad.ReleaseBuffer();

		// ************** READ THRESHOLD VALUE **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_VCB_THRES_VALUE, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		cszLoad.Empty();
		cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
		stIeeConv.nValue = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		m_stPDUConfigInfo.fVCBThresValue[nNum] = stIeeConv.fValue;
		cszLoad.ReleaseBuffer();

		// LCA
		// ************** READ LOW CURRENT ALERT ENABLE STATUS **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_ENB_LCA, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		m_stPDUConfigInfo.byEnbAlertLCA[nNum] = byDataBuffer[0];

		// ************** READ MAX GRACE PERIOD **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_LCA_MAX_GRAC_PER, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		cszLoad.Empty();
		cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
		stIeeConv.nValue = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		m_stPDUConfigInfo.dwLCAMaxGracPer[nNum] = (DWORD)stIeeConv.fValue;
		cszLoad.ReleaseBuffer();

		// ************** READ THRESHOLD VALUE **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_LCA_THRES_VALUE, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		cszLoad.Empty();
		cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
		stIeeConv.nValue = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		m_stPDUConfigInfo.fLCAThresValue[nNum] = stIeeConv.fValue;
		cszLoad.ReleaseBuffer();

		// HCA
		// ************** READ HIGH CURRENT ALERT ENABLE STATUS **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_ENB_HCA, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		m_stPDUConfigInfo.byEnbAlertHCA[nNum] = byDataBuffer[0];

		// ************** READ MAX GRACE PERIOD **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_HCA_MAX_GRAC_PER, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		cszLoad.Empty();
		cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
		stIeeConv.nValue = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		m_stPDUConfigInfo.dwHCAMaxGracPer[nNum] = (DWORD)stIeeConv.fValue;
		cszLoad.ReleaseBuffer();

		// ************** READ THRESHOLD VALUE **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_HCA_THRES_VALUE, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		cszPDUAddress.ReleaseBuffer();
		cszLoad.Empty();
		cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
		stIeeConv.nValue = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		m_stPDUConfigInfo.fHCAThresValue[nNum] = stIeeConv.fValue;
		cszLoad.ReleaseBuffer();
	}

	return bRet;
}

BOOL CPDUConfigDlg::UpdateInfoInAllPages(UINT nPageNum)
{
	BOOL			bRet = TRUE;
	int				nPos = 0;
	UINT			nNum = 0;
	CString			cszLoad;

	// ********** UPDATE NETWORK PAGE ******************
	if (m_pPageNetwork)
	{
		// Enable DHCP check box
		m_pPageNetwork->m_cChkDHCP.SetCheck(m_stPDUConfigInfo.byEnbDHCP);
		m_pPageNetwork->OnBnClickedDHCP();

		// Network IP Address
		m_pPageNetwork->m_cIPAddrCtrl.SetAddress(m_stPDUConfigInfo.byIPField[0], m_stPDUConfigInfo.byIPField[1], m_stPDUConfigInfo.byIPField[2], m_stPDUConfigInfo.byIPField[3]);

		// Subnet Mask
		m_pPageNetwork->m_cSubnetMsk.SetAddress(m_stPDUConfigInfo.bySMField[0], m_stPDUConfigInfo.bySMField[1], m_stPDUConfigInfo.bySMField[2], m_stPDUConfigInfo.bySMField[3]);

		// Default Gateway
		m_pPageNetwork->m_cGateway.SetAddress(m_stPDUConfigInfo.byGWField[0], m_stPDUConfigInfo.byGWField[1], m_stPDUConfigInfo.byGWField[2], m_stPDUConfigInfo.byGWField[3]);

		// Primary DNS Address
		m_pPageNetwork->m_cPrimDNS.SetAddress(m_stPDUConfigInfo.byPDNSField[0], m_stPDUConfigInfo.byPDNSField[1], m_stPDUConfigInfo.byPDNSField[2], m_stPDUConfigInfo.byPDNSField[3]);

		// Secondary DNS Address
		m_pPageNetwork->m_cSecDNS.SetAddress(m_stPDUConfigInfo.bySDNSField[0], m_stPDUConfigInfo.bySDNSField[1], m_stPDUConfigInfo.bySDNSField[2], m_stPDUConfigInfo.bySDNSField[3]);
	}

	// ********** UPDATE OTHER SETTINGS PAGE ******************
	if (m_pPageSNMP)
	{
		// Enable SNMP Trap
		m_pPageSNMP->m_cChkSNMPTrap.SetCheck(m_stPDUConfigInfo.byEnbSNMPTrap);
		m_pPageSNMP->OnBnClickedCheckSnmpTrap();

		// Receiver IP Address
		m_pPageSNMP->m_cReceIPAddr.SetAddress(m_stPDUConfigInfo.bySNMPRecevrField[0], m_stPDUConfigInfo.bySNMPRecevrField[1], m_stPDUConfigInfo.bySNMPRecevrField[2], m_stPDUConfigInfo.bySNMPRecevrField[3]);

		// SNMP Trap Port Number
		m_pPageSNMP->SetDlgItemInt(IDC_EDIT_SNMP_TRAP_PORT, m_stPDUConfigInfo.bySNMPTrapPortNum);

		if (m_stPDUConfigInfo.bNtpEnabled)
		{
			m_pPageSNMP->m_cBtnEnbNTPDateTime.SetCheck(BST_CHECKED);
			m_pPageSNMP->OnBnClickedCheckEnbNtpDtTm();
			nPos = GetTimeZonePos(m_stPDUConfigInfo.fNtpZone);
			if (nPos == -1)
			{
				nPos = 0;
			}
			m_pPageSNMP->m_cComboTimeZone.SetCurSel(nPos);

			cszLoad.Empty();
			cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byTimeZoneDateTime);
			m_pPageSNMP->CopyDateTime(cszLoad);
		}
		else
		{
			m_pPageSNMP->m_cBtnEnbNTPDateTime.SetCheck(BST_UNCHECKED);
			m_pPageSNMP->OnBnClickedCheckEnbNtpDtTm();
			cszLoad.Empty();
			cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byTimeZoneDateTime);
			m_pPageSNMP->CopyDateTime(cszLoad);
		}
	}

	// ********** SNMP SETTINGS PAGE ******************
	if (m_pPageSNMPVer)
	{
		// Read Community
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.bySNMPReadCmn);
		m_pPageSNMPVer->m_cEditV3ReadComm.SetWindowText(cszLoad);

		// Write Community
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.bySNMPWriteCmn);
		m_pPageSNMPVer->m_cEditV3WriteComm.SetWindowText(cszLoad);

		if (m_stPDUConfigInfo.bySNMPV3_Enabled)
		{
			cszLoad.Empty();
			cszLoad.Format(_T("%s"), m_stPDUConfigInfo.bySNMPV3_UserName);
			m_pPageSNMPVer->m_cEditSNMPV3User.SetWindowText(cszLoad);

			m_pPageSNMPVer->m_cComboAuthLevel.SetCurSel(0);

			m_pPageSNMPVer->SelectControl(1);
		}
		else
		{
			m_pPageSNMPVer->SelectControl(0);
		}
	}

	// ********** UPDATE EMAIL PAGE ******************
	if (m_pPageEmail)
	{
		// Enable Email Notification
		m_pPageEmail->m_cChkEmail.SetCheck(m_stPDUConfigInfo.byEnbEmailNotify);
		m_pPageEmail->OnBnClickedCheckEnableEmail();

		// SMTP Server name
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.bySMTPServerAddrs);
		m_pPageEmail->m_cEditSMTPServer.SetWindowText(cszLoad);

		// SMTP Port number
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.bySMTPPortNum);
		m_pPageEmail->SetDlgItemText(IDC_EDIT_SMTP_PORT, cszLoad);

		// SMTP Sender User name
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byFrmUserName);
		m_pPageEmail->m_cEditSMTPSendrUserNm.SetWindowText(cszLoad);

		// SMTP Sender Password
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byFrmPassword);
		m_pPageEmail->m_cEditSMTPSendrPasswd.SetWindowText(cszLoad);

		// SMTP Receiver Email Id
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byRecevrEmailAddrs);
		m_pPageEmail->m_cEditSMTPRecevrEmailId.SetWindowText(cszLoad);
	}

	// ********** UPDATE GENERAL PAGE ******************
	if (m_pPageGeneral)
	{
		// System Name
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byGeneralSysNm);
		m_pPageGeneral->m_cEditGenSysNm.SetWindowText(cszLoad);

		// System Location
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byGeneralSysLoc);
		m_pPageGeneral->m_cEditGenSysLoctn.SetWindowText(cszLoad);

		// System Contact
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byGeneralSysCont);
		m_pPageGeneral->m_cEditGenSysCont.SetWindowText(cszLoad);

		// Stagger On Delay
		m_pPageGeneral->SetDlgItemInt(IDC_EDIT_GEN_STAGGER_ON_DELAY, m_stPDUConfigInfo.dwStaggerOnDelay);

		// Stagger Off Delay
		m_pPageGeneral->SetDlgItemInt(IDC_EDIT_GEN_STAGGER_OFF_DELAY, m_stPDUConfigInfo.dwStaggerOffDelay);

		if (m_stPDUConfigInfo.byTelnetEnabled)
		{
			m_pPageGeneral->m_cCheckEnbTelnet.SetCheck(BST_CHECKED);
		}
		else
		{
			m_pPageGeneral->m_cCheckEnbTelnet.SetCheck(BST_UNCHECKED);
		}

		if (m_stPDUConfigInfo.byHTTPEnabled)
		{
			m_pPageGeneral->m_cCheckEnbHTTP.SetCheck(BST_CHECKED);
		}
		else
		{
			m_pPageGeneral->m_cCheckEnbHTTP.SetCheck(BST_UNCHECKED);
		}

		if (m_stPDUConfigInfo.byHTTPSEnabled)
		{
			m_pPageGeneral->m_cCheckEnbHTTPS.SetCheck(BST_CHECKED);
		}
		else
		{
			m_pPageGeneral->m_cCheckEnbHTTPS.SetCheck(BST_UNCHECKED);
		}
	}

	// ********** UPDATE OUTLET PAGE ******************
	if (m_pPagePDU)
	{
		m_pPagePDU->m_cszOutletNames.RemoveAll();

		for(nNum = 0; nNum < m_byMaxOutlets; nNum++)
		{
			cszLoad.Empty();
			cszLoad.Format(_T("%s"), m_stPDUConfigInfo.byOutletNm[nNum]);
			m_pPagePDU->m_cszOutletNames.Add(cszLoad);
		}

		m_pPagePDU->UpdateOutletNamesInfo();
	}

	// ********** UPDATE ALERT PAGE ******************
	if (m_pPageAlert)
	{
		m_pPageAlert->m_bFirstTime = TRUE;

		m_pPageAlert->FillOutletsInCombo(m_byMaxOutlets);

		for(nNum = 0; nNum < m_byMaxOutlets; nNum++)
		{
			m_pPageAlert->m_stOutletAlertInfo.byEnbAlertVCB[nNum] = m_stPDUConfigInfo.byEnbAlertVCB[nNum];
			m_pPageAlert->m_stOutletAlertInfo.dwVCBMaxGracPer[nNum] = m_stPDUConfigInfo.dwVCBMaxGracPer[nNum];
			m_pPageAlert->m_stOutletAlertInfo.fVCBThresValue[nNum] = m_stPDUConfigInfo.fVCBThresValue[nNum];

			m_pPageAlert->m_stOutletAlertInfo.byEnbAlertLCA[nNum] = m_stPDUConfigInfo.byEnbAlertLCA[nNum];
			m_pPageAlert->m_stOutletAlertInfo.dwLCAMaxGracPer[nNum] = m_stPDUConfigInfo.dwLCAMaxGracPer[nNum];
			m_pPageAlert->m_stOutletAlertInfo.fLCAThresValue[nNum] = m_stPDUConfigInfo.fLCAThresValue[nNum];

			m_pPageAlert->m_stOutletAlertInfo.byEnbAlertHCA[nNum] = m_stPDUConfigInfo.byEnbAlertHCA[nNum];
			m_pPageAlert->m_stOutletAlertInfo.dwHCAMaxGracPer[nNum] = m_stPDUConfigInfo.dwHCAMaxGracPer[nNum];
			m_pPageAlert->m_stOutletAlertInfo.fHCAThresValue[nNum] = m_stPDUConfigInfo.fHCAThresValue[nNum];
		}

		m_pPageAlert->OnCbnSelchangeOutletNumLst();
	}

	return bRet;
}

BOOL CPDUConfigDlg::OpenCurrPage(int nPageNo)
{
	if (m_cTabConfig.GetCurSel() == nPageNo)
		return FALSE;

	NMHDR			nmhdr = {0}; 

	nmhdr.hwndFrom = m_cTabConfig.GetSafeHwnd();  
	nmhdr.idFrom= m_cTabConfig.GetDlgCtrlID();

	nmhdr.code = TCN_SELCHANGING;
	::SendMessage(m_cTabConfig.GetSafeHwnd(), WM_NOTIFY, MAKELONG(TCN_SELCHANGING, m_cTabConfig.GetCurSel()), (LPARAM)(&nmhdr));

	m_cTabConfig.SetCurSel(nPageNo);

	nmhdr.code = TCN_SELCHANGE;
	::SendMessage(m_cTabConfig.GetSafeHwnd(), WM_NOTIFY, MAKELONG(TCN_SELCHANGE, ID_PAGE_NETWORK), (LPARAM)(&nmhdr));	

	return TRUE;
}

void CPDUConfigDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	UINT				nNum = 0;
	CString				cszLoad;
	BYTE				byPart[4] = {0};

	// Validate all fields before WRITE process...
	if (m_pPageNetwork)
	{
		if (m_pPageNetwork->m_cChkDHCP.GetCheck() == BST_UNCHECKED)
		{
			// NETWORK IP ADDRESS
			m_pPageNetwork->m_cIPAddrCtrl.GetAddress(byPart[0], byPart[1], byPart[2], byPart[3]);
			//if (((byPart[0] == 0) && (byPart[1] == 0) && (byPart[2] == 0) && (byPart[3] == 0)) || (byPart[3] == 0))
			if ((byPart[0] == 0) || (byPart[3] == 0))
			{
				OpenCurrPage(ID_PAGE_NETWORK);

				cszLoad.LoadString(IDS_STR_VALID_NETWORK_IP_ADDRS);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageNetwork->m_cIPAddrCtrl.SetFocus();
				return;
			}

			memset(byPart, 0, sizeof(byPart));

			// SUBNET MASK
			m_pPageNetwork->m_cSubnetMsk.GetAddress(byPart[0], byPart[1], byPart[2], byPart[3]);
			//if (((byPart[0] == 0) && (byPart[1] == 0) && (byPart[2] == 0) && (byPart[3] == 0)) || (byPart[3] == 0))
			if (byPart[0] == 0)
			{
				OpenCurrPage(ID_PAGE_NETWORK);

				cszLoad.LoadString(IDS_STR_VALID_SM);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageNetwork->m_cSubnetMsk.SetFocus();
				return;
			}

		#if 0
			memset(byPart, 0, sizeof(byPart));

			// Default Gateway
			m_pPageNetwork->m_cGateway.GetAddress(byPart[0], byPart[1], byPart[2], byPart[3]);
			if (((byPart[0] == 0) && (byPart[1] == 0) && (byPart[2] == 0) && (byPart[3] == 0)) || (byPart[3] == 0))
			{
				OpenCurrPage(ID_PAGE_NETWORK);

				cszLoad.LoadString(IDS_STR_VALID_SM);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageNetwork->m_cGateway.SetFocus();
				return;
			}

			memset(byPart, 0, sizeof(byPart));

			// Primary DNS Address
			m_pPageNetwork->m_cPrimDNS.GetAddress(byPart[0], byPart[1], byPart[2], byPart[3]);
			if (((byPart[0] == 0) && (byPart[1] == 0) && (byPart[2] == 0) && (byPart[3] == 0)) || (byPart[3] == 0))
			{
				OpenCurrPage(ID_PAGE_NETWORK);

				cszLoad.LoadString(IDS_STR_VALID_SM);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageNetwork->m_cPrimDNS.SetFocus();
				return;
			}

			memset(byPart, 0, sizeof(byPart));

			// Secondary DNS Address
			m_pPageNetwork->m_cSecDNS.GetAddress(byPart[0], byPart[1], byPart[2], byPart[3]);
			if (((byPart[0] == 0) && (byPart[1] == 0) && (byPart[2] == 0) && (byPart[3] == 0)) || (byPart[3] == 0))
			{
				OpenCurrPage(ID_PAGE_NETWORK);

				cszLoad.LoadString(IDS_STR_VALID_SM);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageNetwork->m_cSecDNS.SetFocus();
				return;
			}
		#endif
		}
	}

	if (m_pPageSNMP)
	{
		if (m_pPageSNMP->m_cChkSNMPTrap.GetCheck() == BST_CHECKED)
		{
			memset(byPart, 0, sizeof(byPart));

			// RECEIVER IP Address
			m_pPageSNMP->m_cReceIPAddr.GetAddress(byPart[0], byPart[1], byPart[2], byPart[3]);
			//if (((byPart[0] == 0) && (byPart[1] == 0) && (byPart[2] == 0) && (byPart[3] == 0)) || (byPart[3] == 0))
			if ((byPart[0] == 0) || (byPart[3] == 0))
			{
				OpenCurrPage(ID_PAGE_SNMP);

				cszLoad.LoadString(IDS_STR_VALID_RIP);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageSNMP->m_cReceIPAddr.SetFocus();
				return;
			}
		}
	}

	if (m_pPageSNMPVer)
	{
		CString			cszConfPass;

		if (m_pPageSNMPVer->m_cComboAuthLevel.GetCurSel() == 1)
		{
			cszLoad.Empty();
			m_pPageSNMPVer->m_cEditAuthPassKey.GetWindowText(cszLoad);
			if (cszLoad.GetLength() < 4)
			{
				AfxMessageBox(_T("Authentication Password should not be empty or less than 4."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditAuthPassKey.SetFocus();
				return;
			}

			cszConfPass.Empty();
			m_pPageSNMPVer->m_cEditConfAuthPassKey.GetWindowText(cszConfPass);
			if (cszConfPass.GetLength() < 4)
			{
				AfxMessageBox(_T("Confirm Authentication Password should not be empty or less than 4."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditConfAuthPassKey.SetFocus();
				return;
			}

			if (cszConfPass.Compare(cszLoad))
			{
				AfxMessageBox(_T("Authentication Passwords you typed do not match. Please retype the password in both boxes."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditAuthPassKey.SetFocus();
				return;
			}
		}
		else if (m_pPageSNMPVer->m_cComboAuthLevel.GetCurSel() == 2)
		{
			cszLoad.Empty();
			m_pPageSNMPVer->m_cEditAuthPassKey.GetWindowText(cszLoad);
			if (cszLoad.GetLength() < 4)
			{
				AfxMessageBox(_T("Authentication Password should not be empty or less than 4."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditAuthPassKey.SetFocus();
				return;
			}

			cszConfPass.Empty();
			m_pPageSNMPVer->m_cEditConfAuthPassKey.GetWindowText(cszConfPass);
			if (cszConfPass.GetLength() < 4)
			{
				AfxMessageBox(_T("Confirm Authentication Password should not be empty or less than 4."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditConfAuthPassKey.SetFocus();
				return;
			}

			if (cszConfPass.Compare(cszLoad))
			{
				AfxMessageBox(_T("Authentication Passwords you typed do not match. Please retype the password in both boxes."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditAuthPassKey.SetFocus();
				return;
			}


			cszLoad.Empty();
			m_pPageSNMPVer->m_cEditPrivPassKey.GetWindowText(cszLoad);
			if (cszLoad.GetLength() < 4)
			{
				AfxMessageBox(_T("Privacy Password should not be empty or less than 4."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditPrivPassKey.SetFocus();
				return;
			}

			cszConfPass.Empty();
			m_pPageSNMPVer->m_cEditPrivConfPasskey.GetWindowText(cszConfPass);
			if (cszConfPass.GetLength() < 4)
			{
				AfxMessageBox(_T("Confirm Privacy Password should not be empty or less than 4."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditPrivConfPasskey.SetFocus();
				return;
			}

			if (cszConfPass.Compare(cszLoad))
			{
				AfxMessageBox(_T("Privacy Passwords you typed do not match. Please retype the password in both boxes."), MB_ICONSTOP);

				m_pPageSNMPVer->m_cEditPrivPassKey.SetFocus();
				return;
			}
		}
	}

	if (m_pPageEmail)
	{
		if (m_pPageEmail->m_cChkEmail.GetCheck() == BST_CHECKED)
		{
			// SMTP Server Name
			cszLoad.Empty();
			m_pPageEmail->m_cEditSMTPServer.GetWindowText(cszLoad);
			if (cszLoad.IsEmpty())
			{
				OpenCurrPage(ID_PAGE_EMAIL);

				cszLoad.LoadString(IDS_STR_VALID_SMTP_SER_NM);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageEmail->m_cEditSMTPServer.SetFocus();
				return;
			}

			// SMTP Port Number
			if (m_pPageEmail->GetDlgItemInt(IDC_EDIT_SMTP_PORT) == 0)
			{
				OpenCurrPage(ID_PAGE_EMAIL);

				cszLoad.LoadString(IDS_STR_VALID_SMTP_PORT_NUM);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageEmail->GetDlgItem(IDC_EDIT_SMTP_PORT)->SetFocus();
				return;
			}

			// SMTP Sender User Name
			cszLoad.Empty();
			m_pPageEmail->m_cEditSMTPSendrUserNm.GetWindowText(cszLoad);
			if (cszLoad.IsEmpty())
			{
				OpenCurrPage(ID_PAGE_EMAIL);

				cszLoad.LoadString(IDS_STR_VALID_EMAIL_USERNM);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageEmail->m_cEditSMTPSendrUserNm.SetFocus();
				return;
			}

			// SMTP Sender Password
			cszLoad.Empty();
			m_pPageEmail->m_cEditSMTPSendrPasswd.GetWindowText(cszLoad);
			if (cszLoad.IsEmpty())
			{
				OpenCurrPage(ID_PAGE_EMAIL);

				cszLoad.LoadString(IDS_STR_VALID_EMAIL_PASSWD);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageEmail->m_cEditSMTPSendrPasswd.SetFocus();
				return;
			}

			// SMTP Receiver Email Id
			cszLoad.Empty();
			m_pPageEmail->m_cEditSMTPRecevrEmailId.GetWindowText(cszLoad);
			if (cszLoad.IsEmpty())
			{
				OpenCurrPage(ID_PAGE_EMAIL);

				cszLoad.LoadString(IDS_STR_VALID_EMAIL_ID);
				AfxMessageBox(cszLoad, MB_ICONSTOP);

				m_pPageEmail->m_cEditSMTPRecevrEmailId.SetFocus();
				return;
			}
		}
	}

	if (m_pPageGeneral)
	{
		
	}

	if (m_pPageAlert)
	{
		CString			cszTemp;

		m_pPageAlert->SaveAlertInfo();

		for (nNum = 0; nNum < m_byMaxOutlets; nNum++)
		{
			if (m_pPageAlert->m_stOutletAlertInfo.byEnbAlertVCB[nNum] == 1)
			{
				if (m_pPageAlert->m_stOutletAlertInfo.dwVCBMaxGracPer[nNum] == 0)
				{
					OpenCurrPage(ID_PAGE_ALERT_SETTING);

					if (m_pPageAlert->m_cComboOutletNum.GetCurSel() != nNum)
					{
						m_pPageAlert->m_cComboOutletNum.SetCurSel(nNum);
						m_pPageAlert->OnCbnSelchangeOutletNumLst();
					}

					cszTemp.Empty();
					cszTemp.LoadString(IDS_STR_VALID_MX_GRC_PER);
					cszLoad.Format(cszTemp, (nNum + 1));
					AfxMessageBox(cszLoad, MB_ICONSTOP);

					m_pPageAlert->m_cEditVCBMaxGrcPer.SetFocus();
					return;
				}
			}

			if (m_pPageAlert->m_stOutletAlertInfo.byEnbAlertLCA[nNum] == 1)
			{
				if (m_pPageAlert->m_stOutletAlertInfo.dwLCAMaxGracPer[nNum] == 0)
				{
					OpenCurrPage(ID_PAGE_ALERT_SETTING);

					if (m_pPageAlert->m_cComboOutletNum.GetCurSel() != nNum)
					{
						m_pPageAlert->m_cComboOutletNum.SetCurSel(nNum);
						m_pPageAlert->OnCbnSelchangeOutletNumLst();
					}

					cszTemp.Empty();
					cszTemp.LoadString(IDS_STR_VALID_MX_GRC_PER);
					cszLoad.Format(cszTemp, (nNum + 1));
					AfxMessageBox(cszLoad, MB_ICONSTOP);

					m_pPageAlert->m_cEditLCAMaxGrcPer.SetFocus();
					return;
				}
			}

			if (m_pPageAlert->m_stOutletAlertInfo.byEnbAlertHCA[nNum] == 1)
			{
				if (m_pPageAlert->m_stOutletAlertInfo.dwHCAMaxGracPer[nNum] == 0)
				{
					OpenCurrPage(ID_PAGE_ALERT_SETTING);

					if (m_pPageAlert->m_cComboOutletNum.GetCurSel() != nNum)
					{
						m_pPageAlert->m_cComboOutletNum.SetCurSel(nNum);
						m_pPageAlert->OnCbnSelchangeOutletNumLst();
					}

					cszTemp.Empty();
					cszTemp.LoadString(IDS_STR_VALID_MX_GRC_PER);
					cszLoad.Format(cszTemp, (nNum + 1));
					AfxMessageBox(cszLoad, MB_ICONSTOP);

					m_pPageAlert->m_cEditHCAMaxGrcPer.SetFocus();
					return;
				}
			}
		}
	}

	// #################################################################### COLLECT DATA FROM ALL PAGES ########################################################################

	memset(&m_stPDUConfigInfo, 0, sizeof(PDU_CONFIG_INFO));

	// ********** GATHER DATA FRM NETWORK PAGE ******************
	if (m_pPageNetwork)
	{
		// DHCP Enable status
		m_stPDUConfigInfo.byEnbDHCP = 0;
		if (m_pPageNetwork->m_cChkDHCP.GetCheck() == BST_CHECKED)
		{
			m_stPDUConfigInfo.byEnbDHCP = 1;
		}

		// Network IP Address
		m_pPageNetwork->m_cIPAddrCtrl.GetAddress(m_stPDUConfigInfo.byIPField[0], m_stPDUConfigInfo.byIPField[1], m_stPDUConfigInfo.byIPField[2], m_stPDUConfigInfo.byIPField[3]);

		// Subnet Mask
		m_pPageNetwork->m_cSubnetMsk.GetAddress(m_stPDUConfigInfo.bySMField[0], m_stPDUConfigInfo.bySMField[1], m_stPDUConfigInfo.bySMField[2], m_stPDUConfigInfo.bySMField[3]);

		// Default Gateway
		m_pPageNetwork->m_cGateway.GetAddress(m_stPDUConfigInfo.byGWField[0], m_stPDUConfigInfo.byGWField[1], m_stPDUConfigInfo.byGWField[2], m_stPDUConfigInfo.byGWField[3]);

		// Primary DNS Address
		m_pPageNetwork->m_cPrimDNS.GetAddress(m_stPDUConfigInfo.byPDNSField[0], m_stPDUConfigInfo.byPDNSField[1], m_stPDUConfigInfo.byPDNSField[2], m_stPDUConfigInfo.byPDNSField[3]);

		// Secondary DNS Address
		m_pPageNetwork->m_cSecDNS.GetAddress(m_stPDUConfigInfo.bySDNSField[0], m_stPDUConfigInfo.bySDNSField[1], m_stPDUConfigInfo.bySDNSField[2], m_stPDUConfigInfo.bySDNSField[3]);
	}

	// ********** GATHER DATA FRM OTHER SETTINGS PAGE ******************
	if (m_pPageSNMP)
	{
		// SNMP TRAP STATUS
		m_stPDUConfigInfo.byEnbSNMPTrap = 0;
		if (m_pPageSNMP->m_cChkSNMPTrap.GetCheck() == BST_CHECKED)
		{
			m_stPDUConfigInfo.byEnbSNMPTrap = 1;
		}
		
		// Receiver IP Address
		m_pPageSNMP->m_cReceIPAddr.GetAddress(m_stPDUConfigInfo.bySNMPRecevrField[0], m_stPDUConfigInfo.bySNMPRecevrField[1], m_stPDUConfigInfo.bySNMPRecevrField[2], m_stPDUConfigInfo.bySNMPRecevrField[3]);

		// SNMP Trap Port Number
		m_stPDUConfigInfo.bySNMPTrapPortNum = m_pPageSNMP->GetDlgItemInt(IDC_EDIT_SNMP_TRAP_PORT);

		m_stPDUConfigInfo.bNtpEnabled = 0;
		if (m_pPageSNMP->m_cBtnEnbNTPDateTime.GetCheck() == BST_CHECKED)
		{
			m_stPDUConfigInfo.bNtpEnabled = 1;
		
			if (-1 != m_pPageSNMP->m_cComboTimeZone.GetCurSel())
			{
				m_stPDUConfigInfo.fNtpZone = m_fTimeZones[m_pPageSNMP->m_cComboTimeZone.GetCurSel()];
			}
		}
		else
		{
			COleDateTime			cDateTool;
			COleDateTime			cTimeTool;
			CString					cszTool;
				
			m_pPageSNMP->m_ctrlDateHired->GetTime(cDateTool);
			m_pPageSNMP->m_ctrlTimeHired->GetTime(cTimeTool);

			cszTool.Empty();
			cszTool.Format(_T("%2d-%2d-%4d %2d:%2d"), cDateTool.GetMonth(), cDateTool.GetDay(), cDateTool.GetYear(), cTimeTool.GetHour(), cTimeTool.GetMinute());
			wsprintf((LPSTR)m_stPDUConfigInfo.byTimeZoneDateTime, _T("%s"), cszTool);
		}
	}

	if (m_pPageSNMPVer)
	{
		if (!m_pPageSNMPVer->GetV3CheckStatus())
		{
			m_stPDUConfigInfo.bySNMPV3_Enabled = 0;

			m_stPDUConfigInfo.byAuthLevel = 0;

			cszLoad.Empty();
			m_pPageSNMPVer->m_cEditSNMPV3User.GetWindowText(cszLoad);
			wsprintf((LPSTR)m_stPDUConfigInfo.bySNMPV3_UserName, _T("%s"), cszLoad);

			// Read Community
			cszLoad.Empty();
			m_pPageSNMPVer->m_cEditV3ReadComm.GetWindowText(cszLoad);
			wsprintf((LPSTR)m_stPDUConfigInfo.bySNMPReadCmn, _T("%s"), cszLoad);

			// Write Community
			cszLoad.Empty();
			m_pPageSNMPVer->m_cEditV3WriteComm.GetWindowText(cszLoad);
			wsprintf((LPSTR)m_stPDUConfigInfo.bySNMPWriteCmn, _T("%s"), cszLoad);
		}
		else
		{
			m_stPDUConfigInfo.bySNMPV3_Enabled = 1;

			m_stPDUConfigInfo.byAuthLevel = m_pPageSNMPVer->m_cComboAuthLevel.GetCurSel();
			cszLoad.Empty();
			m_pPageSNMPVer->m_cEditSNMPV3User.GetWindowText(cszLoad);
			wsprintf((LPSTR)m_stPDUConfigInfo.bySNMPV3_UserName, _T("%s"), cszLoad);

			if (m_stPDUConfigInfo.byAuthLevel == 1)
			{
				cszLoad.Empty();
				m_pPageSNMPVer->m_cEditAuthPassKey.GetWindowText(cszLoad);
				wsprintf((LPSTR)m_stPDUConfigInfo.byAuthPassword, _T("%s"), cszLoad);
			}
			else if (m_stPDUConfigInfo.byAuthLevel == 2)
			{
				cszLoad.Empty();
				m_pPageSNMPVer->m_cEditAuthPassKey.GetWindowText(cszLoad);
				wsprintf((LPSTR)m_stPDUConfigInfo.byAuthPassword, _T("%s"), cszLoad);

				cszLoad.Empty();
				m_pPageSNMPVer->m_cEditPrivPassKey.GetWindowText(cszLoad);
				wsprintf((LPSTR)m_stPDUConfigInfo.byPrivPassword, _T("%s"), cszLoad);
			}
		}
	}

	// ********** GATHER DATA FRM EMAIL PAGE ******************
	if (m_pPageEmail)
	{
		// Email Notification Enable Status
		m_stPDUConfigInfo.byEnbEmailNotify = 0;
		if (m_pPageEmail->m_cChkEmail.GetCheck() == BST_CHECKED)
		{
			m_stPDUConfigInfo.byEnbEmailNotify = 1;
		}

		// SMTP Server Name
		cszLoad.Empty();
		m_pPageEmail->m_cEditSMTPServer.GetWindowText(cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.bySMTPServerAddrs, _T("%s"), cszLoad);

		// SMTP Port Number
		cszLoad.Empty();
		m_pPageEmail->GetDlgItemText(IDC_EDIT_SMTP_PORT, cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.bySMTPPortNum, _T("%s"), cszLoad);

		// SMTP Sender User Name
		cszLoad.Empty();
		m_pPageEmail->m_cEditSMTPSendrUserNm.GetWindowText(cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.byFrmUserName, _T("%s"), cszLoad);

		// SMTP Sender Password
		cszLoad.Empty();
		m_pPageEmail->m_cEditSMTPSendrPasswd.GetWindowText(cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.byFrmPassword, _T("%s"), cszLoad);

		// SMTP Receiver Email Id
		cszLoad.Empty();
		m_pPageEmail->m_cEditSMTPRecevrEmailId.GetWindowText(cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.byRecevrEmailAddrs, _T("%s"), cszLoad);
	}

	// ********** GATHER DATA FRM GENERAL PAGE ******************
	if (m_pPageGeneral)
	{
		// System Name
		cszLoad.Empty();
		m_pPageGeneral->m_cEditGenSysNm.GetWindowText(cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.byGeneralSysNm, _T("%s"), cszLoad);

		// System Location
		cszLoad.Empty();
		m_pPageGeneral->m_cEditGenSysLoctn.GetWindowText(cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.byGeneralSysLoc, _T("%s"), cszLoad);

		// System Contact
		cszLoad.Empty();
		m_pPageGeneral->m_cEditGenSysCont.GetWindowText(cszLoad);
		wsprintf((LPSTR)m_stPDUConfigInfo.byGeneralSysCont, _T("%s"), cszLoad);

		// Stagger On Delay
		m_stPDUConfigInfo.dwStaggerOnDelay = m_pPageGeneral->GetDlgItemInt(IDC_EDIT_GEN_STAGGER_ON_DELAY);

		// Stagger Off Delay
		m_stPDUConfigInfo.dwStaggerOffDelay = m_pPageGeneral->GetDlgItemInt(IDC_EDIT_GEN_STAGGER_OFF_DELAY);

		m_stPDUConfigInfo.byTelnetEnabled = 0;
		if (m_pPageGeneral->m_cCheckEnbTelnet.GetCheck() == BST_CHECKED)
		{
			m_stPDUConfigInfo.byTelnetEnabled = 1;
		}

		m_stPDUConfigInfo.byHTTPEnabled = 0;
		if (m_pPageGeneral->m_cCheckEnbHTTP.GetCheck() == BST_CHECKED)
		{
			m_stPDUConfigInfo.byHTTPEnabled = 1;
		}

		m_stPDUConfigInfo.byHTTPSEnabled = 0;
		if (m_pPageGeneral->m_cCheckEnbHTTPS.GetCheck() == BST_CHECKED)
		{
			m_stPDUConfigInfo.byHTTPSEnabled = 1;
		}

	}

	// ********** GATHER DATA FRM OUTLET PAGE ******************
	if ((m_pPagePDU) && (m_pPageAlert))
	{
		m_pPagePDU->CollectOutletNmsInfo();
		m_pPageAlert->SaveAlertInfo();

		for (nNum = 0; nNum < m_byMaxOutlets; nNum++)
		{
			wsprintf((LPSTR)m_stPDUConfigInfo.byOutletNm[nNum], _T("%s"), m_pPagePDU->m_cszOutletNames.ElementAt(nNum));			
		
			// ********** GATHER DATA FRM ALERT PAGE ******************
			// VCB Enable Status
			m_stPDUConfigInfo.byEnbAlertVCB[nNum] = m_pPageAlert->m_stOutletAlertInfo.byEnbAlertVCB[nNum];

			// MAX GRACE PERIOD
			m_stPDUConfigInfo.dwVCBMaxGracPer[nNum] = m_pPageAlert->m_stOutletAlertInfo.dwVCBMaxGracPer[nNum];

			// THRESHOLD VALUE
			m_stPDUConfigInfo.fVCBThresValue[nNum] = m_pPageAlert->m_stOutletAlertInfo.fVCBThresValue[nNum];


			// LCA Enable Status
			m_stPDUConfigInfo.byEnbAlertLCA[nNum] = m_pPageAlert->m_stOutletAlertInfo.byEnbAlertLCA[nNum];

			// MAX GRACE PERIOD
			m_stPDUConfigInfo.dwLCAMaxGracPer[nNum] = m_pPageAlert->m_stOutletAlertInfo.dwLCAMaxGracPer[nNum];

			// THRESHOLD VALUE
			m_stPDUConfigInfo.fLCAThresValue[nNum] = m_pPageAlert->m_stOutletAlertInfo.fLCAThresValue[nNum];

			// HCA Enable Status
			m_stPDUConfigInfo.byEnbAlertHCA[nNum] = m_pPageAlert->m_stOutletAlertInfo.byEnbAlertHCA[nNum];

			// MAX GRACE PERIOD
			m_stPDUConfigInfo.dwHCAMaxGracPer[nNum] = m_pPageAlert->m_stOutletAlertInfo.dwHCAMaxGracPer[nNum];

			// THRESHOLD VALUE
			m_stPDUConfigInfo.fHCAThresValue[nNum] = m_pPageAlert->m_stOutletAlertInfo.fHCAThresValue[nNum];
		}
	}

	m_cButWrite.EnableWindow(FALSE);
	WriteConfigData();
	m_cButWrite.EnableWindow(TRUE);
}

BOOL CPDUConfigDlg::WriteConfigData()
{
	typedef union 
	{
		INT32	nValue;
		float	fValue;
	}IEE_CONV;

	typedef union 
	{
		BYTE	byValue[4];
		float	fAnswer;
	}Float_Conv;

	BOOL				bRet = TRUE;
	BYTE				byErrCode = 0;
	BYTE				byInfoSize = 0;
	BYTE				byDataBuffer[66] = {0};
	UINT				nNum = 0;
	CString				cszPDUAddress;
	CString				cszLoad;
	IEE_CONV			stIeeConv = {0};
	Float_Conv			stFloatConv = {0};
	DWORD				dwValue = 0;
	SNMPV3_SETTINGS		stSNMP_V3_Settings = {0};
	NTP_SETTINGS_INFO	stNTP_Settings = {0};

	ShowPopupDlg(DLG_POPUP_CREATE);

	cszPDUAddress.Empty();

	if (m_nComboSel < (int)m_cszPDUList.GetCount())
	{
		cszPDUAddress = m_cszPDUList.ElementAt(m_nComboSel);
	}

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// ------------------------ NETWORK SETTINGS ------------------------------------------------------------------------
			
	// ************** NETWORK ENBABLE DHCP **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.byEnbDHCP;
	byInfoSize = 1;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_ENB_DHCP, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** NETWORK IP ADDRESS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.byIPField[0];
	byDataBuffer[1] = m_stPDUConfigInfo.byIPField[1];
	byDataBuffer[2] = m_stPDUConfigInfo.byIPField[2];
	byDataBuffer[3] = m_stPDUConfigInfo.byIPField[3];
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_IP_ADDRS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** SUBNET MASK **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.bySMField[0];
	byDataBuffer[1] = m_stPDUConfigInfo.bySMField[1];
	byDataBuffer[2] = m_stPDUConfigInfo.bySMField[2];
	byDataBuffer[3] = m_stPDUConfigInfo.bySMField[3];
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_SUBNET_MSK, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** DEFAULT GATEWAY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.byGWField[0];
	byDataBuffer[1] = m_stPDUConfigInfo.byGWField[1];
	byDataBuffer[2] = m_stPDUConfigInfo.byGWField[2];
	byDataBuffer[3] = m_stPDUConfigInfo.byGWField[3];
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_DEF_GATEWAY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** PRIMARY DNS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.byPDNSField[0];
	byDataBuffer[1] = m_stPDUConfigInfo.byPDNSField[1];
	byDataBuffer[2] = m_stPDUConfigInfo.byPDNSField[2];
	byDataBuffer[3] = m_stPDUConfigInfo.byPDNSField[3];
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_PRIM_DNS_ADDRS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** SECONDARY DNS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.bySDNSField[0];
	byDataBuffer[1] = m_stPDUConfigInfo.bySDNSField[1];
	byDataBuffer[2] = m_stPDUConfigInfo.bySDNSField[2];
	byDataBuffer[3] = m_stPDUConfigInfo.bySDNSField[3];
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NETWORK_SECN_DNS_ADDRS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// ------------------------ SNMP SETTINGS ---------------------------------------------------------------------------------------------

	if (!m_stPDUConfigInfo.bySNMPV3_Enabled)
	{
		// ************** READ COMMUNITY **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		memcpy(byDataBuffer, m_stPDUConfigInfo.bySNMPReadCmn, sizeof(m_stPDUConfigInfo.bySNMPReadCmn));
		byInfoSize = sizeof(m_stPDUConfigInfo.bySNMPReadCmn);

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_READ_COMMUNITY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** WRITE COMMUNITY **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		memcpy(byDataBuffer, m_stPDUConfigInfo.bySNMPWriteCmn, sizeof(m_stPDUConfigInfo.bySNMPWriteCmn));
		byInfoSize = sizeof(m_stPDUConfigInfo.bySNMPWriteCmn);

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_WRITE_COMMUNITY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** DISABLE SNMP V3 **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		memset(&stSNMP_V3_Settings, 0, sizeof(SNMPV3_SETTINGS));
		stSNMP_V3_Settings.byEnabled = 0;
		stSNMP_V3_Settings.byAuthLevel = 0;
		memcpy(stSNMP_V3_Settings.byUserName, m_stPDUConfigInfo.bySNMPV3_UserName, sizeof(m_stPDUConfigInfo.bySNMPV3_UserName));
		stSNMP_V3_Settings.byAuthHashType = m_stPDUConfigInfo.byAuthHashType;
		stSNMP_V3_Settings.byPrivType = m_stPDUConfigInfo.byPrivType;

		memcpy(byDataBuffer, &stSNMP_V3_Settings, sizeof(SNMPV3_SETTINGS));
		byInfoSize = sizeof(SNMPV3_SETTINGS);

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_V3_SETTINGS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();
	}
	else
	{
		// ************** ENABLE SNMP V3 **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		memset(&stSNMP_V3_Settings, 0, sizeof(SNMPV3_SETTINGS));
		stSNMP_V3_Settings.byEnabled = 1;
		stSNMP_V3_Settings.byAuthLevel = m_stPDUConfigInfo.byAuthLevel;
		memcpy(stSNMP_V3_Settings.byUserName, m_stPDUConfigInfo.bySNMPV3_UserName, sizeof(m_stPDUConfigInfo.bySNMPV3_UserName));
		stSNMP_V3_Settings.byAuthHashType = m_stPDUConfigInfo.byAuthHashType;
		stSNMP_V3_Settings.byPrivType = m_stPDUConfigInfo.byPrivType;

		if (m_stPDUConfigInfo.byAuthLevel == 1)
		{
			memcpy(stSNMP_V3_Settings.byAuthPassword, m_stPDUConfigInfo.byAuthPassword, sizeof(m_stPDUConfigInfo.byAuthPassword));
		}
		else if (m_stPDUConfigInfo.byAuthLevel == 2)
		{
			memcpy(stSNMP_V3_Settings.byAuthPassword, m_stPDUConfigInfo.byAuthPassword, sizeof(m_stPDUConfigInfo.byAuthPassword));
			memcpy(stSNMP_V3_Settings.byPrivPassword, m_stPDUConfigInfo.byPrivPassword, sizeof(m_stPDUConfigInfo.byPrivPassword));
		}

		memcpy(byDataBuffer, &stSNMP_V3_Settings, sizeof(SNMPV3_SETTINGS));
		byInfoSize = sizeof(SNMPV3_SETTINGS);

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_V3_SETTINGS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();
	}

	// ************** SNMP TRAP ENBABLE STATUS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.byEnbSNMPTrap;
	byInfoSize = 1;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_ENB_SNMP_TRAP, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** SNMP RECEIVER IP ADDRESS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.bySNMPRecevrField[0];
	byDataBuffer[1] = m_stPDUConfigInfo.bySNMPRecevrField[1];
	byDataBuffer[2] = m_stPDUConfigInfo.bySNMPRecevrField[2];
	byDataBuffer[3] = m_stPDUConfigInfo.bySNMPRecevrField[3];
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_RECEVR_IP_ADDRS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** SNMP TRAP PORT NUM **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.bySNMPTrapPortNum;
	byInfoSize = 1;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_SNMP_RECEVR_PORT_NUM, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** NTP  **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memset(&stNTP_Settings, 0, sizeof(NTP_SETTINGS_INFO));

	if (m_stPDUConfigInfo.bNtpEnabled)
	{
		// ************** ENABLE TIME ZONE SETTINGS **************
		stFloatConv.fAnswer = m_stPDUConfigInfo.fNtpZone;
		byDataBuffer[0] = m_stPDUConfigInfo.bNtpEnabled;
		byDataBuffer[1] = stFloatConv.byValue[0];
		byDataBuffer[2] = stFloatConv.byValue[1];
		byDataBuffer[3] = stFloatConv.byValue[2];
		byDataBuffer[4] = stFloatConv.byValue[3];
		byInfoSize = 5;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NTP_SETTINGS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();
	}
	else
	{
		// ************** DISABLE TIME ZONE SETTINGS **************
		stFloatConv.fAnswer = 0.0;
		byDataBuffer[0] = m_stPDUConfigInfo.bNtpEnabled;
		byDataBuffer[1] = stFloatConv.byValue[0];
		byDataBuffer[2] = stFloatConv.byValue[1];
		byDataBuffer[3] = stFloatConv.byValue[2];
		byDataBuffer[4] = stFloatConv.byValue[3];
		byInfoSize = 5;
		
		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NTP_SETTINGS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** MANUAL DATE AND TIME SETTINGS **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		memcpy(byDataBuffer, m_stPDUConfigInfo.byTimeZoneDateTime, sizeof(m_stPDUConfigInfo.byTimeZoneDateTime));
		byInfoSize = sizeof(m_stPDUConfigInfo.byTimeZoneDateTime);

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NTP_DATE_TIME, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();
	}

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// ------------------------ EMAIL SETTINGS --------------------------------------------------------------------------------------------

	// ************** EMAIL NOTIFICATION ENABLE STATUS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));

	byDataBuffer[0] = m_stPDUConfigInfo.byEnbEmailNotify;
	byInfoSize = 1;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_ENB_EMAIL_NOTIFY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** SMTP SERVER ADDRESS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.bySMTPServerAddrs, sizeof(m_stPDUConfigInfo.bySMTPServerAddrs));
	byInfoSize = sizeof(m_stPDUConfigInfo.bySMTPServerAddrs);

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_SMTP_SERVER_ADDRS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** SMTP PORT NUMBER **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.bySMTPPortNum, sizeof(m_stPDUConfigInfo.bySMTPPortNum));
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_SMTP_PORT_NUM, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** USER NAME FOR SENDER EMAIL DETAILS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.byFrmUserName, sizeof(m_stPDUConfigInfo.byFrmUserName));
	byInfoSize = sizeof(m_stPDUConfigInfo.byFrmUserName);

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_USER_NAME, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** PASSWORD FOR SENDER EMAIL DETAILS **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.byFrmPassword, sizeof(m_stPDUConfigInfo.byFrmPassword));
	byInfoSize = sizeof(m_stPDUConfigInfo.byFrmPassword);

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_PASSWORD, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** RECEIVER EMAIL ID **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.byRecevrEmailAddrs, sizeof(m_stPDUConfigInfo.byRecevrEmailAddrs));
	byInfoSize = sizeof(m_stPDUConfigInfo.byRecevrEmailAddrs);

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_EMAIL_RECEVR_EMAIL_ID, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// ------------------------ GENERAL SETTINGS ------------------------------------------------------------------------------------------

	// ************** PDU SYSTEM NAME **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.byGeneralSysNm, sizeof(m_stPDUConfigInfo.byGeneralSysNm));
	byInfoSize = sizeof(m_stPDUConfigInfo.byGeneralSysNm);

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GENERAL_SYSTEM_NAME, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** GENERAL SYSTEM LOCATION **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.byGeneralSysLoc, sizeof(m_stPDUConfigInfo.byGeneralSysLoc));
	byInfoSize = sizeof(m_stPDUConfigInfo.byGeneralSysLoc);

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GENERAL_SYSTEM_LOCATE, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** GENERAL SYSTEM CONTACT **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	memcpy(byDataBuffer, m_stPDUConfigInfo.byGeneralSysCont, sizeof(m_stPDUConfigInfo.byGeneralSysCont));
	byInfoSize = sizeof(m_stPDUConfigInfo.byGeneralSysCont);

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GENERAL_SYSTEM_CONTACT, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** GENERAL STAGGER ON DELAY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	byDataBuffer[0] = HIBYTE(HIWORD(m_stPDUConfigInfo.dwStaggerOnDelay)); 
	byDataBuffer[1] = LOBYTE(HIWORD(m_stPDUConfigInfo.dwStaggerOnDelay));
	byDataBuffer[2] = HIBYTE(LOWORD(m_stPDUConfigInfo.dwStaggerOnDelay)); 
	byDataBuffer[3] = LOBYTE(LOWORD(m_stPDUConfigInfo.dwStaggerOnDelay)); 
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_STAGGER_ON_DELAY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	// ************** GENERAL STAGGER OFF DELAY **************
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	byDataBuffer[0] = HIBYTE(HIWORD(m_stPDUConfigInfo.dwStaggerOffDelay)); 
	byDataBuffer[1] = LOBYTE(HIWORD(m_stPDUConfigInfo.dwStaggerOffDelay));
	byDataBuffer[2] = HIBYTE(LOWORD(m_stPDUConfigInfo.dwStaggerOffDelay)); 
	byDataBuffer[3] = LOBYTE(LOWORD(m_stPDUConfigInfo.dwStaggerOffDelay)); 
	byInfoSize = 4;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_STAGGER_OFF_DELAY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	byDataBuffer[0] = m_stPDUConfigInfo.byTelnetEnabled;
	byDataBuffer[1] = m_stPDUConfigInfo.byHTTPEnabled;
	byDataBuffer[2] = m_stPDUConfigInfo.byHTTPSEnabled;
	byInfoSize = 3;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_NET_SETTINGS, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
	cszPDUAddress.ReleaseBuffer();

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// ------------------------ PDU OUTLET SETTINGS ---------------------------------------------------------------------------------------
	for (nNum = 0; nNum < m_byMaxOutlets; nNum++)
	{
		ShowPopupDlg(DLG_POPUP_REFRESH);

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = (nNum + 1);
		memcpy(byDataBuffer + 1, m_stPDUConfigInfo.byOutletNm[nNum], sizeof(m_stPDUConfigInfo.byOutletNm[nNum]));
		byInfoSize = sizeof(m_stPDUConfigInfo.byOutletNm[nNum]) + 1;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_GET_SET_OUTLET_NAME, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ------------------------ ALERT SETTINGS ---------------------------------------------------------------------------------------------

		// VCB
		// ************** VIRTUAL CIRCUIT BREAKER ENABLE STATUS **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = m_stPDUConfigInfo.byEnbAlertVCB[nNum];
		byInfoSize = 2;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_ENB_VCB, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** MAX GRACE PERIOD **************
		stIeeConv.fValue = (float)m_stPDUConfigInfo.dwVCBMaxGracPer[nNum];
		dwValue = stIeeConv.nValue;

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = HIBYTE(HIWORD(dwValue)); 
		byDataBuffer[2] = LOBYTE(HIWORD(dwValue));
		byDataBuffer[3] = HIBYTE(LOWORD(dwValue)); 
		byDataBuffer[4] = LOBYTE(LOWORD(dwValue));
		byInfoSize = 5;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_VCB_MAX_GRAC_PER, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** THRESHOLD VALUE **************
		stIeeConv.fValue = m_stPDUConfigInfo.fVCBThresValue[nNum];
		dwValue = stIeeConv.nValue;

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = HIBYTE(HIWORD(dwValue)); 
		byDataBuffer[2] = LOBYTE(HIWORD(dwValue));
		byDataBuffer[3] = HIBYTE(LOWORD(dwValue)); 
		byDataBuffer[4] = LOBYTE(LOWORD(dwValue));
		byInfoSize = 5;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_VCB_THRES_VALUE, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);

		// LCA
		// ************** LOW CURRENT ALERT ENABLE STATUS **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = m_stPDUConfigInfo.byEnbAlertLCA[nNum];
		byInfoSize = 2;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_ENB_LCA, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** MAX GRACE PERIOD **************
		stIeeConv.fValue = (float)m_stPDUConfigInfo.dwLCAMaxGracPer[nNum];
		dwValue = stIeeConv.nValue;

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = HIBYTE(HIWORD(dwValue)); 
		byDataBuffer[2] = LOBYTE(HIWORD(dwValue));
		byDataBuffer[3] = HIBYTE(LOWORD(dwValue)); 
		byDataBuffer[4] = LOBYTE(LOWORD(dwValue));
		byInfoSize = 5;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_LCA_MAX_GRAC_PER, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** THRESHOLD VALUE **************
		stIeeConv.fValue = m_stPDUConfigInfo.fLCAThresValue[nNum];
		dwValue = stIeeConv.nValue;

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = HIBYTE(HIWORD(dwValue)); 
		byDataBuffer[2] = LOBYTE(HIWORD(dwValue));
		byDataBuffer[3] = HIBYTE(LOWORD(dwValue)); 
		byDataBuffer[4] = LOBYTE(LOWORD(dwValue));
		byInfoSize = 5;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_LCA_THRES_VALUE, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);

		// HCA
		// ************** HIGH CURRENT ALERT ENABLE STATUS **************
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = m_stPDUConfigInfo.byEnbAlertHCA[nNum];
		byInfoSize = 2;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_ENB_HCA, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** MAX GRACE PERIOD **************
		stIeeConv.fValue = (float)m_stPDUConfigInfo.dwHCAMaxGracPer[nNum];
		dwValue = stIeeConv.nValue;

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = HIBYTE(HIWORD(dwValue)); 
		byDataBuffer[2] = LOBYTE(HIWORD(dwValue));
		byDataBuffer[3] = HIBYTE(LOWORD(dwValue)); 
		byDataBuffer[4] = LOBYTE(LOWORD(dwValue));
		byInfoSize = 5;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_HCA_MAX_GRAC_PER, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		// ************** THRESHOLD VALUE **************
		stIeeConv.fValue = m_stPDUConfigInfo.fHCAThresValue[nNum];
		dwValue = stIeeConv.nValue;

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = (nNum + 1);
		byDataBuffer[1] = HIBYTE(HIWORD(dwValue)); 
		byDataBuffer[2] = LOBYTE(HIWORD(dwValue));
		byDataBuffer[3] = HIBYTE(LOWORD(dwValue)); 
		byDataBuffer[4] = LOBYTE(LOWORD(dwValue));
		byInfoSize = 5;

		PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_ALERT_HCA_THRES_VALUE, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		cszPDUAddress.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);
	}

	ShowPopupDlg(DLG_POPUP_DESTROY);

	AfxMessageBox(_T("Successfully configured PDU device"));

	return bRet;
}

void CPDUConfigDlg::OnBnClickedFctryReset()
{
	// TODO: Add your control notification handler code here
	// **************** FACTORY RESET *************************
	CString				cszPDUAddress;
	CString				cszLoad;
	UINT				nType = 0;
	BYTE				byDataBuffer[66] = {0};

	cszPDUAddress.Empty();

	if (m_nComboSel < (int)m_cszPDUList.GetCount())
	{
		cszPDUAddress = m_cszPDUList.ElementAt(m_nComboSel);
	}

	if (cszPDUAddress.IsEmpty())
		return;

	ShowPopupDlg(DLG_POPUP_CREATE);

	// byDataBuffer[0] = FACTORY_RESET_ON;

	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_FACTORY_RESET, byDataBuffer, 0, TRUE, NULL, TRUE);
	cszPDUAddress.ReleaseBuffer();

	for (nType = 1; nType <= 6; nType++)
	{
		ShowPopupDlg(DLG_POPUP_REFRESH);
	
		Sleep(600); // WAIT FOR RESETTING PDU
	}

	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(cszPDUAddress.GetBuffer(cszPDUAddress.GetLength()), PDU_FACTORY_RESET, byDataBuffer, 0, FALSE, NULL, TRUE);
	cszPDUAddress.ReleaseBuffer();

	cszLoad.Empty();
	if (byDataBuffer[0] == FACTORY_RESET_PASS)
	{
		cszLoad.LoadString(IDS_STR_FACTORY_RESET_PASS);
		nType = MB_OK | MB_ICONINFORMATION;
	}
	else if (byDataBuffer[0] == FACTORY_RESET_FAIL)
	{
		cszLoad.LoadString(IDS_STR_FACTORY_RESET_FAIL);
		nType = MB_OK | MB_ICONWARNING;
	}

	m_nComboSel = -1;
	OnCbnSelchangeComboPm10Pdu();

	ShowPopupDlg(DLG_POPUP_DESTROY);

	//m_cTabConfig.SetCurSel(0);
	//OnConfigTabChanged(NULL, NULL);

	AfxMessageBox(cszLoad, nType);
}

void CPDUConfigDlg::OnBnClickedClearAll()
{
	// TODO: Add your control notification handler code here
	// CLEAR ALL FIELDS

	// ********** CLEAR VALUES IN NETWORK PAGE ******************
	if (m_pPageNetwork)
	{
		m_pPageNetwork->m_cChkDHCP.SetCheck(BST_UNCHECKED);
		m_pPageNetwork->OnBnClickedDHCP();

		m_pPageNetwork->m_cIPAddrCtrl.SetAddress(0);

		m_pPageNetwork->m_cSubnetMsk.SetAddress(0);

		m_pPageNetwork->m_cGateway.SetAddress(0);

		m_pPageNetwork->m_cPrimDNS.SetAddress(0);

		m_pPageNetwork->m_cSecDNS.SetAddress(0);
	}

	// ********** OTHER Settings PAGE ******************
	if (m_pPageSNMP)
	{
		m_pPageSNMP->m_cChkSNMPTrap.SetCheck(BST_UNCHECKED);
		m_pPageSNMP->OnBnClickedCheckSnmpTrap();

		m_pPageSNMP->m_cReceIPAddr.SetAddress(0);

		m_pPageSNMP->SetDlgItemText(IDC_EDIT_SNMP_TRAP_PORT, _T(""));
	}

	// ********** SNMP Settings PAGE ******************
	if (m_pPageSNMPVer)
	{
		m_pPageSNMPVer->m_cEditV3ReadComm.SetWindowText(_T(""));
		m_pPageSNMPVer->m_cEditV3WriteComm.SetWindowText(_T(""));
	}

	// ********** CLEAR ALL IN EMAIL PAGE ******************
	if (m_pPageEmail)
	{
		m_pPageEmail->m_cChkEmail.SetCheck(BST_UNCHECKED);
		m_pPageEmail->OnBnClickedCheckEnableEmail();

		m_pPageEmail->m_cEditSMTPServer.SetWindowText(_T(""));

		m_pPageEmail->SetDlgItemText(IDC_EDIT_SMTP_PORT, _T(""));

		m_pPageEmail->m_cEditSMTPSendrUserNm.SetWindowText(_T(""));

		m_pPageEmail->m_cEditSMTPSendrPasswd.SetWindowText(_T(""));

		m_pPageEmail->m_cEditSMTPRecevrEmailId.SetWindowText(_T(""));
	}

	// ********** CLEAR ALL IN GENERAL PAGE ******************
	if (m_pPageGeneral)
	{
		m_pPageGeneral->m_cEditGenSysNm.SetWindowText(_T(""));

		m_pPageGeneral->m_cEditGenSysLoctn.SetWindowText(_T(""));

		m_pPageGeneral->m_cEditGenSysCont.SetWindowText(_T(""));

		m_pPageGeneral->SetDlgItemInt(IDC_EDIT_GEN_STAGGER_ON_DELAY, 0);
		m_pPageGeneral->SetDlgItemInt(IDC_EDIT_GEN_STAGGER_OFF_DELAY, 0);
	}

	// ********** CLEAR ALL OUTLET NAME ******************
	if (m_pPagePDU)
	{
		
	}

	// ********** CLEAR ALL IN ALERT PAGE ******************
	if (m_pPageAlert)
	{
		m_pPageAlert->m_cChkVCB.SetCheck(BST_UNCHECKED);
		m_pPageAlert->OnBnClickedChkVcb();

		m_pPageAlert->m_cEditVCBMaxGrcPer.SetWindowText(_T(""));

		m_pPageAlert->m_cEditVCBThresVal.SetWindowText(_T(""));

		m_pPageAlert->m_cChkLCA.SetCheck(BST_UNCHECKED);
		m_pPageAlert->OnBnClickedChkLca();

		m_pPageAlert->m_cEditLCAMaxGrcPer.SetWindowText(_T(""));

		m_pPageAlert->m_cEditLCAThresVal.SetWindowText(_T(""));

		m_pPageAlert->m_cChkHCA.SetCheck(BST_UNCHECKED);
		m_pPageAlert->OnBnClickedChkHca();

		m_pPageAlert->m_cEditHCAMaxGrcPer.SetWindowText(_T(""));

		m_pPageAlert->m_cEditHCAThresVal.SetWindowText(_T(""));
	}
}

BOOL CPDUConfigDlg::LoadVidPidValues()
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
	cszEpowerFile = cszBasePath;
	cszEpowerFile += STR_DEVICE_IDS;

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

void CPDUConfigDlg::ShowPopupDlg(BYTE byType)
{
	if (byType == DLG_POPUP_CREATE)
	{
		m_cPopUpWnd = new CPopUp();

		if (m_cPopUpWnd)
		{
			m_cPopUpWnd->Create(IDD_DLG_POPUP, NULL);
			m_cPopUpWnd->AnimateWindow(600, AW_ACTIVATE | AW_BLEND);
			m_cPopUpWnd->ShowWindow(TRUE);
		}
	}
	else if (byType == DLG_POPUP_REFRESH)
	{
		MSG				msg;
		MSG				msg1;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_cPopUpWnd)
			{
				if (!::IsDialogMessage(m_cPopUpWnd->m_hWnd, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			if (!::IsDialogMessage(this->m_hWnd, &msg1))
			{
				TranslateMessage(&msg1);
				DispatchMessage(&msg1);
			}
		}
	}
	else if (byType == DLG_POPUP_DESTROY)
	{
		if (m_cPopUpWnd)
		{
			m_cPopUpWnd->AnimateWindow(600, AW_HIDE | AW_BLEND);
			m_cPopUpWnd->DestroyWindow();
			delete m_cPopUpWnd;
			m_cPopUpWnd = NULL;
		}
	}
}

void CPDUConfigDlg::InitiateTimeZoneValues()
{
	m_fTimeZones[0] = -12.0;
	m_fTimeZones[1] = -11.0;
	m_fTimeZones[2] = -10.0;
	m_fTimeZones[3] = -9.0;
	m_fTimeZones[4] = -8.0;
	m_fTimeZones[5] = -7.0;
	m_fTimeZones[6] = -6.0;
	m_fTimeZones[7] = -5.0;
	m_fTimeZones[8] = -4.0;
	m_fTimeZones[9] = -3.5;
	m_fTimeZones[10] = -3.0;
	m_fTimeZones[11] = -2.0;
	m_fTimeZones[12] = -1.0;
	m_fTimeZones[13] = 0.0;
	m_fTimeZones[14] = 1.0;
	m_fTimeZones[15] = 2.0;
	m_fTimeZones[16] = 3.0;
	m_fTimeZones[17] = 3.5;
	m_fTimeZones[18] = 4.0;
	m_fTimeZones[19] = 4.5;
	m_fTimeZones[20] = 5.0;
	m_fTimeZones[21] = 5.5;
	m_fTimeZones[22] = 5.75;
	m_fTimeZones[23] = 6.0;
	m_fTimeZones[24] = 7.0;
	m_fTimeZones[25] = 8.0;
	m_fTimeZones[26] = 9.0;
	m_fTimeZones[27] = 9.5;
	m_fTimeZones[28] = 10.0;
	m_fTimeZones[29] = 11.0;
	m_fTimeZones[30] = 12.0;
	m_fTimeZones[31] = 13.0;
	m_fTimeZones[32] = 14.0;
}

int CPDUConfigDlg::GetTimeZonePos(float fValue)
{
	BOOL	bFound = FALSE;
	int		nNum = 0;

	for (nNum = 0; nNum < 33; nNum++)
	{
		if (m_fTimeZones[nNum] == fValue)
		{
			bFound = TRUE;
			break;
		}
	}

	if (bFound)
	{
		return nNum;
	}
	else
	{
		return -1;
	}
}