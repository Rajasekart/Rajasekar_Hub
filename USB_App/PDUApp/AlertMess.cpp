// AlertMess.cpp : implementation file
//

#include "stdafx.h"
#include "PDUApp.h"
#include "AlertMess.h"


// CAlertMess dialog

IMPLEMENT_DYNAMIC(CAlertMess, CDialog)

CAlertMess::CAlertMess(CWnd* pParent /*=NULL*/)
	: CDialog(CAlertMess::IDD, pParent)
{

}

CAlertMess::~CAlertMess()
{
}

void CAlertMess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAlertMess, CDialog)
END_MESSAGE_MAP()


// CAlertMess message handlers
