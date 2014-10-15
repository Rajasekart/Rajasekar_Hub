#include "stdafx.h"
#include "General.h"


PPDU_TREE_DET				gpHeadPDUInfo = NULL;
PPDU_TREE_DET				gpCurrPDUInfo = NULL;
PPDU_TREE_DET				gpNewPDUInfo = NULL;
PPDU_BANK_INFO				gpNewBankInfo = NULL;
PPDU_BANK_INFO				gpCurrBankInfo = NULL;
PPDU_OUTLET_INFO			gpNewOutletInfo = NULL;
PPDU_OUTLET_INFO			gpCurrOutletInfo = NULL;


VOID * Malloc(size_t size)
{
	ASSERT(size);

	BYTE *pNew = NULL;
	
	pNew = new BYTE[size];
	
	ASSERT(pNew);

	if(pNew)
		memset(pNew, 0, size);
	else
		AfxMessageBox(_T("Memory allocation error"));

	return (VOID *)pNew;
}

VOID Free(PVOID pNew)
{
	ASSERT(pNew);

	if(pNew)
	{
		delete [] pNew;
		pNew = NULL;
	}
}