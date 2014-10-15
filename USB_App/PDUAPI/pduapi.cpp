// pduapi.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "pduapi.h"
#include "general.h"
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

//Application global variables 

	CHAR			szLogPath[MAX_PATH] = {0};
	CString			cszPDUConfIniPath;
	CString			cszPDUAlertMessIniPath;
	union IntFloat {  __int32 i;  float f;  };
	union IntFloat val; 

	BOOL			g_bReadWrite = FALSE;
	BOOL			g_bQuickExit = FALSE;
	CStringArray	gcszPidValues;
	CStringArray	gcszVidValues;
	CString			gcszBasePath;
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//


// CpduapiApp

BEGIN_MESSAGE_MAP(CpduapiApp, CWinApp)
END_MESSAGE_MAP()


// CpduapiApp construction

CpduapiApp::CpduapiApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	memset(szLogPath, 0, sizeof(szLogPath));
	memset(szNodeName, 0x00, sizeof(szNodeName));
	GetModuleFileName(NULL, szNodeName, sizeof(szNodeName));
	ASSERT(lstrlen(szNodeName));
	*(strrchr(szNodeName, '\\') + 1) = '\0';
	lstrcpy(szLogPath, szNodeName);
	
	cszPDUConfIniPath.Empty();
	cszPDUConfIniPath = szNodeName;
	cszPDUConfIniPath += STR_PDU_CONG_INI_NM;

	cszPDUAlertMessIniPath.Empty();
	cszPDUAlertMessIniPath = szNodeName;
	cszPDUAlertMessIniPath += STR_PDU_ALERT_MESS_INI_NM;

	gcszBasePath.Empty();
	gcszBasePath = szNodeName;

	lstrcat(szLogPath, "pduapi.log");

	m_byThreadExit.RemoveAll();
	m_AlertThreadArray.RemoveAll();
}

// The one and only CpduapiApp object

CpduapiApp theApp;


// CpduapiApp initialization

BOOL CpduapiApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

BOOL LoadVidPidValues()
{
	BOOL			bFoundOk = TRUE;
	UINT			nNum = 0;
	UINT			nTotal = 0;
	CString			cszLoad;
	CString			cszSection;
	CString			cszKey;
	CString			cszEpowerFile;
	TCHAR			szBuffer[MAX_PATH] = {0};

	gcszPidValues.RemoveAll();
	gcszVidValues.RemoveAll();

	cszEpowerFile.Empty();
	cszEpowerFile = gcszBasePath;
	cszEpowerFile += STR_PDU_UNITS;

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
		gcszVidValues.Add(cszLoad);

		// PID Values
		cszKey.Empty();
		cszKey.Format(STR_PID_NO, nNum);

		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		gcszPidValues.Add(cszLoad);
	}

	if (!nTotal)
	{
		bFoundOk = FALSE;
	}

	return bFoundOk;
}

/* -----------------------------------------------------------------------
	1) PduEnumerateDevice

	This Function detects the PDU device connected in the system and fill
	the number of PDU device and address of the all PDU device in the
	PPDU_ADDRESS structure Pointer.
	----------------------------------------------------------------------- */

__declspec(dllexport) BOOL PduEnumerateDevice(PPDU_ADDRESS pPdu_address, WORD wPid, WORD wVid, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	BOOL										bRet = FALSE;
	BOOL										bPidVidOk = FALSE;
	UINT										nNum = 0;
	CString										cszPid;
	CString										cszVid;
	CHAR										byDevPath[MAX_PATH] = {0};
	BOOL										bDeviceDetected = FALSE; 
	ULONG										Length = 0;
	int											MemberIndex = 0;
	HIDD_ATTRIBUTES								Attributes;
	SP_DEVICE_INTERFACE_DATA					devInfoData;
	HANDLE										hDevInfo = INVALID_HANDLE_VALUE;
	LONG										Result = 0;	
	PSP_DEVICE_INTERFACE_DETAIL_DATA			detailData = NULL;
	GUID										HidGuid;
	ULONG										Required = 0;
	HANDLE										DeviceHandle = INVALID_HANDLE_VALUE;

	LoadVidPidValues();		// Load all our device id list

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

	MemberIndex = 0;


	for (MemberIndex = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData); ++MemberIndex)
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

		Result = SetupDiGetDeviceInterfaceDetail(hDevInfo,	&devInfoData, NULL, 0, &Length, NULL);


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

		DeviceHandle = CreateFile (detailData->DevicePath, 
											0, 
											FILE_SHARE_READ|FILE_SHARE_WRITE, 
											(LPSECURITY_ATTRIBUTES)NULL,
											OPEN_EXISTING, 
											0, 
											NULL);


		//DisplayLastError("CreateFile: "); 

		if (DeviceHandle != INVALID_HANDLE_VALUE)
		{
			CONFIRMLOG(ST_SECT_PDUENUM, "DeviceHandle", ("Pass"), szLogPath, bLog);
		}
		else
		{
			CONFIRMLOG(ST_SECT_PDUENUM, "DeviceHandle", ("Fail"), szLogPath, bLog);
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

		Result = HidD_GetAttributes (DeviceHandle, &Attributes);
		
		//DisplayLastError("HidD_GetAttributes: ");
		CONFIRMLOG(ST_SECT_PDUENUM, "DeviceAttrib", ("HidD_GetAttributes: "), szLogPath, bLog);
		
		//Is it the desired device?
		CONFIRMLOG(ST_SECT_PDUENUM, "Device VID", ("0x%04X", Attributes.VendorID), szLogPath, bLog);
		CONFIRMLOG(ST_SECT_PDUENUM, "Device PID", ("0x%04X", Attributes.ProductID), szLogPath, bLog);

		bPidVidOk = FALSE;
		for (nNum = 0; nNum < (UINT)gcszVidValues.GetSize(); nNum++)
		{
			cszPid.Empty();
			cszPid.Format(_T("0x%04X"), Attributes.ProductID);

			cszVid.Empty();
			cszVid.Format(_T("0x%04X"), Attributes.VendorID);

			if ((!gcszVidValues.ElementAt(nNum).CompareNoCase(cszVid)) && (!gcszPidValues.ElementAt(nNum).CompareNoCase(cszPid)))
			{
				bPidVidOk = TRUE;
			}
		}

		if (bPidVidOk) //((Attributes.VendorID == wVid) && (Attributes.ProductID == wPid))
		{
			//Both the Vendor ID and Product ID match.
			bDeviceDetected = TRUE;
			bRet = TRUE;

		/* -----------------------------------------------------------------------
			Device Path is needed to create the device handle.
			----------------------------------------------------------------------- */
			memcpy(pPdu_address->byPduAdd[pPdu_address->byTotalPdu],detailData->DevicePath, strlen(detailData->DevicePath)); 
			pPdu_address->byTotalPdu++; 

			//DisplayData("Device detected");
			CONFIRMLOG(ST_SECT_PDUENUM, "Device detected", ("%s", detailData->DevicePath), szLogPath, bLog);

			memset(byDevPath, PDU_BUFF_NULL, sizeof(byDevPath));
			lstrcpy(byDevPath, detailData->DevicePath);
			GetDeviceCapabilities(byDevPath, bLog, DeviceHandle);
#if 0

		  // Find out if the device is a system mouse or keyboard.
		  	
		  DeviceUsage = (Capabilities.UsagePage * 256) + Capabilities.Usage;

		  if (DeviceUsage == 0x102)
		  {
		  	UsageDescription = "mouse";
		  }
		  
		  if (DeviceUsage == 0x106)
		  {
		  	UsageDescription = "keyboard";
		  }

		  if ((DeviceUsage == 0x102) | (DeviceUsage == 0x106)) 
		  {
		  	DisplayData("");
		  	DisplayData("*************************");
		  	DisplayData("The device is a system " + UsageDescription + ".");
		  	DisplayData("Windows 2000 and Windows XP don't allow applications");
		  	DisplayData("to directly request Input reports from or "); 
		  	DisplayData("write Output reports to these devices.");
		  	DisplayData("*************************");
		  	DisplayData("");
 		  }

#endif
			// Prepare to read reports using Overlapped I/O.
			//PrepareForOverlappedTransfer();
			//Test(detailData->DevicePath);

			CloseHandle(DeviceHandle);
			DeviceHandle = INVALID_HANDLE_VALUE;
	 	} 
		else
		{
			//The Vendor ID doesn't match.
			CloseHandle(DeviceHandle);
			DeviceHandle = INVALID_HANDLE_VALUE;
		}

		//Free the memory used by the detailData structure (no longer needed).
		free(detailData);

	}//for loop

	if (bDeviceDetected == FALSE)
	{
		*byErrCode = 0x01;
		CONFIRMLOG(ST_SECT_PDUENUM, "PDU detected", ("No"), szLogPath, bLog);
	}
	else
	{
		CONFIRMLOG(ST_SECT_PDUENUM, "PDU detected", ("Yes"), szLogPath, bLog);
	}

	if (DeviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(DeviceHandle);
		DeviceHandle = INVALID_HANDLE_VALUE;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}


/* -----------------------------------------------------------------------
	2) PduGetBankOcbInformation

	This Function takes the PDU address as input parameter and then
	it returns the total number of Banks and the number of OCBs in
	each Bank.
	----------------------------------------------------------------------- */

__declspec(dllexport) BOOL PduGetBankOcbInformation(CHAR *byPduadd, PBANK_OUTLET_INFO pBankOcbInfo, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	BOOL									bRet = FALSE;
	CHAR									bySection[MAX_PATH] = {0};
	CHAR									byLocalBuf[MAX_PATH] = {0};
	CHAR									byInfoStr[60] = {0};
	DWORD									dwByteWritten = 0;
	DWORD									NumberOfBytesRead = 0;
	BYTE									InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};	//Allocate a memory buffer equal to our endpoint size + 1
	BYTE									byCount = 0;
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
	wsprintf(bySection, "%s_%s", ST_SECT_BANKOCB_INFO, szTmpstring);

	/* -----------------------------------------------------------------------
		Send the Pass through Command to Read the Number of Banks.
	----------------------------------------------------------------------- */

	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID;			//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = PDU_STATIC_READ;	//for Read Static Information
	InputPacketBuff[2] = PDU_TOTAL_BANKS;

	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%x_%d", ST_SECT_STATIC_RDWR, szTmpstring, PDU_STATIC_READ, PDU_TOTAL_BANKS);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	if (PDU_TOTAL_BANKS == InputPacketBuff[1])
	{
		CONFIRMLOG(bySection, "Data", ("%s",  "GoodData"), szLogPath, bLog);

	
	#if 0
		/* -----------------------------------------------------------------------
		Copy the Buffer
		----------------------------------------------------------------------- */
		for (byCount = 0; byCount < 4; byCount++)
		{
			if ((*(InputPacketBuff+(2+byCount)) != PDU_BUFF_END) && (*(InputPacketBuff+(2+byCount)) != PDU_BUFF_NULL))
			{
				*(byInfoStr + byCount) =  *(InputPacketBuff+(2+byCount));
			}
			else
			if ((*(InputPacketBuff+(2+byCount)) == PDU_BUFF_END) || (*(InputPacketBuff+(2+byCount)) == PDU_BUFF_NULL))
			{
				bRet = TRUE;
				break;
			}
		}
	#else
		for (byCount = 0; byCount < InputPacketBuff[2]; byCount++)
		{
			*(byInfoStr + byCount) =  *(InputPacketBuff + (3 + byCount));
		}
	#endif

	/* -----------------------------------------------------------------------
		Take the number of Banks from the Buffer
		----------------------------------------------------------------------- */
		pBankOcbInfo->dwTotalBanks = byInfoStr[0];
		//pBankOcbInfo->dwTotalBanks = 6;
	}
	else
	{
		CONFIRMLOG(bySection, "Data", ("%s",  "BadData"), szLogPath, bLog);
	}


/* -----------------------------------------------------------------------
	Send the Pass through Command to Read the Number of OCBs in each Bank.
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = PDU_STATIC_READ;		//for Read Static Information
	InputPacketBuff[2] = PDU_OUTLETS_PER_BANK;

	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%x_%d", ST_SECT_STATIC_RDWR, szTmpstring, 0x82, PDU_OUTLETS_PER_BANK);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	if (PDU_OUTLETS_PER_BANK == InputPacketBuff[1])
	{
		CONFIRMLOG(bySection, "Data", ("%s",  "GoodData"), szLogPath, bLog);

	#if 0
	/* -----------------------------------------------------------------------
		Copy the Buffer
		----------------------------------------------------------------------- */
		memset(byInfoStr, PDU_BUFF_NULL, sizeof(byInfoStr));
		for (byCount = 0; byCount < 32; byCount++)
		{
			if ((*(InputPacketBuff+(2+byCount)) != PDU_BUFF_END) && (*(InputPacketBuff+(2+byCount)) != PDU_BUFF_NULL))
			{
				*(byInfoStr + byCount) =  *(InputPacketBuff+(2+byCount));
		
			}
			else
			if ((*(InputPacketBuff+(2+byCount)) == PDU_BUFF_END) || (*(InputPacketBuff+(2+byCount)) == PDU_BUFF_NULL))
			{
				bRet = TRUE;
				break;
			}
			
		}
	#else
		for (byCount = 0; byCount < InputPacketBuff[2]; byCount++)
		{
			*(byInfoStr + byCount) =  *(InputPacketBuff + (3 + byCount));
		}
	#endif

	/* -----------------------------------------------------------------------
		Take the number of Banks from the Buffer
		----------------------------------------------------------------------- */
		for (byCount = 0; byCount < pBankOcbInfo->dwTotalBanks; byCount++)
 		{
			pBankOcbInfo->dwOutletsOnBank[byCount] = byInfoStr[0];
			//pBankOcbInfo->dwOcbsOnBank[byCount] = 4;
		}

		bRet = TRUE;
	}
	else
	{
		CONFIRMLOG(bySection, "Data", ("%s",  "BadData"), szLogPath, bLog);
	}

	return bRet;
}


/* -----------------------------------------------------------------------
	3) PduSwitchOutletState 

	This API is used to switch ON, switch OFF or Re-boot the OCB. These APIs 
	main input 	parameters are Pdu Address, Bank number and OCB number.

	----------------------------------------------------------------------- */
__declspec(dllexport) BOOL PduSwitchOutletState(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byCurrState, BYTE byInpValue, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	BOOL									bRet = FALSE;
	BYTE									byType = 0;
	DWORD									dwDelaySecs = 0;
	CHAR									bySection[MAX_PATH] = {0};
	CHAR									byLocalBuf[MAX_PATH] = {0};
	BYTE									byOCBStateBuff[MAX_PDU_BUFF_SIZE] = {0};		//Allocate a memory buffer equal to our endpoint size + 1
	DWORD									dwByteWritten = 0;
	CString									szTmpstring;


	// ON & REBOOT --> use same Stagger ON Delay
	byType = PDU_STAGGER_ON_DELAY;
	if (byCurrState == RELAY_OPEN)
	{
		byType = PDU_STAGGER_OFF_DELAY;
	}

	dwDelaySecs = PduReadStaggerDelays(byPduadd, byType);

	/* -----------------------------------------------------------------------
		Make Section name for the Log.
	----------------------------------------------------------------------- */
	
	memcpy(byLocalBuf, byPduadd, strlen(byPduadd));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_OCBSTATE, szTmpstring, byBank, byOutlet);

	/* -----------------------------------------------------------------------
		Send the Pass through Command to devcie
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	memset(byOCBStateBuff, 0, sizeof(byOCBStateBuff));
	byOCBStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	byOCBStateBuff[1] = RELAY_STATE;
	byOCBStateBuff[2] = byOCB;
	byOCBStateBuff[3] = byRealOutlet;
	byOCBStateBuff[4] = byCurrState;
	byOCBStateBuff[5] = byInpValue;
	
	do
	{
		if (!g_bReadWrite)
		{
			Sleep(dwDelaySecs);		// Need to wait 6 seconds for rebooting the selected OUTLET
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, byOCBStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	if ((byOCBStateBuff[1] == byRealOutlet) && (byOCBStateBuff[2] == byCurrState))
	{
		bRet = TRUE;
		CONFIRMLOG(bySection, "OCB STATE CHANGED", ("Pass"), szLogPath, bLog);
	}
	else 
	{
		bRet = FALSE;
		CONFIRMLOG(bySection, "OCB STATE CHANGED", ("Fail"), szLogPath, bLog);
	}

	return bRet;
}

/* -----------------------------------------------------------------------
	4) PduGetOUTLETData

	This API retern the OCB data. The main input parameter of this APIs are
	Pdu address, Bank number and Ocb number.
	----------------------------------------------------------------------- */

__declspec(dllexport) BOOL PduGetOUTLETData(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, POUTLET_DATA_INFO pOcb_Data_Info, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	BOOL									bRet = FALSE;
	CHAR									bySection[100] = {0};
	CHAR									byLocalBuf[MAX_PATH] = {0};
	BYTE									InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};
	DWORD									dwByteWritten = 0;
	DWORD									NumberOfBytesRead = 0;
	//OUTLET_DATA_INFO						Ocb_Data_Info = {0};
	//POUTLET_DATA_INFO						pOcb_Data_info = &Ocb_Data_Info;
	CString									cszLoad;
	CString									szTmpstring;
	float									fValue1 = 0.0;
	float									fValue2 = 0.0;

	/* -----------------------------------------------------------------------
		Make Section name for the Log.
	----------------------------------------------------------------------- */
	
	memcpy(byLocalBuf, byPduadd, strlen(byPduadd));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet);

	/* -----------------------------------------------------------------------
		Send the Pass through Command to get VOLTAGE
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byOCB;
	InputPacketBuff[3] = TIME_STAMP_ID;			// Time Stamp
	InputPacketBuff[4] = VOLTAGE_ID;				// Voltage
	InputPacketBuff[5] = byRealOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
	
	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%d_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet, VOLTAGE_ID);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	CONFIRMLOG(bySection, "HexVoltage", ("%s", cszLoad), szLogPath, bLog);
	
	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	pOcb_Data_Info->fVoltage = val.f;

/* -----------------------------------------------------------------------
	Send the Pass through Command to get CURRENT
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byOCB;
	InputPacketBuff[3] = TIME_STAMP_ID;			// Time Stamp
	InputPacketBuff[4] = CURRENT_ID;				// CURRENT
	InputPacketBuff[5] = byRealOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
	
	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%d_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet, CURRENT_ID);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	CONFIRMLOG(bySection, "HexCurrent", ("%s", cszLoad), szLogPath, bLog);

	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	pOcb_Data_Info->fCurrent = val.f;

/* -----------------------------------------------------------------------
	Send the Pass through Command to get POWER FACTOR
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byOCB;
	InputPacketBuff[3] = TIME_STAMP_ID;			// Time Stamp
	InputPacketBuff[4] = POWER_FACTOR_ID;		// Power Factor
	InputPacketBuff[5] = byRealOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;

	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%d_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet, POWER_FACTOR_ID);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	CONFIRMLOG(bySection, "HexPowerFactor", ("%s", cszLoad), szLogPath, bLog);
	
	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	pOcb_Data_Info->fPoFactor = val.f;

/* -----------------------------------------------------------------------
	Send the Pass through Command to get CREST FACTOR
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byOCB;
	InputPacketBuff[3] = TIME_STAMP_ID;			// Time Stamp
	InputPacketBuff[4] = CREST_FACTOR_ID;		// Crest Factor
	InputPacketBuff[5] = byRealOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
 
	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%d_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet, CREST_FACTOR_ID);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	CONFIRMLOG(bySection, "HexCrestFactor", ("%s", cszLoad), szLogPath, bLog);
	
	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	pOcb_Data_Info->fCrFactor = val.f;

/* -----------------------------------------------------------------------
	Send the Pass through Command to get FREQUENCY
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byOCB;
	InputPacketBuff[3] = TIME_STAMP_ID;		// Time Stamp
	InputPacketBuff[4] = FREQUENCY_ID;		// FREQUENCY
	InputPacketBuff[5] = byRealOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
 
	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%d_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet, FREQUENCY_ID);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	CONFIRMLOG(bySection, "HexFrequency", ("%s", cszLoad), szLogPath, bLog);

	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	pOcb_Data_Info->fFrequency = val.f;

	/* -----------------------------------------------------------------------
		Calculate VOLTAGE * CURRENT to get APPARENT POWER
	----------------------------------------------------------------------- */
	cszLoad.Empty();
	cszLoad.Format(_T("%.1f"), pOcb_Data_Info->fVoltage);
	fValue1 = (float)atof(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();

	cszLoad.Empty();
	cszLoad.Format(_T("%.1f"), pOcb_Data_Info->fCurrent);
	fValue2 = (float)atof(cszLoad.GetBuffer(cszLoad.GetLength()));
	cszLoad.ReleaseBuffer();
	
	//pOcb_Data_Info->fApparentPow = pOcb_Data_Info->fVoltage * pOcb_Data_Info->fCurrent;
	pOcb_Data_Info->fApparentPow = fValue1 * fValue2;
	
/* -----------------------------------------------------------------------
	Send the Pass through Command to get REAL ENERY
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byOCB;
	InputPacketBuff[3] = TIME_STAMP_ID;		// Time Stamp
	InputPacketBuff[4] = REAL_ENERGY_ID;		// REAL ENERGY
	InputPacketBuff[5] = byRealOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
 
	//To avoid log overwrite
	memset(bySection, 0x00, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%d_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet, REAL_ENERGY_ID);
	
	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	if (((InputPacketBuff[1] == 0xFF) && (InputPacketBuff[2] == 0xEE)) || ((InputPacketBuff[4] == 0x00) && (InputPacketBuff[5] == 0x00) && (InputPacketBuff[6] == 0x00) && (InputPacketBuff[7] == 0x00)))
	{
		/* -----------------------------------------------------------------------
			Calculate APPARENT POWER * POWER FACTOR to get REAL ENERGY
		----------------------------------------------------------------------- */
		cszLoad.Empty();
		cszLoad.Format(_T("%.1f"), pOcb_Data_Info->fApparentPow);
		fValue1 = (float)atof(cszLoad.GetBuffer(cszLoad.GetLength()));
		cszLoad.ReleaseBuffer();

		cszLoad.Empty();
		cszLoad.Format(_T("%.1f"), pOcb_Data_Info->fPoFactor);
		fValue2 = (float)atof(cszLoad.GetBuffer(cszLoad.GetLength()));
		cszLoad.ReleaseBuffer();

		//pOcb_Data_Info->fReEnergy = pOcb_Data_Info->fApparentPow * pOcb_Data_Info->fPoFactor;
		pOcb_Data_Info->fReEnergy = (fValue1 * fValue2);
	}
	else
	{
		cszLoad.Empty();
		cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

		CONFIRMLOG(bySection, "HexRealEnergy", ("%s", cszLoad), szLogPath, bLog);

		val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		cszLoad.ReleaseBuffer();

		pOcb_Data_Info->fReEnergy = val.f;
	}

	bRet = TRUE;

	return bRet;
}


#if 0
//This function can be used in application  only
void RegisterForDeviceNotifications()
{

	// Request to receive messages when a device is attached or removed.
	// Also see WM_DEVICECHANGE in BEGIN_MESSAGE_MAP(CUsbhidiocDlg, CDialog).

	DEV_BROADCAST_DEVICEINTERFACE DevBroadcastDeviceInterface;
	HDEVNOTIFY DeviceNotificationHandle;

	DevBroadcastDeviceInterface.dbcc_size = sizeof(DevBroadcastDeviceInterface);
	DevBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	DevBroadcastDeviceInterface.dbcc_classguid = HidGuid;

	DeviceNotificationHandle =
		RegisterDeviceNotification(m_hWnd, &DevBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);

}
#endif

void GetDeviceCapabilities(CHAR *byDevPath, BOOL bLog, HANDLE hDeviceHandle)
{
	//Get the Capabilities structure for the device.
	CHAR									bySection[MAX_PATH] = {0};
	CHAR									byLocalBuf[MAX_PATH] = {0};
	PHIDP_PREPARSED_DATA					PreparsedData;
	HIDP_CAPS								Capabilities;
	CString									szTmpstring;

	/*
	API function: HidD_GetPreparsedData
	Returns: a pointer to a buffer containing the information about the device's capabilities.
	Requires: A handle returned by CreateFile.
	There's no need to access the buffer directly,
	but HidP_GetCaps and other API functions require a pointer to the buffer.
	*/

	HidD_GetPreparsedData(hDeviceHandle, &PreparsedData);
	//DisplayLastError("HidD_GetPreparsedData: ");

	/*
	API function: HidP_GetCaps
	Learn the device's capabilities.
	For standard devices such as joysticks, you can find out the specific
	capabilities of the device.
	For a custom device, the software will probably know what the device is capable of,
	and the call only verifies the information.
	Requires: the pointer to the buffer returned by HidD_GetPreparsedData.
	Returns: a Capabilities structure containing the information.
	*/
	
	HidP_GetCaps(PreparsedData, &Capabilities);
	//DisplayLastError("HidP_GetCaps: ");

	//Display the capabilities


/* -----------------------------------------------------------------------
	Make Section name for the Log.
	----------------------------------------------------------------------- */
	
	memcpy(byLocalBuf, byDevPath, strlen(byDevPath));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_PDUENUM, szTmpstring);


	//ValueToDisplay.Format("%s%X", "Usage Page: ", Capabilities.UsagePage);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Usage Page", ("0x%X", Capabilities.UsagePage), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Input Report Byte Length: ", Capabilities.InputReportByteLength);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Input Report Byte Length: ", ("%d", Capabilities.InputReportByteLength), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Output Report Byte Length: ", Capabilities.OutputReportByteLength);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Output Report Byte Length: ", ("%d", Capabilities.OutputReportByteLength), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Feature Report Byte Length: ", Capabilities.FeatureReportByteLength);
	//DisplayData(ValueToDisplay);
	CONFIRMLOG(bySection, "Feature Report Byte Length: ", ("%d", Capabilities.FeatureReportByteLength), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Link Collection Nodes: ", Capabilities.NumberLinkCollectionNodes);
	//DisplayData(ValueToDisplay);
	CONFIRMLOG(bySection, "Number of Link Collection Nodes: ", ("%d", Capabilities.NumberLinkCollectionNodes), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Input Button Caps: ", Capabilities.NumberInputButtonCaps);
	//DisplayData(ValueToDisplay);
	CONFIRMLOG(bySection, "Number of Input Button Caps: ", ("%d", Capabilities.NumberInputButtonCaps), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of InputValue Caps: ", Capabilities.NumberInputValueCaps);
	//DisplayData(ValueToDisplay);
	CONFIRMLOG(bySection, "Number of InputValue Caps: ", ("%d", Capabilities.NumberInputValueCaps), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of InputData Indices: ", Capabilities.NumberInputDataIndices);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Number of InputData Indices: ", ("%d", Capabilities.NumberInputDataIndices), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Output Button Caps: ", Capabilities.NumberOutputButtonCaps);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Number of Output Button Caps: ", ("%d", Capabilities.NumberOutputButtonCaps), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Output Value Caps: ", Capabilities.NumberOutputValueCaps);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Number of Output Value Caps: ", ("%d", Capabilities.NumberOutputValueCaps), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Output Data Indices: ", Capabilities.NumberOutputDataIndices);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Number of Output Data Indices: ", ("%d", Capabilities.NumberOutputDataIndices), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Feature Button Caps: ", Capabilities.NumberFeatureButtonCaps);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Number of Feature Button Caps: ", ("%d", Capabilities.NumberFeatureButtonCaps), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Feature Value Caps: ", Capabilities.NumberFeatureValueCaps);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Number of Feature Value Caps: ", ("%d", Capabilities.NumberFeatureValueCaps), szLogPath, bLog);

	//ValueToDisplay.Format("%s%d", "Number of Feature Data Indices: ", Capabilities.NumberFeatureDataIndices);
	//DisplayData(ValueToDisplay);

	CONFIRMLOG(bySection, "Number of Feature Data Indices: ", ("%d", Capabilities.NumberFeatureDataIndices), szLogPath, bLog);

	//No need for PreparsedData any more, so free the memory it's using.

	HidD_FreePreparsedData(PreparsedData);
	//DisplayLastError("HidD_FreePreparsedData: ") ;
	CONFIRMLOG(bySection, "HidD_FreePreparsedData:  ", ("HidD_FreePreparsedData"), szLogPath, bLog);
}

/* -----------------------------------------------------------------------
	5) PduStaticInfoWriteAndRead

	This API Write and Read the static information of the PDU Device. The 
	main input parameter of this APIs are	Pdu address, DataID, Static Data
	and Write/Read Flag.
	----------------------------------------------------------------------- */

__declspec(dllexport) BOOL PduStaticInfoWriteAndRead(CHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BYTE *byErrCode, BOOL bLog, BYTE byOutletNum)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	BOOL									bRet = FALSE;
	CHAR									bySection[100] = {0};
	CHAR									byLocalBuf[MAX_PATH] = {0};
	BYTE									InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};
	DWORD									dwByteWritten = 0;
	DWORD									NumberOfBytesRead = 0;
	CString				 					szTmpstring;
	BYTE									byCount = 0;
	CString									cszLoad;

	/* -----------------------------------------------------------------------
		Make Section name for the Log.
	----------------------------------------------------------------------- */
	
	memcpy(byLocalBuf, byPduadd, strlen(byPduadd));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s", ST_SECT_STATIC_RDWR, szTmpstring);

#if 0
	if (bWrite)
	{
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Write Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		InputPacketBuff[1] = PDU_STATIC_WRITE;		//for Write PDU Static Information
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byInfoSize;		// Time Stamp

		for (byCount = 0; byCount < byInfoSize; byCount++)
		{
			InputPacketBuff[4+byCount] =  *(byInfoStr + byCount);
		}
 
		//To avoid log overwrite
		memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
		wsprintf(bySection, "%s_%s_%x_%d", ST_SECT_STATIC_RDWR, szTmpstring, 0x81, byInfoID);
	
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, TRUE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		bRet = TRUE;
	}
	else
	{
#endif
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Read Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		InputPacketBuff[1] = PDU_STATIC_READ;		//for Read Static Information
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byOutletNum;

 		//To avoid log overwrite
		memset(bySection, 0x00, sizeof(bySection));
		wsprintf(bySection, "%s_%s_%x_%d", ST_SECT_STATIC_RDWR, szTmpstring, 0x82, byInfoID);
	
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		if (byInfoID == InputPacketBuff[1])
		{
			CONFIRMLOG(bySection, "Data", ("%s",  "GoodData"), szLogPath, bLog);

		/* -----------------------------------------------------------------------
			Copy the Buffer
			----------------------------------------------------------------------- */
			/*for (byCount = 0; byCount <= 60; byCount++)
			{
				if ((*(InputPacketBuff+(2+byCount)) != 0xFF) && (*(InputPacketBuff+(2+byCount)) != 0x00))
				{
					*(byInfoStr + byCount) =  *(InputPacketBuff+(2+byCount));
				}
				else
				if ((*(InputPacketBuff+(2+byCount)) == 0xFF) || (*(InputPacketBuff+(2+byCount)) == 0x00))
				{
					bRet = TRUE;
					break;
				}
			}*/

			for (byCount = 0; byCount < InputPacketBuff[2]; byCount++)
			{
				*(byInfoStr + byCount) =  *(InputPacketBuff + (3 + byCount));
			}

			bRet = TRUE;
		}
		else
		{
			CONFIRMLOG(bySection, "Data", ("%s",  "BadData"), szLogPath, bLog);
		}
	//}

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);
	CONFIRMLOG(bySection, "HexFrequency", ("%s", cszLoad), szLogPath, bLog);

	return bRet;
}

/* -----------------------------------------------------------------------
	This API is to Write and Read the static information of the PDU Device.
	Once the Static information is written it should not be changes.
	----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
	6) PduGetOutletState 

	This API is used to get the state like ON, OFF or Re-boot. These APIs 
	main input 	parameters are Pdu Address, Bank number and Outlet number.

	----------------------------------------------------------------------- */
__declspec(dllexport) BOOL PduGetOutletState(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
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
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_OUTLET_STATE, szTmpstring, byBank, byOutlet);

	/* -----------------------------------------------------------------------
		Send the Pass through Command to devcie
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	memset(byStateBuff, 0, sizeof(byStateBuff));
	byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	byStateBuff[1] = RELAY_STATE;
	byStateBuff[2] = byOCB;
	byStateBuff[3] = byRealOutlet;
	byStateBuff[4] = RELAY_STATUS;
	
	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	if (byStateBuff[1] == byRealOutlet)
	{
		bRet = TRUE;
		*byStateValue = byStateBuff[2];

		CONFIRMLOG(bySection, "GET OUTLET STATE", ("Pass"), szLogPath, bLog);
	}
	else if ((byStateBuff[1] == PDU_BUFF_END) && (byStateBuff[2] == PDU_BUFF_ERR))
	{
		bRet = FALSE;
		*byStateValue = RELAY_UNKNOWN;
		CONFIRMLOG(bySection, "GET OUTLET STATE", ("Fail"), szLogPath, bLog);
	}

	return bRet;
}

/* -----------------------------------------------------------------------
	This API is used to store the Alert settings for an Outlet
	main input 	parameters are Pdu Address, Bank number and OCB number.
	----------------------------------------------------------------------- */
__declspec(dllexport) BOOL PduStoreOutletAlertSettings(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, PDU_OUTLET_ALERT_INFO stPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog)
{
	UINT				nNum = 0;
	UINT				nValue = 0;
	UINT				nTotal = 0;
	BYTE				byPDUPos = 0;
	BOOL				bRet = FALSE;
	BOOL				bOldStatus = FALSE;
	BOOL				bStartThrd = FALSE;
	BOOL				bPDUFound = FALSE;
	BOOL				bOutletFound = FALSE;
	CString				cszSectionNm;
	CString				cszKeyNm;
	CString				cszLoad;
	TCHAR				szRetBuff[MAX_PATH] = {0};

	// Need to update the PDU_LIST section to hold the PDU list for reading purpose
	nTotal = GetPrivateProfileInt(STR_PDU_LIST_NM, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

	cszSectionNm.Empty();
	cszSectionNm = bySerNum;

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_PDU_SER_NUM, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(STR_PDU_LIST_NM, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		if (!cszLoad.CompareNoCase(cszSectionNm))
		{
			bPDUFound = TRUE;
			byPDUPos = nNum;
			break;
		}
	}

	if (!bPDUFound)
	{
		// Need to add to the PDU_LIST section
		nTotal++;

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), nTotal);
		
		WritePrivateProfileString(STR_PDU_LIST_NM, STR_TOTAL_NM_KEY, cszLoad, cszPDUConfIniPath);

		// Store the PDU Name in the list to get the list of PDU in AlertCofnig.ini file
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_PDU_SER_NUM, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%s"), bySerNum);

		WritePrivateProfileString(STR_PDU_LIST_NM, cszKeyNm, cszLoad, cszPDUConfIniPath);

		byPDUPos = nTotal;
	}

	cszSectionNm.Empty();
	cszSectionNm = bySerNum;

	cszKeyNm.Empty();
	cszKeyNm = byPduadd;

	memset(szRetBuff, 0, sizeof(szRetBuff));
	GetPrivateProfileString(cszSectionNm, STR_PDU_KEY_NM, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
	cszLoad.Empty();
	cszLoad = szRetBuff;

	if (cszLoad.CompareNoCase(cszKeyNm))
	{
		WritePrivateProfileString(cszSectionNm, STR_PDU_KEY_NM, cszKeyNm, cszPDUConfIniPath);
	}

	nTotal = GetPrivateProfileInt(cszSectionNm, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_BANK_KEY, nNum);

		nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

		if (nValue == byBank)
		{
			// Bank found
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_OUTLET_KEY, nNum);

			nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			if (nValue == byOutlet)
				bOutletFound = TRUE;
		}

		if (bOutletFound)
			break;
	}

	if (bOutletFound)
	{
		nTotal = nNum;
	}
	else
	{
		nTotal++;
	
		// Store the BANK Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_BANK_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byBank);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Store the OCB Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_OCB_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byOCB);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Store the REAL OUTLET Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_REAL_OUTLET_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byRealOutlet);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Store the OUTLET Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_OUTLET_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byOutlet);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);
	}

	// --------------Store the VCB Settings-----------------------
	// store enable status
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_VCB_ENABLED_KEY, nTotal);

	if (bOutletFound)
	{
		// Need to take the previous value for running thread
		// >>> VCB Status <<<
		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		if (!cszLoad.CompareNoCase(STR_YES))
		{
			bOldStatus = TRUE;
		}
	}

	cszLoad.Empty();
	cszLoad = STR_NO;
	if (stPDUOutletAlertInfo.bVCBEnabled)
	{
		cszLoad.Empty();
		cszLoad = STR_YES;
	}

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store threshold value
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_VCB_THRESHOLD_KEY, nTotal);

	cszLoad.Empty();
	cszLoad.Format(_T("%.2f"), stPDUOutletAlertInfo.fVCBThreshold);

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store max grace period value
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_VCB_MAX_GRC_PER_KEY, nTotal);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPDUOutletAlertInfo.nVCBMaxGrcPer);

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store vcb alert status
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_VCB_ALERT, nTotal);

	cszLoad.Empty();
	cszLoad = STR_NO;

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// --------------Store the HCA Settings-----------------------

	// store enable status
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_HCA_ENABLED_KEY, nTotal);

	if ((bOutletFound) && (!bOldStatus))
	{
		// Need to take the previous value for running thread
		// >>> HCA Status <<<
		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		if (!cszLoad.CompareNoCase(STR_YES))
		{
			bOldStatus = TRUE;
		}
	}

	cszLoad.Empty();
	cszLoad = STR_NO;
	if (stPDUOutletAlertInfo.bHCAEnabled)
	{
		cszLoad.Empty();
		cszLoad = STR_YES;
	}

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store threshold value
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_HCA_THRESHOLD_KEY, nTotal);

	cszLoad.Empty();
	cszLoad.Format(_T("%.2f"), stPDUOutletAlertInfo.fHCAThreshold);

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store max grace period value
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_HCA_MAX_GRC_PER_KEY, nTotal);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPDUOutletAlertInfo.nHCAMaxGrcPer);

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store vcb alert status
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_HCA_ALERT, nTotal);

	cszLoad.Empty();
	cszLoad = STR_NO;

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// --------------Store the LCA Settings-----------------------

	// store enable status
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_LCA_ENABLED_KEY, nTotal);

	if ((bOutletFound) && (!bOldStatus))
	{
		// Need to take the previous value for running thread
		// >>> LCA Status <<<
		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		if (!cszLoad.CompareNoCase(STR_YES))
		{
			bOldStatus = TRUE;
		}
	}

	cszLoad.Empty();
	cszLoad = STR_NO;
	if (stPDUOutletAlertInfo.bLCAEnabled)
	{
		cszLoad.Empty();
		cszLoad = STR_YES;
	}

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store threshold value
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_LCA_THRESHOLD_KEY, nTotal);

	cszLoad.Empty();
	cszLoad.Format(_T("%.2f"), stPDUOutletAlertInfo.fLCAThreshold);

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store max grace period value
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_LCA_MAX_GRC_PER_KEY, nTotal);

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), stPDUOutletAlertInfo.nLCAMaxGrcPer);

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	// store vcb alert status
	cszKeyNm.Empty();
	cszKeyNm.Format(STR_ALERT_LCA_ALERT, nTotal);

	cszLoad.Empty();
	cszLoad = STR_NO;

	WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

	if ((stPDUOutletAlertInfo.bVCBEnabled) || (stPDUOutletAlertInfo.bHCAEnabled) || (stPDUOutletAlertInfo.bLCAEnabled))
	{
		bStartThrd = TRUE;
	}

	if (!bOutletFound)
	{
		// Total Num of Outlets
		cszLoad.Empty();
		cszLoad.Format(_T("%d"), nTotal);

		WritePrivateProfileString(cszSectionNm, STR_TOTAL_NM_KEY, cszLoad, cszPDUConfIniPath);
	}
	else if (bOldStatus)
	{
		bStartThrd = FALSE;
	}

	if (bStartThrd)
	{
		theApp.StartOutletAlertThrd(byPDUPos, byBank, byOCB, byOutlet, byRealOutlet, nTotal);
		cszSectionNm.ReleaseBuffer();
	}
	
	return bRet;
}

/* -----------------------------------------------------------------------
	This API is used to get the Alert settings for an Outlet
	main input 	parameters are Pdu Address, Bank number and Outlet number.
	----------------------------------------------------------------------- */
__declspec(dllexport) BOOL PduGetOutletAlertSettings(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, PPDU_OUTLET_ALERT_INFO pPDUOutletAlertInfo, BYTE *byErrCode, BOOL bLog)
{
	UINT				nNum = 0;
	UINT				nTotal = 0;
	UINT				nValue = 0;
	float				fThresValue = 0.0;
	BOOL				bOutletFound = FALSE;
	BOOL				bRet = FALSE;
	TCHAR				szRetBuff[MAX_PATH] = {0};
	CString				cszSectionNm;
	CString				cszKeyNm;
	CString				cszLoad;

	cszSectionNm.Empty();
	cszSectionNm = bySerNum;

	nTotal = GetPrivateProfileInt(cszSectionNm, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_BANK_KEY, nNum);

		nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

		if (nValue == byBank)
		{
			// Bank found
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_OUTLET_KEY, nNum);

			nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			if (nValue == byOutlet)
				bOutletFound = TRUE;
		}

		if (bOutletFound)
			break;
	}

	if (bOutletFound)
	{
		memset(pPDUOutletAlertInfo, 0, sizeof(PDU_OUTLET_ALERT_INFO));

		// --------------Read the VCB Settings-----------------------
		// read enable status
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_VCB_ENABLED_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		pPDUOutletAlertInfo->bVCBEnabled = FALSE;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			pPDUOutletAlertInfo->bVCBEnabled = TRUE;
		}

		// read threshold value
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_VCB_THRESHOLD_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath);
		if (lstrcmpi(szRetBuff, STR_FAIL))
		{
			pPDUOutletAlertInfo->fVCBThreshold = (float)atof(szRetBuff);
		}

		// read max grace period value
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_VCB_MAX_GRC_PER_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath);
		if (lstrcmpi(szRetBuff, STR_FAIL))
		{
			pPDUOutletAlertInfo->nVCBMaxGrcPer = (int)atoi(szRetBuff);
		}

		// --------------Read the HCA Settings-----------------------
		// read enable status
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_HCA_ENABLED_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		pPDUOutletAlertInfo->bHCAEnabled = FALSE;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			pPDUOutletAlertInfo->bHCAEnabled = TRUE;
		}

		// read threshold value
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_HCA_THRESHOLD_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath);
		if (lstrcmpi(szRetBuff, STR_FAIL))
		{
			pPDUOutletAlertInfo->fHCAThreshold = (float)atof(szRetBuff);
		}

		// read max grace period value
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_HCA_MAX_GRC_PER_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath);
		if (lstrcmpi(szRetBuff, STR_FAIL))
		{
			pPDUOutletAlertInfo->nHCAMaxGrcPer = (int)atoi(szRetBuff);
		}

		// --------------Read the LCA Settings-----------------------
		// read enable status
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_LCA_ENABLED_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		pPDUOutletAlertInfo->bLCAEnabled = FALSE;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			pPDUOutletAlertInfo->bLCAEnabled = TRUE;
		}

		// read threshold value
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_LCA_THRESHOLD_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath);
		if (lstrcmpi(szRetBuff, STR_FAIL))
		{
			pPDUOutletAlertInfo->fLCAThreshold = (float)atof(szRetBuff);
		}

		// read max grace period value
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_LCA_MAX_GRC_PER_KEY, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath);
		if (lstrcmpi(szRetBuff, STR_FAIL))
		{
			pPDUOutletAlertInfo->nLCAMaxGrcPer = (int)atoi(szRetBuff);
		}
	}
	else
	{
		// Set the default values for VCB, HCA & LCA

		pPDUOutletAlertInfo->bVCBEnabled = FALSE;
		pPDUOutletAlertInfo->bHCAEnabled = FALSE;
		pPDUOutletAlertInfo->bLCAEnabled = FALSE;

		pPDUOutletAlertInfo->fVCBThreshold = 0.0;
		pPDUOutletAlertInfo->fHCAThreshold = 0.0;
		pPDUOutletAlertInfo->fLCAThreshold = 0.0;

		pPDUOutletAlertInfo->nVCBMaxGrcPer = 0;
		pPDUOutletAlertInfo->nHCAMaxGrcPer = 0;
		pPDUOutletAlertInfo->nLCAMaxGrcPer = 0;
	}

	return bRet;
}

float PduGetOutletCurrent(CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byErrCode, BOOL bLog)
{
	CHAR							byLocalBuf[MAX_PATH] = {0};
	CString							szTmpstring;
	CString							cszLoad;
	DWORD							dwByteWritten = 0;
	DWORD							NumberOfBytesRead = 0;
	CHAR							bySection[100] = {0};
	BYTE							InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};

	/* -----------------------------------------------------------------------
		Make Section name for the Log.
	----------------------------------------------------------------------- */
	memcpy(byLocalBuf, byPduadd, strlen(byPduadd));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet);

	/* -----------------------------------------------------------------------
		Send the Pass through Command to get CURRENT
	----------------------------------------------------------------------- */
	dwByteWritten = 0;
	NumberOfBytesRead = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = CAN_CMD_ID;
	InputPacketBuff[2] = byOCB;
	InputPacketBuff[3] = TIME_STAMP_ID;			// Time Stamp
	InputPacketBuff[4] = CURRENT_ID;				// CURRENT
	InputPacketBuff[5] = byRealOutlet;
	InputPacketBuff[6] = ZERO_ID;
	InputPacketBuff[7] = ZERO_ID;
	InputPacketBuff[8] = ZERO_ID;
	InputPacketBuff[9] = ZERO_ID;
	
	//To avoid log overwrite
	memset(bySection, PDU_BUFF_NULL, sizeof(bySection));
	wsprintf(bySection, "%s_%s_%d_%d_%d", ST_SECT_OCBDATA, szTmpstring, byBank, byOutlet, CURRENT_ID);

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bLog);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);

	CONFIRMLOG(bySection, "HexCurrent", ("%s", cszLoad), szLogPath, bLog);

	val.i = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
	cszLoad.ReleaseBuffer();

	return val.f;
}

__declspec(dllexport) BOOL PduInitiateAlertEvent(BYTE *byErrCode, BOOL bLog)
{
	UINT				nNum = 0;
	UINT				nIns = 0;
	BOOL				bRet = FALSE;
	BOOL				bStartThrd = FALSE;
	UINT				nBankNo = 0;
	UINT				nOCBNum = 0;
	UINT				nOutletNo = 0;
	UINT				nRealOutlet = 0;
	UINT				nTotal = 0;
	UINT				nTotalPDU = 0;
	CString				cszKeyNm;
	CString				cszLoad;
	CString				cszPduAddrs;
	CString				cszSectionNm;
	BYTE				byDataBuff[MAX_PDU_BUFF_SIZE] = {0};
	TCHAR				szRetBuff[MAX_PATH] = {0};

	nTotalPDU = GetPrivateProfileInt(STR_PDU_LIST_NM, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

	for (nIns = 1; nIns <= nTotalPDU; nIns++)
	{
		bStartThrd = FALSE;

		cszKeyNm.Empty();
		cszKeyNm.Format(STR_PDU_SER_NUM, nIns);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(STR_PDU_LIST_NM, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszSectionNm.Empty();
		cszSectionNm = szRetBuff;

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, STR_PDU_KEY_NM, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath);
		cszPduAddrs.Empty();
		cszPduAddrs = szRetBuff;

		if ((cszPduAddrs.IsEmpty()) || (!cszPduAddrs.CompareNoCase(STR_FAIL)))
		{
			continue;
		}

		memset(byDataBuff, 0, sizeof(byDataBuff));
		PduStaticInfoWriteAndRead(cszPduAddrs.GetBuffer(cszPduAddrs.GetLength()), PDU_SERIAL_NUM, byDataBuff, 0, byErrCode, bLog);
		cszPduAddrs.ReleaseBuffer();

		cszLoad.Empty();
		cszLoad = byDataBuff;

		if (cszLoad.CompareNoCase(cszSectionNm))
		{
			continue;
		}

		nTotal = GetPrivateProfileInt(cszSectionNm, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

		for (nNum = 1; nNum <= nTotal; nNum++)
		{
			// BANK NUM
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_BANK_KEY, nNum);

			nBankNo = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			// OCB NUM
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_OCB_KEY, nNum);

			nOCBNum = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			// REAL OUTLET NUM
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_REAL_OUTLET_KEY, nNum);

			nRealOutlet = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			// OUTLET NUM
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_OUTLET_KEY, nNum);

			nOutletNo = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			// VCB ENABLED STATUS
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_VCB_ENABLED_KEY, nNum);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
			cszLoad.Empty();
			cszLoad = szRetBuff;

			if (!cszLoad.CompareNoCase(STR_YES))
			{
				bStartThrd = TRUE;
			}

			if (!bStartThrd)
			{
				// HCA ENABLED STATUS
				cszKeyNm.Empty();
				cszKeyNm.Format(STR_ALERT_HCA_ENABLED_KEY, nNum);

				memset(szRetBuff, 0, sizeof(szRetBuff));
				GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
				cszLoad.Empty();
				cszLoad = szRetBuff;

				if (!cszLoad.CompareNoCase(STR_YES))
				{
					bStartThrd = TRUE;
				}
			}

			if (!bStartThrd)
			{
				// LCA ENABLED STATUS
				cszKeyNm.Empty();
				cszKeyNm.Format(STR_ALERT_LCA_ENABLED_KEY, nNum);

				memset(szRetBuff, 0, sizeof(szRetBuff));
				GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
				cszLoad.Empty();
				cszLoad = szRetBuff;

				if (!cszLoad.CompareNoCase(STR_YES))
				{
					bStartThrd = TRUE;
				}
			}

			if (bStartThrd)
			{
				theApp.StartOutletAlertThrd(nIns, nBankNo, nOCBNum, nOutletNo, nRealOutlet, nNum);
				cszSectionNm.ReleaseBuffer();
				bStartThrd = FALSE;
			}
		}
	}

	return bRet;
}

__declspec(dllexport) BOOL PduCloseAllAlertEvent(BYTE *byErrCode, BOOL bLog)
{
	BOOL			bRet = FALSE;

	g_bQuickExit = TRUE;
	theApp.StopAllAlertEvent();

	return bRet;
}

BOOL CpduapiApp::StartOutletAlertThrd(BYTE byPDUPos, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, UINT nIniFilePos)
{
	BOOL			bRet = FALSE;

	THRD_ALERT_PASS_INFO	*pThreadAlertInfo = new THRD_ALERT_PASS_INFO;

	pThreadAlertInfo->nThreadPos = (UINT)m_byThreadExit.Add(0);
	pThreadAlertInfo->nIniFilePos = nIniFilePos;
	pThreadAlertInfo->byPDUPos = byPDUPos;
	pThreadAlertInfo->nBankNum = byBank;
	pThreadAlertInfo->nOCBNum = byOCB;
	pThreadAlertInfo->nRealOutlet = byRealOutlet;
	pThreadAlertInfo->nOutletNum = byOutlet;

	CWinThread *pAlertThread = AfxBeginThread(ThreadChkHighCurrAlert, (PVOID)pThreadAlertInfo, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
    pAlertThread->m_bAutoDelete = FALSE;
    pAlertThread->ResumeThread();

	m_AlertThreadArray.Add(pAlertThread);

	return bRet;
}

BOOL CpduapiApp::StopAllAlertEvent()
{
	UINT		nNum = 0;
	BOOL		bRet = FALSE;
	UINT		nThreadSize = 0;
	
	for (nNum = 0; nNum < (UINT)m_byThreadExit.GetSize(); nNum++)
	{
		m_byThreadExit.SetAt(nNum, 1);		// exit thread
	}

	nThreadSize = (UINT) m_AlertThreadArray.GetSize();

	HANDLE *hThread = new HANDLE[nThreadSize];

	for (nNum = 0; nNum < nThreadSize; nNum ++)
	{
		hThread[nNum] = m_AlertThreadArray[nNum]->m_hThread;
	}

	::WaitForMultipleObjects(nThreadSize, hThread, TRUE, INFINITE);

	for (nNum = 0; nNum < nThreadSize; nNum++)
	{
		delete m_AlertThreadArray[nNum];
		m_AlertThreadArray[nNum] = NULL;
	}
	delete [] hThread;

	m_AlertThreadArray.RemoveAll();

	/*if (m_pThread != NULL)
	{
		::WaitForSingleObject(m_pThread->m_hThread, 2000);
		delete m_pThread;
		m_pThread = NULL;
	}*/

	return bRet;
}

/* --------------------------------------------------------------------------
	PduGetOutletStateFromConfig

	This API is used to get the state like ON, OFF or Re-boot for configuration
	file. These APIs main input	parameters are Pdu Address, Bank number and 
	Outlet number.
	----------------------------------------------------------------------- */
__declspec(dllexport) BOOL PduGetOutletStateFromConfig(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE *byStateValue, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	UINT				nNum = 0;
	UINT				nTotal = 0;
	UINT				nValue = 0;
	BYTE				byCurrStateValue = 0;
	BOOL				bPDUFound = FALSE;
	BOOL				bOutletFound = FALSE;
	BOOL				bRet = FALSE;
	CString				cszKeyNm;
	CString				cszSectionNm;
	CString				cszLoad;
	TCHAR				szRetBuff[MAX_PATH] = {0};

	// Need to update the PDU_LIST section to hold the PDU list for reading purpose
	nTotal = GetPrivateProfileInt(STR_PDU_LIST_NM, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

	cszSectionNm.Empty();
	cszSectionNm = bySerNum;

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_PDU_SER_NUM, nNum);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(STR_PDU_LIST_NM, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		if (!cszLoad.CompareNoCase(cszSectionNm))
		{
			bPDUFound = TRUE;
			break;
		}
	}

	if (!bPDUFound)
	{
		// Need to add to the PDU_LIST section
		nTotal++;

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), nTotal);
		
		WritePrivateProfileString(STR_PDU_LIST_NM, STR_TOTAL_NM_KEY, cszLoad, cszPDUConfIniPath);

		// Store the PDU Name in the list to get the list of PDU in AlertCofnig.ini file
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_PDU_SER_NUM, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%s"), bySerNum);

		WritePrivateProfileString(STR_PDU_LIST_NM, cszKeyNm, cszLoad, cszPDUConfIniPath);
	}

	cszKeyNm.Empty();
	cszKeyNm = byPduadd;

	memset(szRetBuff, 0, sizeof(szRetBuff));
	GetPrivateProfileString(STR_PDU_LIST_NM, STR_PDU_KEY_NM, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszPDUConfIniPath); 
	cszLoad.Empty();
	cszLoad = szRetBuff;

	if (cszLoad.CompareNoCase(cszKeyNm))
	{
		WritePrivateProfileString(cszSectionNm, STR_PDU_KEY_NM, cszKeyNm, cszPDUConfIniPath);
	}

	cszSectionNm.Empty();
	cszSectionNm = bySerNum;

	nTotal = GetPrivateProfileInt(cszSectionNm, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_BANK_KEY, nNum);

		nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

		if (nValue == byBank)
		{
			// Bank found
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_OUTLET_KEY, nNum);

			nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			if (nValue == byOutlet)
				bOutletFound = TRUE;
		}

		if (bOutletFound)
			break;
	}

	if (bOutletFound)
	{
		nTotal = nNum;

		// Get the Relay Status Value
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_RELAY_STATUS_NM, nTotal);

		nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 100, cszPDUConfIniPath);

		if (nValue == 100)
		{
			PduGetOutletState(byPduadd, byBank, byOCB, byOutlet, byRealOutlet, &byCurrStateValue, byErrCode, bLog);

			if (byCurrStateValue == RELAY_UNKNOWN)
			{
				// Update the default Relay Status
				if (PduSwitchOutletState(byPduadd, byBank, byOCB, byOutlet, byRealOutlet, RELAY_CLOSE, byErrCode, bLog))
					nValue = RELAY_CLOSE;
				else
					nValue = RELAY_OPEN;
			}
			else
				nValue = byCurrStateValue;

			cszLoad.Empty();
			cszLoad.Format(_T("%d"), nValue);

			WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);
		}
	}
	else
	{
		nTotal++;
	
		// Store the BANK Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_BANK_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byBank);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Store the OCB Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_OCB_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byOCB);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Store the REAL OUTLET Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_REAL_OUTLET_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byRealOutlet);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Store the OUTLET Num
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_OUTLET_KEY, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), byOutlet);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Store the Relay Status
		PduGetOutletState(byPduadd, byBank, byOCB, byOutlet, byRealOutlet, &byCurrStateValue, byErrCode, bLog);

		if (byCurrStateValue == RELAY_UNKNOWN)
		{
			// Update the default Relay Status
			if (PduSwitchOutletState(byPduadd, byBank, byOCB, byOutlet, byRealOutlet, RELAY_CLOSE, byErrCode, bLog))
				nValue = RELAY_CLOSE;
			else
				nValue = RELAY_OPEN;
		}
		else
			nValue = byCurrStateValue;

		cszKeyNm.Empty();
		cszKeyNm.Format(STR_RELAY_STATUS_NM, nTotal);

		cszLoad.Empty();
		cszLoad.Format(_T("%d"), nValue);

		WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);

		// Update total outlets
		cszLoad.Empty();
		cszLoad.Format(_T("%d"), nTotal);

		WritePrivateProfileString(cszSectionNm, STR_TOTAL_NM_KEY, cszLoad, cszPDUConfIniPath);
	}

	*byStateValue = nValue;
	bRet = TRUE;

	return bRet;
}

__declspec(dllexport) BOOL PduStoreOutletStateToConfig(CHAR *bySerNum, CHAR *byPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byStateValue, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	UINT				nNum = 0;
	UINT				nTotal = 0;
	UINT				nValue = 0;
	BOOL				bRetValue = FALSE;
	BOOL				bPDUFound = FALSE;
	BOOL				bOutletFound = FALSE;
	BYTE				byCurrStateValue = 0;
	CString				cszKeyNm;
	CString				cszSectionNm;
	CString				cszLoad;

	cszSectionNm.Empty();
	cszSectionNm = bySerNum;

	nTotal = GetPrivateProfileInt(cszSectionNm, STR_TOTAL_NM_KEY, 0, cszPDUConfIniPath);

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_BANK_KEY, nNum);

		nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

		if (nValue == byBank)
		{
			// Bank found
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_OUTLET_KEY, nNum);

			nValue = GetPrivateProfileInt(cszSectionNm, cszKeyNm, 0, cszPDUConfIniPath);

			if (nValue == byOutlet)
				bOutletFound = TRUE;
		}

		if (bOutletFound)
			break;
	}

	if (bOutletFound)
	{
		PduGetOutletState(byPduadd, byBank, byOCB, byOutlet, byRealOutlet, &byCurrStateValue, byErrCode, bLog);

		if (byCurrStateValue == byStateValue)
		{
			bRetValue = TRUE;

			cszKeyNm.Empty();
			cszKeyNm.Format(STR_RELAY_STATUS_NM, nNum);

			cszLoad.Empty();
			cszLoad.Format(_T("%d"), byStateValue);

			WritePrivateProfileString(cszSectionNm, cszKeyNm, cszLoad, cszPDUConfIniPath);
		}
	}

	return bRetValue;
}

__declspec(dllexport) BOOL PduOutletConfigVCB(CHAR *byPduAdd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, BYTE byVCBEnable, float fThreshold, UINT nMaxGracPer, BYTE *byErrCode, BOOL bLog)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	BOOL								bRet = FALSE;
	CHAR								bySection[MAX_PATH] = {0};
	CHAR								byLocalBuf[MAX_PATH] = {0};
	BYTE								byStateBuff[65] = {0};		
	DWORD								dwByteWritten = 0;
	CString								szTmpstring;
	union IntFloat						unValue;

	/* -----------------------------------------------------------------------
		Make Section name for the Log.
	----------------------------------------------------------------------- */
	memcpy(byLocalBuf, byPduAdd, strlen(byPduAdd));
	*(strrchr(byLocalBuf, '#')) = '\0';
	szTmpstring = byLocalBuf;
	szTmpstring.MakeReverse();
	szTmpstring = szTmpstring.Left(szTmpstring.Find('#'));
	szTmpstring.MakeReverse();
	wsprintf(bySection, "%s_%s_%d_%d", ST_SECT_OUTLET_STATE, szTmpstring, byBank, byOutlet);

	if (byVCBEnable == VCB_ENABLE)
	{
		// CONFIGURE VCB_METHOD
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_CONFIG_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_METHOD;

		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);
		
		// CONFIGURE VCB_MAX_DELAY
		unValue.f = (float) nMaxGracPer;
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_CONFIG_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_MAX_DELAY;
		byStateBuff[5] = HIBYTE(HIWORD(unValue.i));
		byStateBuff[6] = LOBYTE(HIWORD(unValue.i));
		byStateBuff[7] = HIBYTE(LOWORD(unValue.i));
		byStateBuff[8] = LOBYTE(LOWORD(unValue.i));
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		// CONFIGURE VCB_MIN_DELAY
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID;	//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_CONFIG_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_MIN_DELAY;
		byStateBuff[5] = VCB_MIN_DELAY_VAL1;
		byStateBuff[6] = VCB_MIN_DELAY_VAL2;
		byStateBuff[7] = VCB_MIN_DELAY_VAL3;
		byStateBuff[8] = VCB_MIN_DELAY_VAL4;
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		// CONFIGURE VCB_CURVE_CONSTANT
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID;	//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_CONFIG_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_CURVE_CONSTANT;
		byStateBuff[5] = VCB_CURVE_CONSTANT_VAL1;
		byStateBuff[6] = VCB_CURVE_CONSTANT_VAL2;
		byStateBuff[7] = VCB_CURVE_CONSTANT_VAL3;
		byStateBuff[8] = VCB_CURVE_CONSTANT_VAL4;
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		// CONFIGURE VCB_THRESHOLD
		unValue.f = fThreshold;
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_CONFIG_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_THRESHOLD;
		byStateBuff[5] = HIBYTE(HIWORD(unValue.i));
		byStateBuff[6] = LOBYTE(HIWORD(unValue.i));
		byStateBuff[7] = HIBYTE(LOWORD(unValue.i));
		byStateBuff[8] = LOBYTE(LOWORD(unValue.i));
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		// ENABLE VCB
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_ENABLE_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_ENABLE;
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);
	}
	else
	{
		// DISABLE VCB_METHOD
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_CONFIG_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_METHOD;
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		// CONFIGURE VCB_THRESHOLD
		unValue.f = fThreshold;
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_CONFIG_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_THRESHOLD;
		byStateBuff[5] = VCB_THRESHOLD_VAL1;
		byStateBuff[6] = VCB_THRESHOLD_VAL2;
		byStateBuff[7] = VCB_THRESHOLD_VAL3;
		byStateBuff[8] = VCB_THRESHOLD_VAL4;
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);

		// DISABLE VCB
		dwByteWritten = 0;
		memset(byStateBuff, 0, sizeof(byStateBuff));
		byStateBuff[0] = ZERO_ID; //The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
		byStateBuff[1] = CSCAN_ENABLE_VCB;
		byStateBuff[2] = byOCB;
		byStateBuff[3] = byRealOutlet;
		byStateBuff[4] = VCB_DISABLE;
		
		do
		{
			if (!g_bReadWrite)
			{
				g_bReadWrite = TRUE;
				PduWriteReadBuffer(byPduAdd, byStateBuff, &dwByteWritten, FALSE, byErrCode, bLog);
				g_bReadWrite = FALSE;
				break;
			}

			if (g_bQuickExit)
			{
				break;
			}
		}
		while(TRUE);
	}

	return TRUE;
}

BOOL PduWriteReadBuffer(CHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bLog)
{
	BOOL			bRet = TRUE;
	UINT			nNumPass = 0;
	BYTE			byBufferCpy[MAX_PDU_BUFF_SIZE] = {0};
	CHAR			bySection[MAX_PATH] = {0};
	HANDLE			hMyPDUHandle = INVALID_HANDLE_VALUE;

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
		CONFIRMLOG(bySection, "DeviceHandle", ("Fail"), szLogPath, bLog);
		*byErrCode = 0x02;
		bRet = FALSE;
		return bRet;
	}
	else
	{
		CONFIRMLOG(bySection, "DeviceHandle", ("Pass"), szLogPath, bLog);
	}

	nNumPass = 0;

	memcpy(byBufferCpy, byDataBuffer, MAX_PDU_BUFF_SIZE);

	do
	{
		if (hMyPDUHandle != INVALID_HANDLE_VALUE)
		{
			WriteFile(hMyPDUHandle, byBufferCpy, MAX_PDU_BUFF_SIZE, dwRWBytes, 0);
			CONFIRMLOG(bySection, "DataWritten", ("%d %s", *dwRWBytes,  "bytes"), szLogPath, bLog);
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
			CONFIRMLOG(bySection, "DataRead", ("%d %s", *dwRWBytes,  "bytes"), szLogPath, bLog);
		}

		nNumPass++;

		if ((byDataBuffer[1] == 0xFF) && (byDataBuffer[2] == 0xEE) && (nNumPass >= 3))
			break;

	} while((byDataBuffer[1] == 0xFF) && (byDataBuffer[2] == 0xEE));

	if (hMyPDUHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMyPDUHandle);
	}

	return bRet;

}

DWORD PduReadStaggerDelays(CHAR *byPduadd, BYTE byDelayType)
{
	DWORD			dwDelaySecs = 0;
	DWORD			dwByteWritten = 0;
	BYTE			InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};
	BYTE			byCount = 0;
	CString			cszLoad;

	dwByteWritten = 0;
	memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

	InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.
	InputPacketBuff[1] = PDU_STATIC_READ;		//for Read Static Information
	InputPacketBuff[2] = byDelayType;

	do
	{
		if (!g_bReadWrite)
		{
			g_bReadWrite = TRUE;
			PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, NULL, FALSE);
			g_bReadWrite = FALSE;
			break;
		}

		if (g_bQuickExit)
		{
			break;
		}
	}
	while(TRUE);

	if (byDelayType == InputPacketBuff[1])
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%02x%02x%02x%02x"), InputPacketBuff[3], InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6]);
		dwDelaySecs = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
		cszLoad.ReleaseBuffer();			
	}
	else
	{
		dwDelaySecs = 10;
	}

	return dwDelaySecs;
}