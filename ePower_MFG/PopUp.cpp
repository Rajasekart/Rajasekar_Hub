// PopUp.cpp : implementation file
//

#include "stdafx.h"
#include "ePower_MFG.h"
#include "PopUp.h"


// CPopUp dialog

IMPLEMENT_DYNAMIC(CPopUp, CDialog)

CPopUp::CPopUp(CWnd* pParent /*=NULL*/)
	: CDialog(CPopUp::IDD, pParent)
{

}

CPopUp::~CPopUp()
{
}

void CPopUp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPopUp, CDialog)
END_MESSAGE_MAP()


// CPopUp message handlers
