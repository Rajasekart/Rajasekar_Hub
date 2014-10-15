// @doc TreeListView_h
//
// @module TreeListView.h -  Declaration of <c CTreeListView> |
// 
// <nl>
// Copyright (C) 1999 <nl>
// Gerolf Kühnel
//
// @head3 Elements defined in this Module | 
// @index class mstuct menum enum struct | TreeListView_h

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//************************************ defines ************************************************************

// @const UINT | TVS_NOHSCROLL | TreeViewStyle, that switches off the Horizontal Scroll bar.
// New Style, that is not defined in Headers, that I could get (ComCtl32.dll version 5.8!)
#ifdef TVS_NOHSCROLL
	#undef TVS_NOHSCROLL
#endif

#define TVS_NOHSCROLL 0x0100000
// @const UINT | TVMYN_INSERTITEM | Message, that an Item was inserted
#define TVMYN_INSERTITEM WM_USER+1
// @func void | UNUSED_PARAM | Makro used to avoid warning for not used parameters of a function
//
// @parm   | a | unused parameter
#define UNUSED_PARAM(a) (void)a

//************************************ struct _HeaderData *************************************************

//@struct Structure for nessassary data wich is to be got from the header for drawing
struct _HeaderData
{
	long nWidth;		//@field width of the column
	RECT rcDefault;		//@field Rectangle of the ColumnHeaderItem (used for left and right value of the item's column)
	UINT nAlingment;	//@field alignment of the column
};
typedef struct _HeaderData HeaderData;

//************************************ CMyTreeObj *********************************************************

// @class A class that holds the texts of the columns for each item.
// I use this class, so I can use a CMap to get the columntexts to a HTREEITEM, because
// CMap wants to have Objects in it that have got an operator=.
// @base public | CObject
class CMyTreeObj : public CObject
{
// public mebers
public:
	// @cmember dynamic array that holds the columntextes for one object
	CStringArray m_strColumns;
	// @cmember operator= to copy the StringArray from one object to an other
	CMyTreeObj& operator=(CMyTreeObj& obj);
};

//************************************ CMyTreeCtrl ********************************************************

// @class The used treecontrol. The standard MFC class is overwritten to get a notification if
// an item is inserted to the treecontrol
// @base public | CTreeCtrl
class CMyTreeCtrl : public CTreeCtrl
{
// protected mebers
protected:

//@group Messagehandler for the TreeCtrl to be used
	// @cmember overwritten, to enable notifying the parent above new items
	afx_msg LRESULT OnInsertItem(WPARAM wParam, LPARAM lParam);
	// @cmember overwritten, to enable doubleclick beside the objects
	afx_msg LRESULT OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	// @cmember overwritten, to enable selektion with click beside the objects
	afx_msg LRESULT OnLButtonClk(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	HTREEITEM		m_hActiveItem;
public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMRdblclk(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL PassTreeEvent(UINT nTreeEvent);
};

//************************************ CTreeListView *******************************************************

// @class A class that behaves like a CTreeView. But the difference is, that it enables you to use
// coplumns like in a ListView.
// @base public | CView
class CTreeListView : public CView
{
	// MFC's standard stuff
	DECLARE_DYNCREATE(CTreeListView)

// protected attributes
protected:
// @group protected attributes
	// @cmember List of columntextes, which can be reached with the corresponding treeitem handle
	CMap<HTREEITEM, HTREEITEM, CMyTreeObj, CMyTreeObj&> m_Entries;
	// @cmember contained treecontrol
	CMyTreeCtrl		m_ctrlTree;
	// @cmember contained headercontrol
	CHeaderCtrl		m_ctrlHeader;
	// @cmember structure with data of the columns. used that we needn't get that values all times at drawing
	HeaderData*		m_vsCol;
	// @cmember backgroundcolor, not really used but inserted for drawing (set to default)
	COLORREF		m_colBackColor;
	// @cmember color of the text of a selected element
	COLORREF		m_colHilightText;
	// @cmember textcolor, not really used but inserted for drawing (set to default)
	COLORREF		m_colText;
	// @cmember brush to fill the background of a selected element
	CBrush			m_BackBrush;
	// a proposal from codeguru (anonymus)
	CBrush			m_BackBrushNormal;
	// @cmember should the header be displayed?
	bool			m_bDescriptionDisplay;
	// @cmember Scrollingposition of the view
	long			m_nOffset;
	// @cmember width of the header, safed for scrolling
	long			m_nHeaderWidth;
	// @cmember -number of columns used
	long			m_nNrColumns;

// @group constructors and destructors
	// @cmember protected constructor used by dynamic creation
	CTreeListView();
	// @cmember destructor to delet the structurs and arrays
	virtual ~CTreeListView();

// @group protected functions (helperfunctions for internal use)
	// @cmember calculates the rectangles for the childcontrols
	void CalculateClientRects(CRect& rcBase, CRect& rcTree, CRect& rcHeader);
	// @cmember resizes all childobjects of the view
	void SizeObjects(void);
	// @cmember checks, if the horizontal scrollbar is necessary and,hides / shows it
	bool ResetScrollbar(void);
	// @cmember draws the text of the treeitems (all columns!!)
	void DrawTreeItem(bool bFocus, CRect rcItem, HDC hdc, HTREEITEM hItem);
	// @cmember updates the m_vsCol variable to the actual value
	void UpdateColStruct(void);
	// @cmember set's up the drawingcontext and triggers the itemds textdrawing
	BOOL OnCustomdrawTree(LPNMHDR pNmhdr, LRESULT* pResult);
	// @cmember triggers the update of the m_vsCol - variable (user resized the Columns)
	BOOL OnEndTrack(LPNMHDR pNmhdr, LRESULT* pResult);
	// @cmember deletes all columndata for this object
	BOOL OnDeleteItem(LPNMHDR pNmhdr, LRESULT* pResult);

// @group protected MFC messagemap and message handlers
	//{{AFX_MSG(CTreeListView)
	// @cmember creates the childcontrols
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	// @cmember resizes the childcontrols
	afx_msg void OnSize(UINT nType, int cx, int cy);
	// @cmember scrolls the tree and the header
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	// @cmember indicates, that an item is inserted in the tree
	afx_msg LRESULT OnInsertItem(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

// @group Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListView)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// public Operations, to be used from the client
public:
// @group public Operations, used from the client
	// @cmember Inserts a new column in the TreeListView
	int InsertColumn( int nCol, const LVCOLUMN* pColumn );
	// @cmember Inserts a new column in the TreeListView
	int InsertColumn( int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 );
	// @cmember Call this function to delete a column from the TreeListView control
	BOOL DeleteColumn( int nCol );
	// @cmember Changes the text of a list view subitem
	BOOL SetSubItemText( HTREEITEM hItem, int nSubItem, CString strBuffer);
	// @cmember Retrieves the text of a list view subitem
	BOOL GetSubItemText( HTREEITEM hItem, int nSubItem, CString& strBuffer);
	// @cmember Retrieves the handle of the Header control used
	CImageList* GetHeaderImageList();
	// @cmember Assigns an image list to the Header control
	CImageList* SetHeaderImageList( CImageList * pImageList);
	// @cmember Returns a reference to the tree control associated with the view
	CTreeCtrl& GetTreeCtrl();
	// @cmember Shows or hides the Header control
	void ShowHeader(bool bShow);
};
