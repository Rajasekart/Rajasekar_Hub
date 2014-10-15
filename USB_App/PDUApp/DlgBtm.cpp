// DlgBtm.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "PDUApp.h"
#include "DlgBtm.h"
#include "sqlite3.h"

// CDlgBtm

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//int UpdateLogList(void *NotUsed, int argc, char **argv, char **azColName);

IMPLEMENT_DYNCREATE(CDlgBtm, CView)

BEGIN_MESSAGE_MAP(CDlgBtm, CView)
	// Standard printing commands
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LOG, OnRClick) 
	ON_COMMAND(ID_LOG_DELETE, &CDlgBtm::OnLogDelete)
	ON_COMMAND(ID_LOG_SELECTALL, &CDlgBtm::OnLogSelectall)
	ON_COMMAND(ID_LOG_DESELECTALL, &CDlgBtm::OnLogDeselectall)
END_MESSAGE_MAP()

// CDlgBtm construction/destruction

CDlgBtm::CDlgBtm()
{
	// TODO: add construction code here
	m_bLogCreated = FALSE;

	m_pThreadBtm = NULL;
	m_bThreadBtmExit = FALSE;
	m_ToolTip.m_hWnd = NULL;
}

CDlgBtm::~CDlgBtm()
{
	if (m_cImages.m_hImageList != NULL)
	{
		m_cImages.DeleteImageList();
	}
}

void CDlgBtm::OnDraw(CDC* pDC)
{
}

BOOL CDlgBtm::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

#ifdef _DEBUG
void CDlgBtm::AssertValid() const
{
	CView::AssertValid();
}

void CDlgBtm::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG


// CDlgBtm message handlers


int CDlgBtm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_LogLstCtrl.Create(LVS_REPORT | LVS_SHOWSELALWAYS | LBS_NOTIFY, CRect(0, 0, 600, 200), this, IDC_LIST_LOG);
	m_LogLstCtrl.SetExtendedStyle(m_LogLstCtrl.GetExtendedStyle() | LVS_EX_FLATSB | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_SUBITEMIMAGES | LVS_EX_INFOTIP | LVS_AUTOARRANGE);	
	
	return 0;
}

void CDlgBtm::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect	r;

	GetClientRect(r);
	m_LogLstCtrl.SetColumnWidth(0, 20 + ((r.right / 3) / 4) / 2);
	m_LogLstCtrl.SetColumnWidth(1, ((r.right / 3) / 4) * 2);
	m_LogLstCtrl.SetColumnWidth(2, ((r.right / 2) + ((r.right / 3) / 4)));
	m_LogLstCtrl.SetColumnWidth(3, (r.right / 4));
	m_LogLstCtrl.SetWindowPos(&wndTop, 0, 0, r.right, r.bottom, SWP_SHOWWINDOW);
}

void CDlgBtm::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CPDUAppApp		*pApp = (CPDUAppApp *)AfxGetApp();

	if (pApp)
	{
		pApp->pBottomView = this;
	}

	if (!m_bLogCreated)
	{
		m_LogLstCtrl.InsertColumn(0, _T("Log No"), LVCFMT_LEFT);
		m_LogLstCtrl.InsertColumn(1, _T("Message Type"), LVCFMT_LEFT);
		m_LogLstCtrl.InsertColumn(2, _T("Message"), LVCFMT_LEFT);
		m_LogLstCtrl.InsertColumn(3, _T("Date logged"), LVCFMT_LEFT);

		if (m_cImages.m_hImageList == NULL)
		{
			m_cImages.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1);
			m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_HIGH));
			m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_LOW));
			m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_ERROR));
			m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_PASS));
			m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_WARN));
			m_cImages.Add(AfxGetApp()->LoadIcon(IDI_ICON_CRIC));
			m_LogLstCtrl.SetImageList(&m_cImages, LVSIL_SMALL);
		}

		if(m_ToolTip.m_hWnd == NULL)
		{
			m_ToolTip.Create(this, TTS_NOPREFIX | TTS_BALLOON | TTS_ALWAYSTIP );
			m_ToolTip.SetMaxTipWidth(32767);

			InitToolTip();
		}
	}

	m_bLogCreated = TRUE;
}

void CDlgBtm::DisplayLogMessage(BYTE byMessType, CString cszMessage, CString cszDate)
{
	UINT		nTotal = 0;
	CString		cszLoad;
	CString		cszMessType;
	MSG			mMsg = {0};
	LVITEM		stLvItem = {0};

	nTotal = m_LogLstCtrl.GetItemCount();

	stLvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	stLvItem.iItem = nTotal;
	stLvItem.iSubItem = LOG_SERNUM;
	stLvItem.iImage = LOG_MESS_NO_IMAGE;

	cszLoad.Empty();
	cszLoad.Format(_T("%d"), nTotal + 1);
	stLvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();

	nTotal = m_LogLstCtrl.InsertItem(&stLvItem);

	cszLoad.Empty();
	switch(byMessType)
	{
		case LOG_MESS_FAIL:
		{
			cszLoad = _T("Error");	
		}
		break;

		case LOG_MESS_PASS:
		{
			cszLoad = _T("Success");
		}
		break;

		case LOG_MESS_WARN:
		{
			cszLoad = _T("Warning");
		}
		break;

		case LOG_MESS_CRIC:
		{
			cszLoad = _T("Critical");	
		}
		break;
	}

	stLvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	stLvItem.iItem = nTotal;
	stLvItem.iSubItem = LOG_MESS_TYPE;
	stLvItem.iImage = byMessType;

	stLvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();

	m_LogLstCtrl.SetItem(&stLvItem);

	m_LogLstCtrl.SetItemText(nTotal, LOG_MESSAGE, cszMessage);
	m_LogLstCtrl.SetItemText(nTotal, LOG_MESS_DATE, cszDate);

	while (::PeekMessage(&mMsg, this->m_hWnd, 0, 0, PM_REMOVE))
	{
		if (!::IsDialogMessage(this->m_hWnd, &mMsg))
		{
			::TranslateMessage(&mMsg);
			::DispatchMessage(&mMsg);
		}
	}

	m_LogLstCtrl.RedrawItems(0, m_LogLstCtrl.GetItemCount() - 1);
	UpdateWindow();

	nTotal = m_LogLstCtrl.GetItemCount();
	if (nTotal > 0)
	{
		m_LogLstCtrl.EnsureVisible((nTotal - 1), FALSE);
		//m_LogLstCtrl.SetItemState((nCount - 1), LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CDlgBtm::UpdateAllAlertMess()
{
	int					nResult = 0;
	UINT				nNum = 0;
	UINT				nRow = 0;
	UINT				nAlertType = 0;
	double				fCurrValue = 0.0;
	CString				cszQuery;
	CString				cszTemp;
	CString				cszLoad;
	CString				cszPDUNm;
	CString				cszDateTime;
	CString				cszPDUDevice;
	CString				cszOutletName;
	UINT				nBankNum = 0;
	UINT				nOCBNum = 0;
	UINT				nRealOutlet = 0;
	UINT				nOutletNum = 0;
	CHAR				szErrMsg[MAX_PATH] = {0};
	sqlite3				*db; // sqlite3 db struct
	sqlite3_stmt		*pStmt = NULL;
	LVITEM				stlvItem = {0};
	CPDUAppApp			*pApp = (CPDUAppApp *)AfxGetApp();

	nResult = sqlite3_open_v2(STR_PDU_CONFG_DB_NM, &db, SQLITE_OPEN_READWRITE, NULL);
	if (nResult != SQLITE_OK)
	{
		cszLoad.Format("Can't open database: %s", sqlite3_errmsg(db));
		//AfxMessageBox(cszLoad, MB_ICONSTOP);
	}
	else
	{
#if 0
		cszLoad.Empty();
		cszLoad.Format(_T("SELECT * FROM %s"), STR_PDU_ALERT_MESS_TABLE);
	/*	nResult = sqlite3_exec(db, cszLoad.GetBuffer(cszLoad.GetLength()), (void *)CDlgBtm::UpdateLogList, 0, (char **)szErrMsg);
		cszLoad.ReleaseBuffer();*/

		int rowiddd = -1; // to store rowid of the specific record
		//lpctstr sql = _T("select * from student"); // deal with all fields in the	table
		sqlite3_prepare16_v2(db, cszLoad.GetBuffer(cszLoad.GetLength()), -1, &ppStmt, NULL);
		cszLoad.ReleaseBuffer();

		nResult = sqlite3_step(ppStmt);
		while(SQLITE_ROW == nResult)
		{

		}
#endif

		/*cszQuery.Empty();
		cszQuery = _T("VACUUM");
		nResult = sqlite3_exec(db, cszQuery.GetBuffer(cszQuery.GetLength()), NULL, 0, (char **)szErrMsg);*/

		cszQuery.Empty();
		cszQuery.Format(_T("SELECT * FROM %s"), STR_PDU_ALERT_MESS_TABLE);
		nResult = sqlite3_prepare_v2(db, cszQuery, cszQuery.GetLength(), &pStmt, NULL);
		if (nResult != SQLITE_OK) 
		{
			cszLoad.Empty();
			cszLoad.Format("Can't query the database records: %s", sqlite3_errmsg(db));
			//AfxMessageBox(cszLoad, MB_ICONSTOP);
			return;
		}

		while (sqlite3_step(pStmt) == SQLITE_ROW)
		{
			memset(&stlvItem, 0, sizeof(stlvItem));

			stlvItem.mask = LVIF_TEXT | LVIF_IMAGE;
			stlvItem.iItem = m_LogLstCtrl.GetItemCount();
			stlvItem.iSubItem = LOG_SERNUM;
			stlvItem.iImage = LOG_MESS_NO_IMAGE;
			
			cszLoad.Empty();
			cszLoad.Format(_T("%d"), m_LogLstCtrl.GetItemCount() + 1);
			stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
			cszLoad.ReleaseBuffer();

			nRow = m_LogLstCtrl.InsertItem(&stlvItem);

			m_LogLstCtrl.SetItemData(nRow, sqlite3_column_int(pStmt, TABLE_SER_NO_COL_ID));

			cszPDUNm.Empty();
			cszPDUNm = sqlite3_column_text(pStmt, TABLE_PDU_NAME_COL_ID);
			cszPDUDevice = FindPDUDevice(cszPDUNm);

			nBankNum = sqlite3_column_int(pStmt, TABLE_BANK_NUM_COL_ID);

			nOCBNum = sqlite3_column_int(pStmt, TABLE_OCB_NUM_COL_ID);

			nOutletNum = sqlite3_column_int(pStmt, TABLE_OUTLET_NUM_COL_ID);

			nRealOutlet = sqlite3_column_int(pStmt, TABLE_REAL_OUTLET_NUM_COL_ID);

			nAlertType = sqlite3_column_int(pStmt, TABLE_ALERT_TYPE_COL_ID);

			fCurrValue = sqlite3_column_double(pStmt, TABLE_THRES_VALUE_COL_ID);

			cszDateTime.Empty();
			cszDateTime = sqlite3_column_text(pStmt, TABLE_DATE_TIME_COL_ID);

			cszTemp.Empty();
			cszLoad.Empty();

			memset(&stlvItem, 0, sizeof(stlvItem));

			stlvItem.mask = LVIF_TEXT | LVIF_IMAGE;
			stlvItem.iItem = nRow;
			stlvItem.iSubItem = LOG_MESS_TYPE;
			
			cszOutletName.Empty();
			if ((pApp) && (pApp->pLeftView))
			{
				cszOutletName = pApp->pLeftView->GetExactOutletNm(cszPDUNm, nBankNum, nOutletNum);
			}

			switch (nAlertType)
			{
				case ALERT_VCB:
				{
					stlvItem.iImage = LOG_MESS_CRIC;

					cszLoad = _T("VCB ALERT");
					stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
					cszLoad.ReleaseBuffer();
					m_LogLstCtrl.SetItem(&stlvItem);

					cszLoad.Empty();
					cszTemp.LoadString(IDS_STR_HIGH_CURR_MESS);
					//cszLoad.Format(cszTemp, cszPDUDevice, nOutletNum, nBankNum, fCurrValue);
					cszLoad.Format(cszTemp, cszPDUDevice, cszOutletName, nBankNum, fCurrValue);

					m_LogLstCtrl.SetItemText(nRow, LOG_MESSAGE, cszLoad);
					break;
				}

				case ALERT_HIGH_CURRENT:
				{
					stlvItem.iImage = LOG_MESS_HIGH_VOLT;

					cszLoad = _T("HIGH CURRENT ALERT");
					stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
					cszLoad.ReleaseBuffer();
					m_LogLstCtrl.SetItem(&stlvItem);

					cszLoad.Empty();
					cszTemp.LoadString(IDS_STR_HIGH_CURR_MESS);
					//cszLoad.Format(cszTemp, cszPDUDevice, nOutletNum, nBankNum, fCurrValue);
					cszLoad.Format(cszTemp, cszPDUDevice, cszOutletName, nBankNum, fCurrValue);
					
					m_LogLstCtrl.SetItemText(nRow, LOG_MESSAGE, cszLoad);
					break;
				}

				case ALERT_LOW_CURRENT:
				{
					stlvItem.iImage = LOG_MESS_LOW_VOLT;

					cszLoad = _T("LOW CURRENT ALERT");
					stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
					cszLoad.ReleaseBuffer();
					m_LogLstCtrl.SetItem(&stlvItem);

					cszLoad.Empty();
					cszTemp.LoadString(IDS_STR_LOW_CURR_MESS);
					//cszLoad.Format(cszTemp, cszPDUDevice, nOutletNum, nBankNum, fCurrValue);
					cszLoad.Format(cszTemp, cszPDUDevice, cszOutletName, nBankNum, fCurrValue);

					m_LogLstCtrl.SetItemText(nRow, LOG_MESSAGE, cszLoad);
					break;
				}
			}

			m_LogLstCtrl.SetItemText(nRow, LOG_MESS_DATE, cszDateTime);

			//m_LogLstCtrl.RedrawItems(0, m_LogLstCtrl.GetItemCount() - 1);
			UpdateWindow();
		}

		// Finally update the alert manage status
		cszQuery.Empty();
		cszQuery.Format(_T("UPDATE %s SET STATUS_NUM = %d WHERE STATUS_NUM != %d"), STR_PDU_ALERT_MESS_TABLE, MESS_UPDATED, MESS_UPDATED);
		nResult = sqlite3_exec(db, cszQuery.GetBuffer(cszQuery.GetLength()), NULL, 0, (char **)szErrMsg);
		cszLoad.ReleaseBuffer();
		if (nResult != SQLITE_OK) 
		{
			cszLoad.Empty();
			cszLoad.Format("Can't execute the Update process: %s", sqlite3_errmsg(db));
			AfxMessageBox(cszLoad, MB_ICONSTOP);
			return;
		}

		if (pStmt != NULL)
		{
			nResult = sqlite3_finalize(pStmt);
			if (nResult == SQLITE_OK) 
				pStmt = NULL;
		}

		sqlite3_close(db);
	}

	StartBaseThread();		// Start the base thread to check the new alert messages
}

int CDlgBtm::UpdateLogList(void *NotUsed, int argc, char **argv, char **azColName)
{
	UINT			nNum = 0;
	CString			cszLoad;

	for (nNum = 0; nNum < (UINT)argc; nNum++)
	{
		cszLoad.Empty();
		cszLoad = azColName[nNum];
		AfxMessageBox(cszLoad);
		
		cszLoad.Empty();
		cszLoad.Format("%s", argv[nNum] ? argv[nNum] : "NULL");
		AfxMessageBox(cszLoad);
	}

	return 0;
}

CString CDlgBtm::FindPDUDevice(CString cszPDUAdd)
{
	CString						cszLoad;
	CString						cszPDUName;
	PPDU_TREE_DET				pParentPDUInfo = NULL;

	pParentPDUInfo = gpHeadPDUInfo;
	while (pParentPDUInfo)
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%s"), pParentPDUInfo->byPDUAddrs);

		if (!cszPDUAdd.CompareNoCase(cszLoad))
		{
			cszLoad.Empty();
			cszLoad.LoadString(IDS_STR_PDU_NM);

			cszPDUName.Empty();
			cszPDUName.Format(cszLoad, pParentPDUInfo->byPDUPos);
			break;
		}

		pParentPDUInfo = pParentPDUInfo->pNextPDUInfo;
	}

	return cszPDUName;
}

void CDlgBtm::AddCreatedAlertMess()
{
	int					nResult = 0;
	int					nCount = 0;
	UINT				nNum = 0;
	UINT				nRow = 0;
	UINT				nAlertType = 0;
	double				fCurrValue = 0.0;
	BOOL				bUpdated = FALSE;
	CString				cszQuery;
	CString				cszTemp;
	CString				cszLoad;
	CString				cszPDUNm;
	CString				cszDateTime;
	CString				cszPDUDevice;
	UINT				nBankNum = 0;
	UINT				nOCBNum = 0;
	UINT				nRealOutlet = 0;
	UINT				nOutletNum = 0;
	CString				cszOutletName;
	CHAR				szErrMsg[MAX_PATH] = {0};
	sqlite3				*db; // sqlite3 db struct
	sqlite3_stmt		*pStmt = NULL;
	LVITEM				stlvItem = {0};
	CPDUAppApp			*pApp = (CPDUAppApp *)AfxGetApp();

	nResult = sqlite3_open_v2(STR_PDU_CONFG_DB_NM, &db, SQLITE_OPEN_READWRITE, NULL);
	if (nResult != SQLITE_OK)
	{
		cszLoad.Format("Can't open database: %s", sqlite3_errmsg(db));
		//AfxMessageBox(cszLoad, MB_ICONSTOP);
	}
	else
	{
		cszQuery.Empty();
		cszQuery.Format(_T("SELECT * FROM %s WHERE STATUS_NUM = %d"), STR_PDU_ALERT_MESS_TABLE, MESS_CREATED);
		nResult = sqlite3_prepare_v2(db, cszQuery, cszQuery.GetLength(), &pStmt, NULL);
		if (nResult != SQLITE_OK) 
		{
			cszLoad.Empty();
			cszLoad.Format("Can't query the database records: %s", sqlite3_errmsg(db));
			//AfxMessageBox(cszLoad, MB_ICONSTOP);
			return;
		}

		while (sqlite3_step(pStmt) == SQLITE_ROW)
		{
			bUpdated = TRUE;

			memset(&stlvItem, 0, sizeof(stlvItem));

			stlvItem.mask = LVIF_TEXT | LVIF_IMAGE;
			stlvItem.iItem = m_LogLstCtrl.GetItemCount();
			stlvItem.iSubItem = LOG_SERNUM;
			stlvItem.iImage = LOG_MESS_NO_IMAGE;
			
			cszLoad.Empty();
			cszLoad.Format(_T("%d"), m_LogLstCtrl.GetItemCount() + 1);
			stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
			cszLoad.ReleaseBuffer();

			nRow = m_LogLstCtrl.InsertItem(&stlvItem);

			m_LogLstCtrl.SetItemData(nRow, sqlite3_column_int(pStmt, TABLE_SER_NO_COL_ID));

			cszPDUNm.Empty();
			cszPDUNm = sqlite3_column_text(pStmt, TABLE_PDU_NAME_COL_ID);
			cszPDUDevice = FindPDUDevice(cszPDUNm);

			nBankNum = sqlite3_column_int(pStmt, TABLE_BANK_NUM_COL_ID);

			nOCBNum = sqlite3_column_int(pStmt, TABLE_OCB_NUM_COL_ID);

			nRealOutlet = sqlite3_column_int(pStmt, TABLE_REAL_OUTLET_NUM_COL_ID);

			nOutletNum = sqlite3_column_int(pStmt, TABLE_OUTLET_NUM_COL_ID);

			nAlertType = sqlite3_column_int(pStmt, TABLE_ALERT_TYPE_COL_ID);

			fCurrValue = sqlite3_column_double(pStmt, TABLE_THRES_VALUE_COL_ID);

			cszDateTime.Empty();
			cszDateTime = sqlite3_column_text(pStmt, TABLE_DATE_TIME_COL_ID);

			cszTemp.Empty();
			cszLoad.Empty();

			memset(&stlvItem, 0, sizeof(stlvItem));

			stlvItem.mask = LVIF_TEXT | LVIF_IMAGE;
			stlvItem.iItem = nRow;
			stlvItem.iSubItem = LOG_MESS_TYPE;

			cszOutletName.Empty();
			if ((pApp) && (pApp->pLeftView))
			{
				cszOutletName = pApp->pLeftView->GetExactOutletNm(cszPDUNm, nBankNum, nOutletNum);
			}
			
			switch (nAlertType)
			{
				case ALERT_VCB:
				{
					stlvItem.iImage = LOG_MESS_CRIC;

					cszLoad = _T("VCB ALERT");
					stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
					cszLoad.ReleaseBuffer();
					m_LogLstCtrl.SetItem(&stlvItem);

					cszLoad.Empty();
					cszTemp.LoadString(IDS_STR_HIGH_CURR_MESS);
					//cszLoad.Format(cszTemp, cszPDUDevice, nOutletNum, nBankNum, fCurrValue);
					cszLoad.Format(cszTemp, cszPDUDevice, cszOutletName, nBankNum, fCurrValue);

					m_LogLstCtrl.SetItemText(nRow, LOG_MESSAGE, cszLoad);

					if ((pApp) && (pApp->pLeftView))
					{
						pApp->pLeftView->UpdateOutletStatus(cszPDUNm, nBankNum, nOCBNum, nOutletNum, nRealOutlet, nAlertType);
					}
					break;
				}

				case ALERT_HIGH_CURRENT:
				{
					stlvItem.iImage = LOG_MESS_HIGH_VOLT;

					cszLoad = _T("HIGH CURRENT ALERT");
					stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
					cszLoad.ReleaseBuffer();
					m_LogLstCtrl.SetItem(&stlvItem);

					cszLoad.Empty();
					cszTemp.LoadString(IDS_STR_HIGH_CURR_MESS);
					//cszLoad.Format(cszTemp, cszPDUDevice, nOutletNum, nBankNum, fCurrValue);
					cszLoad.Format(cszTemp, cszPDUDevice, cszOutletName, nBankNum, fCurrValue);

					m_LogLstCtrl.SetItemText(nRow, LOG_MESSAGE, cszLoad);
					break;
				}

				case ALERT_LOW_CURRENT:
				{
					stlvItem.iImage = LOG_MESS_LOW_VOLT;

					cszLoad = _T("LOW CURRENT ALERT");
					stlvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
					cszLoad.ReleaseBuffer();
					m_LogLstCtrl.SetItem(&stlvItem);

					cszLoad.Empty();
					cszTemp.LoadString(IDS_STR_LOW_CURR_MESS);
					//cszLoad.Format(cszTemp, cszPDUDevice, nOutletNum, nBankNum, fCurrValue);
					cszLoad.Format(cszTemp, cszPDUDevice, cszOutletName, nBankNum, fCurrValue);

					m_LogLstCtrl.SetItemText(nRow, LOG_MESSAGE, cszLoad);
					break;
				}
			}

			m_LogLstCtrl.SetItemText(nRow, LOG_MESS_DATE, cszDateTime);

			m_LogLstCtrl.RedrawItems(0, m_LogLstCtrl.GetItemCount() - 1);
			UpdateWindow();
			
			nCount = m_LogLstCtrl.GetItemCount();
			if (nCount > 0)
			{
				m_LogLstCtrl.EnsureVisible((nCount - 1), FALSE);
				//m_LogLstCtrl.SetItemState((nCount - 1), LVIS_SELECTED, LVIS_SELECTED);
			}
		}

		if (bUpdated)
		{
			// Finally update the alert manage status
			cszQuery.Empty();
			cszQuery.Format(_T("UPDATE %s SET STATUS_NUM = %d WHERE STATUS_NUM != %d"), STR_PDU_ALERT_MESS_TABLE, MESS_UPDATED, MESS_UPDATED);
			nResult = sqlite3_exec(db, cszQuery.GetBuffer(cszQuery.GetLength()), NULL, 0, (char **)szErrMsg);
			cszQuery.ReleaseBuffer();
			if (nResult != SQLITE_OK) 
			{
				cszLoad.Empty();
				cszLoad.Format("Can't execute the Update process: %s", sqlite3_errmsg(db));
				AfxMessageBox(cszLoad, MB_ICONSTOP);
				return;
			}
		}

		if (pStmt != NULL)
		{
			nResult = sqlite3_finalize(pStmt);
			if (nResult == SQLITE_OK) 
				pStmt = NULL;
		}

		sqlite3_close(db);
	}
}

void CDlgBtm::StartBaseThread()
{
	if (m_pThreadBtm == NULL)
	{
		m_pThreadBtm = AfxBeginThread(ThreadGetAlertMess, (PVOID)this, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pThreadBtm->m_bAutoDelete = FALSE;
		m_pThreadBtm->ResumeThread();
	}
}

void CDlgBtm::CloseAllThreads()
{
	m_bThreadBtmExit = TRUE;	// exit base thread

	if (m_pThreadBtm != NULL)
	{
		::WaitForSingleObject(m_pThreadBtm->m_hThread, 2000);
		delete m_pThreadBtm;
		m_pThreadBtm = NULL;
	}
}

void CDlgBtm::OnDestroy()
{
	CloseAllThreads();

	CView::OnDestroy();

	// TODO: Add your message handler code here
}

void CDlgBtm::OnRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	CMenu		menu;
	CMenu		*pop;
	DWORD		dwPos = GetMessagePos();

    CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));

	menu.LoadMenu(IDR_MENU_LOG);
    pop = menu.GetSubMenu(0);

	/* Execute the selected menu command */
	UINT	nNum = 0;
	BOOL	bNoItem = FALSE;
	BOOL	bSelect = FALSE;
	UINT	nTotal = m_LogLstCtrl.GetItemCount();

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		if (ListView_GetCheckState(m_LogLstCtrl.m_hWnd, nNum))
		{
			bSelect = TRUE;
			break;
		}
	}

	if (!nTotal)
	{
		bNoItem = TRUE;
	}

	if ((!bSelect) || (bNoItem))
	{
		if (pop)
		{
			GetParentFrame()->m_bAutoMenuEnable = FALSE;
			pop->EnableMenuItem(ID_LOG_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			pop->EnableMenuItem(ID_LOG_DESELECTALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

			if (bNoItem)
			{
				pop->EnableMenuItem(ID_LOG_SELECTALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			}
		}
	}

	CWnd	*pMenuParent = AfxGetMainWnd();

	UINT uCmd = pop->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, pMenuParent, NULL);

	if (uCmd != 0)
    {
		pMenuParent->SendMessage(WM_COMMAND, uCmd, 0);
	}

	menu.DestroyMenu(); 

	*pResult = 0;
}

void CDlgBtm::OnLogDelete()
{
	// TODO: Add your command handler code here
	sqlite3			*db;
	int				nResult = 0;
	UINT			nNum = 0;
	UINT32 			nValue = 0;
	CString			cszQuery;
	BOOL			bDeleteItem = FALSE;
	CHAR			szErrMsg[MAX_PATH] = {0};
	UINT			nTotal = m_LogLstCtrl.GetItemCount();

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		if (bDeleteItem)
		{
			bDeleteItem = FALSE;
			nTotal = m_LogLstCtrl.GetItemCount();
			nNum--;
		}

		if (ListView_GetCheckState(m_LogLstCtrl.m_hWnd, nNum))
		{
			nValue = (UINT)m_LogLstCtrl.GetItemData(nNum);
			if (nValue)
			{
				nResult = sqlite3_open_v2(STR_PDU_CONFG_DB_NM, &db, SQLITE_OPEN_READWRITE, NULL);
				if (nResult != SQLITE_OK)
				{
					//cszLoad.Format("Can't open database: %s", sqlite3_errmsg(db));
					continue;
				}
				else
				{
					cszQuery.Empty();
					cszQuery.Format(_T("DELETE from %s WHERE SER_NO = %d"), STR_PDU_ALERT_MESS_TABLE, nValue);
					nResult = sqlite3_exec(db, cszQuery, NULL, 0, (char **)szErrMsg);

					sqlite3_close(db);

					m_LogLstCtrl.DeleteItem(nNum);

					bDeleteItem = TRUE;
				}
			}
			else
			{
				m_LogLstCtrl.DeleteItem(nNum);
				bDeleteItem = TRUE;
			}
		}
		else
		{
			// Rearrange the order in the CListCtrl
			nValue = atoi(m_LogLstCtrl.GetItemText(nNum, 0));
			if (nValue != (nNum + 1))
			{
				cszQuery.Empty();
				cszQuery.Format(_T("%d"), (nNum + 1));
				m_LogLstCtrl.SetItemText(nNum, 0, cszQuery);
			}
		}
	}
}

void CDlgBtm::OnLogSelectall()
{
	// TODO: Add your command handler code here
	UINT	nNum = 0;
	UINT	nTotal = m_LogLstCtrl.GetItemCount();

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		ListView_SetCheckState(m_LogLstCtrl.m_hWnd, nNum, TRUE);
	}
}

void CDlgBtm::OnLogDeselectall()
{
	// TODO: Add your command handler code here
	UINT	nNum = 0;
	UINT	nTotal = m_LogLstCtrl.GetItemCount();

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		if (ListView_GetCheckState(m_LogLstCtrl.m_hWnd, nNum))
		{
			ListView_SetCheckState(m_LogLstCtrl.m_hWnd, nNum, FALSE);
		}
	}
}

BOOL CDlgBtm::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_MOUSEMOVE)
	{
		// pass the mouse message to the ToolTip control for processing
		m_ToolTip.RelayEvent(pMsg); 
	}

	return CView::PreTranslateMessage(pMsg);
}

void CDlgBtm::InitToolTip()
{	
	CString		cszToolTipMess;

	cszToolTipMess.Empty();
	cszToolTipMess = _T("Right click will popup a menu to manage the Log messages");
	m_ToolTip.AddTool(GetDlgItem(IDC_LIST_LOG), cszToolTipMess);
}
