// @doc TreeListView_cpp
//
// @module TreeListView.cpp -  Implementation of <c CTreeListView> |
// 
// <nl>
// Copyright (C) 1999 <nl>
// Gerolf Kühnel
//
// @head3 Elements defined in this Module | 
// @index class mstuct menum enum struct | TreeListView_cpp

#include "stdafx.h"
#include "TreeListView.h"
#include "PDUApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//************************************ internal helperfunctions *******************************************

//@func streches a long to an fitting multiply of nMeasure
//
//@parm long | nWidth	| old witdh
//
//@parm long | nMeasure | Measur
//
//@rdesc new width
static long StretchWidth(long nWidth, long nMeasure)
{
	return ((nWidth / nMeasure) + 1) * nMeasure;
}

//************************************ CMyTreeObj *********************************************************

//@mfunc copys the stringarray from one object to the next
//
//@parm CMyTreeObj& | obj | referenz to the object containig the data to be copied
//
//@rdesc referenz to the object
CMyTreeObj& CMyTreeObj::operator=(CMyTreeObj& obj)
{
	m_strColumns.SetSize(obj.m_strColumns.GetSize());

	// the CStringArray class doesn't have a operator= so copy the data by hand
	for(long i=0; i < obj.m_strColumns.GetSize(); i++)
	{
		m_strColumns[i] = obj.m_strColumns[i];
	}

	return *this;
}

//************************************ CMyTreeCtrl ********************************************************

// MessageMap for CMyTreeCtrl
BEGIN_MESSAGE_MAP(CMyTreeCtrl, CTreeCtrl)
	ON_MESSAGE(TVM_INSERTITEM, OnInsertItem)
	ON_MESSAGE(WM_LBUTTONDBLCLK, OnLButtonDblClk)
	ON_MESSAGE(WM_LBUTTONDOWN, OnLButtonClk)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CMyTreeCtrl::OnNMRclick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_CLICK, &CMyTreeCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_RDBLCLK, &CMyTreeCtrl::OnNMRdblclk)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CMyTreeCtrl::OnNMDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CMyTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()

//@mfunc overwienable the parent to do something when an item is inserted into the tree
//
//@parm WPARAM | wParam | 0
//
//@parm LPARAM | lParam | Address of a TVINSERTSTRUCT structure that specifies the attributes of the tree view item
//
//@rdesc handle of the new TreeItem or NULL
LRESULT CMyTreeCtrl::OnInsertItem(WPARAM wParam, LPARAM lParam)
{
	HTREEITEM hItem = (HTREEITEM) DefWindowProc(TVM_INSERTITEM, wParam, lParam);

	// notify the parentwindow
	GetParent()->SendMessage(TVMYN_INSERTITEM, (WPARAM) hItem, 0);

	return (LRESULT) hItem;
}

//@mfunc checks, if there was a doubleclick right of the normal treeitem and puts the x-Pos to left
//
//@parm WPARAM | wParam | Indicates whether various virtual keys are down, see Win32 documentation
//
//@parm LPARAM | lParam | specifies the Mousecursosposition, see Win32 documentation
//
//@rdesc the returnvalue of the default procedure
LRESULT CMyTreeCtrl::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	short xPos = LOWORD(lParam); 
	short yPos = HIWORD(lParam); 	
	
	TVHITTESTINFO HitTestInfo;

	HitTestInfo.pt.x = xPos;
	HitTestInfo.pt.y = yPos;

	HitTest(&HitTestInfo);
	if(HitTestInfo.flags == TVHT_ONITEMRIGHT)
	{
		CRect rc;
		GetItemRect(HitTestInfo.hItem, &rc, TRUE);
		xPos = (short)(rc.left + rc.Width() / 2);
		lParam = yPos;
		lParam <<= 16;
		lParam &= 0x0ffff0000;
		lParam += xPos;
	}

	return DefWindowProc(WM_LBUTTONDBLCLK, wParam, lParam);
}

//@mfunc checks, if there was a click right of the normal treeitem and puts the x-Pos to left
//
//@parm WPARAM | wParam | Indicates whether various virtual keys are down, see Win32 documentation
//
//@parm LPARAM | lParam | specifies the Mousecursosposition, see Win32 documentation
//
//@rdesc the returnvalue of the default procedure
LRESULT CMyTreeCtrl::OnLButtonClk(WPARAM wParam, LPARAM lParam)
{
	short xPos = LOWORD(lParam); 
	short yPos = HIWORD(lParam); 	
	
	TVHITTESTINFO HitTestInfo;

	HitTestInfo.pt.x = xPos;
	HitTestInfo.pt.y = yPos;

	HitTest(&HitTestInfo);
	if(HitTestInfo.flags == TVHT_ONITEMRIGHT)
	{
		CRect rc;
		GetItemRect(HitTestInfo.hItem, &rc, TRUE);
		xPos = (short)(rc.left + rc.Width() / 2);
		lParam = yPos;
		lParam <<= 16;
		lParam &= 0x0ffff0000;
		lParam += xPos;
	}

	return DefWindowProc(WM_LBUTTONDOWN, wParam, lParam);
}

//************************************ CTreeListView ******************************************************


IMPLEMENT_DYNCREATE(CTreeListView, CView)

BEGIN_MESSAGE_MAP(CTreeListView, CView)
	//{{AFX_MSG_MAP(CTreeListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_MESSAGE(TVMYN_INSERTITEM, OnInsertItem)
END_MESSAGE_MAP()

/************************************
 constructors and destructors
************************************/
//@mfunc initialises all variables
CTreeListView::CTreeListView()
{
	m_bDescriptionDisplay = true;									// normaly we have columnheaders
	m_nOffset = 0;													// we have not scrolled yet
	m_nHeaderWidth = 0;												// the width of the columns is zero because we have no
	m_colBackColor = RGB(255, 255, 255);							// the back color is set to white for erasing (may have other values, used for the future)
	m_vsCol = NULL;													// there are no columns so there is no data
	m_nNrColumns = 0;												// the number of columns is zero
	m_BackBrush.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));	// the backgroundbrush for highlightet text is inistialised with the default value (system)
	m_colHilightText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);			// the textcolor for highlightet text is inistialised with the default value (system)
	m_colText = RGB(0,0,0);											// the normal textcolor is black (may have other values, used for the future)
	m_BackBrushNormal.CreateSolidBrush( m_colBackColor );			// a proposal from codeguru (anonymus)
}

//@mfunc frees all memory
CTreeListView::~CTreeListView()
{
	// deletre dynamic array
	if(m_vsCol != NULL)
	{
		delete []m_vsCol;
	}
	
	m_nNrColumns = 0;
}

/************************************
 virtual funcions
************************************/
//@mfunc	normally this function draws the window, not here. we let the system do thw job for us.
//			but it is forbidden to remove the function, because it's pure virtual in the base class
//
//@parm CDC* | pDC | pointer to the device context to draw to (see Win32 documentation)
void CTreeListView::OnDraw(CDC* pDC) 
{
	UNUSED_PARAM(pDC);
}

//@mfunc The framework calls this member function to inform the parent window of a control that an event has occurred in the control or that the control 
//		 requires some kind of information.
//
//@parm  WPARAM   | wParam  | Identifies the control that sends the message if the message is from a control. Otherwise, wParam is 0.
//
//@parm  LPARAM   | lParam  | Pointer to a notification message (NMHDR) structure that contains the notification code and additional information. 
//							  For some notification messages, this parameter points to a larger structure that has the NMHDR structure as its first 
//							  member.
//
//@parm  LRESULT* | pResult | Pointer to an LRESULT variable in which to store the result code if the message is handled.
//
//@rdesc An application returns nonzero if it processes this message; otherwise 0.
BOOL CTreeListView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LPNMHDR pNmhdr = (LPNMHDR)lParam;

	//TreeCtrl notifications
	if(m_ctrlTree.m_hWnd == pNmhdr->hwndFrom)
	{
		switch (pNmhdr->code)
		{
		case NM_CUSTOMDRAW:
			return OnCustomdrawTree(pNmhdr, pResult);
		case TVN_DELETEITEM:
			return OnDeleteItem(pNmhdr, pResult);
		}
	}
	//HeaderCtrl notifications
	else if(m_ctrlHeader.m_hWnd == pNmhdr->hwndFrom)
	{
		switch (pNmhdr->code)
		{
		case HDN_ENDTRACK:
			return OnEndTrack(pNmhdr, pResult);
		}
	}

	return CView::OnNotify(wParam, lParam, pResult);
}

/************************************
 messagehandlers
************************************/
//@mfunc Creates the TreeCtrl and the Header that is used
//
//@parm LPCREATESTRUCT | lpCreateStruct | Points to a CREATESTRUCT structure that contains information about the CWnd object being created.
//
//@rdesc OnCreate must return 0 to continue the creation of the CWnd object. If the application returns -1, the window will be destroyed.
int CTreeListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// set all rects to zero because OnSize is called after creating the windows and we give them the positions in OnSize
	CRect rcTree(0,0,0,0);
	CRect rcHeader(0,0,0,0);
	DWORD dwStyle = 0;

	// first create the TreeCtrl. I use standard styles but you can also use your own styles or get TreeCtrl styles from the parameter
	dwStyle = WS_CHILD | WS_VISIBLE | TVS_DISABLEDRAGDROP | TVS_NOHSCROLL | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	m_ctrlTree.Create(dwStyle, rcTree, this, 0);

	// next create the HeaderCtrl. I use standard styles but you can also use your own styles or get HeaderCtrl styles from the parameter
	dwStyle = WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ;
	m_ctrlHeader.Create(dwStyle, rcHeader, this, 0);
	
	return 0;
}

//@mfunc The framework calls this member function after the window’s size has changed.
//
//@parm UINT	| nType	| Specifies the type of resizing requested. See Win 32 docu
//
//@parm int		| cx	| Specifies the new width of the client area.
//
//@parm int		| cy	| Specifies the new hight of the client area.
void CTreeListView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	// I use this function so i can also resize the windows without the message
	SizeObjects();
}

//@mfunc
//
//@parm UINT		| nSBCode		| Specifies a scroll-bar code that indicates the user’s scrolling request. See win32 docu
//
//@parm UINT		| nPos			| Specifies the scroll-box position if the scroll-bar code is SB_THUMBPOSITION or SB_THUMBTRACK; 
//									  otherwise, not used. Depending on the initial scroll range, nPos may be negative and should be 
//									  cast to an int if necessary.
//
//@parm CScrollBar*	| pScrollBar	| If the scroll message came from a scroll-bar control, contains a pointer to the control. If the 
//									  user clicked a window’s scroll bar, this parameter is NULL. The pointer may be temporary and should 
//									  not be stored for later use.
void CTreeListView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UNUSED_PARAM(pScrollBar);

	CRect	rc;
	// Calculate Scrollsizes
	m_ctrlTree.GetClientRect(&rc);

	long	nWidthPage	= abs(rc.right - rc.left);	// One Page
	long	nWidthLine	= 6;						// Microsofts Linestep in a CListCtrl, got from MFC - sourcecode
	long	nCurPos		= GetScrollPos(SB_HORZ);	// current scrollingposition
	long	nPrevPos	= nCurPos;;					// save current scrolling position for calculating
	int		nScrollMin;								// minimum scrolling value
	int		nScrollMax;								// maximum scrolling value

	GetScrollRange(SB_HORZ, &nScrollMin, &nScrollMax);

	// Check, which kind of scoll is wanted
	switch(nSBCode)
	{
	case SB_LEFT :								// scoll to left most position
		{
			nCurPos = 0;
			break;
		}
	case SB_RIGHT :								// scroll to right most position
		{
			nCurPos = nScrollMax;
			break;
		}
	case SB_LINELEFT :							// scroll left with the button
		{
			nCurPos = max(nCurPos - nWidthLine, 0);
			break;
		}
	case SB_LINERIGHT :							// scroll right with the button
		{
			nCurPos = min(nCurPos + nWidthLine, nScrollMax);
			break;
		}
	case SB_PAGELEFT :							// scroll left with a click to the background of the scrollbar
		{
			nCurPos = max(nCurPos - nWidthPage, 0);
			break;
		}
	case SB_PAGERIGHT :							// scroll left with a click to the background of the scrollbar
		{
			nCurPos = min(nCurPos + nWidthPage, nScrollMax);
			break;
		}
	case SB_THUMBPOSITION :						// scroll by moving the scrollbutton with the mouse
	case SB_THUMBTRACK :						// drop the scrollbarbutton
		{
			// check for illegal positions and correct them (out of the scrollbar?)
			if(nPos == 0)
			{
				nCurPos = 0;
			}
			else
			{
				nCurPos = min(StretchWidth(nPos, nWidthLine), nScrollMax);
			}
			break;
		}
	default:
		{
			break;
		}
	};

	// move the scrollbarbutton to the position (graphically)
	SetScrollPos(SB_HORZ, nCurPos, TRUE);
	m_nOffset = -nCurPos;

	// smoothly Scroll the Tree control
	CRect rcTree;
	m_ctrlTree.GetClientRect(&rcTree);
	m_ctrlTree.ScrollWindowEx(nPrevPos - nCurPos, 0, NULL, NULL, NULL, NULL, SW_INVALIDATE);

	// Scroll the Header
	CRect rcHeader;
	GetClientRect(&rc);
	m_ctrlHeader.GetClientRect(&rcHeader);

	if(rcTree.right - rcTree.left != 0)
	{
		m_ctrlHeader.SetWindowPos(&wndTop, m_nOffset, 0, max(StretchWidth(m_nHeaderWidth, abs(rcTree.right - rcTree.left)), nWidthPage), abs(rcHeader.bottom - rcHeader.top), SWP_SHOWWINDOW);
	}

	// redraw the treecontrol so you can see the scrolling
	m_ctrlTree.Invalidate(TRUE);
}


//@mfunc used to notify the view trhat a new item is inserted, so the view can insert a new string into the lists
//
//@parm WPARAM | wParam | handle of the new treeitem (HTREEITEM)
//
//@parm LPARAM | lParam | not used
//
//@rdesc always 0
LRESULT CTreeListView::OnInsertItem(WPARAM wParam, LPARAM lParam)
{
	UNUSED_PARAM(lParam);

	// get the treeitem
	HTREEITEM hItem = (HTREEITEM) wParam;
	CString str = m_ctrlTree.GetItemText(hItem);

	// insert a new item in the ObjectMap
	CMyTreeObj MyObj;
	MyObj.m_strColumns.SetSize(m_nNrColumns);
	MyObj.m_strColumns[0] = str;

	m_Entries.SetAt(hItem, MyObj);

	return 0;
}


/************************************
 clientaccess
************************************/

//@mfunc Inserts a new column in a treelist view control.
//
//@parm int			| nCol				| The index of the new column.
//
//@parm LPCTSTR		| lpszColumnHeading	| Address of a string containing the column’s heading.
//
//@parm int			| nFormat			| Integer specifying the alignment of the column. It can be one of these values: <nl>
//										  <tab>LVCFMT_LEFT, LVCFMT_RIGHT, or LVCFMT_CENTER.
//
//@parm int			| nWidth			| Width of the column, in pixels. If this parameter is -1, the column width is not set.
//
//@parm int			| nSubItem			| Index of the subitem associated with the column. If this parameter is -1, no subitem 
//										  is associatied with the column.
//
//@rdesc The index of the new column if successful or -1 otherwise.
int CTreeListView::InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	// Create the LVCOLUMN struct and fill it an call the InsertColumn with two parameters
	LVCOLUMN column;
	column.mask = LVCF_TEXT|LVCF_FMT;
	column.pszText = (LPTSTR)lpszColumnHeading;
	column.fmt = nFormat;
	if (nWidth != -1)
	{
		column.mask |= LVCF_WIDTH;
		column.cx = nWidth;
	}
	if (nSubItem != -1)
	{
		column.mask |= LVCF_SUBITEM;
		column.iSubItem = nSubItem;
	}
	return InsertColumn(nCol, &column);
}

//@mfunc Inserts a new column in a treelist view control.
//
//@parm int				| nCol		| The index of the new column.
//
//@parm const LVCOLUMN*	| pColumn	| Address of an LVCOLUMN structure that contains the attributes of the new column.
//
//@rdesc The index of the new column if successful or -1 otherwise.
int CTreeListView::InsertColumn( int nCol, const LVCOLUMN* pColumn )
{
	// generate and initialise a new headeritem
	HDITEM hdi;
	::ZeroMemory(&hdi, sizeof(HDITEM));

	// fill the formatbits 
	if(pColumn->mask & LVCF_FMT)
	{
		hdi.mask |= HDI_FORMAT;

		if(pColumn->fmt & LVCFMT_BITMAP_ON_RIGHT)
			hdi.fmt |= HDF_BITMAP_ON_RIGHT;
		if(pColumn->fmt & LVCFMT_CENTER)
			hdi.fmt |= HDF_CENTER;
		if(pColumn->fmt & LVCFMT_COL_HAS_IMAGES)
			hdi.fmt |= HDF_IMAGE;
		if(pColumn->fmt & LVCFMT_IMAGE)
			hdi.fmt |= HDF_IMAGE;
		if(pColumn->fmt & LVCFMT_JUSTIFYMASK)
			hdi.fmt |= HDF_JUSTIFYMASK;
		if(pColumn->fmt & LVCFMT_LEFT)
			hdi.fmt |= HDF_LEFT;
		if(pColumn->fmt & LVCFMT_RIGHT)
			hdi.fmt |= HDF_RIGHT;
	}

	// should an image be displayed?
	if(pColumn->mask & LVCF_IMAGE)
	{
		hdi.mask |= HDI_IMAGE;
		hdi.iImage = pColumn->iImage;
	}
	
	// insert the column at the spezified position
	if(pColumn->mask & LVCF_ORDER)
	{
		hdi.mask |= HDI_ORDER;
		hdi.iOrder = pColumn->iOrder;
	}

	// dispay a text ?
	if(pColumn->mask & LVCF_TEXT)
	{
		hdi.mask |= HDI_TEXT;
		hdi.pszText = pColumn->pszText;
		hdi.cchTextMax = pColumn->cchTextMax;
	}

	// set a width?
	if(pColumn->mask & LVCF_WIDTH)
	{
		hdi.mask |= HDI_WIDTH;
		hdi.cxy = pColumn->cx;
	}

	// insert the item
	long rCol = m_ctrlHeader.InsertItem(nCol, &hdi);
	m_nNrColumns++;

	// update the internal Lists with the columntexts for the objects
	POSITION pos = m_Entries.GetStartPosition();
	HTREEITEM hItem;
	while(pos != NULL)
	{
		CMyTreeObj item;
		m_Entries.GetNextAssoc(pos, hItem, item);
		item.m_strColumns.InsertAt(rCol, _T(""), 1);
	}

	UpdateColStruct();

	return rCol;
}

//@mfunc Call this function to delete a column from the tree list view control.
//
//@parm int | nCol | Index of the column to be deleted.
//
//@rdesc Nonzero if successful; otherwise zero.
BOOL CTreeListView::DeleteColumn( int nCol )
{
	BOOL bRet = m_ctrlHeader.DeleteItem(nCol);

	// if the specified column could be deleted, delete the columns in our textarray
	if(bRet == TRUE)
	{
		m_nNrColumns--;

		POSITION pos = m_Entries.GetStartPosition();
		HTREEITEM hItem;
		while(pos != NULL)
		{
			CMyTreeObj item;
			m_Entries.GetNextAssoc(pos, hItem, item);
			item.m_strColumns.RemoveAt(nCol, 1);
		}
	}

	UpdateColStruct();
	return bRet;
}

//@mfunc Changes the text of a tree list view item or subitem.
//
//@parm HTREEITEM	| hItem		| Handle of the item whose text is to be set.
//
//@parm int			| nSubItem	| Index of the subitem, or zero to set the item label.
//
//@parm CString		| strBuffer	| String that contains the new item text.
//
//@devnote be sure only to use valid handles, that you got from this view or from the treectrl!!
//
//@rdesc Nonzero if successful; otherwise zero.
BOOL CTreeListView::SetSubItemText( HTREEITEM hItem, int nSubItem, CString strBuffer)
{
	BOOL bRet = TRUE;

	// set the new text
	m_Entries[hItem].m_strColumns[nSubItem] = strBuffer;;

	return bRet;
}

//@mfunc retrieves the text of a specified column of a tree list item
//
//@parm HTREEITEM	| hItem		| Handle of the item whose text is to be set.
//
//@parm int			| nSubItem	| Index of the subitem, or zero to set the item label.
//
//@parm CString&	| strBuffer	| Referenz to a string that should get the item text
//
//@devnote be sure only to use valid handles, that you got from this view or from the treectrl!!
//
//@rdesc Nonzero if successful; otherwise zero.
BOOL CTreeListView::GetSubItemText( HTREEITEM hItem, int nSubItem, CString& strBuffer)
{
	BOOL bRet = TRUE;

	strBuffer = m_Entries[hItem].m_strColumns[nSubItem];

	return bRet;
}

//@mfunc retrieves the imagelist of the headercontrol (if there is one)
//
//@rdesc a pointer to the headers imagelist or NULL
CImageList* CTreeListView::GetHeaderImageList()
{
	return m_ctrlHeader.GetImageList();
}

//@mfunc sets the imagelist of the headercontrol
//
//@parm CImageList* | pImageList | pointer to the imagelist to set
//
//@rdesc a pointer to the headers imagelist or NULL
CImageList* CTreeListView::SetHeaderImageList( CImageList * pImageList)
{
	return m_ctrlHeader.SetImageList(pImageList);
}

//@mfunc Returns a reference to the tree control associated with the view.
//
//@rdesc referenc to the tree ctrl
CTreeCtrl& CTreeListView::GetTreeCtrl()
{
	return m_ctrlTree;
}

//@mfunc turns the columnheaders on / off
//
//@parm bool | bShow | should the columnheaders be shown?
void CTreeListView::ShowHeader(bool bShow)
{
	if(bShow != m_bDescriptionDisplay)
	{
		m_bDescriptionDisplay = bShow;
		SizeObjects();
	}
}

/************************************
 helperfunctions
************************************/

//@mfunc Calculates the Rects for the Windows
//
// @parm CRect& | rcBase	| Referenc to the Bounding Rectangle
//
// @parm CRect& | rcTree	| Referenc to the Tree Rectangle
//
// @parm CRect& | rcHeader	| Referenc to the Header Rectangle
void CTreeListView::CalculateClientRects(CRect &rcBase, CRect &rcTree, CRect &rcHeader)
{
	// Perhapse, show Header
	if(m_bDescriptionDisplay == TRUE)
	{
		rcHeader = rcBase;
		rcHeader.bottom = rcHeader.top + ::GetSystemMetrics(SM_CYMENU);
	}
	else
	{
		RECT rc = {0,0,0,0};
		rcHeader = rc;
	}

	rcTree = rcBase;
	rcTree.top = rcHeader.bottom;
}

// @mfunc resizes the Controls
void CTreeListView::SizeObjects()
{
	ResetScrollbar();

	CRect rcBase;
	CRect rcTree;
	CRect rcHeader;
	long nOffset = m_nOffset;

	UpdateColStruct();

	GetClientRect(&rcBase);
	CalculateClientRects(rcBase, rcTree, rcHeader);

	m_ctrlHeader.SetWindowPos(&wndTop, rcHeader.left, rcHeader.top, rcHeader.Width(), rcHeader.Height(), SWP_NOZORDER);
	m_ctrlTree.SetWindowPos(&wndTop, rcTree.left, rcTree.top, rcTree.Width(), rcTree.Height(), SWP_NOZORDER);

	// restore Scrollposition
	long ScrollCode = nOffset;
	ScrollCode = ((ScrollCode << 16 ) & 0x0ffff0000) + SB_THUMBPOSITION;
	SendMessage(WM_HSCROLL, ScrollCode, 0);
	Invalidate(FALSE);
}

// @mfunc Resets the horizontal scrollbar and resizes the Windows, if neccessary
//  
// @rdesc If successfull, the return value is TRUE else it is FALSE
bool CTreeListView::ResetScrollbar()
{
	CRect rcTree;
	long nWndWidth;

	HDITEM HeaderItem;
	HeaderItem.mask = HDI_WIDTH;

	m_ctrlTree.GetClientRect(&rcTree);
	m_nHeaderWidth = 0;

	// Get summary of headeritem widths
	for(int i = 0; i < m_ctrlHeader.GetItemCount(); i++)
	{
		m_ctrlHeader.GetItem(i, &HeaderItem);
		m_nHeaderWidth += HeaderItem.cxy;
	}
	nWndWidth = rcTree.right - rcTree.left;

	// if the width of all Headeritems is bigger than the width of the ClientRect of the TreeView,
	// the Scrollbar is to be enabled
	if(m_nHeaderWidth > nWndWidth)
	{
		SetScrollRange(SB_HORZ, 0, m_nHeaderWidth - nWndWidth);
		SetScrollPos(SB_HORZ, 0, TRUE);
		ModifyStyle(0, WS_HSCROLL, 0);
		m_nOffset = 0;
	}
	else
	{
		SetScrollRange(SB_HORZ, 0, 0);
		SetScrollPos(SB_HORZ, 0, TRUE);
		ModifyStyle(WS_HSCROLL, 0, 0);
		m_nOffset = 0;
	}

	return true;
}

//@mfunc draws the text of the treeitems (all columns!!)
//
//@parm bool		| bFocus	| is the focus on the specified item?
//
//@parm CRect		| rcItem	| Rectangle of the item 
//
//@parm HDC			| hdc		| Handle of the DeviceContext to draw to
//
//@parm HTREEITEM	| hItem		| Handle of the tree item that is to be drawn
void CTreeListView::DrawTreeItem(bool bFocus, CRect rcItem, HDC hdc, HTREEITEM hItem)
{
	COLORREF colText = m_colText;

	// if the item has got the focus, we have to draw sorouinding rectangle and fill a rect blue
	if(bFocus == true)
	{
		::FillRect(hdc, &rcItem, (HBRUSH)m_BackBrush.m_hObject);

		// Add new focus rect code...
		::DrawFocusRect( hdc, &rcItem );

		colText = m_colHilightText;
	}
	else
	{
		// add this to clear leftover highlight bar
		::FillRect(hdc, &rcItem, (HBRUSH)m_BackBrushNormal.m_hObject);
	}

	// always write text without background
	::SetBkMode(hdc, TRANSPARENT);
	::SetTextColor(hdc, colText);

	// draw all columns of the item
	RECT rc = rcItem;
	for(long i=0; i < m_nNrColumns; i++)
	{
		if(i != 0)
			rc.left = m_vsCol[i].rcDefault.left;
		rc.right = m_vsCol[i].rcDefault.right;

		CString		str;

		if (m_Entries[hItem].m_strColumns.GetCount())
			str = m_Entries[hItem].m_strColumns.ElementAt(i);

		::DrawText(hdc, str, -1, &rc, DT_BOTTOM | DT_SINGLELINE | DT_WORD_ELLIPSIS | m_vsCol[i].nAlingment);
	}
}

//@mfunc updates the m_vsCol variable to the actual value
void CTreeListView::UpdateColStruct(void)
{
	// delete old coulnstructs
	m_nHeaderWidth = 0;
 	delete []m_vsCol;
	m_vsCol = NULL;

	// are there columns?
	if(m_nNrColumns != 0)
	{
		m_vsCol = new HeaderData[m_nNrColumns];

		// fill columnstructures
		for(long i=0; i < m_nNrColumns; i++)
		{
			HDITEM HeaderItem;

			// Get item information from the header control
			HeaderItem.mask = HDI_WIDTH | HDI_LPARAM | HDI_FORMAT;
			m_ctrlHeader.GetItem(i, &HeaderItem);
			m_vsCol[i].nWidth = HeaderItem.cxy;

			// calculate the rects
			if(i == 0)
			{
				m_vsCol[i].rcDefault.left = 2;
			}
			else
				m_vsCol[i].rcDefault.left = m_vsCol[i-1].rcDefault.right + 2;
			m_vsCol[i].rcDefault.right = m_vsCol[i].rcDefault.left  + m_vsCol[i].nWidth - 2;

			// get the alignment formatting
			switch(HeaderItem.fmt & 3)
			{
			case HDF_RIGHT:
				{
					m_vsCol[i].nAlingment = DT_RIGHT;
					break;
				}
			case HDF_CENTER:
				{
					m_vsCol[i].nAlingment = DT_CENTER;
					break;
				}
			case HDF_LEFT:
			default:
				{
					m_vsCol[i].nAlingment = DT_LEFT;
					break;
				}
			};
			
			m_nHeaderWidth += m_vsCol[i].nWidth;
		}
	}
}

//@mfunc set's up the drawingcontext and triggers the itemds textdrawing
//
//@parm LPNMHDR	  | pNmhdr	| Pointer to a notification message (NMHDR) structure that contains the notification code and additional information. 
//							  For some notification messages, this parameter points to a larger structure that has the NMHDR structure as its first 
//							  member.
//
//@parm  LRESULT* | pResult | Pointer to an LRESULT variable in which to store the result code if the message is handled.
//
//@rdesc An application returns nonzero if it processes this message; otherwise 0.
BOOL CTreeListView::OnCustomdrawTree(LPNMHDR pNmhdr, LRESULT* pResult)
{
	static CRect	rcItem;
	static CPoint	poi;
	static bool		bFocus;

	BOOL bRet = FALSE;

	LPNMTVCUSTOMDRAW pCustomDraw = (LPNMTVCUSTOMDRAW)pNmhdr;
	switch (pCustomDraw->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			// Need to process this case and set pResult to CDRF_NOTIFYITEMDRAW, 
			// otherwise parent will never receive CDDS_ITEMPREPAINT notification. (GGH) 
			*pResult = CDRF_NOTIFYITEMDRAW;

			// reposuition the viewport so the TreeCtrl DefWindowProc doesn't draw to viewport 0/0
			::SetViewportOrgEx(pCustomDraw->nmcd.hdc, m_nOffset, 0, NULL);
			bRet = TRUE;
			break;

		case CDDS_ITEMPREPAINT:
			// set the background and foregroundcolor of the item to the background, so you don't see the default drawing of the text
			pCustomDraw->clrText = m_colBackColor;
			pCustomDraw->clrTextBk = m_colBackColor;

			// reset the focus, because it will be drawn of us
			bFocus = false;
			if(	pCustomDraw->nmcd.uItemState & CDIS_FOCUS)
			{
				bFocus = true;
			}

			pCustomDraw->nmcd.uItemState &= ~CDIS_FOCUS;

			// remember the drawing rectangle of the item so we can draw it ourselves
			m_ctrlTree.GetItemRect((HTREEITEM) pCustomDraw->nmcd.dwItemSpec, &rcItem, TRUE);
			rcItem.right = (pCustomDraw->nmcd.rc.right > m_nHeaderWidth) ? pCustomDraw->nmcd.rc.right : m_nHeaderWidth;

			// we want to get the CDDS_ITEMPOSTPAINT notification
			*pResult = CDRF_NOTIFYPOSTPAINT;
			bRet = TRUE;
			break;

		case CDDS_ITEMPOSTPAINT:

			// draw the item
			DrawTreeItem(bFocus, rcItem, pCustomDraw->nmcd.hdc, (HTREEITEM) pCustomDraw->nmcd.dwItemSpec);
			bRet = TRUE;
			break;
	}

	return bRet;
}

//@mfunc indicates, that an Headercolumn has changed his size
//
//@parm LPNMHDR	  | pNmhdr	| Pointer to a notification message (NMHDR) structure that contains the notification code and additional information. 
//							  For some notification messages, this parameter points to a larger structure that has the NMHDR structure as its first 
//							  member.
//
//@parm  LRESULT* | pResult | Pointer to an LRESULT variable in which to store the result code if the message is handled.
//
//@rdesc always FALSE
BOOL CTreeListView::OnEndTrack(LPNMHDR pNmhdr, LRESULT* pResult)
{
	UNUSED_PARAM(pResult);
	UNUSED_PARAM(pNmhdr);

	// we need to post thios message so the headercontrol can take the time to save the information of the new sizes an we can the get it from the control
	PostMessage(WM_SIZE);
	return FALSE;
}

//@mfunc when an item is to be deleted, we delete its columndata (the texts)
//
//@parm LPNMHDR	  | pNmhdr	| Pointer to a notification message (NMHDR) structure that contains the notification code and additional information. 
//							  For some notification messages, this parameter points to a larger structure that has the NMHDR structure as its first 
//							  member.
//
//@parm  LRESULT* | pResult | Pointer to an LRESULT variable in which to store the result code if the message is handled.
//
//@rdesc always TRUE
BOOL CTreeListView::OnDeleteItem(LPNMHDR pNmhdr, LRESULT* pResult)
{
	UNUSED_PARAM(pResult);

	BOOL bRet = TRUE;

	NMTREEVIEW* pnmtv = (NMTREEVIEW*) pNmhdr;

	m_Entries.RemoveKey(pnmtv->itemOld.hItem);

	return bRet;
}

void CMyTreeCtrl::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

    DWORD		dwPos = GetMessagePos();

    CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos)), spt;
    spt = pt;

    this->ScreenToClient( &spt );

    UINT test;
    HTREEITEM hti = this->HitTest(spt, &test);

    if (hti != NULL)
    {
		if (test & (TVHT_ONITEM | TVHT_ONITEMRIGHT))
        {
			BOOL			bShowMenu = TRUE;
			CString			cszLoad;
			CString			cszOutlet;
			HTREEITEM		htCur = this->GetSelectedItem();

            /* Store the value of the right clicked item only *if*
             * it's different from the currently selected item.
             */
            if (hti != htCur)
            {
				m_hActiveItem = hti;
				this->SelectItem(m_hActiveItem);
            }

		/*	cszLoad.Empty();
			cszLoad = GetItemText(hti);

			cszOutlet.Empty();
			cszOutlet.LoadString(IDS_STR_OUTLET_NM_CHECK);

			if (cszOutlet.GetLength() <= cszLoad.GetLength())
			{
				cszLoad = cszLoad.Left(cszOutlet.GetLength());
			}

			if (cszOutlet.CompareNoCase(cszLoad))
			{
				bShowMenu = FALSE;
			}*/

			if (bShowMenu)
			{
				/* Do the normal context menu stuff */
				OnContextMenu(this, pt);
			}
		}
	} 

	PassTreeEvent(TREE_ITEM_RCLICK);

	*pResult = 0;
}

void CMyTreeCtrl::OnNMRdblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	DWORD		dwPos = GetMessagePos();

    CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos)), spt;
    spt = pt;

    this->ScreenToClient( &spt );

    UINT test;
    HTREEITEM hti = this->HitTest(spt, &test);

    if (hti != NULL)
    {
		if (test & (TVHT_ONITEM | TVHT_ONITEMRIGHT))
        {
			BOOL			bShowMenu = TRUE;
			CString			cszLoad;
			CString			cszTemp;
			CString			cszBank;
			CString			cszOutlet;
			HTREEITEM		htCur = this->GetSelectedItem();

            /* Store the value of the right clicked item only *if*
             * it's different from the currently selected item.
             */
            if (hti != htCur)
            {
				m_hActiveItem = hti;
				this->SelectItem(m_hActiveItem);
            }

		/*	cszLoad.Empty();
			cszLoad = GetItemText(hti);

			cszTemp.Empty();
			cszTemp = GetItemText(hti);

			cszOutlet.Empty();
			cszOutlet.LoadString(IDS_STR_OUTLET_NM_CHECK);

			cszBank.Empty();
			cszBank.LoadString(IDS_STR_BANK_NM_CHECK);

			if (cszOutlet.GetLength() <= cszLoad.GetLength())
			{
				cszLoad = cszLoad.Left(cszOutlet.GetLength());
			}

			if (cszBank.GetLength() <= cszTemp.GetLength())
			{
				cszTemp = cszTemp.Left(cszBank.GetLength());
			}

			if ((cszOutlet.CompareNoCase(cszLoad)) && (cszBank.CompareNoCase(cszTemp)))
			{
				bShowMenu = FALSE;
			}*/

			if (bShowMenu)
			{
				/* Do the normal context menu stuff */
				OnContextMenu(this, pt);
			}
		}
	}

	PassTreeEvent(TREE_ITEM_RDCLICK);

	*pResult = 0;
}

void CMyTreeCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	PassTreeEvent(TREE_ITEM_LCLICK);

	*pResult = 0;
}

void CMyTreeCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here

	if (PassTreeEvent(TREE_ITEM_LDCLICK))
		*pResult = -1;
	else
		*pResult = 0;
}

void CMyTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	PassTreeEvent(TREE_ITEM_ITEMCHANGED);

	*pResult = 0;
}

void CMyTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint ptMousePos)
{
	// TODO: Add your message handler code here
	CMenu			menu;
	CMenu			*pop;

	menu.LoadMenu(IDR_MENU_OUTLET_LED);
    pop = menu.GetSubMenu(0);

	if (ptMousePos.x == -1 )
	{
		HTREEITEM	ht = this->GetSelectedItem();
		RECT		rect;

		this->GetItemRect(ht, &rect, true);
		this->ClientToScreen(&rect);
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
			HTREEITEM hOldSel = this->GetSelectedItem();

			this->Select(m_hActiveItem, TVGN_CARET);

			/* Execute the selected menu command */
			pMenuParent->SendMessage(WM_COMMAND, uCmd, 0);

			this->Select(hOldSel, TVGN_CARET);
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
}

BOOL CMyTreeCtrl::PassTreeEvent(UINT nTreeEvent)
{
	BOOL			bResult = FALSE;
	CPDUAppApp		*pApp = (CPDUAppApp *) AfxGetApp();

	if ((pApp) && (pApp->pLeftView))
	{
		bResult = pApp->pLeftView->OnTreeItemSelection(nTreeEvent);
	}

	return bResult;
}
