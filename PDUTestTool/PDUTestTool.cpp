// PDUTestTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PDUTestTool.h"
#include "MainDlg.h"
#include "ChangeOCBDlg.h"
#include "PDUTestToolDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDUTestToolApp

BEGIN_MESSAGE_MAP(CPDUTestToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPDUTestToolApp construction

CPDUTestToolApp::CPDUTestToolApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPDUTestToolApp object

CPDUTestToolApp theApp;


// CPDUTestToolApp initialization

BOOL CPDUTestToolApp::InitInstance()
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
	TCHAR					szNodeName[1024] = {0};
	HANDLE 					hMapObjectSingleInst = NULL;


	cszLoad.Empty();
	//cszLoad.LoadString(AFX_IDS_APP_TITLE);
	cszLoad = _T("PDUTestTool");

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

	m_cszLogFolderPath.Empty();
	m_cszLogFolderPath = szNodeName;

	INT_PTR		nResponse = NULL;
	CMainDlg	cMainDlg;

	cMainDlg.DoModal();
	if (cMainDlg.m_byOption == OPT_OCB_CHANGE)
	{
		CChangeOCBDlg	dlg;
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
	}
	else if (cMainDlg.m_byOption == OPT_RELAY_TEST)
	{
		CPDUTestToolDlg dlg;
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
	}

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

CString CPDUTestToolApp::GetLogPathWithDate(CString cszLogPath)
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

BOOL CPDUTestToolApp::PduGetOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog)
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

BYTE CPDUTestToolApp::PduGetHardwareModel(CHAR *byPduadd, BYTE byOCB, BYTE *byErrCode, BOOL bLog)
{
	BYTE								byStateBuff[MAX_PDU_BUFF_SIZE] = {0};		
	DWORD								dwByteWritten = 0;

	/* -----------------------------------------------------------------------
		Send the Pass through Command to devcie
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	memset(byStateBuff, 0, sizeof(byStateBuff));
	byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	byStateBuff[1] = HARDWARE_MODEL;
	byStateBuff[2] = byOCB;
	
	PduWriteReadBuffer(byPduadd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);

	if ((byStateBuff[1] >= PM10_4_CHANNELS) && (byStateBuff[1] <= PM10_PLUS_NETWORK_3_PHASE_DELTA))
		return 0;
	else
		return 1;
}

BOOL CPDUTestToolApp::PduSwitchOutletState(CHAR *byPduadd, BYTE byOCB, BYTE byOutlet, BYTE byCurrState, DWORD dwDelaySecs, BYTE *byErrCode, BOOL bLog)
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

BOOL CPDUTestToolApp::PduChangeOCBid(CHAR *byPduadd, BYTE byCurrOCB, BYTE byNewOCB,BYTE *byErrCode, BOOL bLog)
{
	BOOL									bRet = FALSE;
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
	wsprintf(bySection, "%s_%s_%d", ST_SECT_CHANGEOCB, szTmpstring, byCurrOCB);

	/* -----------------------------------------------------------------------
		Change the Nodeid
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	memset(byOCBStateBuff, 0, sizeof(byOCBStateBuff));
	byOCBStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	byOCBStateBuff[1] = RELAY_CHANGE;
	byOCBStateBuff[2] = byCurrOCB;
	byOCBStateBuff[3] = byNewOCB;
	byOCBStateBuff[4] = 0x00;
	byOCBStateBuff[5] = 0x00;

	Sleep(2000);

	PduWriteReadBuffer(byPduadd, byOCBStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);

	if ((byOCBStateBuff[1] == byNewOCB))
	{
		bRet = TRUE;
		WriteLogDetails(bySection, _T("OCB STATE CHANGED -- PASS"));
	}
	else 
	{
		bRet = FALSE;
		WriteLogDetails(bySection, _T("OCB STATE CHANGED -- FAIL"));
	}

	///* -----------------------------------------------------------------------
	//	Reset the devcie
	//----------------------------------------------------------------------- */
	//if(bRet)
	//{
	//	dwByteWritten = 0;
	//	memset(byOCBStateBuff, 0, sizeof(byOCBStateBuff));
	//	byOCBStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	//	byOCBStateBuff[1] = RELAY_RESET;
	//	byOCBStateBuff[2] = byCurrOCB;
	//	byOCBStateBuff[3] = 0x00;
	//	byOCBStateBuff[4] = 0x00;
	//	byOCBStateBuff[5] = 0x00;

	//	PduWriteReadBuffer(byPduadd, byOCBStateBuff, &dwByteWritten, TRUE, byErrCode, bLog);
	//	Sleep(5000);
	//}

	return bRet;
}
BOOL CPDUTestToolApp::PduResetOCB(CHAR *byPduadd, BYTE byCurrOCB, BYTE *byErrCode, BOOL bLog)
{
	BOOL									bRet = FALSE;
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
	wsprintf(bySection, "%s_%s_%d", ST_SECT_CHANGEOCB, szTmpstring, byCurrOCB);

	/* -----------------------------------------------------------------------
		Reset the devcie
	----------------------------------------------------------------------- */
	
	dwByteWritten = 0;
	memset(byOCBStateBuff, 0, sizeof(byOCBStateBuff));
	byOCBStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	byOCBStateBuff[1] = RELAY_RESET;
	byOCBStateBuff[2] = byCurrOCB;
	byOCBStateBuff[3] = 0x00;
	byOCBStateBuff[4] = 0x00;
	byOCBStateBuff[5] = 0x00;

	PduWriteReadBuffer(byPduadd, byOCBStateBuff, &dwByteWritten, TRUE, byErrCode, bLog);
	Sleep(5000);

	return bRet;
}

BOOL CPDUTestToolApp::PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog)
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

BOOL CPDUTestToolApp::LoadVidPidValues()
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

BOOL CPDUTestToolApp::PduEnumerateDevice(BOOL bWrLog)
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

BOOL CPDUTestToolApp::WriteLogDetails(CString cszSection, CString cszData)
{
	BOOL			bRet = TRUE;

	if ((!m_cszDevLogFile.IsEmpty()) && (!cszSection.IsEmpty()) && (!cszData.IsEmpty()))
	{
		WritePrivateProfileString(STR_PDU_LOG, cszSection, cszData, m_cszDevLogFile);
	}

	return bRet;
}