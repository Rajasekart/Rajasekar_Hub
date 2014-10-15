// ChangeOCBDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDUTestTool.h"
#include "ChangeOCBDlg.h"
#include "DlgPDUSettings.h"
#include "SummaryDlg.h"

// CChangeOCBDlg dialog

IMPLEMENT_DYNAMIC(CChangeOCBDlg, CDialog)

CChangeOCBDlg::CChangeOCBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChangeOCBDlg::IDD, pParent)
{
	m_bGreen = FALSE;
	m_bProcessStarted = FALSE;
	m_cszPDUAddr.Empty();
	m_cszIniFilePath.Empty();

	m_cszIniFilePath.Empty();
	m_cszIniFilePath = theApp.m_cszIniFilePath;

	m_cszLogFolderPath.Empty();
	m_cszLogFilePath.Empty();

	memset(&m_stPduSettings, 0, sizeof(PDU_SETTINGS));
}

CChangeOCBDlg::~CChangeOCBDlg()
{
}

void CChangeOCBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUT_CHANGE, m_cButChange);
	DDX_Control(pDX, IDC_LIST_OCB, m_cListCtrlOCB);
}


BEGIN_MESSAGE_MAP(CChangeOCBDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CChangeOCBDlg::OnBnClickedButtonRefresh)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUT_CHANGE, &CChangeOCBDlg::OnBnClickedButChange)
	ON_BN_CLICKED(IDC_BUTTON_PDU_SETGS, &CChangeOCBDlg::OnBnClickedButtonPduSetgs)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_LOG, &CChangeOCBDlg::OnBnClickedButtonSaveLog)
	ON_BN_CLICKED(IDC_BUTTON_CLS, &CChangeOCBDlg::OnBnClickedButtonCls)
END_MESSAGE_MAP()


// CChangeOCBDlg message handlers

void CChangeOCBDlg::OnBnClickedButtonRefresh()
{
	// TODO: Add your control notification handler code here
	RefreshPDUList();
}

void CChangeOCBDlg::RefreshPDUList(BOOL bSilent)
{
	WORD						wPid = 0;
	WORD						wVid = 0;
	BYTE						byErrCode = 0;
	CString						cszLoad;
	BOOL 						bRetStatus = FALSE;

	m_bGreen = FALSE;
	cszLoad.Empty();
	m_cszPDUAddr.Empty();

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
		SetDlgItemText(IDC_STATIC_PDU_DET, _T("PDU Device found"));
		ViewTestCtrls(TRUE);

		m_cszPDUAddr.Empty();
		m_cszPDUAddr = theApp.m_cszPDUAddr;
	}
	else
	{
		ViewTestCtrls(FALSE);
		SetDlgItemText(IDC_STATIC_PDU_DET, _T("No PDU device found"));
		if (!bSilent)
		{
			cszLoad = _T("No PDU device connected to the system. Press 'Refresh' button again, after connected the PDU device.");
			AfxMessageBox(cszLoad, MB_ICONSTOP | MB_OK);
		}
	}
}
HBRUSH CChangeOCBDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PDU_DET)
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

void CChangeOCBDlg::ViewTestCtrls(BOOL bEnable)
{
	m_cButChange.EnableWindow(bEnable);
}

BOOL CChangeOCBDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	DWORD			dwStyle = 0;

	RefreshPDUList();

	dwStyle = m_cListCtrlOCB.GetExtendedStyle();
	m_cListCtrlOCB.SetExtendedStyle(dwStyle | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	m_cListCtrlOCB.InsertColumn(0, _T(""), LVCFMT_LEFT);
	m_cListCtrlOCB.SetColumnWidth(0, 524);

	GetDlgItem(IDC_EDIT_SYS_ID)->SetWindowText(_T("001"));
	((CEdit *)GetDlgItem(IDC_EDIT_SYS_ID))->SetLimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_CURR_ID))->SetLimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT_NEW_ID))->SetLimitText(3);

	SetDlgItemInt(IDC_EDIT_CURR_ID, 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeOCBDlg::OnBnClickedButChange()
{
	// TODO: Add your control notification handler code here
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
		OnBnClickedButtonCls();

		cszLoad = _T("&STOP");
		m_cButChange.SetWindowText(cszLoad);

		ChangeOCBID();

		cszLoad = _T("&CHANGE");
		m_cButChange.SetWindowText(cszLoad);

		m_bProcessStarted = FALSE;
	}
	else
	{
		cszLoad = _T("&CHANGE");
		m_cButChange.SetWindowText(cszLoad);
	}
}

BOOL CChangeOCBDlg::PrintBufferInTestInfoList(CString cszBuffer, int nTxtColor)
{
	int					nRow = 0;
	LVITEM				lvItem = {0};

	nRow = m_cListCtrlOCB.GetItemCount();

	memset(&lvItem, 0, sizeof(LVITEM));
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = nRow;
	lvItem.pszText = cszBuffer.GetBuffer(cszBuffer.GetLength());
	cszBuffer.ReleaseBuffer();
	lvItem.cchTextMax = cszBuffer.GetLength();
	lvItem.iSubItem = 0;
	lvItem.lParam = nTxtColor;
	nRow = m_cListCtrlOCB.InsertItem(&lvItem);

	if (nRow == MINUS_ONE)
		return FALSE;

	nRow = m_cListCtrlOCB.GetItemCount();
	if (nRow > 0)
	{
		m_cListCtrlOCB.EnsureVisible((nRow - 1), FALSE);
	}
	
	return TRUE;
}

CString CChangeOCBDlg::GetRelayText(BYTE byRelayState)
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

BOOL CChangeOCBDlg::ChangeOCBID()
{
	BYTE			byOCB2 = 0;
	BYTE			byNum = 0;
	BYTE			byCount = 0;
	BYTE			byTestOCB = 0;
	BYTE			byCurrOCB = 0;
	BYTE			byNewOCB = 0;
	BYTE			byOutlet = 0;
	BYTE			byCurrState = 0;
	BYTE			byErrorCode = 0;
	BOOL			bResult = FALSE;
	BOOL			bTestFail = FALSE;
	BOOL			bChangeOCB_Result = FALSE;
	CString			cszLoad;
	BYTE			byOutletResult[100] = {0};

	byCurrOCB = GetDlgItemInt(IDC_EDIT_CURR_ID);
	byNewOCB = GetDlgItemInt(IDC_EDIT_NEW_ID);

	byOCB2 = theApp.PduGetHardwareModel(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byCurrOCB, &byErrorCode, TRUE);
	m_cszPDUAddr.ReleaseBuffer();

	if (!m_bProcessStarted)
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to continue... Process stopped by user.");
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

	if (!byCurrOCB)
	{
		cszLoad.Empty();
		cszLoad = _T("Current OCB ID cannot be empty. Please enter the valid Current OCB ID");
		//PrintBufferInTestInfoList(cszLoad, RED_TEXT);
		AfxMessageBox(cszLoad, MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_CURR_ID, _T(""));
		GetDlgItem(IDC_EDIT_CURR_ID)->SetFocus();

		return FALSE;
	}

	if (!byNewOCB)
	{
		cszLoad.Empty();
		cszLoad = _T("New OCB ID cannot be empty. Please enter the valid New OCB ID");
		//PrintBufferInTestInfoList(cszLoad, RED_TEXT);
		AfxMessageBox(cszLoad, MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_NEW_ID, _T(""));
		GetDlgItem(IDC_EDIT_NEW_ID)->SetFocus();

		return FALSE;
	}

	byCount = 0;
	bTestFail = FALSE;
	for (byNum = 0; byNum <= 1; byNum++)
	{
		if (!byNum)
		{
			byTestOCB = byCurrOCB;
		}
		else if (byNum)
		{
			byTestOCB = byNewOCB;
		}

		memset(byOutletResult,0x01,sizeof(byOutletResult));

		for (byOutlet = 1; byOutlet <= m_stPduSettings.byNumOfOutletsPerOCB; byOutlet++)
		{
			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			cszLoad.Empty();
			cszLoad.Format(_T("Testing Relay : Bank %u"), byTestOCB);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			RefreshOurApp();

			// Start Outlet
			cszLoad.Empty();
			cszLoad.Format(_T("Testing Relay : Outlet %u"), byOutlet);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			RefreshOurApp();

			cszLoad.Empty();
			cszLoad.Format(_T("Rebooting the Outlet %u..."), byOutlet);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			RefreshOurApp();

			if (!m_bProcessStarted)
			{
				cszLoad.Empty();
				cszLoad = _T("Unable to continue... Process stopped by user.");
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				return FALSE;
			}

			bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byTestOCB, byOutlet, RELAY_REBOOT, m_stPduSettings.dwStaggerOnDelay, &byErrorCode, TRUE);
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

			cszLoad.Empty();
			cszLoad.Format(_T("Turning power OFF the Outlet %u..."), byOutlet);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byTestOCB, byOutlet, RELAY_OPEN, m_stPduSettings.dwStaggerOffDelay, &byErrorCode, TRUE);
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

			bResult = theApp.PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byTestOCB, byOutlet, &byCurrState, &byErrorCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();

			if (byCurrState != RELAY_OPEN)
			{
				bTestFail = TRUE;
				byOutletResult[byCount] = 0;
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
			cszLoad.Format(_T("Turning power ON the Outlet %u..."), byOutlet);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);
			
			//bResult = (*fnSetOutletState) ((CHAR *)m_stPduAddress.byPduAdd[0], byOCB, byOCB, byOutlet, byOutlet, RELAY_CLOSE, &byErrorCode, TRUE);
			bResult = theApp.PduSwitchOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byTestOCB, byOutlet, RELAY_CLOSE, m_stPduSettings.dwStaggerOnDelay, &byErrorCode, TRUE);
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

			bResult = theApp.PduGetOutletState(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), byTestOCB, byOutlet, &byCurrState, &byErrorCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();

			if (byCurrState != RELAY_CLOSE)
			{
				bTestFail = TRUE;
				byOutletResult[byCount] = 0;
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
			cszLoad.Format(_T("Outlet %u present relay state is %s"), byOutlet, GetRelayText(byCurrState));
			PrintBufferInTestInfoList(cszLoad);

			byCount++;
		}

		if (bTestFail)
		{
			cszLoad.Empty();
			cszLoad = _T("Testing Relay Fail. Unable to continue the process.");
			PrintBufferInTestInfoList(cszLoad, RED_TEXT);
			break;
		}

		if (!byNum)
		{
			// CHANGE OCB ID
			cszLoad.Empty();
			cszLoad.Format(_T("Changing the OCB ID from %02d to %02d"),byCurrOCB,byNewOCB);
			PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

			RefreshOurApp();

			if(theApp.PduChangeOCBid(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()),byCurrOCB,byNewOCB, &byErrorCode,TRUE))
			{
				cszLoad.Empty();
				cszLoad.Format(_T("Changed the OCB ID from %02d to %02d successfully"),byCurrOCB,byNewOCB);
				PrintBufferInTestInfoList(cszLoad, GREEN_TEXT);

				RefreshOurApp();

				if (!byOCB2)
				{
					cszLoad.Empty();
					cszLoad = _T("Resetting the device.....");
					PrintBufferInTestInfoList(cszLoad, BLUE_TEXT);

					theApp.PduResetOCB(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()),byCurrOCB, &byErrorCode,TRUE);
				}

				bChangeOCB_Result = TRUE;
			}
			else
			{
				cszLoad.Empty();
				cszLoad.Format(_T("Failed to change the OCB ID from %02d to %02d"),byCurrOCB,byNewOCB);
				PrintBufferInTestInfoList(cszLoad, RED_TEXT);

				bChangeOCB_Result = FALSE;
			}

			if (byOCB2)
				break;
		}
	}

	CSummaryDlg		cSummDlg;

	cSummDlg.m_bChangeOCB = TRUE;
	cSummDlg.m_bChangeOCB_Result = bChangeOCB_Result;
	cSummDlg.m_bRelayResult = bChangeOCB_Result;

	if (!byNum)
	{
		cSummDlg.m_byTotBanks = 1;
		cSummDlg.m_bySelBanks[0] = byCurrOCB;
	}
	else
	{
		cSummDlg.m_byTotBanks = 2;
		cSummDlg.m_bySelBanks[0] = byCurrOCB;
		cSummDlg.m_bySelBanks[1] = byNewOCB;
	}

	cSummDlg.m_byTotOutlets = cSummDlg.m_byTotBanks * m_stPduSettings.byNumOfOutletsPerOCB;
	cSummDlg.m_byOutletsPerBank = m_stPduSettings.byNumOfOutletsPerOCB;

	for (byNum = 0; byNum < cSummDlg.m_byTotOutlets; byNum++)
	{
		if (byOutletResult[byNum] == 0)
		{
			cSummDlg.m_bRelayResult = FALSE;
		}

		cSummDlg.m_byOutletResult[byNum] = byOutletResult[byNum];
	}

	cSummDlg.DoModal();
	
	return bResult;
}

void CChangeOCBDlg::RefreshOurApp()
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

void CChangeOCBDlg::OnBnClickedButtonPduSetgs()
{
	// TODO: Add your control notification handler code here
	CDlgPDUSettings		cPduSettings(m_cszIniFilePath, TRUE);

	cPduSettings.DoModal();

	ReadSettingsFile();
}

void CChangeOCBDlg::OnBnClickedButtonSaveLog()
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
	m_cszLogFilePath += STR_PDU_CHNG_ID_LOG;

	if (cFinder.FindFile(m_cszLogFilePath))
	{
		cFinder.Close();
		::DeleteFile(m_cszLogFilePath);
	}

	cLogFile.Open(m_cszLogFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &cFileEx);

	nTotal = m_cListCtrlOCB.GetItemCount();

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		cszLoad.Empty();
		cszLoad = m_cListCtrlOCB.GetItemText(nNum, 0);
		cszLoad += STR_NEXT_LINE;
		cLogFile.Write(cszLoad, cszLoad.GetLength());
	}
	
	cszLoad.Empty();
	cszLoad.Format(_T("Successfully created the log file in this path %s"), m_cszLogFilePath);
	AfxMessageBox(cszLoad);
}

BOOL CChangeOCBDlg::ReadSettingsFile()
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

void CChangeOCBDlg::OnBnClickedButtonCls()
{
	// TODO: Add your control notification handler code here
	m_cListCtrlOCB.DeleteAllItems();
}

void CChangeOCBDlg::WaitingText(int nSleep)
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

void CChangeOCBDlg::DeleteLastRow()
{
	int			nRow = 0;

	nRow = m_cListCtrlOCB.GetItemCount();
	if (nRow > 0)
	{
		m_cListCtrlOCB.DeleteItem(nRow - 1);
	}
}