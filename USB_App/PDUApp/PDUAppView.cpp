// PDUAppView.cpp : implementation of the CPDUAppView class
//

#include "stdafx.h"
#include "PDUApp.h"

#include "General.h"
#include "PDUAppView.h"
#include "Splash.h"
#include "AlertMess.h"
#include "AlertDlg.h"
#include "DlgStaticInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPDUAppView

IMPLEMENT_DYNCREATE(CPDUAppView, CTreeListView)

BEGIN_MESSAGE_MAP(CPDUAppView, CTreeListView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_OUTLET_ON, &CPDUAppView::OnSendON)
	ON_COMMAND(ID_OUTLET_OFF, &CPDUAppView::OnSendOFF)
	ON_COMMAND(ID_OUTLET_REBOOT, &CPDUAppView::OnSendREBOOT)
	ON_COMMAND(ID_OUTLET_REFRESH, &CPDUAppView::OnOutletRefresh)
	ON_WM_DESTROY()
//	ON_WM_ENDSESSION()
//	ON_WM_NCDESTROY()
ON_WM_TIMER()
END_MESSAGE_MAP()

// CPDUAppView construction/destruction

CPDUAppView::CPDUAppView()
{
	// TODO: add construction code here
	m_nPollSecs = 0;
	m_pThread = NULL;
	m_bThreadExit = FALSE;
}

CPDUAppView::~CPDUAppView()
{
	if (m_hPDUAPIDLL)
	{
		FreeLibrary(m_hPDUAPIDLL);
		m_hPDUAPIDLL = NULL;
	}

	if (m_cImages.m_hImageList != NULL)
	{
		m_cImages.DeleteImageList();
	}
}

BOOL CPDUAppView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CTreeListView::PreCreateWindow(cs);
}

void CPDUAppView::OnInitialUpdate()
{
	CTreeListView::OnInitialUpdate();

	CPDUAppApp		*pApp = (CPDUAppApp *)AfxGetApp();

	if (pApp)
	{
		pApp->pLeftView = this;
	}

	if (m_cImages.m_hImageList == NULL)
	{
		CString					cszLoad;
		CTreeCtrl				&cTreeVw = GetTreeCtrl();

		m_cImages.Create(12, 12, ILC_COLOR32 | ILC_MASK, 0, 1);
		
	/*	bm.LoadBitmap(IDB_BITMAP1);
		m_cImages.Add(&bm, RGB(255, 255, 255));
		bm.DeleteObject();
		bm.LoadBitmap(IDB_BITMAP2);
		m_cImages.Add(&bm, RGB(255, 255, 255));*/

		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_PDU));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_BANK));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_GRAY));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_GREEN));
		m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_RED));

		// ADD REQUIRED COLUMN HEADERS

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_PDU_HEADER_NM);
		InsertColumn(0, cszLoad, LVCFMT_LEFT, 260);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_VOLTAGE_HEADER_NM);		// VOLTAGE
		InsertColumn(1, cszLoad, LVCFMT_LEFT, 80);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_CURRENT_HEADER_NM);		// CURRENT
		InsertColumn(2, cszLoad, LVCFMT_LEFT, 80);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_POWER_FACTOR_HEADER_NM);	// POWER FACTOR
		InsertColumn(3, cszLoad, LVCFMT_LEFT, 130);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_FREQ_HEADER_NM);			// FREQUENCY
		InsertColumn(4, cszLoad, LVCFMT_LEFT, 100);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_APPR_POWER_HEADER_NM);	// APPARENT POWER
		InsertColumn(5, cszLoad, LVCFMT_LEFT, 130);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_REAL_POWER_HEADER_NM);	// REAL POWER
		InsertColumn(6, cszLoad, LVCFMT_LEFT, 110);

#if 0

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_CREST_FACTOR_HEADER_NM);
		InsertColumn(4, cszLoad, LVCFMT_LEFT, 120);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_REAL_ENERGY_HEADER_NM);
		InsertColumn(6, cszLoad, LVCFMT_LEFT, 110);

#endif

		cTreeVw.SetImageList(&m_cImages, TVSIL_NORMAL);
		cTreeVw.SetImageList(&m_cImages, TVSIL_STATE);

		m_hPDUAPIDLL = NULL;
		LoadAPIDLL();

		SetTimer(UPDATE_PDU_DATA, 500, NULL);

		//GetNDisplayPDU();
		//GetNDisplayBanksOutlets();
		//GetNDisplayOutletData();
	}
}


// CPDUAppView diagnostics

#ifdef _DEBUG
void CPDUAppView::AssertValid() const
{
	CTreeListView::AssertValid();
}

void CPDUAppView::Dump(CDumpContext& dc) const
{
	CTreeListView::Dump(dc);
}
#endif //_DEBUG

// CPDUAppView message handlers

void CPDUAppView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE0("CMyTreeCtrl::OnRClick()\n");


}

void CPDUAppView::OnContextMenu(CWnd* pWnd, CPoint ptMousePos)
{
#if 0
	// TODO: Add your message handler code here
	CTreeCtrl		&cTreeVw = GetTreeCtrl();
    CMenu			menu;
	CMenu			*pop;

	menu.LoadMenu(IDR_MENU_OUTLET_LED);
    pop = menu.GetSubMenu(0);

	if (ptMousePos.x == -1 )
	{
		HTREEITEM	ht = cTreeVw.GetSelectedItem();
		RECT		rect;

		cTreeVw.GetItemRect(ht, &rect, true);
		cTreeVw.ClientToScreen(&rect);
		ptMousePos.x = rect.left + 15;
		ptMousePos.y = rect.top + 8;
	}

	CWnd	*pMenuParent = AfxGetMainWnd();

	UINT uCmd = pop->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMousePos.x, ptMousePos.y, pMenuParent, NULL);

	if (uCmd != 0)
    {
		/* temporarily select any right-clicked item that's different from the currently selected item */
		if (m_hActiveItem)
		{
			HTREEITEM hOldSel = cTreeVw.GetSelectedItem();

			cTreeVw.Select(m_hActiveItem, TVGN_CARET);

			/* Execute the selected menu command */
			pMenuParent->SendMessage(WM_COMMAND, uCmd, 0);

			cTreeVw.Select(hOldSel, TVGN_CARET);
		}
		else
		{
			/* Execute the selected menu command */
			pMenuParent->SendMessage(WM_COMMAND, uCmd, 0);
		}
	}
    
	/* no longer need the temporary signal */
    m_hActiveItem = NULL;
	menu.DestroyMenu(); 
#endif
}




#if 0
void CPDUAppView::Convertstrtoword(char* szTmp, WORD *CMD)
{
	BYTE		m = 0;
	BYTE		n = 0;
	BYTE		x = 0;
	BYTE		z = 0;
	BYTE		bHi = 0;
	BYTE		bLo = 0;
	BYTE 		szTmp1[4] = {0};

	for (int y = 0; y < 4; y++)
	{
		if (*szTmp >= '0' && *szTmp <= '9')
		{
       		szTmp1[y] = *szTmp - '0';
		}
		else if (*szTmp >= 'A' && *szTmp <= 'F')
		{
   			szTmp1[y] = (szTmp1[y] << 4) + *szTmp - 'A' + 0xA;
		}
		else if (*szTmp >= 'a' && *szTmp <= 'f')
		{
   			szTmp1[y] = (szTmp1[y] << 4) + *szTmp - 'a' + 0xA;
		}

		szTmp++;
	}

	x = (BYTE)szTmp1[0];
	z = (BYTE)szTmp1[1];
	x = x << 4;	 

	bHi = x|z;

	m = (BYTE)szTmp1[2];
	n = (BYTE)szTmp1[3];
	m = m << 4;	 
	bLo = m|n;

	*CMD = MAKEWORD(bLo, bHi);
}
#endif

void CPDUAppView::LoadAPIDLL()
{
	//return;

	if (m_hPDUAPIDLL)
	{
		FreeLibrary(m_hPDUAPIDLL);
		m_hPDUAPIDLL = NULL;
	}

	m_hPDUAPIDLL = LoadLibrary(PDUAPI_DLL_NM);
	if (!m_hPDUAPIDLL)
	{
		AfxMessageBox(_T("Unable to load the ePowerUSB.dll file. Make sure the ePowerUSB.dll presence in exe path"));
		exit(0);
	}
}

BOOL CPDUAppView::GetNDisplayPDU()
{
	BOOL 						bRetStatus = FALSE;
	UINT						nNum = 0;
	WORD						wPid = 0;
	WORD						wVid = 0;
	MSG							mMsg = {0};
	BYTE						byErrCode = 0;
	CHAR						cTmpstr[5] = {0};
	DWORD						dwErr = 0;
	CString						cszLoad;
	CString						cszPDUName;
	BYTE						bySerialNum[16] = {0};
	BYTE						byDataBuffer[66] = {0};
	PDU_ADDRESS	 				stPDU_address = {0};
	FN_GETPDUDEVICE				fnGetPDUDevice = NULL;
	FN_GET_STATIC_INFO			fnGetStaticPDUInfo = NULL;
	CTreeCtrl					&cTreeVw = GetTreeCtrl();

	cTreeVw.DeleteAllItems();

	while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
	{
		if (!::IsDialogMessage(this->m_hWnd, &mMsg))
		{
			::TranslateMessage(&mMsg);
			::DispatchMessage(&mMsg);
		}
	}

#if 0
	GetDlgItemText(IDC_STATIC_VID, cTmpstr, 5); 
	Convertstrtoword(cTmpstr, &wVid);

	memset(cTmpstr, 0x00, sizeof(cTmpstr));

	GetDlgItemText(IDC_EDIT_PID, cTmpstr, 5); 
	Convertstrtoword(cTmpstr, &wPid);
#endif

#if 1
	fnGetPDUDevice = (FN_GETPDUDEVICE) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduEnumerateDevice");

	dwErr = GetLastError();
	if (fnGetPDUDevice)
	{
		bRetStatus = (*fnGetPDUDevice)(&stPDU_address, wPid, wVid, &byErrCode, TRUE);
	}

	fnGetStaticPDUInfo = (FN_GET_STATIC_INFO) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStaticInfoWriteAndRead");

#else	// For DEBUG MODE
	bRetStatus = TRUE;
	stPDU_address.byTotalPdu = 2;
	memcpy(stPDU_address.byPduAdd[0], _T("PDU device 1"), 12);
	memcpy(stPDU_address.byPduAdd[1], _T("PDU device 2"), 12);
	memcpy(stPDU_address.byPduAdd[2], _T("PDU device 3"), 12);
	memcpy(stPDU_address.byPduAdd[3], _T("PDU device 4"), 12);
	memcpy(stPDU_address.byPduAdd[4], _T("PDU device 5"), 12);
	memcpy(stPDU_address.byPduAdd[5], _T("PDU device 6"), 12);
#endif

	if (bRetStatus)
	{
		for (nNum = 0; nNum < stPDU_address.byTotalPdu; nNum++)
		{
			if (fnGetStaticPDUInfo)
			{
				memset(byDataBuffer, 0, sizeof(byDataBuffer));
				bRetStatus = (*fnGetStaticPDUInfo) ((CHAR *)stPDU_address.byPduAdd[nNum], PDU_SERIAL_NUM, byDataBuffer, 0, &byErrCode, TRUE, 0);
				memset(bySerialNum, 0, sizeof(bySerialNum));
				memcpy(bySerialNum, byDataBuffer, 15);

				cszLoad.Empty();
				cszLoad.Format("%s", bySerialNum);
				if (cszLoad.IsEmpty())
				{
					cszLoad.Empty();
					cszLoad.LoadString(IDS_STR_SER_NUM_ERR);
					AfxMessageBox(cszLoad, MB_ICONSTOP);
					continue;
				}
			}
			
			// Allocate memory for the available PDU devices
			gpNewPDUInfo = (PPDU_TREE_DET) Malloc(sizeof(PDU_TREE_DET));
			if (!gpNewPDUInfo)
			{
				AfxMessageBox(IDS_STR_NO_MEM, MB_OK | MB_ICONSTOP);
				break;
			}
			memset(gpNewPDUInfo, 0, sizeof(PDU_TREE_DET));

			memcpy(gpNewPDUInfo->byPDUAddrs, stPDU_address.byPduAdd[nNum], sizeof(stPDU_address.byPduAdd[nNum]));
			memcpy(gpNewPDUInfo->bySerialNum, bySerialNum, 15);
			gpNewPDUInfo->byPDUPos = (nNum + 1);

			cszLoad.Empty();
			cszLoad.LoadString(IDS_STR_PDU_NM);

			cszPDUName.Empty();
			cszPDUName.Format(cszLoad, gpNewPDUInfo->byPDUPos);

			gpNewPDUInfo->hPDUTreeHandle = cTreeVw.InsertItem(cszPDUName, PDU_ICON, PDU_ICON, TVI_ROOT);

			if (!nNum)
			{
				gpHeadPDUInfo = gpNewPDUInfo;
			}
			else
			{
				gpCurrPDUInfo->pNextPDUInfo = gpNewPDUInfo;
				gpNewPDUInfo->pPrevPDUInfo = gpCurrPDUInfo;
			}

			gpCurrPDUInfo = gpNewPDUInfo;

			//m_cszPDUDevArray.Add((LPCSTR)stPDU_address.byPduAdd[nNum]);
		}
	}

	if (!gpHeadPDUInfo)
	{
		bRetStatus = FALSE;
		AfxMessageBox(IDS_STR_NO_PDU_MESS, MB_OK | MB_ICONSTOP);


		CPDUAppApp				*pApp = (CPDUAppApp *)AfxGetApp();

		cszPDUName.Empty();
		cszPDUName.LoadString(IDS_STR_NO_PDU_MESS);

		cszLoad.Empty();
		cszLoad = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

		if (pApp)
		{
			pApp->pBottomView->DisplayLogMessage(LOG_MESS_FAIL, cszPDUName, cszLoad);
		}

	}
	else
	{
		bRetStatus = TRUE;
	}

	return bRetStatus;
}

void CPDUAppView::GetNDisplayBanksOutlets()
{
	BOOL 								bRetStatus = FALSE;
	UINT								nNum = 0;
	UINT								nOut = 0;
	UINT								nOCBCount = 0;
	UINT								nOCB = 0;
	UINT								nOutlet = 0;
	MSG									mMsg = {0};
	BYTE								byDataBuffer[66] = {0};
	BYTE								byOutletRelayStatus = 0;
	BYTE								byPDUDevNm[MAX_PATH] = {0};
	BYTE								byErrCode = 0;
	DWORD								dwErr = 0;
	CString								cszLoad;
	CString								cszName;
	BANK_OUTLET_INFO					stBankOcbInfo = {0};
	CTreeCtrl							&cTreeVw = GetTreeCtrl();
	FN_GET_STATIC_INFO					fnGetStaticPDUInfo = NULL;
	FN_GET_BANK_OCB_COUNTS				fnGetBanksOutletsCnt = NULL;
	FN_GET_OUTLET_STATE_FRM_CONFIG		fnGetOutletStateConfig = NULL;

	while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
	{
		if (!::IsDialogMessage(this->m_hWnd, &mMsg))
		{
			::TranslateMessage(&mMsg);
			::DispatchMessage(&mMsg);
		}
	}

#if 1
	fnGetBanksOutletsCnt = (FN_GET_BANK_OCB_COUNTS) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduGetBankOcbInformation");
	dwErr = GetLastError();
#endif

	gpCurrPDUInfo = gpHeadPDUInfo;

	while (gpCurrPDUInfo)
	{
		nOCBCount = 1;
		nOCB = 1;

		nOutlet = 0;
		memset(byPDUDevNm, 0, sizeof(byPDUDevNm));
		memcpy(byPDUDevNm, gpCurrPDUInfo->byPDUAddrs, sizeof(gpCurrPDUInfo->byPDUAddrs));

#if 1

		if (fnGetBanksOutletsCnt)
		{
			bRetStatus = (*fnGetBanksOutletsCnt) ((BYTE *)byPDUDevNm, &stBankOcbInfo, &byErrCode, TRUE);
		}

#else	// For DEBUG MODE only
		bRetStatus = TRUE;
		stBankOcbInfo.dwTotalBanks = 1;
		stBankOcbInfo.dwOcbsOnBank[0] = 4;
		//stBankOcbInfo.byOcbsOnBank[1][0] = 8;
#endif

		if (bRetStatus)
		{
			if (m_hPDUAPIDLL)
			{
				fnGetOutletStateConfig = (FN_GET_OUTLET_STATE_FRM_CONFIG) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduGetOutletStateFromConfig");
				fnGetStaticPDUInfo = (FN_GET_STATIC_INFO) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStaticInfoWriteAndRead");
			}

			for (nNum = 0; nNum < stBankOcbInfo.dwTotalBanks; nNum++)
			{
				nOCBCount = 1;

				// Allocate memory for available BANKS in the current PDU Device.
				gpNewBankInfo = (PPDU_BANK_INFO) Malloc(sizeof(PDU_BANK_INFO));
				if (!gpNewBankInfo)
				{
					AfxMessageBox(IDS_STR_NO_MEM, MB_OK | MB_ICONSTOP);
					break;	
				}
				memset(gpNewBankInfo, 0, sizeof(PDU_BANK_INFO));

				gpNewBankInfo->dwTotOUTLET = stBankOcbInfo.dwOutletsOnBank[nNum];
				gpNewBankInfo->byBANKPos = (nNum + 1);

				cszLoad.Empty();
				cszLoad.LoadString(IDS_STR_BANK_NM);

				cszName.Empty();
				cszName.Format(cszLoad, gpNewBankInfo->byBANKPos);

				gpNewBankInfo->hBANKTreeHandle = cTreeVw.InsertItem(cszName, BANK_ICON, BANK_ICON, gpCurrPDUInfo->hPDUTreeHandle);

				if (!nNum)
				{
					gpCurrPDUInfo->pBankInfo = gpNewBankInfo; 
				}
				else
				{
					gpCurrBankInfo->pNextBankInfo = gpNewBankInfo;
					gpNewBankInfo->pPrevBankInfo = gpCurrBankInfo;
				}

				gpCurrBankInfo = gpNewBankInfo;
				cTreeVw.Expand(gpCurrPDUInfo->hPDUTreeHandle, TVE_EXPAND);

				// Add available OUTELTS to the current BANK
				for (nOut = 0; nOut < gpCurrBankInfo->dwTotOUTLET; nOut++)
				{
					// Allocate memory for available OUTLETS in each BANK of the current PDU device.
					gpNewOutletInfo = (PPDU_OUTLET_INFO) Malloc(sizeof(PDU_OUTLET_INFO));
					if (!gpNewOutletInfo)
					{
						AfxMessageBox(IDS_STR_NO_MEM, MB_OK | MB_ICONSTOP);
						break;	
					}
					memset(gpNewOutletInfo, 0, sizeof(PDU_OUTLET_INFO));

					nOutlet++;

					gpNewOutletInfo->byOCBNum = nOCB;
					gpNewOutletInfo->byRealOutlet = nOCBCount;
					//gpNewOutletInfo->byOUTLETPos = (nOut + 1);
					gpNewOutletInfo->byOUTLETPos = nOutlet;

					if (nOCBCount == OUTLET_PER_OCB)
					{
						nOCB++;
						nOCBCount = 1;
					}
					else
					{
						nOCBCount++;
					}

					//fnGetOutletStateConfig = (FN_GET_OUTLET_STATE_FRM_CONFIG) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduGetOutletStateFromConfig");
					if (fnGetOutletStateConfig)
					{
						bRetStatus = (*fnGetOutletStateConfig) ((CHAR *)gpCurrPDUInfo->bySerialNum, 
																(CHAR *)byPDUDevNm, 
																gpCurrBankInfo->byBANKPos,
																gpNewOutletInfo->byOCBNum, 
																gpNewOutletInfo->byOUTLETPos, 
																gpNewOutletInfo->byRealOutlet, 
																&byOutletRelayStatus, &byErrCode, TRUE);
					}

					gpNewOutletInfo->byRelayStatus = byOutletRelayStatus;

					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					if (fnGetStaticPDUInfo)
					{
						bRetStatus = (*fnGetStaticPDUInfo) ((CHAR *)byPDUDevNm, PDU_GET_SET_OUTLET_NAME, byDataBuffer, 0, &byErrCode, FALSE, nOutlet);
					}

					if (byDataBuffer[0] == 0xFF)
					{
						memset(byDataBuffer, 0, sizeof(byDataBuffer));
					}

					cszName.Empty();
					cszName.Format(_T("%s"), byDataBuffer);
					
					/*cszLoad.Empty();
					cszLoad.LoadString(IDS_STR_OUTLET_NM);

					cszName.Empty();
					cszName.Format(cszLoad, nOutlet);//gpNewOutletInfo->byOUTLETPos);*/

					//gpNewOutletInfo->hOUTLETTreeHandle = cTreeVw.InsertItem(cszName, 0, 0, gpCurrBankInfo->hBANKTreeHandle);
					gpNewOutletInfo->hOUTLETTreeHandle = cTreeVw.InsertItem(cszName, gpCurrBankInfo->hBANKTreeHandle);

					if (byOutletRelayStatus == RELAY_OPEN)
						byOutletRelayStatus = OUTLET_OFF_ICON;
					else if (byOutletRelayStatus == RELAY_CLOSE)
						byOutletRelayStatus = OUTLET_ON_ICON;
					else if (byOutletRelayStatus == RELAY_TRIP)
						byOutletRelayStatus = OUTLET_TRIP_ICON;

					cTreeVw.SetItemImage(gpNewOutletInfo->hOUTLETTreeHandle, byOutletRelayStatus, byOutletRelayStatus);

					if (!nOut)
					{
						gpCurrBankInfo->pOutletInfo = gpNewOutletInfo;
					}
					else
					{
						gpCurrOutletInfo->pNextOutletInfo = gpNewOutletInfo;
						gpNewOutletInfo->pPrevOutletInfo = gpCurrOutletInfo;
					}

					gpCurrOutletInfo = gpNewOutletInfo;

				} // for (nOut

				cTreeVw.Expand(gpCurrBankInfo->hBANKTreeHandle, TVE_EXPAND);

			} // for (nNum

			if (!stBankOcbInfo.dwTotalBanks)
			{
				CString					cszTemp;
				CPDUAppApp				*pApp = (CPDUAppApp *)AfxGetApp();
			
				cszTemp.Empty();
				cszTemp.LoadString(IDS_STR_PDU_NM);

				cszLoad.Empty();
				cszLoad.Format(cszTemp, gpCurrPDUInfo->byPDUPos);

				cszTemp.Empty();
				cszTemp.LoadString(IDS_STR_ZERO_BANK_OUTLET_MESS);

				cszName.Empty();
				cszName.Format(cszTemp, cszLoad);

				cszLoad.Empty();
				cszLoad = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

				if (pApp)
				{
					pApp->pBottomView->DisplayLogMessage(LOG_MESS_FAIL, cszName, cszLoad);
				}
			}

		} // if bRetStatus
		else
		{
			CPDUAppApp				*pApp = (CPDUAppApp *)AfxGetApp();
			
			cszLoad.Empty();
			cszLoad = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

			cszName.Empty();
			cszName.LoadString(IDS_STR_NO_BANK_OUTLET_MESS);

			if (pApp)
			{
				pApp->pBottomView->DisplayLogMessage(LOG_MESS_FAIL, cszName, cszLoad);
			}
		}

		gpCurrPDUInfo = gpCurrPDUInfo->pNextPDUInfo;
	}

}

void CPDUAppView::GetNDisplayOutletData()
{
	BOOL 						bRetStatus = FALSE;
	UINT						nNum = 0;
	BYTE						byBank = 0;
	BYTE						byOCB = 0;
	MSG							mMsg = {0};
	CString						cszLoad;
	BYTE						byPDUDevNm[MAX_PATH] = {0};
	BYTE						byErrCode = 0;
	CTreeCtrl					&cTreeVw = GetTreeCtrl();
	OUTLET_DATA_INFO			stOcb_Data_Info = {0};
	FN_GET_OUTLET_DATA			fnGetOutletData = NULL;

	while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
	{
		if (!::IsDialogMessage(this->m_hWnd, &mMsg))
		{
			::TranslateMessage(&mMsg);
			::DispatchMessage(&mMsg);
		}
	}

	fnGetOutletData = (FN_GET_OUTLET_DATA) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduGetOUTLETData");

	gpCurrPDUInfo = gpHeadPDUInfo;

	while (gpCurrPDUInfo)
	{
		memset(byPDUDevNm, 0, sizeof(byPDUDevNm));
		memcpy(byPDUDevNm, gpCurrPDUInfo->byPDUAddrs, sizeof(gpCurrPDUInfo->byPDUAddrs));

		gpCurrBankInfo = gpCurrPDUInfo->pBankInfo;

		if (m_bThreadExit)
			return;

		while (gpCurrBankInfo)
		{
			gpCurrOutletInfo = gpCurrBankInfo->pOutletInfo;

			if (m_bThreadExit)
				return;

			while (gpCurrOutletInfo)
			{
				if (m_bThreadExit)
					return;

				if (fnGetOutletData)
				{
					memset(&stOcb_Data_Info, 0, sizeof(OUTLET_DATA_INFO));
					//__declspec(dllexport) BOOL PduGetOUTLETData(CHAR *byPduadd, BYTE byBank, BYTE byOCB, POUTLET_DATA_INFO pOcb_Data_Info, BYTE *byErrCode, BOOL bLog)
					bRetStatus = (*fnGetOutletData) ((CHAR *)byPDUDevNm, 
													 gpCurrBankInfo->byBANKPos, 
													 gpCurrOutletInfo->byOCBNum, 
													 gpCurrOutletInfo->byOUTLETPos, 
													 gpCurrOutletInfo->byRealOutlet, 
													 &stOcb_Data_Info, 
													 &byErrCode, 
													 TRUE);
				}

				if (m_bThreadExit)
					return;

				if (bRetStatus)
				{
					cszLoad.Empty();
					cszLoad.Format(_T("%.2f V"), stOcb_Data_Info.fVoltage);
					SetSubItemText(gpCurrOutletInfo->hOUTLETTreeHandle, VOLTAGE_COL_ID, cszLoad);
					gpCurrOutletInfo->fVoltage = stOcb_Data_Info.fVoltage;

					if (m_bThreadExit)
						return;

					cszLoad.Empty();
					cszLoad.Format(_T("%.2f A"), stOcb_Data_Info.fCurrent);
					SetSubItemText(gpCurrOutletInfo->hOUTLETTreeHandle, CURRENT_COL_ID, cszLoad);
					gpCurrOutletInfo->fCurrent = stOcb_Data_Info.fCurrent;

					if (m_bThreadExit)
						return;

					cszLoad.Empty();
					cszLoad.Format(_T("%.2f"), stOcb_Data_Info.fPoFactor);
					SetSubItemText(gpCurrOutletInfo->hOUTLETTreeHandle, POWER_FACTOR_COL_ID, cszLoad);
					gpCurrOutletInfo->fPoFactor = stOcb_Data_Info.fPoFactor;

					if (m_bThreadExit)
						return;

					cszLoad.Empty();
					cszLoad.Format(_T("%.2f Hz"), stOcb_Data_Info.fFrequency);
					SetSubItemText(gpCurrOutletInfo->hOUTLETTreeHandle, FREQUENCY_COL_ID, cszLoad);
					gpCurrOutletInfo->fFrequency = stOcb_Data_Info.fFrequency;

					if (m_bThreadExit)
						return;

					cszLoad.Empty();
					cszLoad.Format(_T("%.2f VA"), stOcb_Data_Info.fApparentPow);
					SetSubItemText(gpCurrOutletInfo->hOUTLETTreeHandle, APPARENT_POWER_COL_ID, cszLoad);

					gpCurrOutletInfo->fReEnergy = stOcb_Data_Info.fReEnergy;
					gpCurrOutletInfo->fCrFactor = stOcb_Data_Info.fCrFactor;

					if (m_bThreadExit)
						return;

					cszLoad.Empty();
					cszLoad.Format(_T("%.2f W"), stOcb_Data_Info.fReEnergy);
					SetSubItemText(gpCurrOutletInfo->hOUTLETTreeHandle, REAL_POWER_COL_ID, cszLoad);

					if (m_bThreadExit)
						return;

					cTreeVw.EnsureVisible(gpCurrOutletInfo->hOUTLETTreeHandle);

					cTreeVw.SetRedraw(FALSE);
					cTreeVw.SetRedraw(TRUE);

					while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
					{
						if (m_bThreadExit)
							return;

						if (!::IsDialogMessage(this->m_hWnd, &mMsg))
						{
							::TranslateMessage(&mMsg);
							::DispatchMessage(&mMsg);
						}
					}

					if (m_bThreadExit)
						return;
				}
				else
				{
					CString					cszPDUStr;
					CString					cszBankStr;
					CString					cszOutletStr;
					CString					cszMessage;
					CPDUAppApp				*pApp = (CPDUAppApp *)AfxGetApp();
			
					// PDU NAME
					cszLoad.Empty();
					cszLoad.LoadString(IDS_STR_PDU_NM);

					cszPDUStr.Empty();
					cszPDUStr.Format(cszLoad, gpCurrPDUInfo->byPDUPos);

					// BANK NAME
					cszLoad.Empty();
					cszLoad.LoadString(IDS_STR_BANK_NM);

					cszBankStr.Empty();
					cszBankStr.Format(cszLoad, gpCurrBankInfo->byBANKPos);

					// OUTLET NAME
					//cszLoad.Empty();
					//cszLoad.LoadString(IDS_STR_OUTLET_NM);

					cszOutletStr.Empty();
					cszOutletStr = cTreeVw.GetItemText(gpCurrOutletInfo->hOUTLETTreeHandle);
					//Format(cszLoad, gpCurrOutletInfo->byOUTLETPos);
	
					cszLoad.Empty();
					cszLoad.LoadString(IDS_STR_FETCH_OUTLET_DATA_ERR);

					cszMessage.Empty();
					cszMessage.Format(cszLoad, cszPDUStr, cszBankStr, cszOutletStr);

					cszLoad.Empty();
					cszLoad = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");
	
					if (pApp)
					{
						pApp->pBottomView->DisplayLogMessage(LOG_MESS_FAIL, cszMessage, cszLoad);
					}
				}

				gpCurrOutletInfo = gpCurrOutletInfo->pNextOutletInfo;
			}

			gpCurrBankInfo = gpCurrBankInfo->pNextBankInfo;
		}

		gpCurrPDUInfo =  gpCurrPDUInfo->pNextPDUInfo;
	}

	UpdateBankPDUReadings();

	cTreeVw.SetRedraw(FALSE);
	cTreeVw.SetRedraw(TRUE);

	if (m_bThreadExit)
		return;

	UpdateValuesInRightView();

}

BOOL CPDUAppView::UpdateBankPDUReadings()
{
	CString				cszLoad;
	MSG					mMsg = {0};
	float				fCurrentSum = 0;
	float				fVoltageSum = 0;
	float				fBankCurrentSum = 0;
	float				fBankVoltageSum = 0;
	PPDU_BANK_INFO		pPduBankInfo = NULL;
	PPDU_TREE_DET		pTreeDetails = NULL;
	PPDU_OUTLET_INFO	pPduOutletInfo = NULL;

	pTreeDetails = gpHeadPDUInfo;

	while (pTreeDetails)
	{
		while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
		{
			if (!::IsDialogMessage(this->m_hWnd, &mMsg))
			{
				::TranslateMessage(&mMsg);
				::DispatchMessage(&mMsg);
			}
		}

		pPduBankInfo = pTreeDetails->pBankInfo;

		if ((pPduBankInfo) && (pPduBankInfo->pOutletInfo))
		{
			fBankVoltageSum = pPduBankInfo->pOutletInfo->fVoltage;
		}

		while (pPduBankInfo)
		{
			pPduOutletInfo = pPduBankInfo->pOutletInfo;

			// Get VOLTAGE value
			if (pPduOutletInfo)
			{
				fVoltageSum = pPduOutletInfo->fVoltage;
			}

			// Get TOTAL CURRENT value
			while (pPduOutletInfo)
			{
				fCurrentSum += pPduOutletInfo->fCurrent;
				
				pPduOutletInfo = pPduOutletInfo->pNextOutletInfo;
			}

			// Print the Current & Voltage values
			cszLoad.Empty();
			cszLoad.Format(_T("%.2f V"), fVoltageSum);
			SetSubItemText(pPduBankInfo->hBANKTreeHandle, VOLTAGE_COL_ID, cszLoad);

			cszLoad.Empty();
			cszLoad.Format(_T("%.2f A"), fCurrentSum);
			SetSubItemText(pPduBankInfo->hBANKTreeHandle, CURRENT_COL_ID, cszLoad);

			fBankCurrentSum += fCurrentSum;

			pPduBankInfo = pPduBankInfo->pNextBankInfo;
			fCurrentSum = 0;
		}

		cszLoad.Empty();
		cszLoad.Format(_T("%.2f V"), fBankVoltageSum);
		SetSubItemText(pTreeDetails->hPDUTreeHandle, VOLTAGE_COL_ID, cszLoad);

		cszLoad.Empty();
		cszLoad.Format(_T("%.2f A"), fBankCurrentSum);
		SetSubItemText(pTreeDetails->hPDUTreeHandle, CURRENT_COL_ID, cszLoad);

		pTreeDetails = pTreeDetails->pNextPDUInfo;
	}

	return TRUE;
}

BOOL CPDUAppView::GetAllUIPositionNAddress(HTREEITEM hChildTreeItem, PCURR_SEL_DETAILS pCurrSelInfo, BYTE byNeed)
{
	BYTE						byPosition = 0;
	BOOL						bSelItemFound = FALSE;
	BOOL						bParentPDUFound = FALSE;
	HTREEITEM					hPDUParentTreeItem = NULL;
	PPDU_OUTLET_INFO			pSelOutletInfo = NULL;
	PPDU_BANK_INFO				pParentBankInfo = NULL;
	PPDU_TREE_DET				pParentPDUInfo = NULL;
	CTreeCtrl					&cTreeVw = GetTreeCtrl();

	if (hChildTreeItem == NULL)
		return FALSE;

	// Lets find out the PDU parent tree item
	hPDUParentTreeItem = hChildTreeItem;
	while (cTreeVw.GetParentItem(hPDUParentTreeItem) != NULL)
	{
		hPDUParentTreeItem = cTreeVw.GetParentItem(hPDUParentTreeItem);
	}

	if (hPDUParentTreeItem == NULL)
		return FALSE;

	pParentPDUInfo = gpHeadPDUInfo;

	// Lets identify the PDU Position in the Link list
	while (pParentPDUInfo)
	{
		if (pParentPDUInfo->hPDUTreeHandle == hPDUParentTreeItem)
		{
			bParentPDUFound = TRUE;
			break;
		}

		pParentPDUInfo = pParentPDUInfo->pNextPDUInfo;
	}

	if ((bParentPDUFound) && (pParentPDUInfo != NULL))
	{
		// Now find the selected outlet position in linked list
		pParentBankInfo = pParentPDUInfo->pBankInfo;

		while (pParentBankInfo)
		{
			if (byNeed == CHILD_OUTLET)
			{
				pSelOutletInfo = pParentBankInfo->pOutletInfo;
				while(pSelOutletInfo)
				{
					if (pSelOutletInfo->hOUTLETTreeHandle == hChildTreeItem)
					{
						bSelItemFound = TRUE;
						pCurrSelInfo->byCurrRealOutlet = pSelOutletInfo->byRealOutlet;
						pCurrSelInfo->byCurrOUTLETPos = pSelOutletInfo->byOUTLETPos;
						pCurrSelInfo->byCurrOCBPos = pSelOutletInfo->byOCBNum;
						pCurrSelInfo->byCurrBANKPos = pParentBankInfo->byBANKPos;
						pCurrSelInfo->byCurrPDUPos = pParentPDUInfo->byPDUPos;
						memcpy(pCurrSelInfo->byPDUAddress, pParentPDUInfo->byPDUAddrs, sizeof(pParentPDUInfo->byPDUAddrs));
						memcpy(pCurrSelInfo->bySerialNum, pParentPDUInfo->bySerialNum, sizeof(pParentPDUInfo->bySerialNum));
						break;
					}

					pSelOutletInfo = pSelOutletInfo->pNextOutletInfo;
				}
			}
			else if (byNeed == CHILD_BANK)
			{
				if (pParentBankInfo->hBANKTreeHandle == hChildTreeItem)
				{
					bSelItemFound = TRUE;
					pCurrSelInfo->byCurrBANKPos = pParentBankInfo->byBANKPos;
					pCurrSelInfo->byCurrPDUPos = pParentPDUInfo->byPDUPos;
					memcpy(pCurrSelInfo->byPDUAddress, pParentPDUInfo->byPDUAddrs, sizeof(pParentPDUInfo->byPDUAddrs));
					memcpy(pCurrSelInfo->bySerialNum, pParentPDUInfo->bySerialNum, sizeof(pParentPDUInfo->bySerialNum));
				}	
			}


			if (bSelItemFound)
				break;

			pParentBankInfo = pParentBankInfo->pNextBankInfo;
		}
	}

	return bSelItemFound;

}

void CPDUAppView::OnSendON()
{
	SendRelayStatus(RELAY_CLOSE);
}

void CPDUAppView::OnSendOFF()
{
	SendRelayStatus(RELAY_OPEN);
}

void CPDUAppView::OnSendREBOOT()
{
	SendRelayStatus(RELAY_REBOOT);
}

void CPDUAppView::SendRelayStatus(BYTE byRelayStatus)
{
	UINT					nNum = 0;
	CString					cszLoad;
	CString					cszCompare;
	HTREEITEM				hSelItem = NULL;
	CAlertMess				*cPopupMsg = new CAlertMess();
	BYTE					byItemLevel = ITEM_NONE;
	CTreeCtrl				&cTreeVw = GetTreeCtrl();

	hSelItem = cTreeVw.GetSelectedItem();

	if (hSelItem == NULL)
		return;

	for (nNum = 0; nNum < 3; nNum++)
	{
		cszLoad.Empty();
		cszLoad = cTreeVw.GetItemText(hSelItem);

		cszCompare.Empty();

		if (nNum == 0)
			cszCompare.LoadString(IDS_STR_PDU_DEV_NM_CHECK);
		else if (nNum == 1)
			cszCompare.LoadString(IDS_STR_BANK_NM_CHECK);
		else if (nNum == 2)
		{
			byItemLevel = CHILD_OUTLET;
			break;
			//	cszCompare.LoadString(IDS_STR_OUTLET_NM_CHECK);
		}

		if (m_bThreadExit)
			return;

		if (cszCompare.GetLength() <= cszLoad.GetLength())
		{
			cszLoad = cszLoad.Left(cszCompare.GetLength());
		}

		if (!cszCompare.CompareNoCase(cszLoad))
		{
			if (nNum == 0)
			{
				byItemLevel = PARENT_PDU;
				break;
			}
			else if (nNum == 1)
			{
				byItemLevel = CHILD_BANK;
				break;
			}
			else if (nNum == 2)
			{
				byItemLevel = CHILD_OUTLET;
				break;
			}
		}
	}

	cPopupMsg->Create(IDD_ALERT_POPUP, NULL);

	if (cPopupMsg)
	{
		cPopupMsg->AnimateWindow(1000, AW_ACTIVATE | AW_BLEND);
		cPopupMsg->ShowWindow(TRUE);
	}

	if (byItemLevel == PARENT_PDU)
	{
		HTREEITEM			hBankItem = NULL;
		HTREEITEM			hCurrItem = NULL;

		if (cTreeVw.ItemHasChildren(hSelItem))
		{
			hBankItem = cTreeVw.GetChildItem(hSelItem);

			while (hBankItem != NULL)
			{
				if (cTreeVw.ItemHasChildren(hBankItem))
				{
					hCurrItem = cTreeVw.GetChildItem(hBankItem);

					while (hCurrItem != NULL)
					{
						if (byRelayStatus == RELAY_CLOSE)
						{
							OnOutletOn(hCurrItem);
							Sleep(2000);
						}
						else if (byRelayStatus == RELAY_OPEN)
						{
							OnOutletOff(hCurrItem);
						}
						else if (byRelayStatus == RELAY_REBOOT)
						{
							OnOutletReboot(hCurrItem);
							Sleep(2000);
						}

						hCurrItem = cTreeVw.GetNextSiblingItem(hCurrItem);

					} // while (hCurrItem != NULL)

				} // if (cTreeVw.ItemHasChildren(hBankItem))

				hBankItem = cTreeVw.GetNextSiblingItem(hBankItem);

			} // while (hBankItem != NULL)

		} // if (cTreeVw.ItemHasChildren(hSelItem))
	}
	else if (byItemLevel == CHILD_BANK)
	{
		HTREEITEM			hCurrItem = NULL;

		if (cTreeVw.ItemHasChildren(hSelItem))
		{
			hCurrItem = cTreeVw.GetChildItem(hSelItem);

			while (hCurrItem != NULL)
			{
				if (byRelayStatus == RELAY_CLOSE)
				{
					OnOutletOn(hCurrItem);
					Sleep(2000);
				}
				else if (byRelayStatus == RELAY_OPEN)
				{
					OnOutletOff(hCurrItem);
					Sleep(500);
				}
				else if (byRelayStatus == RELAY_REBOOT)
				{
					OnOutletReboot(hCurrItem);
					Sleep(2000);
				}

				hCurrItem = cTreeVw.GetNextSiblingItem(hCurrItem);
			}
		}
	}
	else if (byItemLevel == CHILD_OUTLET)
	{
		if (byRelayStatus == RELAY_CLOSE)
			OnOutletOn(hSelItem);
		else if (byRelayStatus == RELAY_OPEN)
			OnOutletOff(hSelItem);
		else if (byRelayStatus == RELAY_REBOOT)
			OnOutletReboot(hSelItem);
	}

	
	if (cPopupMsg)
	{
		cPopupMsg->AnimateWindow(1000, AW_HIDE | AW_BLEND);
		cPopupMsg->DestroyWindow();
	}
}


void CPDUAppView::OnOutletOn(HTREEITEM hCurrItem)
{
	// TODO: Add your command handler code here
	BOOL								bResult = FALSE;
	BOOL								bFoundALL = FALSE;
	BYTE								byErrCode = 0;
	BYTE								byMessType = 0;
	MSG									mMsg = {0};
	CString								cszMessage;
	CString								cszDispalyResult;
	CString								cszOutletNm;
	CString								cszStatus;
	CURR_SEL_DETAILS					stCurrSelInfo = {0};
	CTreeCtrl							&cTreeVw = GetTreeCtrl();
	FN_SET_OUTLET_STATE					fnSetOutletState = NULL;
	CPDUAppApp							*pApp = (CPDUAppApp *)AfxGetApp();

	if (hCurrItem != NULL)
	{
		memset(&stCurrSelInfo, 0, sizeof(CURR_SEL_DETAILS));
		bFoundALL = GetAllUIPositionNAddress(hCurrItem, &stCurrSelInfo, CHILD_OUTLET);

		if (bFoundALL)
		{
			fnSetOutletState = (FN_SET_OUTLET_STATE) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduSwitchOutletState");

			if (fnSetOutletState)
			{
				bResult = (*fnSetOutletState) ((CHAR *)stCurrSelInfo.byPDUAddress, 
											   stCurrSelInfo.byCurrBANKPos, 
											   stCurrSelInfo.byCurrOCBPos, 
											   stCurrSelInfo.byCurrOUTLETPos, 
											   stCurrSelInfo.byCurrRealOutlet, 
											   RELAY_CLOSE, 
											   &byErrCode, 
											   TRUE);
			}

			cszOutletNm.Empty();
			cszOutletNm = cTreeVw.GetItemText(hCurrItem);

			cszMessage.Empty();
			cszStatus.Empty();
			cszDispalyResult.Empty();

			cszStatus.LoadString(IDS_STR_ON);

			if (bResult)
			{
				// Store the current Outlet Relay status
				FN_STORE_OUTLET_STATE_TO_CONFIG				fnStoreOutletStateToConfig;

				fnStoreOutletStateToConfig = (FN_STORE_OUTLET_STATE_TO_CONFIG) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStoreOutletStateToConfig");
				if (fnStoreOutletStateToConfig)
				{
					bResult = (*fnStoreOutletStateToConfig) ((CHAR *)stCurrSelInfo.bySerialNum, 
															 (CHAR *)stCurrSelInfo.byPDUAddress, 
															 stCurrSelInfo.byCurrBANKPos, 
															 stCurrSelInfo.byCurrOCBPos, 
															 stCurrSelInfo.byCurrOUTLETPos, 
															 stCurrSelInfo.byCurrRealOutlet, 
															 RELAY_CLOSE, 
															 &byErrCode, 
															 TRUE);
				}
			}

			if (bResult)
			{
				cszMessage.LoadString(IDS_STR_SEL_OUTLET_RELAY_MESS);
				cszDispalyResult.Format(cszMessage, cszOutletNm, cszStatus);

				cTreeVw.SetItemImage(hCurrItem, OUTLET_ON_ICON, OUTLET_ON_ICON);

				while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
				{
					if (!::IsDialogMessage(this->m_hWnd, &mMsg))
					{
						::TranslateMessage(&mMsg);
						::DispatchMessage(&mMsg);
					}
				}

				cszMessage.Empty();
				cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

				byMessType = LOG_MESS_PASS;
			}
			else
			{
				cszMessage.LoadString(IDS_STR_SEL_OUTLET_RELAY_ERR);
				cszDispalyResult.Format(cszMessage, cszStatus, cszOutletNm);

				cszMessage.Empty();
				cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

				byMessType = LOG_MESS_FAIL;
			}
		}
		else
		{
			cszDispalyResult.Empty();
			cszDispalyResult.LoadString(IDS_STR_SEL_OUTLET_ERR);

			cszMessage.Empty();
			cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

			byMessType = LOG_MESS_FAIL;
		}

		//AfxMessageBox(cszDispalyResult);
		if (pApp)
		{
			pApp->pBottomView->DisplayLogMessage(byMessType, cszDispalyResult, cszMessage);
		}
	}
}

void CPDUAppView::OnOutletOff(HTREEITEM hCurrItem)
{
	// TODO: Add your command handler code here
	BOOL								bResult = FALSE;
	BOOL								bFoundALL = FALSE;
	BYTE								byErrCode = 0;
	BYTE								byMessType = 0;
	MSG									mMsg = {0};
	CString								cszMessage;
	CString								cszDispalyResult;
	CString								cszOutletNm;
	CString								cszStatus;
	CURR_SEL_DETAILS					stCurrSelInfo = {0};
	CTreeCtrl							&cTreeVw = GetTreeCtrl();
	FN_SET_OUTLET_STATE					fnSetOutletState = NULL;
	CPDUAppApp							*pApp = (CPDUAppApp *)AfxGetApp();

	if (hCurrItem != NULL)
	{
		memset(&stCurrSelInfo, 0, sizeof(CURR_SEL_DETAILS));
		bFoundALL = GetAllUIPositionNAddress(hCurrItem, &stCurrSelInfo, CHILD_OUTLET);

		if (bFoundALL)
		{
			fnSetOutletState = (FN_SET_OUTLET_STATE) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduSwitchOutletState");

			if (fnSetOutletState)
			{
				bResult = (*fnSetOutletState) ((CHAR *)stCurrSelInfo.byPDUAddress, 
											   stCurrSelInfo.byCurrBANKPos, 
											   stCurrSelInfo.byCurrOCBPos, 
											   stCurrSelInfo.byCurrOUTLETPos, 
											   stCurrSelInfo.byCurrRealOutlet, 
											   RELAY_OPEN, 
											   &byErrCode, 
											   TRUE);
			}

			cszOutletNm.Empty();
			cszOutletNm = cTreeVw.GetItemText(hCurrItem);

			cszMessage.Empty();
			cszStatus.Empty();
			cszDispalyResult.Empty();

			cszStatus.LoadString(IDS_STR_OFF);

			if (bResult)
			{
				// Store the current Outlet Relay status
				FN_STORE_OUTLET_STATE_TO_CONFIG				fnStoreOutletStateToConfig;

				fnStoreOutletStateToConfig = (FN_STORE_OUTLET_STATE_TO_CONFIG) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStoreOutletStateToConfig");
				if (fnStoreOutletStateToConfig)
				{
					bResult = (*fnStoreOutletStateToConfig) ((CHAR *)stCurrSelInfo.bySerialNum, 
															 (CHAR *)stCurrSelInfo.byPDUAddress, 
															 stCurrSelInfo.byCurrBANKPos, 
															 stCurrSelInfo.byCurrOCBPos, 
															 stCurrSelInfo.byCurrOUTLETPos, 
															 stCurrSelInfo.byCurrRealOutlet, 
															 RELAY_OPEN, 
															 &byErrCode, 
															 TRUE);
				}
			}

			if (bResult)
			{
				cszMessage.LoadString(IDS_STR_SEL_OUTLET_RELAY_MESS);
				cszDispalyResult.Format(cszMessage, cszOutletNm, cszStatus);

				cTreeVw.SetItemImage(hCurrItem, OUTLET_OFF_ICON, OUTLET_OFF_ICON);

				while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
				{
					if (!::IsDialogMessage(this->m_hWnd, &mMsg))
					{
						::TranslateMessage(&mMsg);
						::DispatchMessage(&mMsg);
					}
				}

				cszMessage.Empty();
				cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

				byMessType = LOG_MESS_PASS;
			}
			else
			{
				cszMessage.LoadString(IDS_STR_SEL_OUTLET_RELAY_ERR);
				cszDispalyResult.Format(cszMessage, cszStatus, cszOutletNm);

				cszMessage.Empty();
				cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

				byMessType = LOG_MESS_FAIL;
			}
		}
		else
		{
			cszDispalyResult.Empty();
			cszDispalyResult.LoadString(IDS_STR_SEL_OUTLET_ERR);

			cszMessage.Empty();
			cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

			byMessType = LOG_MESS_FAIL;
		}

		//AfxMessageBox(cszDispalyResult);
		if (pApp)
		{
			pApp->pBottomView->DisplayLogMessage(byMessType, cszDispalyResult, cszMessage);
		}
	}
}

void CPDUAppView::OnOutletReboot(HTREEITEM hCurrItem)
{
	// TODO: Add your command handler code here
	BOOL								bResult = FALSE;
	BOOL								bFoundALL = FALSE;
	BYTE								byErrCode = 0;
	BYTE								byMessType = 0;
	MSG									mMsg = {0};
	CString								cszMessage;
	CString								cszDispalyResult;
	CString								cszOutletNm;
	CString								cszStatus;
	CURR_SEL_DETAILS					stCurrSelInfo = {0};
	CTreeCtrl							&cTreeVw = GetTreeCtrl();
	FN_SET_OUTLET_STATE					fnSetOutletState = NULL;
	CPDUAppApp							*pApp = (CPDUAppApp *)AfxGetApp();


	if (hCurrItem != NULL)
	{
		memset(&stCurrSelInfo, 0, sizeof(CURR_SEL_DETAILS));
		bFoundALL = GetAllUIPositionNAddress(hCurrItem, &stCurrSelInfo, CHILD_OUTLET);

		if (bFoundALL)
		{
			fnSetOutletState = (FN_SET_OUTLET_STATE) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduSwitchOutletState");

			if (fnSetOutletState)
			{
				cTreeVw.SetItemImage(hCurrItem, OUTLET_OFF_ICON, OUTLET_OFF_ICON);

				while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
				{
					if (!::IsDialogMessage(this->m_hWnd, &mMsg))
					{
						::TranslateMessage(&mMsg);
						::DispatchMessage(&mMsg);
					}
				}

				bResult = (*fnSetOutletState) ((CHAR *)stCurrSelInfo.byPDUAddress, 
											   stCurrSelInfo.byCurrBANKPos, 
											   stCurrSelInfo.byCurrOCBPos, 
											   stCurrSelInfo.byCurrOUTLETPos, 
											   stCurrSelInfo.byCurrRealOutlet, 
											   RELAY_REBOOT, 
											   &byErrCode, 
											   TRUE);
			}

			cszOutletNm.Empty();
			cszOutletNm = cTreeVw.GetItemText(hCurrItem);

			cszMessage.Empty();
			cszStatus.Empty();
			cszDispalyResult.Empty();

			// WAIT FOR 6 SECONDS AND THEN READ THE OUTLET STATUS AFTER REBOOT CALL
			Sleep(6000);
	
			BYTE						byCurrStatus = 0;
			BYTE						byStatus = 0;
			FN_GET_OUTLET_STATE			fnGetOutletState;

			fnGetOutletState = (FN_GET_OUTLET_STATE) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduGetOutletState");
			if (fnGetOutletState)
			{
				(*fnGetOutletState) ((CHAR *)stCurrSelInfo.byPDUAddress, 
									 stCurrSelInfo.byCurrBANKPos, 
									 stCurrSelInfo.byCurrOCBPos, 
									 stCurrSelInfo.byCurrOUTLETPos, 
									 stCurrSelInfo.byCurrRealOutlet, 
									 &byCurrStatus, 
									 &byErrCode, 
									 TRUE);
			}

			if (byCurrStatus == RELAY_OPEN)
			{
				byStatus = OUTLET_OFF_ICON;
			}
			else if (byCurrStatus == RELAY_CLOSE)
			{
				byStatus = OUTLET_ON_ICON;
			}
			else if (byCurrStatus == RELAY_TRIP)
			{
				byStatus = OUTLET_TRIP_ICON;
			}

			cTreeVw.SetItemImage(hCurrItem, byStatus, byStatus);

			while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
			{
				if (!::IsDialogMessage(this->m_hWnd, &mMsg))
				{
					::TranslateMessage(&mMsg);
					::DispatchMessage(&mMsg);
				}
			}

			// Store the current Outlet Relay status
			FN_STORE_OUTLET_STATE_TO_CONFIG				fnStoreOutletStateToConfig;

			fnStoreOutletStateToConfig = (FN_STORE_OUTLET_STATE_TO_CONFIG) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStoreOutletStateToConfig");
			if (fnStoreOutletStateToConfig)
			{
				bResult = (*fnStoreOutletStateToConfig) ((CHAR *)stCurrSelInfo.bySerialNum, 
														 (CHAR *)stCurrSelInfo.byPDUAddress, 
														 stCurrSelInfo.byCurrBANKPos, 
														 stCurrSelInfo.byCurrOCBPos, 
														 stCurrSelInfo.byCurrOUTLETPos, 
														 stCurrSelInfo.byCurrRealOutlet, 
														 byCurrStatus, 
														 &byErrCode, 
														 TRUE);
			}

			if (bResult)
			{
				cszStatus.LoadString(IDS_STR_REBOOTED);
				cszMessage.LoadString(IDS_STR_SEL_OUTLET_RELAY_MESS);
				cszDispalyResult.Format(cszMessage, cszOutletNm, cszStatus);

				cszMessage.Empty();
				cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

				byMessType = LOG_MESS_PASS;
			}
			else
			{
				cszStatus.LoadString(IDS_STR_REBOOT);
				cszMessage.LoadString(IDS_STR_SEL_OUTLET_RELAY_ERR);
				cszDispalyResult.Format(cszMessage, cszStatus, cszOutletNm);

				cszMessage.Empty();
				cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

				byMessType = LOG_MESS_FAIL;
			}
		}
		else
		{
			cszDispalyResult.Empty();
			cszDispalyResult.LoadString(IDS_STR_SEL_OUTLET_ERR);

			cszMessage.Empty();
			cszMessage = CTime::GetCurrentTime().Format("%A, %B %d, %Y - %H:%M:%S");

			byMessType = LOG_MESS_FAIL;
		}

		//AfxMessageBox(cszDispalyResult);
		if (pApp)
		{
			pApp->pBottomView->DisplayLogMessage(byMessType, cszDispalyResult, cszMessage);
		}
	}
}

void CPDUAppView::OnOutletRefresh()
{
	// TODO: Add your command handler code here

	CSplash			*cSplash = new CSplash();
	CTreeCtrl		&cTreeVw = GetTreeCtrl();
	
	cSplash->Create(IDD_SPLASH, NULL);

	if (cSplash)
	{
		cSplash->AnimateWindow(1000, AW_ACTIVATE | AW_BLEND);
		cSplash->ShowWindow(TRUE);
	}

	GetNDisplayOutletData();
	//GetDocument()->UpdateAllViews(this);

	if (cSplash)
	{
		cSplash->AnimateWindow(1000, AW_HIDE | AW_BLEND);
		cSplash->DestroyWindow();
	}

	AfxMessageBox(_T("All values of all OUTLETS are updated..."));
}

BOOL CPDUAppView::OnTreeItemSelection(UINT nTreeItemEvent)
{
	BOOL					bRet = FALSE;

	UpdateValuesInRightView();

	if (TREE_ITEM_LDCLICK == nTreeItemEvent)
	{
		UINT					nNum = 0;
		CString					cszLoad;
		CString					cszCompare;
		HTREEITEM				hSelItem = NULL;
		BYTE					byItemLevel = ITEM_NONE;
		CTreeCtrl				&cTreeVw = GetTreeCtrl();
		
		hSelItem = cTreeVw.GetSelectedItem();

		if (hSelItem == NULL)
			return bRet;

		for (nNum = 0; nNum < 3; nNum++)
		{
			cszLoad.Empty();
			cszLoad = cTreeVw.GetItemText(hSelItem);

			cszCompare.Empty();

			if (nNum == 0)
				cszCompare.LoadString(IDS_STR_PDU_DEV_NM_CHECK);
			else if (nNum == 1)
				cszCompare.LoadString(IDS_STR_BANK_NM_CHECK);
			else if (nNum == 2)
			{
				//cszCompare.LoadString(IDS_STR_OUTLET_NM_CHECK);
				byItemLevel = CHILD_OUTLET;
				break;
			}

			if (m_bThreadExit)
				return bRet;

			if (cszCompare.GetLength() <= cszLoad.GetLength())
			{
				cszLoad = cszLoad.Left(cszCompare.GetLength());
			}

			if (!cszCompare.CompareNoCase(cszLoad))
			{
				if (nNum == 0)
				{
					byItemLevel = PARENT_PDU;
					break;
				}
				else if (nNum == 1)
				{
					byItemLevel = CHILD_BANK;
					break;
				}
				else if (nNum == 2)
				{
					byItemLevel = CHILD_OUTLET;
					break;
				}
			}
		}

		if (m_bThreadExit)
			return bRet;

		if (byItemLevel == CHILD_OUTLET)
		{
			CAlertDlg							cAlertDlg;
			BYTE								byErrCode = 0;
			BOOL								bFoundALL = FALSE;
			CURR_SEL_DETAILS					stCurrSelInfo = {0};
			PDU_OUTLET_ALERT_INFO				stPDUOutletAlertInfo = {0};
			FN_GET_OUTLET_ALERT_SETTINGS		fnGetOutletAlertSettings;
			FN_STORE_OUTLET_ALERT_SETTINGS		fnStoreOutletAlertSettings;

			memset(&stCurrSelInfo, 0, sizeof(CURR_SEL_DETAILS));
			bFoundALL = GetAllUIPositionNAddress(hSelItem, &stCurrSelInfo, byItemLevel);

			if (bFoundALL)
			{
				fnGetOutletAlertSettings = (FN_GET_OUTLET_ALERT_SETTINGS) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduGetOutletAlertSettings");
				if (fnGetOutletAlertSettings)
				{
					(*fnGetOutletAlertSettings) ((CHAR *)stCurrSelInfo.bySerialNum, 
												 (CHAR *)stCurrSelInfo.byPDUAddress, 
												 stCurrSelInfo.byCurrBANKPos, 
												 stCurrSelInfo.byCurrOCBPos, 
												 stCurrSelInfo.byCurrOUTLETPos, 
												 stCurrSelInfo.byCurrRealOutlet, 
												 &stPDUOutletAlertInfo, 
												 &byErrCode, 
												 TRUE);
				}

				memcpy(&cAlertDlg.m_stPduOutletAlrtInfo, &stPDUOutletAlertInfo, sizeof(PDU_OUTLET_ALERT_INFO)); 

				cAlertDlg.DoModal();

				memset(&stPDUOutletAlertInfo, 0, sizeof(PDU_OUTLET_ALERT_INFO));
				memcpy(&stPDUOutletAlertInfo, &cAlertDlg.m_stPduOutletAlrtInfo, sizeof(PDU_OUTLET_ALERT_INFO));

				fnStoreOutletAlertSettings = (FN_STORE_OUTLET_ALERT_SETTINGS) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStoreOutletAlertSettings");
				if (fnStoreOutletAlertSettings)
				{
					(*fnStoreOutletAlertSettings) ((CHAR *)stCurrSelInfo.bySerialNum, 
												   (CHAR *)stCurrSelInfo.byPDUAddress, 
												   stCurrSelInfo.byCurrBANKPos, 
												   stCurrSelInfo.byCurrOCBPos, 
												   stCurrSelInfo.byCurrOUTLETPos, 
												   stCurrSelInfo.byCurrRealOutlet, 
												   stPDUOutletAlertInfo, 
												   &byErrCode, 
												   TRUE);
				}
			}
		}
		else if (byItemLevel == PARENT_PDU)
		{
			BYTE					byCount = 0;
			UINT					nNum = 0;
			BOOL					bLog	= TRUE;
			BYTE					byInfoID = 0;
			BYTE					byValue[4] = {0};
			BOOL					bRetStatus = FALSE;
			BOOL					bParentPDUFound = FALSE;
			BYTE					byPDUAddress[MAX_PATH] = {0};
			BYTE					byDataBuffer[66] = {0};
			BYTE					byErrCode = NULL;
			CDlgStaticInfo			cDlgStaticInfo;
			PPDU_TREE_DET			pParentPDUInfo = NULL;
			FN_GET_STATIC_INFO		fnGetStaticPDUInfo = NULL;

			cDlgStaticInfo.cszPDUName.Empty();
			cDlgStaticInfo.cszPDUName = cTreeVw.GetItemText(hSelItem);

			pParentPDUInfo = gpHeadPDUInfo;

			while (pParentPDUInfo)
			{
				if (pParentPDUInfo->hPDUTreeHandle == hSelItem)
				{
					bParentPDUFound = TRUE;
					break;
				}

				pParentPDUInfo = pParentPDUInfo->pNextPDUInfo;
			}

			if ((bParentPDUFound) && (pParentPDUInfo != NULL))
			{
				memset(byPDUAddress, 0, sizeof(byPDUAddress));
				memcpy(byPDUAddress, pParentPDUInfo->byPDUAddrs, sizeof(pParentPDUInfo->byPDUAddrs));

				if (m_hPDUAPIDLL)
				{
					fnGetStaticPDUInfo = (FN_GET_STATIC_INFO) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStaticInfoWriteAndRead");

					for (nNum = 1; nNum < 14; nNum++)
					{
						if ((nNum == 5) || (nNum == 6))
						{
							continue;
						}

						memset(byDataBuffer, 0, sizeof(byDataBuffer));

						if (fnGetStaticPDUInfo)
						{
							bRetStatus = (*fnGetStaticPDUInfo) ((CHAR *)byPDUAddress, nNum, byDataBuffer, 0, &byErrCode, bLog, 0);
						}

						if (!bRetStatus)
						{
							cszLoad.Empty();
							cszLoad.LoadString(IDS_STR_UNABLE_GET_STATIC_ERR_MESS);
							AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);

							break;
						}
						else
						{
							switch(nNum)
							{
								case PDU_MODEL_NM:
								{
									memcpy(cDlgStaticInfo.stPDUStaticInfo.byModelNm, byDataBuffer, 42);
								}
								break;

								case PDU_SERIAL_NUM:
								{
									memcpy(cDlgStaticInfo.stPDUStaticInfo.bySerialNo, byDataBuffer, 14);
								}
								break;

								case PDU_MODEL_NUM:
								{
									memcpy(cDlgStaticInfo.stPDUStaticInfo.byModelNo, byDataBuffer, 22);
								}
								break;

								case PDU_MANUFAC_DATE:
								{
									memcpy(cDlgStaticInfo.stPDUStaticInfo.byManfDate, byDataBuffer, 14);
								}
								break;

								case PDU_STAGGER_ON_DELAY:
								{
									cszLoad.Empty();
									cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
									cDlgStaticInfo.stPDUStaticInfo.dwStaggOnDelay = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
									cszLoad.ReleaseBuffer();
								}
								break;

								case PDU_STAGGER_OFF_DELAY:
								{
									cszLoad.Empty();
									cszLoad.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
									cDlgStaticInfo.stPDUStaticInfo.dwStaggOffDelay = strtoul(cszLoad.GetBuffer(cszLoad.GetLength()), NULL, 16);
									cszLoad.ReleaseBuffer();
								}
								break;

								case PDU_FIRMWARE_VERSION:
								{
									memcpy(cDlgStaticInfo.stPDUStaticInfo.byFirmwareVer, byDataBuffer, 6);
								}
								break;

								case PDU_TOTAL_OUTLETS:
								{
									cDlgStaticInfo.stPDUStaticInfo.nTotalOutlets = byDataBuffer[0];
								}
								break;

								case PDU_TOTAL_BANKS:
								{
									cDlgStaticInfo.stPDUStaticInfo.nTotalBanks = byDataBuffer[0];
								}
								break;

								case PDU_OUTLETS_PER_BANK:
								{
									cDlgStaticInfo.stPDUStaticInfo.nOutletsPerBank = byDataBuffer[0];
								}
								break;

								case PDU_PHASE_INP_CONN_TYPE:
								{
									cDlgStaticInfo.stPDUStaticInfo.nPhaseType = byDataBuffer[0];
									cDlgStaticInfo.stPDUStaticInfo.byInpConnType = byDataBuffer[1];
								}
								break;

								default:
								{
									break;
								}

							} // switch(nNum)

						} // if (!bRetStatus)

					} // for (nNum

				} // if (m_hPDUAPIDLL)

			} //if ((bParentPDUFound) && (pParentPDUInfo != NULL))

			cDlgStaticInfo.DoModal();

			bRet = TRUE;
		}
	}

#if 0 // FOR DEBUG MODE

	switch(nTreeItemEvent)
	{
		case TREE_ITEM_LCLICK:
			{
				AfxMessageBox(_T("Left mouse click on Tree item"));
			}
			break;

		case TREE_ITEM_LDCLICK:
			{
				AfxMessageBox(_T("Left mouse double click on Tree item"));
			}
			break;

		case TREE_ITEM_RCLICK:
			{
				AfxMessageBox(_T("Right mouse click on Tree item"));
			}
			break;

		case TREE_ITEM_RDCLICK:
			{
				AfxMessageBox(_T("Right mouse double click on Tree item"));
			}
			break;

		case TREE_ITEM_ITEMCHANGED:
			{
				AfxMessageBox(_T("Item selection changed on Tree item"));
			}
			break;

		default:
			{
				AfxMessageBox(_T("Event not generated"));
			}
			break;
	}

	AfxMessageBox(cszLoad);

#endif

	return bRet;

}

void CPDUAppView::StartBaseThread(UINT nPoll)
{
	m_nPollSecs = nPoll;

	if (m_pThread == NULL)
	{
		m_pThread = AfxBeginThread(ThreadGetNUpdatePDUData, (PVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pThread->m_bAutoDelete = FALSE;
		m_pThread->ResumeThread();
	}
}

void CPDUAppView::SetPollingSecs(UINT nPoll)
{
	m_nPollSecs = nPoll;
}

void CPDUAppView::CloseAllThreads()
{
	BOOL							bRetStatus = FALSE;
	BYTE							byErrCode = NULL;
	FN_CLOSE_ALL_ALERT_EVENT		fnCloseAllAlertThreads = NULL;

	fnCloseAllAlertThreads = (FN_CLOSE_ALL_ALERT_EVENT) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduCloseAllAlertEvent");
	if (fnCloseAllAlertThreads)
	{
		bRetStatus = (*fnCloseAllAlertThreads) (&byErrCode, TRUE);
	}

	m_bThreadExit = TRUE;	// exit base thread

	if (m_pThread != NULL)
	{
		::WaitForSingleObject(m_pThread->m_hThread, 3000);
		delete m_pThread;
		m_pThread = NULL;
	}
}

void CPDUAppView::OnDestroy()
{
	CloseAllThreads();

	CTreeListView::OnDestroy();

	// TODO: Add your message handler code here
}

void CPDUAppView::UpdateValuesInRightView()
{
	UINT								nNum = 0;
	BYTE								byInfoID = 0;
	BYTE								byPDUAddress[MAX_PATH] = {0};
	BYTE								byItemLevel = ITEM_NONE;
	BYTE								byDataBuffer[65] = {0};
	BYTE								byErrCode = NULL;
	CString								cszLoad;
	CString								cszCompare;
	CStringArray						cszValuesArray;
	BOOL								bLog = TRUE;
	BOOL								bRetStatus = FALSE;
	BOOL								bSelItemFound = FALSE;
	BOOL								bParentPDUFound = FALSE;
	HTREEITEM							hSelItem = NULL;
	PPDU_OUTLET_INFO					pSelOutletInfo = NULL;
	PPDU_BANK_INFO						pParentBankInfo = NULL;
	PPDU_TREE_DET						pParentPDUInfo = NULL;
	HTREEITEM							hPDUParentTreeItem = NULL;
	CPDUAppApp							*pApp = (CPDUAppApp *)AfxGetApp();
	CTreeCtrl							&cTreeVw = GetTreeCtrl();
	FN_GET_STATIC_INFO					fnGetStaticPDUInfo = NULL;

	hSelItem = cTreeVw.GetSelectedItem();

	if (hSelItem == NULL)
		return;

	for (nNum = 0; nNum < 3; nNum++)
	{
		cszLoad.Empty();
		cszLoad = cTreeVw.GetItemText(hSelItem);

		cszCompare.Empty();

		if (nNum == 0)
			cszCompare.LoadString(IDS_STR_PDU_DEV_NM_CHECK);
		else if (nNum == 1)
			cszCompare.LoadString(IDS_STR_BANK_NM_CHECK);
		else if (nNum == 2)
		{
			//cszCompare.LoadString(IDS_STR_OUTLET_NM_CHECK);
			byItemLevel = CHILD_OUTLET;
			break;
		}

		if (m_bThreadExit)
			return;

		if (cszCompare.GetLength() <= cszLoad.GetLength())
		{
			cszLoad = cszLoad.Left(cszCompare.GetLength());
		}

		if (!cszCompare.CompareNoCase(cszLoad))
		{
			if (nNum == 0)
			{
				byItemLevel = PARENT_PDU;
				break;
			}
			else if (nNum == 1)
			{
				byItemLevel = CHILD_BANK;
				break;
			}
			else if (nNum == 2)
			{
				byItemLevel = CHILD_OUTLET;
				break;
			}
		}
	}

	if (m_bThreadExit)
		return;

	hPDUParentTreeItem = hSelItem;

	if (byItemLevel != PARENT_PDU)
	{
		while (cTreeVw.GetParentItem(hPDUParentTreeItem) != NULL)
		{
			hPDUParentTreeItem = cTreeVw.GetParentItem(hPDUParentTreeItem);
		}
	}

	if (hPDUParentTreeItem == NULL)
		return;

	pParentPDUInfo = gpHeadPDUInfo;

	if (m_bThreadExit)
		return;

	// Lets identify the PDU Position in the Link list
	while (pParentPDUInfo)
	{
		if (pParentPDUInfo->hPDUTreeHandle == hPDUParentTreeItem)
		{
			bParentPDUFound = TRUE;
			break;
		}

		pParentPDUInfo = pParentPDUInfo->pNextPDUInfo;
	}

	if (m_bThreadExit)
		return;

	cszValuesArray.RemoveAll();

	if ((bParentPDUFound) && (pParentPDUInfo != NULL))
	{
		if ((byItemLevel == CHILD_BANK) || (byItemLevel == CHILD_OUTLET))
		{
			pParentBankInfo = pParentPDUInfo->pBankInfo;

			while (pParentBankInfo)
			{
				if (m_bThreadExit)
					return;

				if (byItemLevel == CHILD_OUTLET)
				{
					pSelOutletInfo = pParentBankInfo->pOutletInfo;
					while(pSelOutletInfo)
					{
						if (pSelOutletInfo->hOUTLETTreeHandle == hSelItem)
						{
							bSelItemFound = TRUE;
							break;
						}

						pSelOutletInfo = pSelOutletInfo->pNextOutletInfo;

						if (m_bThreadExit)
							return;
					}

					if (bSelItemFound)
					{
						cszLoad.Empty();
						cszLoad = cTreeVw.GetItemText(hSelItem);
						cszValuesArray.Add(cszLoad);

						cszLoad.Empty();
						GetSubItemText(hSelItem, VOLTAGE_COL_ID, cszLoad);
						cszValuesArray.Add(cszLoad);

						cszLoad.Empty();
						GetSubItemText(hSelItem, CURRENT_COL_ID, cszLoad);
						cszValuesArray.Add(cszLoad);

						cszLoad.Empty();
						GetSubItemText(hSelItem, POWER_FACTOR_COL_ID, cszLoad);
						cszValuesArray.Add(cszLoad);

						cszLoad.Empty();
						GetSubItemText(hSelItem, FREQUENCY_COL_ID, cszLoad);
						cszValuesArray.Add(cszLoad);

						cszLoad.Empty();
						GetSubItemText(hSelItem, APPARENT_POWER_COL_ID, cszLoad);
						cszValuesArray.Add(cszLoad);

						cszLoad.Empty();
						GetSubItemText(hSelItem, REAL_POWER_COL_ID, cszLoad);
						cszValuesArray.Add(cszLoad);

						if (m_bThreadExit)
							return;
					}
				/*	else
					{
						cszValuesArray.RemoveAll();
						byItemLevel = ITEM_NONE;
					}*/
				}
				else if (byItemLevel == CHILD_BANK)
				{
					if (pParentBankInfo->hBANKTreeHandle == hSelItem)
					{
						bSelItemFound = TRUE;

						cszValuesArray.Add(_T(""));
						cszValuesArray.Add(_T("0.0 A"));
						cszValuesArray.Add(_T("0.0 V"));
						cszValuesArray.Add(_T("0.0 Hz"));
						cszValuesArray.Add(_T("0.0 VA"));
						cszValuesArray.Add(_T("0.0 W"));
						cszValuesArray.Add(_T("0.00"));
					}
				}

				if (bSelItemFound)
					break;

				if (m_bThreadExit)
					return;

				pParentBankInfo = pParentBankInfo->pNextBankInfo;
			}
		}
		else if (byItemLevel == PARENT_PDU)
		{
			// COLLECT MANUFACTURED DATA FROM THE SELECTED PDU DEVICE
			memset(byPDUAddress, 0, sizeof(byPDUAddress));
			memcpy(byPDUAddress, pParentPDUInfo->byPDUAddrs, sizeof(pParentPDUInfo->byPDUAddrs));

			cszValuesArray.RemoveAll();

			cszLoad.Empty();
			cszLoad = cTreeVw.GetItemText(hSelItem);
			cszValuesArray.Add(cszLoad);

			if (m_hPDUAPIDLL)
			{
				fnGetStaticPDUInfo = (FN_GET_STATIC_INFO) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStaticInfoWriteAndRead");

				for (nNum = 1; nNum < 5; nNum++)
				{
					if (nNum == 4)
					{
						nNum = PDU_FIRMWARE_VERSION;
					}

					memset(byDataBuffer, 0, sizeof(byDataBuffer));

					if (fnGetStaticPDUInfo)
					{
						bRetStatus = (*fnGetStaticPDUInfo) ((CHAR *)byPDUAddress, nNum, byDataBuffer, 0, &byErrCode, bLog, 0);
					}

					if (!bRetStatus)
					{
						cszValuesArray.RemoveAll();

						cszLoad.Empty();
						cszLoad = cTreeVw.GetItemText(hSelItem);
						cszValuesArray.Add(cszLoad);

						cszLoad.Empty();
						cszLoad.LoadString(IDS_STR_UNABLE_GET_STATIC_ERR_MESS);
						cszValuesArray.Add(cszLoad);

						break;
					}
					else
					{
						cszLoad.Empty();
						cszLoad.Format("%s", byDataBuffer);
						cszValuesArray.Add(cszLoad);
					}

				} // for (nNum

			} // if (m_hPDUAPIDLL)

		} //else if (byItemLevel == PARENT_PDU)
	}
	else
	{
		byItemLevel = ITEM_NONE;
		cszValuesArray.RemoveAll();
	}

	if (m_bThreadExit)
		return;

	if ((pApp) && (pApp->pRightView))
	{
		pApp->pRightView->UpdateAllValues(cszValuesArray, byItemLevel);
	}
}

void CPDUAppView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == UPDATE_PDU_DATA)
	{
		KillTimer(UPDATE_PDU_DATA);

		CSplash					*cSplash = new CSplash();
	
		cSplash->Create(IDD_SPLASH, NULL);

		if (cSplash)
		{
			cSplash->AnimateWindow(1000, AW_ACTIVATE | AW_BLEND);
			cSplash->ShowWindow(TRUE);
		}

#if 0
		CPDUAppApp				*pApp = (CPDUAppApp *)AfxGetApp();

		if ((pApp) && (pApp->pBottomView))
		{
			pApp->pBottomView->UpdateAllAlertMess();
		}

		if (cSplash)
		{
			cSplash->AnimateWindow(500, AW_HIDE | AW_BLEND);
			cSplash->DestroyWindow();
		}
#else
		if (GetNDisplayPDU())
		{
			CPDUAppApp				*pApp = (CPDUAppApp *)AfxGetApp();

			GetNDisplayBanksOutlets();
			GetNDisplayOutletData();

			if ((pApp) && (pApp->pBottomView))
			{
				pApp->pBottomView->UpdateAllAlertMess();
			}

			InitiateAlertEvent();

			if (cSplash)
			{
				cSplash->AnimateWindow(500, AW_HIDE | AW_BLEND);
				cSplash->DestroyWindow();
			}
		}
		else if (cSplash)
		{
			cSplash->DestroyWindow();
		}
#endif
	}

	CTreeListView::OnTimer(nIDEvent);
}

void CPDUAppView::InitiateAlertEvent()
{
	BYTE							byErrCode = 0;
	FN_INITIATE_ALERT_EVENT			fnInitiateAlertEvent;

	fnInitiateAlertEvent = (FN_INITIATE_ALERT_EVENT) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduInitiateAlertEvent");

	if (fnInitiateAlertEvent)
	{
		(*fnInitiateAlertEvent) (&byErrCode, TRUE);
	}
}

BOOL CPDUAppView::UpdateOutletStatus(CString cszPDUDevice, UINT nBankNum, UINT nOCBNum, UINT nOutletNum, UINT nRealOutlet, UINT nAlertType)
{
	BOOL			bRet = FALSE;
	BYTE			byErrCode = 0;

	if ((cszPDUDevice.IsEmpty()) || (!nBankNum) || (!nOutletNum) || (nAlertType == ALERT_RESERVED_0x00) || (nAlertType == ALERT_RESERVED_0xFF))
		return bRet;

	// Store the current Outlet Relay status
	BOOL										bResult = FALSE;
	BOOL										bParentPDUFound = FALSE;
	BOOL										bBankFound = FALSE;
	BOOL										bOutletFound = FALSE;
	FN_STORE_OUTLET_STATE_TO_CONFIG				fnStoreOutletStateToConfig;
	PPDU_OUTLET_INFO							pSelOutletInfo = NULL;
	PPDU_BANK_INFO								pParentBankInfo = NULL;
	PPDU_TREE_DET								pParentPDUInfo = NULL;
	CTreeCtrl									&cTreeVw = GetTreeCtrl();
	CString										cszLoad;
	BYTE										bySerialNum[16] = {0};

	pParentPDUInfo = gpHeadPDUInfo;

	while (pParentPDUInfo)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), pParentPDUInfo->byPDUAddrs);

		if (!cszPDUDevice.CompareNoCase(cszLoad))
		{
			bParentPDUFound = TRUE;
			memcpy(bySerialNum, pParentPDUInfo->bySerialNum, sizeof(pParentPDUInfo->bySerialNum));
			break;
		}

		pParentPDUInfo = pParentPDUInfo->pNextPDUInfo;
	}

	if ((bParentPDUFound) && (pParentPDUInfo != NULL))
	{
		// Now find the selected outlet position in linked list
		pParentBankInfo = pParentPDUInfo->pBankInfo;

		while (pParentBankInfo)
		{
			if (pParentBankInfo->byBANKPos == nBankNum)
			{
				bBankFound = TRUE;
				break;
			}

			pParentBankInfo = pParentBankInfo->pNextBankInfo;
		}

		if ((bBankFound) && (pParentBankInfo != NULL))
		{
			pSelOutletInfo = pParentBankInfo->pOutletInfo;
			while(pSelOutletInfo)
			{
				if (pSelOutletInfo->byOUTLETPos == nOutletNum)
				{
					bOutletFound = TRUE;
					break;
				}

				pSelOutletInfo = pSelOutletInfo->pNextOutletInfo;
			}
		}
	}

	if ((bOutletFound) && (pSelOutletInfo != NULL))
	{
		if (nAlertType == ALERT_VCB)
		{
			cTreeVw.SetItemImage(pSelOutletInfo->hOUTLETTreeHandle, OUTLET_TRIP_ICON, OUTLET_TRIP_ICON);

			fnStoreOutletStateToConfig = (FN_STORE_OUTLET_STATE_TO_CONFIG) GetProcAddress((HMODULE) m_hPDUAPIDLL, (LPCSTR) "PduStoreOutletStateToConfig");
			if (fnStoreOutletStateToConfig)
			{
				bResult = (*fnStoreOutletStateToConfig) ((CHAR *)bySerialNum, 
														 cszPDUDevice.GetBuffer(cszPDUDevice.GetLength()), 
														 nBankNum, 
														 nOCBNum, 
														 nOutletNum, 
														 nRealOutlet, 
														 RELAY_TRIP, 
														 &byErrCode, 
														 TRUE);
				cszPDUDevice.ReleaseBuffer();
			}
		}
	}

	return bRet;
}

CString CPDUAppView::GetExactOutletNm(CString cszPDUDevice, UINT nBankNum, UINT nOutletNum)
{
	CString							cszLoad;
	CString							cszOutletNm;
	BOOL							bFound = FALSE;
	BOOL							bOutletFound = FALSE;
	PPDU_OUTLET_INFO				pSelOutletInfo = NULL;
	PPDU_BANK_INFO					pParentBankInfo = NULL;
	PPDU_TREE_DET					pParentPDUInfo = NULL;
	CTreeCtrl						&cTreeVw = GetTreeCtrl();

	pParentPDUInfo = gpHeadPDUInfo;

	while (pParentPDUInfo)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), pParentPDUInfo->byPDUAddrs);

		if (!cszPDUDevice.CompareNoCase(cszLoad))
		{
			bFound = TRUE;
			break;
		}

		pParentPDUInfo = pParentPDUInfo->pNextPDUInfo;
	}

	if ((bFound) && (pParentPDUInfo != NULL))
	{
		pParentBankInfo = pParentPDUInfo->pBankInfo;
		bFound = FALSE;

		while (pParentBankInfo)
		{
			if (pParentBankInfo->byBANKPos == nBankNum)
			{
				bFound = TRUE;
				break;
			}

			pParentBankInfo = pParentBankInfo->pNextBankInfo;
		}

		if ((bFound) && (pParentBankInfo != NULL))
		{
			pSelOutletInfo = pParentBankInfo->pOutletInfo;
			bFound = FALSE;

			while (pSelOutletInfo)
			{
				if (pSelOutletInfo->byOUTLETPos == nOutletNum)
				{
					bOutletFound = TRUE;
					break;
				}

				pSelOutletInfo = pSelOutletInfo->pNextOutletInfo;
			}
		}
	}

	cszOutletNm.Empty();
	if (bOutletFound)
	{
		cszOutletNm = cTreeVw.GetItemText(pSelOutletInfo->hOUTLETTreeHandle);
	}

	return cszOutletNm;
}