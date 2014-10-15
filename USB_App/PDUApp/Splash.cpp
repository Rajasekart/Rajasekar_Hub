// Splash.cpp : implementation file
//

#include "stdafx.h"
#include "PDUApp.h"
#include "Splash.h"


// CSplash dialog

IMPLEMENT_DYNAMIC(CSplash, CDialog)

CSplash::CSplash(CWnd* pParent /*=NULL*/)
	: CDialog(CSplash::IDD, pParent)
{

}

CSplash::~CSplash()
{
}

void CSplash::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSplash, CDialog)
END_MESSAGE_MAP()


// CSplash message handlers
