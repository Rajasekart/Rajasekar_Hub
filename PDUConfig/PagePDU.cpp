// PagePDU.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PagePDU.h"
#include "General.h"

// CPagePDU dialog

IMPLEMENT_DYNAMIC(CPagePDU, CDialog)

CPagePDU::CPagePDU(CWnd* pParent /*=NULL*/)
	: CDialog(CPagePDU::IDD, pParent)
{
	m_cszOutletNames.RemoveAll();
}

CPagePDU::~CPagePDU()
{
}

void CPagePDU::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OUTLET, m_cListOutlet);
}


BEGIN_MESSAGE_MAP(CPagePDU, CDialog)
//	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_OUTLET, &CPagePDU::OnLvnItemchangedListOutlet)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_OUTLET, &CPagePDU::OnLvnItemchangingListOutlet)
END_MESSAGE_MAP()


// CPagePDU message handlers

BOOL CPagePDU::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
/*	if (pMsg->wParam == VK_RETURN)
	{
		pMsg->wParam = NULL;
	}
	else*/ if (pMsg->wParam == VK_ESCAPE)
	{
		pMsg->wParam = NULL;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CPagePDU::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString				cszLoad;
	
	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_OUTLET_NM);
	m_cListOutlet.InsertColumn(0, cszLoad, LVCFMT_LEFT, 100);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_OUTLET_NM_TITLE);
	m_cListOutlet.InsertColumn(1, cszLoad, LVCFMT_LEFT, 280);

	m_cListOutlet.SetReadOnlyColumns(0);
	m_cListOutlet.SetValidEditCtrlCharacters(cszLoad);
	m_cListOutlet.EnableVScroll(); 

	m_cListOutlet.SetExtendedStyle(m_cListOutlet.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPagePDU::OnLvnItemchangedListOutlet(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CPagePDU::OnLvnItemchangingListOutlet(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	*pResult = 0;
}

void CPagePDU::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialog::OnOK();
}

void CPagePDU::UpdateOutletNamesInfo()
{
	UINT			nNum = 0;
	UINT			nRow = 0;
	CString			cszLoad;
	CString			cszTemp;
	LVITEM			lvItem = {0};

	m_cListOutlet.DeleteAllItems();
	m_cListOutlet.m_cszOrgOutletNm.RemoveAll();

	memset(&lvItem, 0, sizeof(LVITEM));
	lvItem.mask = LVIF_TEXT;
	
	for (nNum = 0; nNum < (UINT)m_cszOutletNames.GetSize(); nNum++)
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_OUTLET_NO);
		cszLoad.Format(cszTemp, nNum + 1);

		lvItem.iItem = nNum;
		lvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
		cszLoad.ReleaseBuffer();
		lvItem.iSubItem = 0;
		nRow = m_cListOutlet.InsertItem(&lvItem);

		cszLoad.Empty();
		cszLoad =  m_cszOutletNames.ElementAt(nNum);
		m_cListOutlet.SetItemText(nRow, OUTLET_NM_ID, cszLoad.GetBuffer(cszLoad.GetLength()));
		cszLoad.ReleaseBuffer();
	}

	m_cListOutlet.m_cszOrgOutletNm.RemoveAll();
	m_cListOutlet.m_cszOrgOutletNm.Copy(m_cszOutletNames);
}

void CPagePDU::CollectOutletNmsInfo()
{
	UINT			nNum = 0;
	CString			cszLoad;

	m_cszOutletNames.RemoveAll();
	for (nNum = 0; nNum < (UINT)m_cListOutlet.GetItemCount(); nNum++)
	{
		cszLoad.Empty();
		cszLoad = m_cListOutlet.GetItemText(nNum, OUTLET_NM_ID);
		m_cszOutletNames.Add(cszLoad);
	}
}