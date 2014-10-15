#include "StdAfx.h"
#include "resource.h"
#include "General.h"
#include "pduapi.h"

#include "sqlite3.h"

CByteArray		m_byThreadExit;

UINT ThreadChkHighCurrAlert(PVOID pParam)
{
	BYTE						byErrcode = 0;
	UINT						nHIGHMaxSecs = 0;
	UINT						nLOWMaxSecs = 0;
	BOOL						bVCBAlertStatus = FALSE;
	BYTE						byActivateVCB = 1;
	BYTE						byVCBEnabled = 0;
	BYTE						byVCBOldEnabled = 0;
	BYTE						byHIGHEnabled = 0;
	BYTE						byLOWEnabled = 0;
	BYTE						byErrorCode = 0;
	BYTE						byCurrStateValue = 0;
	UINT						nLCAMaxGrcPer = 0;
	UINT						nLCAMaxGPOld = 0;
	UINT						nHCAMaxGrcPer = 0;
	UINT						nHCAMaxGPOld = 0;
	UINT						nVCBMaxGrcPer = 0;
	UINT						nVCBMaxGPOld = 0;
	float						fVCBThreshold = 0.0;
	float						fVCBThresOLD = 0.0;
	float						fHCAThreshold = 0.0;
	float						fHCAThresOLD = 0.0;
	float						fLCAThreshold = 0.0;
	float						fLCAThresOLD = 0.0;
	float						fActualCurrent = 0.0;
	TCHAR						szFilePath[MAX_PATH] = {0};
	CString						cszLoad;
	CString						cszKeyNm;
	CString						cszSectionNm;
	CString						cszConfigFile;
	CString						cszPduAddress;
	TCHAR						szRetBuff[MAX_PATH] = {0};
	THRD_ALERT_PASS_INFO		*pThreadAlertInfo = reinterpret_cast<THRD_ALERT_PASS_INFO *> (pParam);

	memset(szFilePath, 0, sizeof(szFilePath));
	GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
	*(strrchr(szFilePath, '\\') + 1) = '\0';
	
	cszConfigFile.Empty();
	cszConfigFile = szFilePath;
	cszConfigFile += STR_PDU_CONG_INI_NM;

	cszKeyNm.Empty();
	cszKeyNm.Format(STR_PDU_SER_NUM, pThreadAlertInfo->byPDUPos);

	memset(szRetBuff, 0, sizeof(szRetBuff));
	GetPrivateProfileString(STR_PDU_LIST_NM, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
	cszSectionNm.Empty();
	cszSectionNm = szRetBuff;

	memset(szRetBuff, 0, sizeof(szRetBuff));
	GetPrivateProfileString(cszSectionNm, STR_PDU_KEY_NM, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile);
	cszPduAddress.Empty();
	cszPduAddress = szRetBuff;

	while (!m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
	{
		///////////////////////////////////////// VCB ENABLED STATUS //////////////////////////////////////////////////////////
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_VCB_ENABLED_KEY, pThreadAlertInfo->nIniFilePos);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		byVCBEnabled = 0;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			byVCBEnabled = 1;
		}

		if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
			break;

		///////////////////////////////////////// HCA ENABLED STATUS //////////////////////////////////////////////////////////
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_HCA_ENABLED_KEY, pThreadAlertInfo->nIniFilePos);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		byHIGHEnabled = 0;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			byHIGHEnabled = 1;
		}

		if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
			break;

		///////////////////////////////////////// LCA ENABLED STATUS //////////////////////////////////////////////////////////
		cszKeyNm.Empty();
		cszKeyNm.Format(STR_ALERT_LCA_ENABLED_KEY, pThreadAlertInfo->nIniFilePos);

		memset(szRetBuff, 0, sizeof(szRetBuff));
		GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
		cszLoad.Empty();
		cszLoad = szRetBuff;

		byLOWEnabled = 0;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			byLOWEnabled = 1;
		}

		if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
			break;

		if (byVCBEnabled)
		{
			///////////////////////////// VCB CHECKING /////////////////////////////////////
			byVCBOldEnabled = byVCBEnabled;

			// read threshold value
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_VCB_THRESHOLD_KEY, pThreadAlertInfo->nIniFilePos);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
			if (lstrcmpi(szRetBuff, STR_FAIL))
			{
				fVCBThreshold = (float)atof(szRetBuff);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			// read max grace period value
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_VCB_MAX_GRC_PER_KEY, pThreadAlertInfo->nIniFilePos);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
			if (lstrcmpi(szRetBuff, STR_FAIL))
			{
				nVCBMaxGrcPer = (int)atoi(szRetBuff);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			// read VCB alert status
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_VCB_ALERT, pThreadAlertInfo->nIniFilePos);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_NO, szRetBuff, sizeof(szRetBuff), cszConfigFile);
			if (!lstrcmpi(szRetBuff, STR_NO))
			{
				bVCBAlertStatus = FALSE;
			}
			else
			{
				bVCBAlertStatus = TRUE;
			}

			if ((fVCBThreshold != fVCBThresOLD) || (nVCBMaxGrcPer != nVCBMaxGPOld))
			{
				byActivateVCB = 1;

				fVCBThresOLD = fVCBThreshold;
				nVCBMaxGPOld = nVCBMaxGrcPer;

				bVCBAlertStatus = FALSE;
				// set VCB alert status
				cszKeyNm.Empty();
				cszKeyNm.Format(STR_ALERT_VCB_ALERT, pThreadAlertInfo->nIniFilePos);

				WritePrivateProfileString(cszSectionNm, cszKeyNm, STR_NO, cszConfigFile);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			if (byActivateVCB)
			{
				byActivateVCB = 0;

				// ENABLE VCB
				PduOutletConfigVCB(cszPduAddress.GetBuffer(cszPduAddress.GetLength()), 
					               pThreadAlertInfo->nBankNum, 
								   pThreadAlertInfo->nOCBNum, 
								   pThreadAlertInfo->nOutletNum, 
								   pThreadAlertInfo->nRealOutlet, 
								   VCB_ENABLE, 
								   fVCBThreshold, 
								   nVCBMaxGrcPer, 
								   &byErrcode, 
								   FALSE);
				cszPduAddress.ReleaseBuffer();
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			PduGetOutletState(cszPduAddress.GetBuffer(cszPduAddress.GetLength()), 
				              pThreadAlertInfo->nBankNum, 
							  pThreadAlertInfo->nOCBNum, 
							  pThreadAlertInfo->nOutletNum, 
							  pThreadAlertInfo->nRealOutlet, 
							  &byCurrStateValue, 
							  &byErrcode, 
							  FALSE);
			cszPduAddress.ReleaseBuffer();

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			if ((byCurrStateValue == RELAY_TRIP) && (!bVCBAlertStatus))
			{
				// set VCB alert status
				cszKeyNm.Empty();
				cszKeyNm.Format(STR_ALERT_VCB_ALERT, pThreadAlertInfo->nIniFilePos);

				WritePrivateProfileString(cszSectionNm, cszKeyNm, STR_YES, cszConfigFile);

				fActualCurrent = PduGetOutletCurrent(cszPduAddress.GetBuffer(cszPduAddress.GetLength()), 
													 pThreadAlertInfo->nBankNum, 
													 pThreadAlertInfo->nOCBNum, 
													 pThreadAlertInfo->nOutletNum, 
													 pThreadAlertInfo->nRealOutlet, 
													 &byErrcode, 
													 FALSE);
				cszPduAddress.ReleaseBuffer();

				UpdateAlertMessage(cszPduAddress, 
								   pThreadAlertInfo->nBankNum, 
								   pThreadAlertInfo->nOCBNum, 
								   pThreadAlertInfo->nOutletNum, 
								   pThreadAlertInfo->nRealOutlet, 
								   ALERT_VCB, 
								   fActualCurrent);
			}
			else if (byCurrStateValue != RELAY_TRIP)
			{
				// set VCB alert status to check and send message
				cszKeyNm.Empty();
				cszKeyNm.Format(STR_ALERT_VCB_ALERT, pThreadAlertInfo->nIniFilePos);

				WritePrivateProfileString(cszSectionNm, cszKeyNm, STR_NO, cszConfigFile);
			}
		}
		else if (byVCBOldEnabled)
		{
			byVCBOldEnabled = 0;
			byActivateVCB = 1;

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			// Disable the VCB configuration
			PduOutletConfigVCB(cszPduAddress.GetBuffer(cszPduAddress.GetLength()), 
							   pThreadAlertInfo->nBankNum, 
							   pThreadAlertInfo->nOCBNum, 
							   pThreadAlertInfo->nOutletNum, 
							   pThreadAlertInfo->nRealOutlet, 
							   VCB_DISABLE, 
							   0.0, 
							   0, 
							   &byErrcode, 
							   FALSE);
			cszPduAddress.ReleaseBuffer();

			// set VCB alert status
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_VCB_ALERT, pThreadAlertInfo->nIniFilePos);

			WritePrivateProfileString(cszSectionNm, cszKeyNm, STR_NO, cszConfigFile);

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;
		}

		if ((!byVCBEnabled) && (!byHIGHEnabled) && (!byLOWEnabled))
		{
			m_byThreadExit.SetAt(pThreadAlertInfo->nThreadPos, 1);
			break;
		}

		if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
			break;
		
		if (byHIGHEnabled)
		{
			///////////////////////////// HCA CHECKING /////////////////////////////////////

			// read threshold value
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_HCA_THRESHOLD_KEY, pThreadAlertInfo->nIniFilePos);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
			if (lstrcmpi(szRetBuff, STR_FAIL))
			{
				fHCAThreshold = (float)atof(szRetBuff);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			// read max grace period value
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_HCA_MAX_GRC_PER_KEY, pThreadAlertInfo->nIniFilePos);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
			if (lstrcmpi(szRetBuff, STR_FAIL))
			{
				nHCAMaxGrcPer = (int)atoi(szRetBuff);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			if ((fHCAThreshold != fHCAThresOLD) || (nHCAMaxGrcPer != nHCAMaxGPOld))
			{
				nHIGHMaxSecs = 0;
				fHCAThresOLD = fHCAThreshold;
				nHCAMaxGPOld = nHCAMaxGrcPer;
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;
			
			fActualCurrent = PduGetOutletCurrent(cszPduAddress.GetBuffer(cszPduAddress.GetLength()), 
				                                 pThreadAlertInfo->nBankNum, 
												 pThreadAlertInfo->nOCBNum, 
												 pThreadAlertInfo->nOutletNum, 
												 pThreadAlertInfo->nRealOutlet, 
												 &byErrcode, 
												 FALSE);
			cszPduAddress.ReleaseBuffer();

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			if (fActualCurrent > fHCAThreshold)
			{
				nHIGHMaxSecs++;
			}
			else
			{
				nHIGHMaxSecs = 0;
			}

			if (nHIGHMaxSecs > nHCAMaxGrcPer)
			{
				// Need to send the HIGH CURRENT ALERT MESSAGE
				nHIGHMaxSecs = 0;

				if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
					break;
				
				UpdateAlertMessage(cszPduAddress, 
								   pThreadAlertInfo->nBankNum, 
								   pThreadAlertInfo->nOCBNum, 
								   pThreadAlertInfo->nOutletNum, 
								   pThreadAlertInfo->nRealOutlet, 
								   ALERT_HIGH_CURRENT, 
								   fActualCurrent);

				if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
					break;
			}

		}
		
		if (byLOWEnabled)
		{
			///////////////////////////// LCA CHECKING /////////////////////////////////////
			nLOWMaxSecs++;

			// read threshold value
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_LCA_THRESHOLD_KEY, pThreadAlertInfo->nIniFilePos);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
			if (lstrcmpi(szRetBuff, STR_FAIL))
			{
				fLCAThreshold = (float)atof(szRetBuff);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			// read max grace period value
			cszKeyNm.Empty();
			cszKeyNm.Format(STR_ALERT_LCA_MAX_GRC_PER_KEY, pThreadAlertInfo->nIniFilePos);

			memset(szRetBuff, 0, sizeof(szRetBuff));
			GetPrivateProfileString(cszSectionNm, cszKeyNm, STR_FAIL, szRetBuff, sizeof(szRetBuff), cszConfigFile); 
			if (lstrcmpi(szRetBuff, STR_FAIL))
			{
				nLCAMaxGrcPer = (int)atoi(szRetBuff);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			if ((fLCAThreshold != fLCAThresOLD) || (nLCAMaxGrcPer != nLCAMaxGPOld))
			{
				nLOWMaxSecs = 0;
				fLCAThresOLD = fLCAThreshold;
				nLCAMaxGPOld = nLCAMaxGrcPer;
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			fActualCurrent = PduGetOutletCurrent(cszPduAddress.GetBuffer(cszPduAddress.GetLength()), 
												 pThreadAlertInfo->nBankNum, 
												 pThreadAlertInfo->nOCBNum, 
												 pThreadAlertInfo->nOutletNum, 
												 pThreadAlertInfo->nRealOutlet, 
												 &byErrcode, 
												 FALSE);
			cszPduAddress.ReleaseBuffer();

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			if (fActualCurrent < fLCAThreshold)
			{
				nLOWMaxSecs++;
			}
			else
			{
				nLOWMaxSecs = 0;
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;

			if (nLOWMaxSecs > nLCAMaxGrcPer)
			{
				nLOWMaxSecs = 0;

				if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
					break;

				UpdateAlertMessage(cszPduAddress, 
								   pThreadAlertInfo->nBankNum, 
								   pThreadAlertInfo->nOCBNum, 
								   pThreadAlertInfo->nOutletNum, 
								   pThreadAlertInfo->nRealOutlet, 
								   ALERT_LOW_CURRENT, 
								   fActualCurrent);
			}

			if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
				break;
		}

		if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
			break;

		Sleep(1000); // Wait for 1 sec and again check the current value

		if (m_byThreadExit.ElementAt(pThreadAlertInfo->nThreadPos))
			break;
	}

	delete pThreadAlertInfo;

	return 0;
}

BOOL UpdateAlertMessage(CString cszPduadd, BYTE byBank, BYTE byOCB, BYTE byOutlet, BYTE byRealOutlet, UINT nAlertType, float fCurrThres)
{
	BOOL				bRet = FALSE;
	UINT				nTotal = 0;
	CString				cszLoad;
	CString				cszDateTime;
	int					nResult = 0;
	CHAR				szErrMsg[MAX_PATH] = {0};
	sqlite3				*db; // sqlite3 db struct

	cszLoad.Empty();

	nResult = sqlite3_open_v2(STR_PDU_CONFG_DB_NM, &db, SQLITE_OPEN_READWRITE, NULL);
	if (nResult != SQLITE_OK)
	{
		nResult = sqlite3_open_v2(STR_PDU_CONFG_DB_NM, &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL);
		if (nResult != SQLITE_OK)
		{
			cszLoad.Format("Can't open database: %s", sqlite3_errmsg(db));
		}
		else
		{
			cszLoad = "Open database successfully";
			
			cszLoad.Empty();
			cszLoad = "create table pduAlertMess (SER_NO INTEGER PRIMARY KEY, PDU_NAME varchar(66), BANK_NUM smallint, OCB_NUM smallint, OUTLET_NUM smallint, REAL_OUTLET_NUM smallint, ALERT_TYPE smallint, THRES_VALUE float, DATE_TIME varchar(42), STATUS_NUM smallint)";
			nResult = sqlite3_exec(db, cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 0, (char **)szErrMsg);
			cszLoad.ReleaseBuffer();
		}
	}
	else
	{
		cszLoad = "Open database successfully";
	}
	//AfxMessageBox(cszLoad);

	cszDateTime.Empty();
	cszDateTime = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

	cszLoad.Empty();
	cszLoad.Format("insert into pduAlertMess (PDU_NAME, BANK_NUM, OCB_NUM, OUTLET_NUM, REAL_OUTLET_NUM, ALERT_TYPE, THRES_VALUE, DATE_TIME, STATUS_NUM) values ('%s', %d, %d, %d, %d, %d, %f, '%s', %d)", 
					 cszPduadd, byBank, byOCB, byOutlet, byRealOutlet, nAlertType, fCurrThres, cszDateTime, MESS_CREATED);
	nResult = sqlite3_exec(db, cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 0, (char **)szErrMsg);
	cszLoad.ReleaseBuffer();

	sqlite3_close(db);

	return bRet;
}