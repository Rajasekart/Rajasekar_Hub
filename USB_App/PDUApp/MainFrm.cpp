// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PDUApp.h"
#include "General.h"

#include "MainFrm.h"
#include "PDUAppView.h"
#include "DlgRight.h"
#include "DlgBtm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bSplitter = FALSE;
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#if 0

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

#endif

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	// create splitter window
	if (!m_wndMainSplitter.CreateStatic(this, 2, 1))
		return FALSE;

	// create sub splitter window
	if (!m_wndSubSplitter.CreateStatic(&m_wndMainSplitter, 1, 2, WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndMainSplitter.IdFromRowCol(0, 0)))
	{
		TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}

	CRect	cr;

	GetWindowRect(&cr); 

	if (!m_wndSubSplitter.CreateView(0, 0, RUNTIME_CLASS(CPDUAppView), CSize((cr.Width() * 3)/4, (cr.Height() * 3)/4), pContext) ||
		!m_wndSubSplitter.CreateView(0, 1, RUNTIME_CLASS(CDlgRight), CSize(cr.Width()/4, (cr.Height() * 3)/4), pContext))
	{
		m_wndSubSplitter.DestroyWindow();
		m_wndMainSplitter.DestroyWindow();
		return FALSE;
	}

	if (!m_wndMainSplitter.CreateView(1, 0, RUNTIME_CLASS(CDlgBtm), CSize(cr.Width(), cr.Height()/4), pContext))
	{
		m_wndSubSplitter.DestroyWindow();
		m_wndMainSplitter.DestroyWindow();
		return FALSE;
	}

	m_bSplitter = TRUE;

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

#if 1

	if ((m_bSplitter) && (nType != SIZE_MINIMIZED))
	{
		CRect		cr;

		GetWindowRect(&cr);

		m_wndMainSplitter.SetRowInfo(0, (int)((cr.Height() * 3)/4), 50);
		m_wndMainSplitter.SetRowInfo(1, (int)(cr.Height()/4), 50);

		m_wndSubSplitter.SetColumnInfo(0, (int)((cr.Width() * 3)/4), 50);
		m_wndSubSplitter.SetColumnInfo(1, (int)(cr.Width()/4), 50);

		m_wndSubSplitter.RecalcLayout();
		m_wndMainSplitter.RecalcLayout();

	}

#endif
}
