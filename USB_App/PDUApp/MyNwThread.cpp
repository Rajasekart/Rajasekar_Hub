#include "StdAfx.h"
#include "resource.h"
#include "General.h"
#include "PDUAppView.h"
#include "DlgBtm.h"

UINT ThreadGetNUpdatePDUData(PVOID pParam)
{
	UINT					nNum = 0;
	UINT					nPollSecs = 0;
	CString					cszLoad;
	CPDUAppView				*pPDUTreeListWnd = reinterpret_cast<CPDUAppView *> (pParam);

	if (!pPDUTreeListWnd)
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_WND_HAN_GET_FAIL);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return 1;
	}

	while (!pPDUTreeListWnd->m_bThreadExit)
	{
		pPDUTreeListWnd->GetNDisplayOutletData();

		if (pPDUTreeListWnd->m_bThreadExit)
			break;

		for (nNum = 1; nNum <= pPDUTreeListWnd->m_nPollSecs; nNum++)
		{
			if (pPDUTreeListWnd->m_bThreadExit)
				break;

			Sleep(100);

			if (pPDUTreeListWnd->m_bThreadExit)
				break;
		}

	}

	pPDUTreeListWnd->m_bThreadExit = FALSE;

	return 0;
}

UINT ThreadGetAlertMess(PVOID pParam)
{
	UINT				nNum = 0;
	CString				cszLoad;
	CDlgBtm				*pBtmLogWnd = reinterpret_cast<CDlgBtm *> (pParam);

	if (!pBtmLogWnd)
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_WND_HAN_GET_FAIL);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return 1;
	}

	while (!pBtmLogWnd->m_bThreadBtmExit)
	{
		pBtmLogWnd->AddCreatedAlertMess();

		if (pBtmLogWnd->m_bThreadBtmExit)
			break;

		for (nNum = 1; nNum <= 10; nNum++)
		{
			if (pBtmLogWnd->m_bThreadBtmExit)
				break;

			Sleep(100);

			if (pBtmLogWnd->m_bThreadBtmExit)
				break;
		}

	}

	pBtmLogWnd->m_bThreadBtmExit = FALSE;

	return 0;
}
