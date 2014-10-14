// ePower_MFGDlg.cpp : implementation file
//

#include "stdafx.h"
#include "General.h"
#include "ePower_MFG.h"
#include "ePower_MFGDlg.h"
#include "FileCSV.h"

#include <wtypes.h>
#include <initguid.h>

extern "C" {

#include "hidsdi.h"
#include <setupapi.h>
#include <dbt.h>
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// CePower_MFGDlg dialog

CePower_MFGDlg::CePower_MFGDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CePower_MFGDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_cszBaseUnitNms.RemoveAll();
	m_csz3PhInptConnNms.RemoveAll();
	m_csz1PhInptConnNms.RemoveAll();

	m_by3PhInpConnType.RemoveAll();
	m_by1PhInpConnType.RemoveAll();

	m_byTotalOutlets.RemoveAll();
	m_byTotalBanks.RemoveAll();
	m_byOutletsPerBank.RemoveAll();

	m_csz3PhSubUnitNms.RemoveAll();
	m_csz1PhSubUnitNms.RemoveAll();

	m_byMonitorOnly.RemoveAll();

	m_cszGenRandNum.Empty();
	m_nPosition = 0;
	m_nDivValue = 0;
	m_nDeviceType = -1;
	m_bGreen = FALSE;

	m_cszPidValues.RemoveAll();
	m_cszVidValues.RemoveAll();
	m_cszSplCharsList.RemoveAll();

	memset(&m_stPhaseInfo, 0, sizeof(PHASE_INFO));
}

void CePower_MFGDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_L_OUTLET_NM, m_cListCtrlLeft);
	DDX_Control(pDX, IDC_LIST_R_OUTLET_NM, m_cListCtrlRight);
	DDX_Control(pDX, IDC_EDIT_MODEL_NM, m_cEditModelNm);
	DDX_Control(pDX, IDC_EDIT_SER_NUM, m_cEditSerialNum);
	DDX_Control(pDX, IDC_EDIT_MANU_DATE, m_cEditManufacDate);
	DDX_Control(pDX, IDC_EDIT_HOST_NM, m_cEditHostNm);
	DDX_Control(pDX, IDC_EDIT_MAC_ADDR, m_cEditMACAddr);
	DDX_Control(pDX, IDC_EDIT_STAGGER_ON_DELAY, m_cEditStaggerOnDelay);
	DDX_Control(pDX, IDC_EDIT_STAGGER_OFF_DELAY, m_cEditStaggerOffDelay);
	DDX_Control(pDX, IDC_EDIT_FIRM_VERS, m_cEditFirmVers);
	DDX_Control(pDX, IDC_EDIT_EPOW_BASE_UNIT, m_cEditBaseUnit);
	DDX_Control(pDX, IDC_EDIT_EPOW_INP_CONN, m_cEditInputConn);
	DDX_Control(pDX, IDC_COMBO_MODEL_NUM, m_cComboModelNum);
	DDX_Control(pDX, IDC_COMBO_MODEL_SUB_NUM, m_cComboModelNumSub);
	DDX_Control(pDX, IDC_COMBO_MODEL_NUM_END, m_cComboModelNumEnd);
	DDX_Control(pDX, IDC_BUTTON_READ_PDU, m_cButReadPDU);
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_cComboDevice);
	DDX_Control(pDX, IDC_EDIT_SYSTEM_NM, m_cEditSysNm);
	DDX_Control(pDX, IDOK, m_cButWrite);
}

BEGIN_MESSAGE_MAP(CePower_MFGDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_L_OUTLET_NM, &CePower_MFGDlg::OnNMCustomdrawListLOutletNm)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_R_OUTLET_NM, &CePower_MFGDlg::OnNMCustomdrawListROutletNm)
	ON_CBN_SELCHANGE(IDC_COMBO_MODEL_NUM, &CePower_MFGDlg::OnCbnSelchangeModelNum)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CePower_MFGDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTN_GENERATE, &CePower_MFGDlg::OnBnClickedGenerate)
	ON_CBN_SELCHANGE(IDC_COMBO_MODEL_SUB_NUM, &CePower_MFGDlg::OnCbnSelchangeModelSubNum)
	ON_BN_CLICKED(IDC_BUTTON_READ_PDU, &CePower_MFGDlg::OnBnClickedReadPdu)
	ON_EN_CHANGE(IDC_EDIT_STAGGER_OFF_DELAY, &CePower_MFGDlg::OnEnChangeStaggerOffDelay)
	ON_EN_CHANGE(IDC_EDIT_STAGGER_ON_DELAY, &CePower_MFGDlg::OnEnChangeStaggerOnDelay)
	ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, &CePower_MFGDlg::OnCbnSelchangeDevice)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CePower_MFGDlg::OnBnClickedButtonLoad)
	ON_EN_CHANGE(IDC_EDIT_MODEL_NM, &CePower_MFGDlg::OnEnChangeEditModelNm)
	ON_EN_CHANGE(IDC_EDIT_SER_NUM, &CePower_MFGDlg::OnEnChangeEditSerNum)
	ON_EN_CHANGE(IDC_EDIT_MANU_DATE, &CePower_MFGDlg::OnEnChangeEditManuDate)
	ON_EN_CHANGE(IDC_EDIT_HOST_NM, &CePower_MFGDlg::OnEnChangeEditHostNm)
	ON_EN_CHANGE(IDC_EDIT_MAC_ADDR, &CePower_MFGDlg::OnEnChangeEditMacAddr)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CePower_MFGDlg::OnBnClickedBtnRefresh)
END_MESSAGE_MAP()


// CePower_MFGDlg message handlers

BOOL CePower_MFGDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR			szCurrPath[MAX_PATH] = {0};
	CString			cszLoad;
	CFileFind		cFinder;

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LoadSplChars();

	m_cszPDUAddr.Empty();

	m_cEditModelNm.SetLimitText(40);
	m_cEditSerialNum.SetLimitText(12);
	//m_cEditModelNum.SetLimitText(20);
	m_cEditManufacDate.SetLimitText(12);
	m_cEditHostNm.SetLimitText(15);
	m_cEditMACAddr.SetLimitText(17);
	m_cEditStaggerOnDelay.SetLimitText(4);
	m_cEditStaggerOffDelay.SetLimitText(4);
	m_cEditSysNm.SetLimitText(15);

	m_cszCurrPath.Empty();
	GetModuleFileName(NULL, szCurrPath, sizeof(szCurrPath));
	*(strrchr(szCurrPath, '\\') + 1) = '\0';
	m_cszCurrPath = szCurrPath;

	m_cszBasePath.Empty();
	m_cszBasePath = m_cszCurrPath;

	m_cszCurrPath += STR_LOG_FILE_NM;
	if (cFinder.FindFile(m_cszCurrPath))
	{
		cFinder.Close();
		::DeleteFile(m_cszCurrPath);
	}

	cszLoad.Empty();
	cszLoad = m_cszBasePath;
	cszLoad += STR_EPOWERHD_UNITS;
	if (!cFinder.FindFile(cszLoad))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_EPHD_UNIT_ERR_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return TRUE;
	}
	cFinder.Close();

	cszLoad.Empty();
	cszLoad.LoadString(AFX_IDS_APP_TITLE);
	SetWindowText(cszLoad);
	
#if 0
	if (!LoadVidPidValues())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_EPHD_UNIT_ERR_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return TRUE;
	}
#endif

	if (!LoadAllPhaseInfo())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_PHASE_INFO_ERR_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return TRUE;
	}

#if 0
	if (!PduEnumerateDevice(TRUE))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_PDU_NOT_FOUND_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return TRUE;
	}
#endif

	OnBnClickedBtnRefresh();

	// --------------------- SYSTEM INFO --------------------

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SYS_INFO_TITLE);
	SetDlgItemText(IDC_FRAME_SYSINFO, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_MODEL_NM_TITLE);
	SetDlgItemText(IDC_STATIC_MODEL_NM, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SER_NUM_TITLE);
	SetDlgItemText(IDC_STATIC_SER_NO, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_MODEL_NUM_TITLE);
	SetDlgItemText(IDC_STATIC_MODEL_NUM, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_MANUFAC_DATE_TITLE);
	SetDlgItemText(IDC_STATIC_MANUFAC_DATE, cszLoad);

	// --------------------- NETWORK INFO --------------------

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_NETWORK_INFO_TITLE);
	SetDlgItemText(IDC_FRAME_NETWORK_INFO, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_HOST_NM_TITLE);
	SetDlgItemText(IDC_STATIC_HOST_NM, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_MAC_ADDR_TITLE);
	SetDlgItemText(IDC_STATIC_MAC_ADDR, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_GENERATE_TITLE);
	SetDlgItemText(IDC_BUTN_GENERATE, cszLoad);

	// --------------------- GENERAL INFO -----------------

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_GENERAL_INFO_TITLE);
	SetDlgItemText(IDC_FRAME_GEN_INFO, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_STAGGER_ON_DELAY_TITLE);
	SetDlgItemText(IDC_STATIC_STAGGER_ON_DELAY, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SECOND_TITLE);
	SetDlgItemText(IDC_STATIC_SECS1, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_STAGGER_OFF_DELAY_TITLE);
	SetDlgItemText(IDC_STATIC_STAGGER_OFF_DELAY, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SECOND_TITLE);
	SetDlgItemText(IDC_STATIC_SECS2, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SAV_SET_DEFAULT_TITLE);
	SetDlgItemText(IDC_BUTTON_LOAD, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_READ_PDU_TITLE);
	SetDlgItemText(IDC_BUTTON_READ_PDU, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_WRITE_TITLE);
	SetDlgItemText(IDOK, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_CANCEL_TITLE);
	SetDlgItemText(IDCANCEL, cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_OUTLET_NM);
	m_cListCtrlLeft.InsertColumn(COL_OUTLET_CAP, cszLoad, LVCFMT_LEFT, 62);
	m_cListCtrlRight.InsertColumn(COL_OUTLET_CAP, cszLoad, LVCFMT_LEFT, 62);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_OUTLET_NM_TITLE);
	m_cListCtrlLeft.InsertColumn(COL_OUTLET_NAM, cszLoad, LVCFMT_LEFT, 126);
	m_cListCtrlRight.InsertColumn(COL_OUTLET_NAM, cszLoad, LVCFMT_LEFT, 126);

	m_cListCtrlLeft.InsertColumn(COL_OUTLET_VOL, _T("Voltage(V)"), LVCFMT_LEFT, 66);
	m_cListCtrlRight.InsertColumn(COL_OUTLET_VOL, _T("Voltage(V)"), LVCFMT_LEFT, 66);

	m_cListCtrlLeft.InsertColumn(COL_OUTLET_FRQ, _T("Frequency(Hz)"), LVCFMT_LEFT, 89);
	m_cListCtrlRight.InsertColumn(COL_OUTLET_FRQ, _T("Frequency(Hz)"), LVCFMT_LEFT, 89);

	m_cListCtrlLeft.SetReadOnlyColumns(0);
	m_cListCtrlLeft.SetValidEditCtrlCharacters(cszLoad);
	m_cListCtrlLeft.EnableVScroll(); 

	m_cListCtrlLeft.SetExtendedStyle(m_cListCtrlLeft.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_cListCtrlRight.SetReadOnlyColumns(0);
	m_cListCtrlRight.SetValidEditCtrlCharacters(cszLoad);
	m_cListCtrlRight.EnableVScroll(); 

	m_cListCtrlRight.SetExtendedStyle(m_cListCtrlLeft.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_EPOWER);
	m_cComboDevice.AddString(cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_PM9S9);
	m_cComboDevice.AddString(cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_PM10);
	m_cComboDevice.AddString(cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_EPOWER_MAN_CT);
	m_cComboDevice.AddString(cszLoad);

	if (!ReadCSVDeviceType())
	{
		// DEFAULT SELECT EPOWER DEVICE
		m_cComboDevice.SetCurSel(DEVICE_EPOWER);
		OnCbnSelchangeDevice();
	}

	ReadFirmwareVer();

	m_cButReadPDU.EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CePower_MFGDlg::LoadSplChars()
{
	m_cszSplCharsList.RemoveAll();
	m_cszSplCharsList.Add(SP_SLASH);
	m_cszSplCharsList.Add(SP_BACK_SLASH);
	m_cszSplCharsList.Add(SP_STAR);
	m_cszSplCharsList.Add(SP_QUES);
	m_cszSplCharsList.Add(SP_DB_CODES);
	m_cszSplCharsList.Add(SP_SYM_LESS);
	m_cszSplCharsList.Add(SP_SYM_GREAT);
	m_cszSplCharsList.Add(SP_SYM_OR);
	m_cszSplCharsList.Add(SP_SIN_CODES);
	m_cszSplCharsList.Add(SP_SYM_PLUS);
	m_cszSplCharsList.Add(SP_SYM_APS);
	m_cszSplCharsList.Add(SP_SYM_NEG);
	m_cszSplCharsList.Add(SP_SYM_EXC);
	m_cszSplCharsList.Add(SP_SYM_AT);
	m_cszSplCharsList.Add(SP_SYM_HAS);
	m_cszSplCharsList.Add(SP_SYM_DLR);
	m_cszSplCharsList.Add(SP_SYM_PERC);
	m_cszSplCharsList.Add(SP_SYM_UPA);
	m_cszSplCharsList.Add(SP_SYM_AND);
	m_cszSplCharsList.Add(SP_SYM_DOT);
	m_cszSplCharsList.Add(SP_SYM_COMMA);
	m_cszSplCharsList.Add(SP_SYM_EQUAL);
	m_cszSplCharsList.Add(SP_EBRACE);
	m_cszSplCharsList.Add(SP_SBRACE);
	m_cszSplCharsList.Add(SP_SET_SBRC);
	m_cszSplCharsList.Add(SP_SET_EBRC);
	m_cszSplCharsList.Add(SP_SYM_SCOLON);
}

BOOL CePower_MFGDlg::LoadAllPhaseInfo()
{
	UINT			nNum = 0;
	UINT			nTotal = 0;
	CString			cszLoad;
	CString			cszSection;
	CString			cszKey;
	CString			cszEpowerFile;
	TCHAR			szBuffer[MAX_PATH] = {0};

	memset(&m_stPhaseInfo, 0, sizeof(PHASE_INFO));

	cszEpowerFile.Empty();
	cszEpowerFile = m_cszBasePath;
	cszEpowerFile += STR_EPOWERHD_UNITS;

	cszSection.Empty();
	cszSection = STR_DEVICE_DETAILS;

	// !!!!!!!!!!!!!!!!!!!!!!!!! GET PRODUCT GROUP NAMES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	cszKey.Empty();
	cszKey = STR_DEV_EPOWER_MAN_CT;	// FOR ePowerCT version
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chePowerManCT, szBuffer);

	cszKey.Empty();
	cszKey = STR_DEV_EPOWERHD_NM;	// FOR ePowerHD version
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chePowerHDNm, szBuffer);

	cszKey.Empty();
	cszKey = STR_DEV_PM10_NM;	// For PM10 version
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chPM10Nm, szBuffer);

	cszKey.Empty();
	cszKey = STR_DEV_PM10U_NM;	// For PM10U version
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chPM10UNm, szBuffer);

	cszKey.Empty();
	cszKey = STR_DEV_PM9S9_NM;	// For PM9S9 version
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chPM9S9Nm, szBuffer);

	// !!!!!!!!!!!!!!!!!!!!!!!!! GET BANK - 3PHASE ID - DELTA !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	cszKey.Empty();
	cszKey = STR_DELTA_PHS3_ID_1;	// FOR ID - AB
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chDelta3PhsID_AB, szBuffer);

	cszKey.Empty();
	cszKey = STR_DELTA_PHS3_ID_2;	// FOR ID - BC
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chDelta3PhsID_BC, szBuffer);

	cszKey.Empty();
	cszKey = STR_DELTA_PHS3_ID_3;	// FOR ID - CA
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chDelta3PhsID_CA, szBuffer);

	// !!!!!!!!!!!!!!!!!!!!!!!!! GET BANK - 3PHASE ID - WYE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	cszKey.Empty();
	cszKey = STR_WYE_PHS3_ID_1;	// FOR ID - AN
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chWye3PhsID_AN, szBuffer);

	cszKey.Empty();
	cszKey = STR_WYE_PHS3_ID_2;	// FOR ID - BN
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chWye3PhsID_BN, szBuffer);

	cszKey.Empty();
	cszKey = STR_WYE_PHS3_ID_3;	// FOR ID - CN
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.chWye3PhsID_CN, szBuffer);

	// !!!!!!!!!!!!!!!!!!!!!!!!! GET BANK - 1PHASE ID !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	cszKey.Empty();
	cszKey = STR_PHS1_ID_1;	// FOR ID - AN
	memset(szBuffer, 0, sizeof(szBuffer));
	GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
	cszLoad.Empty();
	cszLoad = szBuffer;
	if ((cszLoad.IsEmpty()) || (!cszLoad.CompareNoCase(STR_FAIL)))
	{
		return FALSE;
	}
	lstrcpy(m_stPhaseInfo.ch1PhsID_AN, szBuffer);

	return TRUE;
}

void CePower_MFGDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CePower_MFGDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CePower_MFGDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CePower_MFGDlg::GenerateOutletNamesInfo(UINT nTotal, BYTE byAccessFlag)
{
	BOOL			bPM10U = FALSE;
	UINT			nNum = 0;
	UINT			nCell = 0;
	UINT			nCount = 0;
	UINT			nOddVal = 0;
	UINT			nNumRowsChk = 0;
	CString			cszTemp;
	CString			cszLoad;
	LVITEM			lvItem = {0};
	UINT			nTotalOutlets = nTotal;
	LVCOLUMN		col;

	m_cListCtrlLeft.DeleteAllItems();
	m_cListCtrlLeft.m_cszOrgOutletNm.RemoveAll();

	m_cListCtrlLeft.m_byInpVolFreqEdit.RemoveAll();
	m_cListCtrlLeft.m_cszOrgOutletVolt.RemoveAll();
	m_cListCtrlLeft.m_cszOrgOutletFreq.RemoveAll();

	m_cListCtrlRight.DeleteAllItems();
	m_cListCtrlRight.m_cszOrgOutletNm.RemoveAll();

	m_cListCtrlRight.m_byInpVolFreqEdit.RemoveAll();
	m_cListCtrlRight.m_cszOrgOutletVolt.RemoveAll();
	m_cListCtrlRight.m_cszOrgOutletFreq.RemoveAll();

	memset(&lvItem, 0, sizeof(LVITEM));
	lvItem.mask = LVIF_TEXT;

	nNumRowsChk = MAX_OUTLET_NUM;
	if (nTotalOutlets > OUTLET_24)
	{
		nNumRowsChk = nTotalOutlets / 2;
	}

	cszLoad.Empty();
	m_cComboModelNum.GetLBText(m_cComboModelNum.GetCurSel(), cszLoad);

	if ((m_nDeviceType == DEVICE_PM10) && (!cszLoad.CompareNoCase(STR_PM10_U_VERS)))
	{
		bPM10U = TRUE;
		nNumRowsChk = MAX_OUTLET_NUM;
		nTotalOutlets = PM10U_MAX_OUTLETS;
		
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_CIRCUIT_TIT);

		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_CIRCUIT_NM_TIT);
	}
	else
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_OUTLET_NM);

		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_OUTLET_NM_TITLE);
	}

	col.mask = LVCF_TEXT;
	col.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
	cszLoad.ReleaseBuffer();
	m_cListCtrlLeft.SetColumn(0, &col);
	m_cListCtrlRight.SetColumn(0, &col);

	col.pszText = cszTemp.GetBuffer(cszTemp.GetLength());
	cszTemp.ReleaseBuffer();
	m_cListCtrlLeft.SetColumn(1, &col);
	m_cListCtrlRight.SetColumn(1, &col);

	nCell = 0;
	nCount = 0;
	for (nNum = 0; nNum < nTotalOutlets; nNum++)
	{
		if (nNum == nNumRowsChk)
		{
			nCell = 0;
		}

		cszTemp.Empty();
		if (bPM10U)
		{
			cszTemp.LoadString(IDS_STR_CIRCUIT_NM);
		}
		else
		{
			cszTemp.LoadString(IDS_STR_OUTLET_NO);
		}
		cszLoad.Format(cszTemp, nNum + 1);

		lvItem.iItem = nCell;
		lvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
		cszLoad.ReleaseBuffer();
		lvItem.iSubItem = 0;

		if (nNum < nNumRowsChk)
		{
			m_cListCtrlLeft.InsertItem(&lvItem);

			m_cListCtrlLeft.SetItemText(nCell, OUTLET_NM_ID, cszLoad.GetBuffer(cszLoad.GetLength()));
			cszLoad.ReleaseBuffer();

			m_cListCtrlLeft.m_cszOrgOutletNm.Add(cszLoad);

			cszTemp.Empty();
			cszLoad.Empty();
			if (byAccessFlag == AF_FULL_CONTROL)
			{
				cszLoad = STR_NOT_APP;
				cszTemp = STR_NOT_APP;
				m_cListCtrlLeft.m_byInpVolFreqEdit.Add(0);
			}
			else if (byAccessFlag == AF_MONITOR_FULL)
			{
				nOddVal = 0;
				nOddVal = nCount / 4;
				nOddVal = nOddVal % 2;

				if (m_nDeviceType == DEVICE_EPOWER_CT)
				{
					nOddVal = 1;
					if (nNum == (nTotalOutlets - 1))
						nOddVal = 0;
				}

				if (!nOddVal)
				{
					cszLoad = STR_NOT_APP;
					cszTemp = STR_NOT_APP;
					m_cListCtrlLeft.m_byInpVolFreqEdit.Add(0);
				}
				else
				{
					cszLoad = _T("00.00");
					cszTemp = _T("60");
					m_cListCtrlLeft.m_byInpVolFreqEdit.Add(1);
				}
			}
			else if (byAccessFlag == AF_MONITOR_ONLY)
			{
				cszLoad = _T("00.00");
				cszTemp = _T("60");
				m_cListCtrlLeft.m_byInpVolFreqEdit.Add(1);
			}

			m_cListCtrlLeft.SetItemText(nCell, COL_OUTLET_VOL, cszLoad.GetBuffer(cszLoad.GetLength()));
			cszLoad.ReleaseBuffer();
			
			m_cListCtrlLeft.SetItemText(nCell, COL_OUTLET_FRQ, cszTemp.GetBuffer(cszTemp.GetLength()));
			cszTemp.ReleaseBuffer();

			m_cListCtrlLeft.m_cszOrgOutletVolt.Add(cszLoad);
			m_cListCtrlLeft.m_cszOrgOutletFreq.Add(cszTemp);
		}
		else
		{
			m_cListCtrlRight.InsertItem(&lvItem);

			m_cListCtrlRight.SetItemText(nCell, OUTLET_NM_ID, cszLoad.GetBuffer(cszLoad.GetLength()));
			cszLoad.ReleaseBuffer();

			m_cListCtrlRight.m_cszOrgOutletNm.Add(cszLoad);

			cszTemp.Empty();
			cszLoad.Empty();
			if (byAccessFlag == AF_FULL_CONTROL)
			{
				cszLoad = STR_NOT_APP;
				cszTemp = STR_NOT_APP;
				m_cListCtrlRight.m_byInpVolFreqEdit.Add(0);
			}
			else if (byAccessFlag == AF_MONITOR_FULL)
			{
				nOddVal = 0;
				nOddVal = nCount / 4;
				nOddVal = nOddVal % 2;

				if (!nOddVal)
				{
					cszLoad = STR_NOT_APP;
					cszTemp = STR_NOT_APP;
					m_cListCtrlRight.m_byInpVolFreqEdit.Add(0);
				}
				else
				{
					cszLoad = _T("00.00");
					cszTemp = _T("60");
					m_cListCtrlRight.m_byInpVolFreqEdit.Add(1);
				}
			}
			else if (byAccessFlag == AF_MONITOR_ONLY)
			{
				cszLoad = _T("00.00");
				cszTemp = _T("60");
				m_cListCtrlRight.m_byInpVolFreqEdit.Add(1);
			}

			m_cListCtrlRight.SetItemText(nCell, COL_OUTLET_VOL, cszLoad.GetBuffer(cszLoad.GetLength()));
			cszLoad.ReleaseBuffer();

			m_cListCtrlRight.SetItemText(nCell, COL_OUTLET_FRQ, cszTemp.GetBuffer(cszTemp.GetLength()));
			cszTemp.ReleaseBuffer();

			m_cListCtrlRight.m_cszOrgOutletVolt.Add(cszLoad);
			m_cListCtrlRight.m_cszOrgOutletFreq.Add(cszTemp);
		}

		nCell++;
		nCount++;
	}
}

void CePower_MFGDlg::OnNMCustomdrawListLOutletNm(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lpLVCustomDraw->nmcd.dwDrawStage)
	{
		case CDDS_ITEMPREPAINT:
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
			{
				/*if (0 == ((lpLVCustomDraw->nmcd.dwItemSpec + lpLVCustomDraw->iSubItem) % 2))
				{
					lpLVCustomDraw->clrText = RGB(255,255,255); // wh0ite text
					lpLVCustomDraw->clrTextBk = RGB(185, 185, 185); // black background
				}
				else 
				{
					lpLVCustomDraw->clrText = CLR_DEFAULT;
					lpLVCustomDraw->clrTextBk = CLR_DEFAULT;
				}*/

				if (lpLVCustomDraw->iSubItem)
				{
					lpLVCustomDraw->clrText = CLR_DEFAULT;
					lpLVCustomDraw->clrTextBk = CLR_DEFAULT;
				}
				else
				{
					lpLVCustomDraw->clrText = RGB(0, 0, 0); // wh0ite text
					lpLVCustomDraw->clrTextBk = RGB(242, 241, 243); // black background
				}
			}
			break;

		default: break;    
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;	
}

void CePower_MFGDlg::OnNMCustomdrawListROutletNm(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	switch(lpLVCustomDraw->nmcd.dwDrawStage)
	{
		case CDDS_ITEMPREPAINT:
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
			{
				/*if (0 == ((lpLVCustomDraw->nmcd.dwItemSpec + lpLVCustomDraw->iSubItem) % 2))
				{
					lpLVCustomDraw->clrText = RGB(255,255,255); // wh0ite text
					lpLVCustomDraw->clrTextBk = RGB(185, 185, 185); // black background
				}
				else 
				{
					lpLVCustomDraw->clrText = CLR_DEFAULT;
					lpLVCustomDraw->clrTextBk = CLR_DEFAULT;
				}*/

				if (lpLVCustomDraw->iSubItem)
				{
					lpLVCustomDraw->clrText = CLR_DEFAULT;
					lpLVCustomDraw->clrTextBk = CLR_DEFAULT;
				}
				else
				{
					lpLVCustomDraw->clrText = RGB(0, 0, 0); // wh0ite text
					lpLVCustomDraw->clrTextBk = RGB(242, 241, 243); // black background
				}
			}
			break;

		default: break;    
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;	
}

void CePower_MFGDlg::GenerateModelNum()
{
	TCHAR			szBuffer[MAX_PATH] = {0};
	UINT			nNum = 0;
	UINT			nTotal = 0;
	CString			cszLoad;
	CString			cszKey;
	CString			cszSection;
	CString			cszEpowerFile;

	m_cszBaseUnitNms.RemoveAll();
	m_byTotalOutlets.RemoveAll();
	m_byTotalBanks.RemoveAll();
	m_byOutletsPerBank.RemoveAll();
	m_byMonitorOnly.RemoveAll();

	m_cComboModelNum.ResetContent();
	m_cComboModelNumEnd.ResetContent();

	cszEpowerFile.Empty();
	cszEpowerFile = m_cszBasePath;
	cszEpowerFile += STR_EPOWERHD_UNITS;

	// TOTAL 3PHASE UNITS
	cszSection.Empty();
	if (m_nDeviceType == DEVICE_EPOWER)
	{
		cszSection = STR_HEAD_EPOW_3PHS;
	}
	else if (m_nDeviceType == DEVICE_PM10)
	{
		cszSection = STR_HEAD_PM10_3PHS;
	}
	else if (m_nDeviceType == DEVICE_PM9S9)
	{
		cszSection = STR_HEAD_PM9S9_3PHS;
	}
	else if (m_nDeviceType == DEVICE_EPOWER_CT)
	{
		cszSection = STR_HEAD_EPOW_CT_3PH_UNITS;
	}

	nTotal = GetPrivateProfileInt(cszSection, STR_TOTAL, 0, cszEpowerFile);
	m_nPosition = nTotal;

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		cszKey.Empty();
		cszKey.Format(STR_UNIT_NUM, (nNum + 1));

		cszSection.Empty();
		if (m_nDeviceType == DEVICE_EPOWER)
		{
			cszSection = STR_HEAD_EPOW_3PHS;
		}
		else if (m_nDeviceType == DEVICE_PM10)
		{
			cszSection = STR_HEAD_PM10_3PHS;
		}
		else if (m_nDeviceType == DEVICE_PM9S9)
		{
			cszSection = STR_HEAD_PM9S9_3PHS;
		}
		else if (m_nDeviceType == DEVICE_EPOWER_CT)
		{
			cszSection = STR_HEAD_EPOW_CT_3PH_UNITS;
		}

		// 3PHASE UNIT ID
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cComboModelNum.AddString(cszLoad);

		cszSection.Empty();
		cszSection = cszLoad;

		// TOTAL OUTLETS
		m_byTotalOutlets.Add(GetPrivateProfileInt(cszSection, STR_TOT_OUTLETS, 0, cszEpowerFile));

		// TOTAL BANKS
		m_byTotalBanks.Add(GetPrivateProfileInt(cszSection, STR_TOT_BANKS, 0, cszEpowerFile));

		// OUTLETS PER BANK
		m_byOutletsPerBank.Add(GetPrivateProfileInt(cszSection, STR_OUTLETS_PER_BANK, 0, cszEpowerFile));

		// MONITOR ONLY FLAG
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, STR_MONITOR_ONLY, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			m_byMonitorOnly.Add(1);
		}
		else
		{
			m_byMonitorOnly.Add(0);
		}

		// FULL CONTROL FLAG
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, STR_FULL_CONTROL, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			m_byFullControl.Add(1);
		}
		else
		{
			m_byFullControl.Add(0);
		}

		// 3PHASE UNIT NAME
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, STR_UNIT_NM, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cszBaseUnitNms.Add(cszLoad);
	}

	// TOTAL 1PHASE UNITS
	cszSection.Empty();
	if (m_nDeviceType == DEVICE_EPOWER)
	{
		cszSection = STR_HEAD_EPOW_1PHS;
	}
	else if (m_nDeviceType == DEVICE_PM10)
	{
		cszSection = STR_HEAD_PM10_1PHS;
	}
	else if (m_nDeviceType == DEVICE_PM9S9)
	{
		cszSection = STR_HEAD_PM9S9_1PHS;
	}
	else if (m_nDeviceType == DEVICE_EPOWER_CT)
	{
		cszSection = STR_HEAD_EPOW_CT_1PH_UNITS;
	}

	nTotal = GetPrivateProfileInt(cszSection, STR_TOTAL, 0, cszEpowerFile);

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		cszKey.Empty();
		cszKey.Format(STR_UNIT_NUM, (nNum + 1));

		cszSection.Empty();
		if (m_nDeviceType == DEVICE_EPOWER)
		{
			cszSection = STR_HEAD_EPOW_1PHS;
		}
		else if (m_nDeviceType == DEVICE_PM10)
		{
			cszSection = STR_HEAD_PM10_1PHS;
		}
		else if (m_nDeviceType == DEVICE_PM9S9)
		{
			cszSection = STR_HEAD_PM9S9_1PHS;
		}
		else if (m_nDeviceType == DEVICE_EPOWER_CT)
		{
			cszSection = STR_HEAD_EPOW_CT_1PH_UNITS;
		}

		// 1PHASE UNIT ID
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cComboModelNum.AddString(cszLoad);

		cszSection.Empty();
		cszSection = cszLoad;

		// TOTAL OUTLETS
		m_byTotalOutlets.Add(GetPrivateProfileInt(cszSection, STR_TOT_OUTLETS, 0, cszEpowerFile));

		// TOTAL BANKS
		m_byTotalBanks.Add(GetPrivateProfileInt(cszSection, STR_TOT_BANKS, 0, cszEpowerFile));

		// OUTLETS PER BANK
		m_byOutletsPerBank.Add(GetPrivateProfileInt(cszSection, STR_OUTLETS_PER_BANK, 0, cszEpowerFile));

		// MONITOR ONLY FLAG
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, STR_MONITOR_ONLY, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			m_byMonitorOnly.Add(1);
		}
		else
		{
			m_byMonitorOnly.Add(0);
		}

		// FULL CONTROL FLAG
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, STR_FULL_CONTROL, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		if (!cszLoad.CompareNoCase(STR_YES))
		{
			m_byFullControl.Add(1);
		}
		else
		{
			m_byFullControl.Add(0);
		}

		// 1PHASE UNIT NAME
		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, STR_UNIT_NM, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cszBaseUnitNms.Add(cszLoad);
	}

	// ADD END PART OF MODEL NUMBER
	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_NONE);
	m_cComboModelNumEnd.AddString(cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_P);
	m_cComboModelNumEnd.AddString(cszLoad);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_S);
	m_cComboModelNumEnd.AddString(cszLoad);

	m_cComboModelNum.SetCurSel(0);
	OnCbnSelchangeModelNum();
	m_cComboModelNumEnd.SetCurSel(0);
}

void CePower_MFGDlg::GenerateSubModelNum()
{
	int				nMode = 0;
	UINT			nTotal = 0;
	UINT			nNum = 0;
	CString			cszLoad;

	m_cComboModelNumSub.ResetContent();

	nMode = m_cComboModelNum.GetCurSel();

	if (CB_ERR == nMode)
		return;

	if (nMode < m_nPosition)
	{
		nTotal = m_csz3PhSubUnitNms.GetSize();
	}
	else
	{
		nTotal = m_csz1PhSubUnitNms.GetSize();
	}

	for (nNum = 0; nNum < nTotal; nNum++)
	{
		cszLoad.Empty();
		if (nMode < m_nPosition)
		{
			cszLoad = m_csz3PhSubUnitNms.ElementAt(nNum);
		}
		else
		{
			cszLoad = m_csz1PhSubUnitNms.ElementAt(nNum);
		}
		m_cComboModelNumSub.AddString(cszLoad);
	}

	m_cComboModelNumSub.SetCurSel(0);
	OnCbnSelchangeModelSubNum();
}

void CePower_MFGDlg::LoadModelSubNum()
{
	TCHAR			szBuffer[MAX_PATH] = {0};
	UINT			nLoop = 0;
	UINT			nNum = 0;
	UINT			nTotal = 0;
	CString			cszLoad;
	CString			cszKey;
	CString			cszSection;
	CString			cszEpowerFile;

	m_csz3PhInptConnNms.RemoveAll();
	m_csz1PhInptConnNms.RemoveAll();

	m_csz3PhSubUnitNms.RemoveAll();
	m_csz1PhSubUnitNms.RemoveAll();

	m_by3PhInpConnType.RemoveAll();
	m_by1PhInpConnType.RemoveAll();

	cszEpowerFile.Empty();
	cszEpowerFile = m_cszBasePath;
	cszEpowerFile += STR_EPOWERHD_UNITS;

	for (nLoop = 0; nLoop < 2; nLoop++)
	{
		cszSection.Empty();
		if (!nLoop)
		{
			if (m_nDeviceType == DEVICE_EPOWER)
			{
				cszSection = STR_HEAD_3PHS_INP_CONN;
			}
			else if (m_nDeviceType == DEVICE_PM10)
			{
				cszSection = STR_HEAD_3PHS_INP_CONN_PM10;
			}
			else if (m_nDeviceType == DEVICE_PM9S9)
			{
				cszSection = STR_HEAD_3PHS_INP_CONN_PM9S9;
			}
			else if (m_nDeviceType == DEVICE_EPOWER_CT)
			{
				cszSection = STR_HEAD_EPOW_CT_3PH_INP_CONN;
			}
		}
		else
		{
			if (m_nDeviceType == DEVICE_EPOWER)
			{
				cszSection = STR_HEAD_1PHS_INP_CONN;
			}
			else if (m_nDeviceType == DEVICE_PM10)
			{
				cszSection = STR_HEAD_1PHS_INP_CONN_PM10;
			}
			else if (m_nDeviceType == DEVICE_PM9S9)
			{
				cszSection = STR_HEAD_1PHS_INP_CONN_PM9S9;
			}
			else if (m_nDeviceType == DEVICE_EPOWER_CT)
			{
				cszSection = STR_HEAD_EPOW_CT_1PH_INP_CONN;
			}
		}

		// TOTAL PHASE SUB UNITS
		nTotal = GetPrivateProfileInt(cszSection, STR_TOTAL, 0, cszEpowerFile);

		for (nNum = 0; nNum < nTotal; nNum++)
		{
			cszKey.Empty();
			cszKey.Format(STR_UNIT_NUM, (nNum + 1));

			// PHASE SUB UNIT ID
			memset(szBuffer, 0, sizeof(szBuffer));
			GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
			cszLoad.Empty();
			cszLoad = szBuffer;
			if (!nLoop)
			{
				m_csz3PhSubUnitNms.Add(cszLoad);
			}
			else
			{
				m_csz1PhSubUnitNms.Add(cszLoad);
			}

			// ePOWER UNIT TYPE
			cszKey.Empty();
			cszKey.Format(STR_UNIT_NUM_TYPE, (nNum + 1));
			GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
			if (!nLoop)
			{
				m_by3PhInpConnType.Add(szBuffer[0]);
			}
			else
			{
				m_by1PhInpConnType.Add(szBuffer[0]);
			}

			// PHASE SUB UNIT NAME
			memset(szBuffer, 0, sizeof(szBuffer));
			cszKey.Empty();
			cszKey.Format(STR_UNIT_NUM_NM, (nNum + 1));

			GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
			cszLoad.Empty();
			cszLoad = szBuffer;
			if (!nLoop)
			{
				m_csz3PhInptConnNms.Add(cszLoad);
			}
			else
			{
				m_csz1PhInptConnNms.Add(cszLoad);
			}
		}
	}
}

void CePower_MFGDlg::OnCbnSelchangeModelNum()
{
	// TODO: Add your control notification handler code here
	BYTE			byAccessFlag = 0;
	int				nCurrCell = 0;
	CString			cszLoad;

	nCurrCell = m_cComboModelNum.GetCurSel();

	if (nCurrCell == CB_ERR)
		return;

	GenerateSubModelNum();

	// BASE UNIT & INPUT CONNECTION TITLE
	if (nCurrCell < m_nPosition)
	{
		cszLoad.Empty();
		if (m_nDeviceType == DEVICE_EPOWER)
		{
			cszLoad.LoadString(IDS_STR_3PHS_TITLE);
		}
		else if (m_nDeviceType == DEVICE_PM10)
		{
			cszLoad.LoadString(IDS_STR_3PHS_TITLE_PM10);
		}
		else if (m_nDeviceType == DEVICE_PM9S9)
		{
			cszLoad.LoadString(IDS_STR_3PHS_TITLE_PM9S9);
		}
		else if (m_nDeviceType == DEVICE_EPOWER_CT)
		{
			cszLoad.LoadString(IDS_STR_3PHS_TITLE_EPOW_CT);
		}
		SetDlgItemText(IDC_STATIC_EPOW_BASE_UINTS, cszLoad);

		cszLoad.Empty();
		if (m_nDeviceType == DEVICE_EPOWER)
		{
			cszLoad.LoadString(IDS_STR_3PHS_INP_CONN_TITLE);
		}
		else if (m_nDeviceType == DEVICE_PM10)
		{
			cszLoad.LoadString(IDS_STR_3PHS_INP_CONN_TITLE_PM10);
		}
		else if (m_nDeviceType == DEVICE_PM9S9)
		{
			cszLoad.LoadString(IDS_STR_3PHS_INP_CONN_TITLE_PM9S9);
		}
		else if (m_nDeviceType == DEVICE_EPOWER_CT)
		{
			cszLoad.LoadString(IDS_STR_3PHS_INP_CONN_TITLE_EPOWCT);
		}
		SetDlgItemText(IDC_STATIC_EPOW_INP_CONN, cszLoad);
	}
	else
	{
		cszLoad.Empty();
		if (m_nDeviceType == DEVICE_EPOWER)
		{
			cszLoad.LoadString(IDS_STR_1PHS_TITLE);
		}
		else if (m_nDeviceType == DEVICE_PM10)
		{
			cszLoad.LoadString(IDS_STR_1PHS_TITLE_PM10);
		}
		else if (m_nDeviceType == DEVICE_PM9S9)
		{
			cszLoad.LoadString(IDS_STR_1PHS_TITLE_PM10);
		}
		else if (m_nDeviceType == DEVICE_EPOWER_CT)
		{
			cszLoad.LoadString(IDS_STR_1PHS_TITLE_EPOWE_CT);
		}
		SetDlgItemText(IDC_STATIC_EPOW_BASE_UINTS, cszLoad);

		cszLoad.Empty();
		if (m_nDeviceType == DEVICE_EPOWER)
		{
			cszLoad.LoadString(IDS_STR_1PHS_INP_CONN_TITLE);
		}
		else if (m_nDeviceType == DEVICE_PM10)
		{
			cszLoad.LoadString(IDS_STR_1PHS_INP_CONN_TITLE_PM10);
		}
		else if (m_nDeviceType == DEVICE_PM9S9)
		{
			cszLoad.LoadString(IDS_STR_1PHS_INP_CONN_TITLE_PM9S9);
		}
		else if (m_nDeviceType == DEVICE_EPOWER_CT)
		{
			cszLoad.LoadString(IDS_STR_1PHS_INP_CONN_TITLE_EPOWER_CT);
		}
		SetDlgItemText(IDC_STATIC_EPOW_INP_CONN, cszLoad);
	}

	// BASE UNIT STRING
	cszLoad.Empty();
	if (nCurrCell < m_cszBaseUnitNms.GetSize())
	{
		cszLoad = m_cszBaseUnitNms.ElementAt(nCurrCell);
		m_cEditBaseUnit.SetWindowText(cszLoad);
	}

	if (nCurrCell < m_byTotalOutlets.GetSize())
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%u"), m_byTotalOutlets.ElementAt(nCurrCell));
		SetDlgItemText(IDC_EDIT_TOT_OUTLETS, cszLoad);

		if ((m_byMonitorOnly.ElementAt(nCurrCell)) && (!m_byFullControl.ElementAt(nCurrCell)))
		{
			byAccessFlag = AF_MONITOR_ONLY;
		}
		else if ((m_byMonitorOnly.ElementAt(nCurrCell)) && (m_byFullControl.ElementAt(nCurrCell)))
		{
			byAccessFlag = AF_MONITOR_FULL;
		}
		else if (!(m_byMonitorOnly.ElementAt(nCurrCell)) && (m_byFullControl.ElementAt(nCurrCell)))
		{
			byAccessFlag = AF_FULL_CONTROL;
		}

		GenerateOutletNamesInfo(m_byTotalOutlets.ElementAt(nCurrCell), byAccessFlag);
	}

	if (nCurrCell < m_byTotalBanks.GetSize())
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%u"), m_byTotalBanks.ElementAt(nCurrCell));
		SetDlgItemText(IDC_EDIT_TOT_BANKS, cszLoad);
	}

	if (nCurrCell < m_byOutletsPerBank.GetSize())
	{
		cszLoad.Empty();
		cszLoad.Format(_T("%u"), m_byOutletsPerBank.ElementAt(nCurrCell));
		SetDlgItemText(IDC_EDIT_OUTLETS_PER_BANK, cszLoad);
	}
}

void CePower_MFGDlg::OnCbnSelchangeModelSubNum()
{
	// TODO: Add your control notification handler code here
	// INPUT CONNECTION STRING
	int				nMainCell = 0;
	int				nCurrCell = 0;
	CString			cszLoad;

	nCurrCell = m_cComboModelNumSub.GetCurSel();
	nMainCell = m_cComboModelNum.GetCurSel();

	if ((CB_ERR == nMainCell) || (CB_ERR == nCurrCell))
		return;

	cszLoad.Empty();
	if (nMainCell < m_nPosition)
	{
		if (nCurrCell < m_csz3PhInptConnNms.GetSize())
		{
			cszLoad = m_csz3PhInptConnNms.ElementAt(nCurrCell);
		}
	}
	else
	{
		if (nCurrCell < m_csz1PhInptConnNms.GetSize())
		{
			cszLoad = m_csz1PhInptConnNms.ElementAt(nCurrCell);
		}
	}

	m_cEditInputConn.SetWindowText(cszLoad);
}

HBRUSH CePower_MFGDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd && ((pWnd->GetDlgCtrlID() == IDC_EDIT_EPOW_BASE_UNIT) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_EPOW_INP_CONN) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_FIRM_VERS) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_TOT_OUTLETS) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_TOT_BANKS) || 
		(pWnd->GetDlgCtrlID() == IDC_EDIT_OUTLETS_PER_BANK)))
	{
		pDC->SetTextColor(RGB(0, 0, 255));
	}
	else if (pWnd && ((pWnd->GetDlgCtrlID() == IDC_STATIC_FIRM_VER) || 
			(pWnd->GetDlgCtrlID() == IDC_STATIC_TOT_OUTLETS_TIT) ||
			(pWnd->GetDlgCtrlID() == IDC_STATIC_TOT_BANKS_TIT) ||
			(pWnd->GetDlgCtrlID() == IDC_STATIC_OUTLETS_PER_BANK_TIT) ||
			(pWnd->GetDlgCtrlID() == IDC_STATIC_EPOW_BASE_UINTS) ||
			(pWnd->GetDlgCtrlID() == IDC_STATIC_EPOW_INP_CONN)))
	{
		pDC->SetTextColor(RGB(38, 72, 128));
	}

	if (pWnd)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_PDU_DETECT)
		{
			if (m_bGreen)
			{
				pDC->SetTextColor(RGB(0, 128, 128)); // GR
			}
			else
			{
				pDC->SetTextColor(RGB(128, 0, 0)); // RED
			}
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

BOOL CePower_MFGDlg::PduStaticInfoWriteAndRead(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum)
{
	BOOL									bRet = FALSE;
	CHAR									byLocalBuf[MAX_PATH] = {0};
	BYTE									InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};
	DWORD									dwByteWritten = 0;
	DWORD									NumberOfBytesRead = 0;
	CString				 					szTmpstring;
	BYTE									byCount = 0;
	CString									cszLoad;
	CString									cszTemp;

	if (bWrite)
	{
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Write Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.

		// ********** WRITE *****************
		InputPacketBuff[1] = PDU_STATIC_WRITE;
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byInfoSize;		// Time Stamp

		for (byCount = 0; byCount < byInfoSize; byCount++)
		{
			InputPacketBuff[4+byCount] =  *(byInfoStr + byCount);
		}
 
		PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, TRUE, byErrCode, bWrLog);
		bRet = TRUE;
	}
	else
	{
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Read Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.

		// ********** READ *****************
		InputPacketBuff[1] = PDU_STATIC_READ;		//for Read Static Information
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byOutletNum;

		PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bWrLog);
		if (byInfoID == InputPacketBuff[1])
		{
	/*		if ((byInfoID != PDU_MAC_ADDRS) && (byInfoID != PDU_STAGGER_ON_DELAY) && (byInfoID != PDU_STAGGER_OFF_DELAY))
			{*/
				/* -----------------------------------------------------------------------
					Copy the Buffer
				----------------------------------------------------------------------- */
				for (byCount = 0; byCount < InputPacketBuff[2]; byCount++)
				{
					*(byInfoStr + byCount) =  *(InputPacketBuff + (3 + byCount));
				}
		/*	}
			else if (byInfoID == PDU_MAC_ADDRS)
			{
				for (byCount = 0; byCount <= 8; byCount++)
				{
					*(byInfoStr + byCount) =  *(InputPacketBuff+(2+byCount));
				}
			}
			else if ((byInfoID == PDU_STAGGER_ON_DELAY) || (byInfoID == PDU_STAGGER_OFF_DELAY))
			{
				for (byCount = 0; byCount <= 6; byCount++)
				{
					*(byInfoStr + byCount) =  *(InputPacketBuff+(2+byCount));
				}
			}*/
		}
		else
		{
			//CONFIRMLOG(bySection, "Data", ("%s",  "BadData"), szLogPath, bLog);
		}
	}

	/*cszLoad.Empty();
	cszLoad.Format(_T("0X%02x%02x%02x%02x"), InputPacketBuff[4], InputPacketBuff[5], InputPacketBuff[6], InputPacketBuff[7]);
	CONFIRMLOG(bySection, "HexFrequency", ("%s", cszLoad), szLogPath, bLog);*/

	return bRet;
}

BOOL CePower_MFGDlg::PduSysNmWrite(TCHAR *byPduadd, BYTE byInfoID, BYTE *byInfoStr, BYTE byInfoSize, BOOL bWrite, BYTE *byErrCode, BOOL bWrLog, BYTE byOutletNum)
{
	BOOL									bRet = FALSE;
	CHAR									byLocalBuf[MAX_PATH] = {0};
	BYTE									InputPacketBuff[MAX_PDU_BUFF_SIZE] = {0};
	DWORD									dwByteWritten = 0;
	DWORD									NumberOfBytesRead = 0;
	CString				 					szTmpstring;
	BYTE									byCount = 0;
	CString									cszLoad;
	CString									cszTemp;

	if (bWrite)
	{
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Write Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.

		// ********** WRITE *****************
		InputPacketBuff[1] = PDU_CONFIG_WRITE;
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byInfoSize;		// Time Stamp

		for (byCount = 0; byCount < byInfoSize; byCount++)
		{
			InputPacketBuff[4+byCount] =  *(byInfoStr + byCount);
		}
 
		PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, TRUE, byErrCode, bWrLog);
		bRet = TRUE;
	}
	else
	{
		/* -----------------------------------------------------------------------
			Send the Pass through Command to Read Model Number
		----------------------------------------------------------------------- */
		dwByteWritten = 0;
		NumberOfBytesRead = 0;
		memset(InputPacketBuff, 0, sizeof(InputPacketBuff));

		InputPacketBuff[0] = ZERO_ID;				//The first byte is the "Report ID" and does not get transmitted over the USB bus.  Always set = 0.

		// ********** READ *****************
		InputPacketBuff[1] = PDU_CONFIG_READ;		//for Read Static Information
		InputPacketBuff[2] = byInfoID;
		InputPacketBuff[3] = byOutletNum;

		PduWriteReadBuffer(byPduadd, InputPacketBuff, &dwByteWritten, FALSE, byErrCode, bWrLog);
		if (byInfoID == InputPacketBuff[1])
		{
			/* -----------------------------------------------------------------------
				Copy the Buffer
			----------------------------------------------------------------------- */
			for (byCount = 0; byCount < InputPacketBuff[2]; byCount++)
			{
				*(byInfoStr + byCount) =  *(InputPacketBuff + (3 + byCount));
			}
		}
	}

	return bRet;
}

BOOL CePower_MFGDlg::PduWriteReadBuffer(TCHAR *byPduAdd, BYTE byDataBuffer[], DWORD *dwRWBytes, BOOL bWriteOnly, BYTE *byErrCode, BOOL bWrLog)
{
	BOOL			bRet = TRUE;
	CString			cszLoad;
	UINT			nNumPass = 0;
	BYTE			byBufferCpy[MAX_PDU_BUFF_SIZE] = {0};
	TCHAR			bySection[MAX_PATH] = {0};
	HANDLE			hMyPDUHandle = INVALID_HANDLE_VALUE;

	if (bWrLog)
	{
		WriteLogDetails(STR_INIT_WRITE_CMD, _T("Initiating the Write Command..."));
	}

	/* -----------------------------------------------------------------------
		Create the WriteHandle to Send the Command.
	----------------------------------------------------------------------- */
	hMyPDUHandle = CreateFile(byPduAdd, 
							 GENERIC_READ | GENERIC_WRITE, 
							 FILE_SHARE_READ | FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, 
							 OPEN_EXISTING, 
							 0, 
							 NULL);

	if (hMyPDUHandle == INVALID_HANDLE_VALUE)
	{
		if (bWrLog)
		{
			WriteLogDetails(STR_DEVICE_HANDLE, _T("Unable to create device handle..."));
		}
		*byErrCode = 0x02;

		bRet = FALSE;
		return bRet;
	}
	else
	{
		if (bWrLog)
		{
			WriteLogDetails(STR_DEVICE_HANDLE, _T("Device handle created successfully..."));
		}
	}

	nNumPass = 0;

	memcpy(byBufferCpy, byDataBuffer, MAX_PDU_BUFF_SIZE);

	do
	{
		if (hMyPDUHandle != INVALID_HANDLE_VALUE)
		{
			WriteFile(hMyPDUHandle, byBufferCpy, MAX_PDU_BUFF_SIZE, dwRWBytes, 0);
			
			if (bWrLog)
			{
				cszLoad.Empty();
				cszLoad.Format(_T("DataWritten %d bytes"), *dwRWBytes);
				WriteLogDetails(STR_DEV_DATA_WRTN_BYTES, cszLoad);
			}
		}

		if (bWriteOnly)
		{
			break;
		}
		
		*dwRWBytes = 0;
		memset(byDataBuffer, 0, MAX_PDU_BUFF_SIZE);
		
		if (hMyPDUHandle != INVALID_HANDLE_VALUE)
		{
			bRet = ReadFile(hMyPDUHandle, byDataBuffer, MAX_PDU_BUFF_SIZE, dwRWBytes, 0);

			if (bWrLog)
			{
				cszLoad.Empty();
				cszLoad.Format(_T("DataRead %d bytes"), *dwRWBytes);
				WriteLogDetails(STR_DEV_DATA_READ_BYTES, cszLoad);
			}
		}

		nNumPass++;

		if ((byDataBuffer[1] == 0xFF) && (byDataBuffer[2] == 0xEE) && (nNumPass >= 3))
			break;

	} while((byDataBuffer[1] == 0xFF) && (byDataBuffer[2] == 0xEE));

	if (bWrLog)
	{
		WriteLogDetails(STR_CLOSE_DEV_HAND, _T("Closing device handle..."));
	}

	if (hMyPDUHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hMyPDUHandle);
	}

	return bRet;

}

BOOL CePower_MFGDlg::LoadVidPidValues()
{
	BOOL			bFoundOk = TRUE;
	UINT			nNum = 0;
	UINT			nTotal = 0;
	CString			cszLoad;
	CString			cszSection;
	CString			cszKey;
	CString			cszEpowerFile;
	TCHAR			szBuffer[MAX_PATH] = {0};

	m_cszPidValues.RemoveAll();
	m_cszVidValues.RemoveAll();

	cszEpowerFile.Empty();
	cszEpowerFile = m_cszBasePath;
	cszEpowerFile += STR_EPOWERHD_UNITS;

	cszSection.Empty();
	cszSection = STR_DEVICE_DETAILS;

	nTotal = GetPrivateProfileInt(cszSection, STR_TOTAL, 0, cszEpowerFile);

	for (nNum = 1; nNum <= nTotal; nNum++)
	{
		// VID Values
		cszKey.Empty();
		cszKey.Format(STR_VID_NO, nNum);

		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cszVidValues.Add(cszLoad);

		// PID Values
		cszKey.Empty();
		cszKey.Format(STR_PID_NO, nNum);

		memset(szBuffer, 0, sizeof(szBuffer));
		GetPrivateProfileString(cszSection, cszKey, STR_FAIL, szBuffer, sizeof(szBuffer), cszEpowerFile);
		cszLoad.Empty();
		cszLoad = szBuffer;
		m_cszPidValues.Add(cszLoad);
	}

	if (!nTotal)
	{
		bFoundOk = FALSE;
	}

	return bFoundOk;
}

BOOL CePower_MFGDlg::PduEnumerateDevice(BOOL bWrLog)
{
	BOOL										bPduFound = FALSE;
	TCHAR										byDevPath[MAX_PATH] = {0};
	BOOL										bPidVidOk = FALSE;
	ULONG										Length = 0;
	UINT										nNum = 0;
	int											MemberIndex = 0;
	HIDD_ATTRIBUTES								Attributes;
	SP_DEVICE_INTERFACE_DATA					devInfoData;
	HANDLE										hDevInfo = INVALID_HANDLE_VALUE;
	LONG										Result = 0;	
	PSP_DEVICE_INTERFACE_DETAIL_DATA			detailData = NULL;
	GUID										HidGuid;
	ULONG										Required = 0;
	CString										cszLoad;
	CString										cszPid;
	CString										cszVid;
	HANDLE										DeviceHandle = INVALID_HANDLE_VALUE;

/* -----------------------------------------------------------------------
	API function: HidD_GetHidGuid	Get the GUID for all system HIDs. Returns: the GUID in HidGuid.
	----------------------------------------------------------------------- */

	HidD_GetHidGuid(&HidGuid);	
	
/* -----------------------------------------------------------------------
	API function: SetupDiGetClassDevs
	Returns: a handle to a device information set for all installed devices.
	Requires: the GUID returned by GetHidGuid.
	----------------------------------------------------------------------- */
	
	hDevInfo = SetupDiGetClassDevs(&HidGuid, 
									NULL, 
									NULL, 
									DIGCF_PRESENT|DIGCF_INTERFACEDEVICE);

	if (hDevInfo == INVALID_HANDLE_VALUE)
		return 0;

	devInfoData.cbSize = sizeof(devInfoData);

/* -----------------------------------------------------------------------
	API function: SetupDiEnumDeviceInterfaces
	On return, MyDeviceInterfaceData contains the handle to a
	SP_DEVICE_INTERFACE_DATA structure for a detected device.

	Requires:
	The DeviceInfoSet returned in SetupDiGetClassDevs.
	The HidGuid returned in GetHidGuid.
	An index to specify a device.
	----------------------------------------------------------------------- */

	if (bWrLog)
	{
		WriteLogDetails(STR_DEVICE_DETECT, _T("Detecting PDU device..."));
	}

	MemberIndex = 0;

	for(MemberIndex = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &HidGuid, MemberIndex, &devInfoData); ++MemberIndex)
	{						
		//A device has been detected, so get more information about it.

	/* -----------------------------------------------------------------------
		API function: SetupDiGetDeviceInterfaceDetail
		Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure
		containing information about a device.
		To retrieve the information, call this function twice.
		The first time returns the size of the structure in Length.
		The second time returns a pointer to the data in DeviceInfoSet.
		Requires:
		A DeviceInfoSet returned by SetupDiGetClassDevs
		The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
		
		The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
		This application doesn't retrieve or use the structure.			
		If retrieving the structure, set 
		MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
		and pass the structure's address.
		----------------------------------------------------------------------- */
		
		//Get the Length value.
		//The call will return with a "buffer too small" error which can be ignored.

		Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInfoData, NULL, 0, &Length, NULL);

		//Allocate memory for the hDevInfo structure, using the returned Length.

		detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(Length);
		
		//Set cbSize in the detailData structure.

		detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		//Call the function again, this time passing it the returned buffer size.

		Result = SetupDiGetDeviceInterfaceDetail(hDevInfo, 
												  &devInfoData, 
												  detailData, 
												  Length, 
												  &Required, 
												  NULL);

		// Open a handle to the device.
		// To enable retrieving information about a system mouse or keyboard,
		// don't request Read or Write access for this handle.

 	/* -----------------------------------------------------------------------
		API function: CreateFile
		Returns: a handle that enables reading and writing to the device.

		Requires:
		The DevicePath in the detailData structure
		returned by SetupDiGetDeviceInterfaceDetail.
		----------------------------------------------------------------------- */
		if (bWrLog)
		{
			WriteLogDetails(STR_DEVICE_PATH, detailData->DevicePath);
		}

		DeviceHandle = CreateFile(detailData->DevicePath, 
									0, 
									FILE_SHARE_READ|FILE_SHARE_WRITE, 
									(LPSECURITY_ATTRIBUTES)NULL,
									OPEN_EXISTING, 
									0, 
									NULL);

		if (DeviceHandle != INVALID_HANDLE_VALUE)
		{
			if (bWrLog)
			{
				WriteLogDetails(STR_DEVICE_HANDLE, _T("Device handle created successfully..."));
			}
		}
		else
		{
			if (bWrLog)
			{
				WriteLogDetails(STR_DEVICE_HANDLE, _T("Unable to create device handle..."));
			}
			continue;
		}

 	/* -----------------------------------------------------------------------
		API function: HidD_GetAttributes
		Requests information from the device.
		Requires: the handle returned by CreateFile.
		Returns: a HIDD_ATTRIBUTES structure containing
		the Vendor ID, Product ID, and Product Version Number.
		Use this information to decide if the detected device is
		the one we're looking for.
		----------------------------------------------------------------------- */

		//Set the Size to the number of bytes in the structure.

		Attributes.Size = sizeof(Attributes);

		Result = HidD_GetAttributes(DeviceHandle, &Attributes);

		if (bWrLog)
		{
			cszLoad.Empty();
			cszLoad.Format(_T("0x%04X"), Attributes.VendorID);
			WriteLogDetails(STR_DEVICE_VID, cszLoad);

			cszLoad.Empty();
			cszLoad.Format(_T("0x%04X"), Attributes.ProductID);
			WriteLogDetails(STR_DEVICE_PID, cszLoad);
		}

		bPidVidOk = FALSE;
		for (nNum = 0; nNum < (UINT)m_cszVidValues.GetSize(); nNum++)
		{
			cszPid.Empty();
			cszPid.Format(_T("0x%04X"), Attributes.ProductID);

			cszVid.Empty();
			cszVid.Format(_T("0x%04X"), Attributes.VendorID);

			if ((!m_cszVidValues.ElementAt(nNum).CompareNoCase(cszVid)) && (!m_cszPidValues.ElementAt(nNum).CompareNoCase(cszPid)))
			{
				bPidVidOk = TRUE;
			}
		}

		if (bPidVidOk) //((Attributes.VendorID == wVid) && (Attributes.ProductID == wPid))
		{
			//Both the Vendor ID and Product ID match.
			if (bWrLog)
			{
				WriteLogDetails(STR_PID_VID, _T("Found"));
			}

		/* -----------------------------------------------------------------------
			Device Path is needed to create the device handle.
			----------------------------------------------------------------------- */
			m_cszPDUAddr.Empty();
			m_cszPDUAddr = detailData->DevicePath;

			//memset(byDevPath, PDU_BUFF_NULL, sizeof(byDevPath));
			//lstrcpy(byDevPath, detailData->DevicePath);
			//GetDeviceCapabilities(byDevPath, bLog, DeviceHandle);

			if (bWrLog)
			{
				WriteLogDetails(STR_CLOSE_DEV_HAND, _T("Closing device handle..."));
			}

			CloseHandle(DeviceHandle);
			DeviceHandle = INVALID_HANDLE_VALUE;

			bPduFound = TRUE;

			break;		// Exit the loop after proper device detection...
	 	} 
		else
		{
			bPduFound = FALSE;
			//The Vendor ID doesn't match.

			if (bWrLog)
			{
				WriteLogDetails(STR_PID_VID, _T("Mismatch"));
				WriteLogDetails(STR_CLOSE_DEV_HAND, _T("Closing device handle..."));
			}

			CloseHandle(DeviceHandle);
			DeviceHandle = INVALID_HANDLE_VALUE;
		}

		//Free the memory used by the detailData structure (no longer needed).
		free(detailData);

	} //for loop

	if (DeviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(DeviceHandle);
		DeviceHandle = INVALID_HANDLE_VALUE;
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bPduFound;
}

BOOL CePower_MFGDlg::WriteLogDetails(CString cszSection, CString cszData)
{
	BOOL			bRet = TRUE;

	if ((!m_cszCurrPath.IsEmpty()) && (!cszSection.IsEmpty()) && (!cszData.IsEmpty()))
	{
		WritePrivateProfileString(STR_PDU_LOG, cszSection, cszData, m_cszCurrPath);
	}

	return bRet;
}

BOOL CePower_MFGDlg::ReadCSVDeviceType()
{
	int					nPos = 0;
	UINT				nNum = 0;
	UINT				nVal = 0;
	UINT				nTotal = 0;
	BOOL				bFileFound = FALSE;
	CString				cszCSVFilePath = m_cszBasePath;
	CString				cszLoad;
	CStringArray		cszDataArray;
	CFileFind			cFinder;

	cszCSVFilePath += STR_CSV_FILE_NM;

	if (cFinder.FindFile(cszCSVFilePath))
	{
		cFinder.Close();

		CFileCSV			cFileCSV(cszCSVFilePath, CFileCSV::modeRead);

		cFileCSV.ReadData(cszDataArray);
		cFileCSV.Close();

		bFileFound = TRUE;
	}

	if (bFileFound)
	{
		nNum = 0;

		// DEVICE TYPE
		cszLoad.Empty();
		cszLoad = cszDataArray.ElementAt(nNum++);
		nPos = atol((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
		cszLoad.ReleaseBuffer();
		m_cComboDevice.SetCurSel(nPos);
		OnCbnSelchangeDevice();

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Copy MODEL NAME
			m_cEditModelNm.SetWindowText(cszDataArray.ElementAt(nNum++));
		}

		#if 0
		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Copy SERIAL NUMBER
			m_cEditSerialNum.SetWindowText(cszDataArray.ElementAt(nNum++));
		}
		#else
		m_cEditSerialNum.SetWindowText(_T(""));
		#endif

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Select MODEL NUMBER
			cszLoad.Empty();
			cszLoad = cszDataArray.ElementAt(nNum++);

			nPos = atol((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
			cszLoad.ReleaseBuffer();

			if (nPos < m_cComboModelNum.GetCount())
			{
				m_cComboModelNum.SetCurSel(nPos);
				OnCbnSelchangeModelNum();
			}
		}

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Select SUB MODEL NUMBER
			cszLoad.Empty();
			cszLoad = cszDataArray.ElementAt(nNum++);

			nPos = atol((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
			cszLoad.ReleaseBuffer();

			if (nPos < m_cComboModelNumSub.GetCount())
			{
				m_cComboModelNumSub.SetCurSel(nPos);
				OnCbnSelchangeModelSubNum();
			}
		}

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Select END MODEL NUMBER
			cszLoad.Empty();
			cszLoad = cszDataArray.ElementAt(nNum++);

			nPos = atol((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
			cszLoad.ReleaseBuffer();

			if (nPos < m_cComboModelNumEnd.GetCount())
			{
				m_cComboModelNumEnd.SetCurSel(nPos);
			}
		}

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Copy MANUFACTURE DATE
			m_cEditManufacDate.SetWindowText(cszDataArray.ElementAt(nNum++));
		}

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Copy HOST NAME
			m_cEditHostNm.SetWindowText(cszDataArray.ElementAt(nNum++));
		}

		#if 0
		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Copy MAC ADDRESS
			cszGenRandNum.Empty();
			cszGenRandNum = cszDataArray.ElementAt(nNum++);
		}
		#else
		m_cEditMACAddr.SetWindowTextA(_T(""));
		#endif

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Copy STAGGER ON DELAY
			m_cEditStaggerOnDelay.SetWindowText(cszDataArray.ElementAt(nNum++));
		}

		if (nNum < (UINT)cszDataArray.GetSize())
		{
			// Copy STAGGER OFF DELAY
			m_cEditStaggerOffDelay.SetWindowText(cszDataArray.ElementAt(nNum++));
		}

		#if 0
		// Generate SERIAL NUMBER & MAC ADDRESS
		OnGenerateMACAddr();
		#endif

		nPos = m_cComboModelNum.GetCurSel();
		if (nPos < m_byTotalOutlets.GetSize())
		{
			UINT			nCell = 0;
			UINT			nRow = 0;
			UINT			nNumRowsChk = 0;
			LVITEM			lvItem = {0};
			CString			cszTemp;

			nTotal = m_byTotalOutlets.ElementAt(nPos);

			m_cListCtrlLeft.DeleteAllItems();
			m_cListCtrlLeft.m_cszOrgOutletNm.RemoveAll();

			m_cListCtrlLeft.m_byInpVolFreqEdit.RemoveAll();
			m_cListCtrlLeft.m_cszOrgOutletVolt.RemoveAll();
			m_cListCtrlLeft.m_cszOrgOutletFreq.RemoveAll();

			m_cListCtrlRight.DeleteAllItems();
			m_cListCtrlRight.m_cszOrgOutletNm.RemoveAll();

			m_cListCtrlRight.m_byInpVolFreqEdit.RemoveAll();
			m_cListCtrlRight.m_cszOrgOutletVolt.RemoveAll();
			m_cListCtrlRight.m_cszOrgOutletFreq.RemoveAll();

			nNumRowsChk = MAX_OUTLET_NUM;
			if (nTotal > OUTLET_24)
			{
				nNumRowsChk = nTotal / 2;
			}

			memset(&lvItem, 0, sizeof(LVITEM));
			lvItem.mask = LVIF_TEXT;

			for (nVal = 0; nVal < nTotal; nVal++)
			{
				if (nVal == nNumRowsChk)
				{
					nCell = 0;
				}

				cszTemp.Empty();
				cszTemp.LoadString(IDS_STR_OUTLET_NO);
				cszLoad.Format(cszTemp, nVal + 1);

				lvItem.iItem = nCell++;
				lvItem.pszText = cszLoad.GetBuffer(cszLoad.GetLength());
				cszLoad.ReleaseBuffer();
				lvItem.iSubItem = 0;

				cszTemp.Empty();
				cszTemp = cszDataArray.ElementAt(nNum++);

				if (nVal < nNumRowsChk)
				{
					nRow = m_cListCtrlLeft.InsertItem(&lvItem);

					m_cListCtrlLeft.SetItemText(nRow, OUTLET_NM_ID, cszTemp.GetBuffer(cszTemp.GetLength()));
					cszTemp.ReleaseBuffer();

					m_cListCtrlLeft.m_cszOrgOutletNm.Add(cszTemp);

					cszTemp.Empty();
					cszTemp = cszDataArray.ElementAt(nNum++);

					m_cListCtrlLeft.SetItemText(nRow, COL_OUTLET_VOL, cszTemp.GetBuffer(cszTemp.GetLength()));
					cszTemp.ReleaseBuffer();

					m_cListCtrlLeft.m_cszOrgOutletVolt.Add(cszTemp);

					cszTemp.Empty();
					cszTemp = cszDataArray.ElementAt(nNum++);

					m_cListCtrlLeft.SetItemText(nRow, COL_OUTLET_FRQ, cszTemp.GetBuffer(cszTemp.GetLength()));
					cszTemp.ReleaseBuffer();

					m_cListCtrlLeft.m_cszOrgOutletFreq.Add(cszTemp);

				}
				else
				{
					nRow = m_cListCtrlRight.InsertItem(&lvItem);

					m_cListCtrlRight.SetItemText(nRow, OUTLET_NM_ID, cszTemp.GetBuffer(cszTemp.GetLength()));
					cszTemp.ReleaseBuffer();

					m_cListCtrlRight.m_cszOrgOutletNm.Add(cszTemp);

					cszTemp.Empty();
					cszTemp = cszDataArray.ElementAt(nNum++);

					m_cListCtrlRight.SetItemText(nRow, COL_OUTLET_VOL, cszTemp.GetBuffer(cszTemp.GetLength()));
					cszTemp.ReleaseBuffer();

					m_cListCtrlRight.m_cszOrgOutletVolt.Add(cszTemp);

					cszTemp.Empty();
					cszTemp = cszDataArray.ElementAt(nNum++);

					m_cListCtrlRight.SetItemText(nRow, COL_OUTLET_FRQ, cszTemp.GetBuffer(cszTemp.GetLength()));
					cszTemp.ReleaseBuffer();

					m_cListCtrlRight.m_cszOrgOutletFreq.Add(cszTemp);
				}

			} // for (nVal

		} // if (nPos < m_nNumOfOutlets.GetSize())
	}

	return bFileFound;
}

BOOL CePower_MFGDlg::WriteCSVFile()
{
	BOOL				bRet = TRUE;
	UINT				nNum = 0;
	UINT				nCell = 0;
	UINT				nTotal = 0;
	UINT				nNumRowsChk = 0;
	CString				cszLoad;
	CString				cszVolt;
	CString				cszFreq;
	CStringArray		cszDataArray;
	CFileFind			cFinder;
	CString				cszCSVFilePath = m_cszBasePath;

	ShowPopupDlg(DLG_POPUP_CREATE);

	cszCSVFilePath += STR_CSV_FILE_NM;

	if (cFinder.FindFile(cszCSVFilePath))
	{
		cFinder.Close();
		::DeleteFile(cszCSVFilePath);
	}

	CFileCSV		cFileCSV(cszCSVFilePath, CFileCSV::modeWrite);

	cszDataArray.RemoveAll();

	// DEVICE TYPE
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), m_cComboDevice.GetCurSel());
	cszDataArray.Add(cszLoad);

	// Read MODEL NAME
	cszLoad.Empty();
	m_cEditModelNm.GetWindowText(cszLoad);
	cszDataArray.Add(cszLoad);

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// Read SERIAL NUMBER
	/*cszLoad.Empty();
	m_cEditSerialNum.GetWindowText(cszLoad);
	cszDataArray.Add(cszLoad);*/

	// Read MODEL NUMBER
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), m_cComboModelNum.GetCurSel());
	cszDataArray.Add(cszLoad);

	// Read MODEL NUMBER
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), m_cComboModelNumSub.GetCurSel());
	cszDataArray.Add(cszLoad);

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// Read MODEL NUMBER
	cszLoad.Empty();
	cszLoad.Format(_T("%d"), m_cComboModelNumEnd.GetCurSel());
	cszDataArray.Add(cszLoad);

	// Read MANUFACTURING DATE
	cszLoad.Empty();
	m_cEditManufacDate.GetWindowText(cszLoad);
	cszDataArray.Add(cszLoad);

	// Read HOST NAME
	cszLoad.Empty();
	m_cEditHostNm.GetWindowText(cszLoad);
	cszDataArray.Add(cszLoad);

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// Read MAC ADDRESS
	/*cszLoad.Empty();
	cszLoad = STR_MAC_ADDR_PREFIX;
	cszLoad += cszGenRandNum;
	cszDataArray.Add(cszLoad);*/

	// Read STAGGER ON DELAY
	cszLoad.Empty();
	m_cEditStaggerOnDelay.GetWindowText(cszLoad);
	cszDataArray.Add(cszLoad);

	// Read STAGGER OFF DELAY
	cszLoad.Empty();
	m_cEditStaggerOffDelay.GetWindowText(cszLoad);
	cszDataArray.Add(cszLoad);

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// Write Outlet Names, voltage and frequency
	if (m_cComboModelNum.GetCurSel() < m_byTotalOutlets.GetSize())
	{
		nCell = 0;
		nTotal = m_byTotalOutlets.ElementAt(m_cComboModelNum.GetCurSel());

		nNumRowsChk = MAX_OUTLET_NUM;
		if (nTotal > OUTLET_24)
		{
			nNumRowsChk = nTotal / 2;
		}

		for (nNum = 0; nNum < nTotal; nNum++)
		{
			ShowPopupDlg(DLG_POPUP_REFRESH);

			if (nNum == nNumRowsChk)
			{
				nCell = 0;
			}

			cszLoad.Empty();
			cszVolt.Empty();
			cszFreq.Empty();
			if (nNum < nNumRowsChk)
			{
				cszLoad = m_cListCtrlLeft.GetItemText(nCell, OUTLET_NM_ID);
				cszVolt = m_cListCtrlLeft.GetItemText(nCell, COL_OUTLET_VOL);
				cszFreq = m_cListCtrlLeft.GetItemText(nCell, COL_OUTLET_FRQ);
			}
			else
			{
				cszLoad = m_cListCtrlRight.GetItemText(nCell, OUTLET_NM_ID);
				cszVolt = m_cListCtrlRight.GetItemText(nCell, COL_OUTLET_VOL);
				cszFreq = m_cListCtrlRight.GetItemText(nCell, COL_OUTLET_FRQ);
			}
			cszDataArray.Add(cszLoad);
			cszDataArray.Add(cszVolt);
			cszDataArray.Add(cszFreq);

			nCell++;
		}
	}

	cFileCSV.WriteData(cszDataArray);
	cFileCSV.Close();

	ShowPopupDlg(DLG_POPUP_DESTROY);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_SAVE_SETTINGS_MESS);
	AfxMessageBox(cszLoad, MB_OK | MB_ICONINFORMATION);

	return bRet;
}

BOOL CePower_MFGDlg::CheckBufferWithSplChar(CString cszBuffer)
{
	UINT		nNum = 0;
	BOOL		bFound = FALSE;

	for (nNum = 0; nNum < (UINT)m_cszSplCharsList.GetSize(); nNum++)
	{
		if (cszBuffer.Find(m_cszSplCharsList.ElementAt(nNum)) != RES_MINUS_ONE)
		{
			bFound = TRUE;
			break;
		}
	}

	return bFound;
}

void CePower_MFGDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	//OnOK();
	UINT				nValue = 0;
	CString				cszLoad;
	CString				cszTemp;

	cszLoad.Empty();
	m_cEditModelNm.GetWindowText(cszLoad);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();
	if (cszLoad.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MODEL_NM_ERR_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return;
	}

	if (CheckBufferWithSplChar(cszLoad))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MODEL_NM_ERR_MSG);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);
		return;
	}

	cszLoad.Empty();
	m_cEditSerialNum.GetWindowText(cszLoad);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();
	if (cszLoad.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_SERIAL_NUM_ERR);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return;
	}

	if (CheckBufferWithSplChar(cszLoad))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_SERIAL_NUM_ERR);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);
		return;
	}

	cszLoad.Empty();
	m_cEditManufacDate.GetWindowText(cszLoad);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();
	if (cszLoad.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MANUF_DATE_ERR);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return;
	}

	if (CheckBufferWithSplChar(cszLoad))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MANUF_DATE_ERR);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);
		return;
	}

	cszLoad.Empty();
	m_cEditHostNm.GetWindowText(cszLoad);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();
	if (cszLoad.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_HOST_NM_ERR);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return;
	}

	if (CheckBufferWithSplChar(cszLoad))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_HOST_NM_ERR);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);
		return;
	}

	cszLoad.Empty();
	m_cEditMACAddr.GetWindowText(cszLoad);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();
	if (cszLoad.IsEmpty() || cszLoad.GetLength() < MAX_MAC_LENGTH)
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MAC_ADDR_ERR);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return;
	}

	if (CheckBufferWithSplChar(cszLoad))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MAC_ADDR_ERR);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);
		return;
	}

	cszLoad.Empty();
	m_cEditStaggerOnDelay.GetWindowText(cszLoad);
	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_ON_DELAY);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();
	if (cszLoad.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_ON_ERR);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return;
	}

	nValue = 0;
	cszLoad.Empty();
	m_cEditStaggerOffDelay.GetWindowText(cszLoad);
	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_OFF_DELAY);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();
	if (cszLoad.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_OFF_ERR);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		return;
	}

	cszLoad.Empty();
	m_cEditSysNm.GetWindowText(cszLoad);
	cszLoad.TrimLeft();
	cszLoad.TrimRight();	
	if (cszLoad.IsEmpty())
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_SYS_NM_ERR);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		m_cEditSysNm.SetFocus();
		return;
	}

	WriteStaticInfo();

}

BOOL CePower_MFGDlg::WriteStaticInfo()
{
	BOOL				bRet = TRUE;
	BOOL				bErrStatus = FALSE;
	int					nVal = 0;
	int					nCellNum = 0;
	int					nNum = 0;
	BYTE				byNum = 0;
	BYTE				byCheckBkup = 0;
	BYTE				byCheck = 0;
	BYTE				byTotalItems = 0;
	BYTE				byCount = 0;
	BYTE				byValue = 0;
	BYTE				byErrCode = 0;
	BYTE				byInfoSize = 0;
	BYTE				byPhaseIDCnt = 0;
	CString				cszLoad;
	CString				cszData;
	CString				cszTemp;
	DWORD				dwTemp = 0;
	DWORD				dwValue = 0;
	BYTE				byDataBuffer[66] = {0};
	IEE_CONV			stIeeConv = {0};

	ShowPopupDlg(DLG_POPUP_CREATE);

	for (nNum = 1; nNum <= PDU_PHASE_INP_CONN_TYPE; nNum++)
	{
		cszLoad.Empty();
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		ShowPopupDlg(DLG_POPUP_REFRESH);

		switch(nNum)
		{
			case PDU_MODEL_NM:
				{
					m_cEditModelNm.GetWindowText(cszLoad);
				}
				break;

			case PDU_SERIAL_NUM:
				{
					m_cEditSerialNum.GetWindowText(cszLoad);
				}
				break;

			case PDU_MODEL_NUM:
				{
					cszTemp.Empty();
					m_cComboModelNum.GetLBText(m_cComboModelNum.GetCurSel(), cszTemp);
					cszLoad = cszTemp;

					cszLoad += STR_HYPEN;

					cszTemp.Empty();
					m_cComboModelNumSub.GetLBText(m_cComboModelNumSub.GetCurSel(), cszTemp);
					cszLoad += cszTemp;

					if (m_cComboModelNumEnd.GetCurSel())
					{
						cszLoad += STR_HYPEN;

						cszTemp.Empty();
						m_cComboModelNumEnd.GetLBText(m_cComboModelNumEnd.GetCurSel(), cszTemp);
						cszLoad += cszTemp;
					}
				}
				break;

			case PDU_MANUFAC_DATE:
				{
					m_cEditManufacDate.GetWindowText(cszLoad);
				}
				break;

			case PDU_HOST_NM:
				{
					m_cEditHostNm.GetWindowText(cszLoad);
				}
				break;

			case PDU_MAC_ADDRS:
				{
					//cszLoad = STR_MAC_ADDR_PREFIX;
					//cszLoad += m_cszGenRandNum;
					m_cEditMACAddr.GetWindowText(cszLoad);
					cszLoad.Replace(_T(":"), _T(""));

					cszTemp.Empty();
					nCellNum = 0;
					for (nVal = 0; nVal < cszLoad.GetLength(); nVal = nVal + 2)
					{
						cszTemp.Format(_T("0x%s"), cszLoad.Mid(nVal, 2));
						byDataBuffer[nCellNum++] = (BYTE) strtoul(cszTemp.GetBuffer(cszTemp.GetLength()), NULL, 0);
						cszTemp.ReleaseBuffer();
						cszTemp.Empty();
					}
				}
				break;

			case PDU_STAGGER_ON_DELAY:
				{
					dwValue = GetDlgItemInt(IDC_EDIT_STAGGER_ON_DELAY);
	
					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					byDataBuffer[0] = HIBYTE(HIWORD(dwValue)); 
					byDataBuffer[1] = LOBYTE(HIWORD(dwValue));
					byDataBuffer[2] = HIBYTE(LOWORD(dwValue)); 
					byDataBuffer[3] = LOBYTE(LOWORD(dwValue)); 
				}
				break;

			case PDU_STAGGER_OFF_DELAY:
				{
					dwValue = GetDlgItemInt(IDC_EDIT_STAGGER_OFF_DELAY);
	
					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					byDataBuffer[0] = HIBYTE(HIWORD(dwValue)); 
					byDataBuffer[1] = LOBYTE(HIWORD(dwValue));
					byDataBuffer[2] = HIBYTE(LOWORD(dwValue)); 
					byDataBuffer[3] = LOBYTE(LOWORD(dwValue)); 
				}
				break;

			case PDU_FIRMWARE_VERSION:
				{
					continue;
				}
				break;

			case PDU_TOTAL_OUTLETS:
				{
					nCellNum = m_cComboModelNum.GetCurSel();

					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					if (nCellNum < m_byTotalOutlets.GetSize())
					{
						byDataBuffer[0] = m_byTotalOutlets.ElementAt(nCellNum);
						byCount = m_byTotalOutlets.ElementAt(nCellNum);
					}
				}
				break;

			case PDU_TOTAL_BANKS:
				{
					nCellNum = m_cComboModelNum.GetCurSel();

					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					if (nCellNum < m_byTotalBanks.GetSize())
					{
						byDataBuffer[0] = m_byTotalBanks.ElementAt(nCellNum);
						byCount = m_byTotalBanks.ElementAt(nCellNum);
					}
				}
				break;

			case PDU_OUTLETS_PER_BANK:
				{
					nCellNum = m_cComboModelNum.GetCurSel();

					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					if (nCellNum < m_byOutletsPerBank.GetSize())
					{
						byDataBuffer[0] = m_byOutletsPerBank.ElementAt(nCellNum);
						byCount = m_byOutletsPerBank.ElementAt(nCellNum);
					}
				}
				break;

			case PDU_PHASE_INP_CONN_TYPE:
				{
					nCellNum = m_cComboModelNum.GetCurSel();

					memset(byDataBuffer, 0, sizeof(byDataBuffer));

					if (nCellNum < m_nPosition)
					{
						nCellNum = m_cComboModelNumSub.GetCurSel();

						if (nCellNum < m_by3PhInpConnType.GetSize())
						{
							byDataBuffer[0] = MODE_3_PHASE;
							byDataBuffer[1] = m_by3PhInpConnType.ElementAt(nCellNum);

							byCount = MODE_3_PHASE;
							byValue = m_by3PhInpConnType.ElementAt(nCellNum);
						}
					}
					else
					{
						nCellNum = m_cComboModelNumSub.GetCurSel();

						if (nCellNum < m_by1PhInpConnType.GetSize())
						{
							byDataBuffer[0] = MODE_1_PHASE;
							byDataBuffer[1] = m_by1PhInpConnType.ElementAt(nCellNum);

							byCount = MODE_1_PHASE;
							byValue = m_by1PhInpConnType.ElementAt(nCellNum);
						}
					}
				}
				break;
		}

		ShowPopupDlg(DLG_POPUP_REFRESH);

		if (nNum < PDU_MAC_ADDRS)
		{
			wsprintf((LPSTR)byDataBuffer, _T("%s"), cszLoad);
		}

		byInfoSize = lstrlen((LPCSTR)byDataBuffer);
		if (byInfoSize > 60)
		{
			byInfoSize = 60;
		}

		if (nNum == PDU_MAC_ADDRS)
		{
			byInfoSize = 6;
		}
		else if ((nNum == PDU_STAGGER_ON_DELAY) || (nNum == PDU_STAGGER_OFF_DELAY))
		{
			byInfoSize = 4;
		}

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), nNum, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);

		//-------------------- READ ------------------------------
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), nNum, byDataBuffer, 0, FALSE, &byErrCode, TRUE);

		ShowPopupDlg(DLG_POPUP_REFRESH);

		if (nNum == PDU_MAC_ADDRS)
		{
			cszData.Empty();
			cszData.Format(_T("%02x%02x%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3], byDataBuffer[4], byDataBuffer[5]);

			if (cszData.CompareNoCase(cszLoad))
			{
				bErrStatus = TRUE;
				break;
			}
		}
		else if ((nNum == PDU_STAGGER_ON_DELAY) || (nNum == PDU_STAGGER_OFF_DELAY))
		{
			cszData.Empty();
			cszData.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
			dwTemp = strtoul(cszData.GetBuffer(cszData.GetLength()), NULL, 16);
			cszData.ReleaseBuffer();

			if (dwTemp != dwValue)
			{
				bErrStatus = TRUE;
				break;
			}
		}
		else if ((nNum == PDU_TOTAL_OUTLETS) || (nNum == PDU_TOTAL_BANKS) || (nNum == PDU_OUTLETS_PER_BANK) || (nNum == PDU_PHASE_INP_CONN_TYPE))
		{
			if (byDataBuffer[0] != byCount)
			{
				bErrStatus = TRUE;
				break;
			}

			if (nNum == PDU_PHASE_INP_CONN_TYPE)
			{
				if (byDataBuffer[1] != byValue)
				{
					bErrStatus = TRUE;
					break;
				}
			}
		}
		else
		{
			cszData.Empty();
			cszData.Format(_T("%s"), byDataBuffer);

			if (cszData.Compare(cszLoad))
			{
				bErrStatus = TRUE;
				break;
			}
		}

		ShowPopupDlg(DLG_POPUP_REFRESH);
	}

	if (!bErrStatus)
	{
		byCount = 1;
		for (nVal = 0; nVal < MAX_LIST_CTRL; nVal++)
		{
			ShowPopupDlg(DLG_POPUP_REFRESH);

			if (!nVal)
			{
				nCellNum = m_cListCtrlLeft.GetItemCount();
			}
			else
			{
				nCellNum = m_cListCtrlRight.GetItemCount();
			}

			// WRITE OUTLET NAMES
			for (nNum = 0; nNum < nCellNum; nNum++)
			{
				ShowPopupDlg(DLG_POPUP_REFRESH);

				cszLoad.Empty();
				if (!nVal)
				{
					cszLoad = m_cListCtrlLeft.GetItemText(nNum, OUTLET_NM_ID);
				}
				else
				{
					cszLoad = m_cListCtrlRight.GetItemText(nNum, OUTLET_NM_ID);
				}

				memset(byDataBuffer, 0, sizeof(byDataBuffer));
				byDataBuffer[0] = byCount++;
				wsprintf((LPSTR)byDataBuffer + 1, _T("%s"), cszLoad);
				byInfoSize = cszLoad.GetLength() + 1;

				PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_GET_SET_OUTLET_NAME, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();

				//-------------------- READ ------------------------------
				memset(byDataBuffer, 0, sizeof(byDataBuffer));
				PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_GET_SET_OUTLET_NAME, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (byCount - 1));
				m_cszPDUAddr.ReleaseBuffer();

				cszData.Empty();
				cszData.Format(_T("%s"), byDataBuffer);

				if (cszData.Compare(cszLoad))
				{
					bErrStatus = TRUE;
					break;
				}

			} // for (nNum = 0

		} // for (nVal = 0

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! WRITE PHASE INFO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		// ===================== WRITE PRODUCT GROUP NAME ==============================
		cszLoad.Empty();
		if (m_nDeviceType == DEVICE_EPOWER)
		{	// For ePowerHD version
			cszLoad = m_stPhaseInfo.chePowerHDNm;
		}
		else if (m_nDeviceType == DEVICE_EPOWER_CT)
		{	// For ePower Manager CT version
			cszLoad = m_stPhaseInfo.chePowerManCT;
		}
		else if (m_nDeviceType == DEVICE_PM10)
		{
			cszLoad.Empty();
			m_cComboModelNum.GetLBText(m_cComboModelNum.GetCurSel(), cszLoad);
			if (!cszLoad.CompareNoCase(STR_PM10_U_VERS))
			{	// For PM10U version
				cszLoad = m_stPhaseInfo.chPM10UNm;
			}
			else
			{	// For PM10 version
				cszLoad = m_stPhaseInfo.chPM10Nm;
			}
		}
		else if (m_nDeviceType == DEVICE_PM9S9)
		{
			cszLoad = m_stPhaseInfo.chPM9S9Nm;
		}

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		wsprintf((LPSTR)byDataBuffer, _T("%s"), cszLoad);
		byInfoSize = lstrlen((LPCSTR)byDataBuffer);
		if (byInfoSize > 60)
		{
			byInfoSize = 60;
		}
		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_PRODUCT_GROUP, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);

		// ==================== WRITE BANK PHASE ID LIST ==========================================
		byTotalItems = 0;
		nCellNum = m_cComboModelNum.GetCurSel();
		if (nCellNum < m_byTotalBanks.GetSize())
		{
			byTotalItems = m_byTotalBanks.ElementAt(nCellNum);

			byCheck = 1;
			if (byTotalItems > 3)
			{	
				byCheck = 2;
			}

			byCheckBkup = byCheck;
			byPhaseIDCnt = A_SERIES;

			for (byNum = 1; byNum <= byTotalItems; byNum++)
			{
				ShowPopupDlg(DLG_POPUP_REFRESH);

				memset(byDataBuffer, 0, sizeof(byDataBuffer));
				byDataBuffer[0] = byNum;

				if (nCellNum < m_nPosition)
				{	// 3 PHASE

					if (!byCheck)
					{
						byCheck = byCheckBkup;
						byPhaseIDCnt++;
					}

					if (byNum == 7)
					{
						byPhaseIDCnt = A_SERIES;
					}

					if (nCellNum < m_by3PhInpConnType.GetSize())
					{
						cszLoad.Empty();
						cszLoad.Format(_T("%c"), m_by3PhInpConnType.ElementAt(nCellNum));
						if (!cszLoad.CompareNoCase("D"))
						{	// DELTA TYPE

							cszLoad.Empty();
							if (byPhaseIDCnt == A_SERIES)
							{
								cszLoad = m_stPhaseInfo.chDelta3PhsID_AB;
							}
							else if (byPhaseIDCnt == B_SERIES)
							{
								cszLoad = m_stPhaseInfo.chDelta3PhsID_BC;
							}
							else if (byPhaseIDCnt == C_SERIES)
							{
								cszLoad = m_stPhaseInfo.chDelta3PhsID_CA;
							}
						}
						else if (!cszLoad.CompareNoCase("Y"))
						{	// WYE TYPE
							
							cszLoad.Empty();
							if (byPhaseIDCnt == A_SERIES)
							{
								cszLoad = m_stPhaseInfo.chWye3PhsID_AN;
							}
							else if (byPhaseIDCnt == B_SERIES)
							{
								cszLoad = m_stPhaseInfo.chWye3PhsID_BN;
							}
							else if (byPhaseIDCnt == C_SERIES)
							{
								cszLoad = m_stPhaseInfo.chWye3PhsID_CN;
							}
						}
					}

					byCheck--;
				}
				else
				{	// 1 PHASE
					cszLoad = m_stPhaseInfo.ch1PhsID_AN;
				}

				wsprintf((LPSTR)byDataBuffer + 1, _T("%s"), cszLoad);
				byInfoSize = cszLoad.GetLength() + 1;

				PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_BANK_PHASE_ID, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
				m_cszPDUAddr.ReleaseBuffer();

			} // for byNum

		} // if nCellNum < m_byTotalBanks

		// ==================== WRITE CAPACITY INFO FOR PDU, BANKS & OUTLETS ==========================================
		// FOR PDU
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		byDataBuffer[0] = 1;
		if (m_nDeviceType == DEVICE_PM9S9)
		{
			byDataBuffer[1] = 30;
		}
		else
		{
			byDataBuffer[1] = 60;
		}
		byInfoSize = 2;

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_CAPACITY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);

		// FOR BANKS
		byTotalItems = 0;
		nCellNum = m_cComboModelNum.GetCurSel();
		if (nCellNum < m_byTotalBanks.GetSize())
		{
			byTotalItems = m_byTotalBanks.ElementAt(nCellNum);
		}

		byCheck = 20;
		if (byTotalItems > 3)
		{
			byCheck = 10;
		}

		if (m_nDeviceType == DEVICE_PM9S9)
		{
			byCheck = 30;
		}

		for (byNum = 1; byNum <= byTotalItems; byNum++)
		{
			ShowPopupDlg(DLG_POPUP_REFRESH);

			memset(byDataBuffer, 0, sizeof(byDataBuffer));
			byDataBuffer[0] = byNum;
			byDataBuffer[1] = byCheck;

			byInfoSize = 2;

			PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_BANK_CAPACITY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();
		}

		// FOR OUTLETS
		nCellNum = m_cComboModelNum.GetCurSel();
		if (nCellNum < m_byTotalOutlets.GetSize())
		{
			byTotalItems = m_byTotalOutlets.ElementAt(nCellNum);
		}

		for (byNum = 1; byNum <= byTotalItems; byNum++)
		{
			ShowPopupDlg(DLG_POPUP_REFRESH);

			memset(byDataBuffer, 0, sizeof(byDataBuffer));
			byDataBuffer[0] = byNum;
			if (m_nDeviceType == DEVICE_PM9S9)
			{
				byDataBuffer[1] = 30;
			}
			else
			{
				byDataBuffer[1] = 16;
			}

			byInfoSize = 2;

			PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_OUTLET_CAPACITY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
			m_cszPDUAddr.ReleaseBuffer();
		}

		// Write MONITOR ONLY Flag
		cszLoad.Empty();
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		ShowPopupDlg(DLG_POPUP_REFRESH);

		nCellNum = m_cComboModelNum.GetCurSel();
		if (nCellNum < m_byMonitorOnly.GetSize())
		{
			byDataBuffer[0] = m_byMonitorOnly.ElementAt(nCellNum);
		}

		byInfoSize = lstrlen((LPCSTR)byDataBuffer);
		if (byInfoSize > 60)
		{
			byInfoSize = 60;
		}

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_MONITOR_ONLY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);

		// Write FULL CONTROL Flag
		cszLoad.Empty();
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		nCellNum = m_cComboModelNum.GetCurSel();
		if (nCellNum < m_byFullControl.GetSize())
		{
			byDataBuffer[0] = m_byFullControl.ElementAt(nCellNum);
		}

		byInfoSize = lstrlen((LPCSTR)byDataBuffer);
		if (byInfoSize > 60)
		{
			byInfoSize = 60;
		}

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_FULL_CONTROL, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

		ShowPopupDlg(DLG_POPUP_REFRESH);

		if (m_byMonitorOnly.ElementAt(nCellNum))
		{
			// Write Voltage
			byCount = 1;
			for (nVal = 0; nVal < MAX_LIST_CTRL; nVal++)
			{
				ShowPopupDlg(DLG_POPUP_REFRESH);

				if (!nVal)
				{
					nCellNum = m_cListCtrlLeft.GetItemCount();
				}
				else
				{
					nCellNum = m_cListCtrlRight.GetItemCount();
				}

				// WRITE OUTLET VOLTAGE
				for (nNum = 0; nNum < nCellNum; nNum++)
				{
					ShowPopupDlg(DLG_POPUP_REFRESH);

					cszLoad.Empty();
					if (!nVal)
					{
						cszLoad = m_cListCtrlLeft.GetItemText(nNum, COL_OUTLET_VOL);
					}
					else
					{
						cszLoad = m_cListCtrlRight.GetItemText(nNum, COL_OUTLET_VOL);
					}

					stIeeConv.fValue = (float)atof((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
					cszLoad.ReleaseBuffer();
					dwValue = stIeeConv.nValue;

					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					byDataBuffer[0] = byCount++;
					byDataBuffer[1] = HIBYTE(HIWORD(dwValue)); 
					byDataBuffer[2] = LOBYTE(HIWORD(dwValue));
					byDataBuffer[3] = HIBYTE(LOWORD(dwValue)); 
					byDataBuffer[4] = LOBYTE(LOWORD(dwValue));
					byInfoSize = 5;

					PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_OUTLET_VOLTAGE, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
					m_cszPDUAddr.ReleaseBuffer();

				} // for (nNum = 0

			} // for (nVal = 0

			// Write Frequency
			byCount = 1;
			for (nVal = 0; nVal < MAX_LIST_CTRL; nVal++)
			{
				ShowPopupDlg(DLG_POPUP_REFRESH);

				if (!nVal)
				{
					nCellNum = m_cListCtrlLeft.GetItemCount();
				}
				else
				{
					nCellNum = m_cListCtrlRight.GetItemCount();
				}

				// WRITE OUTLET FREQUENCY
				for (nNum = 0; nNum < nCellNum; nNum++)
				{
					ShowPopupDlg(DLG_POPUP_REFRESH);

					cszLoad.Empty();
					if (!nVal)
					{
						cszLoad = m_cListCtrlLeft.GetItemText(nNum, COL_OUTLET_FRQ);
					}
					else
					{
						cszLoad = m_cListCtrlRight.GetItemText(nNum, COL_OUTLET_FRQ);
					}

					byValue = (int)atoi((char*)(LPCTSTR)cszLoad.GetBuffer(cszLoad.GetLength()));
					cszLoad.ReleaseBuffer();

					memset(byDataBuffer, 0, sizeof(byDataBuffer));
					byDataBuffer[0] = byCount++;
					byDataBuffer[1] = byValue; 
					byInfoSize = 2;

					PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_OUTLET_FREQUENCY, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE);
					m_cszPDUAddr.ReleaseBuffer();

				} // for (nNum = 0

			} // for (nVal = 0

		} // if bMonitorOnly true

		// Write System Name (like configuration tool)
		cszLoad.Empty();
		m_cEditSysNm.GetWindowText(cszLoad);

		ShowPopupDlg(DLG_POPUP_REFRESH);

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		wsprintf((LPSTR)byDataBuffer, _T("%s"), cszLoad);

		byInfoSize = lstrlen((LPCSTR)byDataBuffer);
		if (byInfoSize > 60)
		{
			byInfoSize = 60;
		}

		PduSysNmWrite(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_GENERAL_SYSTEM_NAME, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE, 1);
		m_cszPDUAddr.ReleaseBuffer();

		// Write Current Date and Time
		CTime	cTm(CTime::GetCurrentTime());

		cszLoad.Empty();
		cszLoad = cTm.Format(_T("%m-%d-%Y %H:%M"));

		ShowPopupDlg(DLG_POPUP_REFRESH);

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		wsprintf((LPSTR)byDataBuffer, _T("%s"), cszLoad);

		byInfoSize = lstrlen((LPCSTR)byDataBuffer);
		if (byInfoSize > 60)
		{
			byInfoSize = 60;
		}

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_SET_DATE_TIME, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE, TRUE);
		m_cszPDUAddr.ReleaseBuffer();

	} // if (!bErrStatus)


	////////////////////////////////////// THIS SHOULD BE THE LAST WRITE CALL //////////////////////////////////////////////////////////////////////////////////
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	byDataBuffer[0] = 0x01;
	byInfoSize = 1;
	PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_WRITE_OVER, byDataBuffer, byInfoSize, TRUE, &byErrCode, TRUE, TRUE);
	m_cszPDUAddr.ReleaseBuffer();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ShowPopupDlg(DLG_POPUP_DESTROY);

	if (bErrStatus)
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_ERR_DATA);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
	}
	else
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_SUCCESS_MESS);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONINFORMATION);

		m_cButReadPDU.EnableWindow(TRUE);
	}

	return bRet;
}

void CePower_MFGDlg::OnBnClickedGenerate()
{
	// TODO: Add your control notification handler code here
	UINT			nNum = 0;
	int				nSize = 0;
	CString			cszTemp;
	CString			cszLoad;

	GenerateSerialNum();

	cszLoad.Empty();

	for (nNum = 0; nNum < 2; nNum++)
	{
		cszTemp.Empty();
		if (!nNum)
		{
			cszTemp = STR_MAC_ADDR_PREFIX;
		}
		else
		{
			cszTemp = m_cszGenRandNum;
		}

		for (nSize = 0; nSize < cszTemp.GetLength(); nSize++)
		{
			cszLoad += cszTemp.GetAt(nSize);

			if ((nNum) && (cszTemp.GetLength() == nSize + 1))
				break;

			if (nSize % 2)
			{
				cszLoad += ":";
			}
		}
	}

	/*cszMACAddrs.Empty();
	cszMACAddrs = STR_MAC_ADDR_PREFIX;
	cszMACAddrs += cszGenRandNum;*/
	m_cEditMACAddr.SetWindowText(cszLoad);

	GenerateManufacDate();
}

CString CePower_MFGDlg::GenerateSerialRandNum()
{
	int				nValue = 0;
	CString			cszLoad;
	CString			cszTemp;

	srand((unsigned)time(NULL));

	cszLoad.Empty();

	nValue = (int)((double)rand() / (RAND_MAX + 1) * (999999 - 1) + 1);

	cszTemp.Empty();
	cszTemp.Format("%06d", nValue);

	cszLoad += cszTemp;

	return cszLoad;
}

CString CePower_MFGDlg::GetWeekNo(int year, int month, int day)
{
	int			nWeekNo = 0;
    long		daynumber = 0;
    long		firstdaynumber = 0;
    long		lastdaynumber = 0;
	CString		cszLoad;
    
    daynumber = 4L + 365L * ((long) year) + 31L * (((long) month) - 1L) + ((long) ((year - 1) / 4)) + (long) (day);

    if ((month > 2) && (year % 4 == 0))
        daynumber++;                    /* after leapday */
    if ((month > 2) && (month < 8))
        daynumber -= (3L + ((month - 3) / 2));
    else if (month > 7)
        daynumber -= (3L + ((month - 4) / 2));

    firstdaynumber = 5L + 365L * ((long) year) + ((long) ((year - 1) / 4));
    lastdaynumber = 4L + 365L * ((long) (year + 1)) + ((long) ((year) / 4));

    if (((lastdaynumber % 7L) <= 2L) && ((lastdaynumber - daynumber) <= (lastdaynumber % 7L)))
        nWeekNo = 1;/* wrap to 1st week next year */
    else if ((firstdaynumber % 7L) <= 3L)/* This year starts Mo..Th */
        nWeekNo = ((daynumber - firstdaynumber / 7L * 7L) / 7L + 1L);
    else if (daynumber < (firstdaynumber + 7L - firstdaynumber % 7L))
    {
        /*
         * Day daynumber belongs to last week of previous year.
         */
        firstdaynumber = 4L + 365L * ((long) (year - 1)) + ((long) ((year - 1) / 4)) + 1L;

        if ((firstdaynumber % 7L) <= 3L)/* Previous year starts Mo..Th */
            nWeekNo = ((daynumber - firstdaynumber / 7L * 7L) / 7L + 1L);
        else
            nWeekNo = ((daynumber - firstdaynumber / 7L * 7L) / 7L);
    }
    else
        nWeekNo = ((daynumber - firstdaynumber / 7L * 7L) / 7L); 

	cszLoad.Empty();
	cszLoad.Format(_T("%02d"), nWeekNo);
    return cszLoad; 
}

CString CePower_MFGDlg::GetYearNo()
{
	CString			cszLoad;
	CTime			cTm(CTime::GetCurrentTime());

	cszLoad.Empty();
	cszLoad.Format(_T("%04d"), cTm.GetYear());

	return cszLoad;
}

BOOL CePower_MFGDlg::GenerateSerialNum()
{
	BOOL			bRet = TRUE;
	CString			cszLoad;
	CTime			cTm(CTime::GetCurrentTime());

	cszLoad.Empty();
	cszLoad = GetYearNo();	// Get YEAR

	cszLoad += GetWeekNo(cTm.GetYear(), cTm.GetMonth(), cTm.GetDay());	// Get WeekNum

	m_cszGenRandNum.Empty();
	m_cszGenRandNum = GenerateSerialRandNum();

	cszLoad += m_cszGenRandNum;

	m_cEditSerialNum.SetWindowText(cszLoad);

	return bRet;
}

BOOL CePower_MFGDlg::GenerateManufacDate()
{
	BOOL			bRet = TRUE;
	CString			cszLoad;
	CTime			cTm(CTime::GetCurrentTime());

	cszLoad.Empty();
	cszLoad = cTm.Format(_T("%d %b %Y"));

	m_cEditManufacDate.SetWindowText(cszLoad);

	return bRet;
}

void CePower_MFGDlg::OnBnClickedReadPdu()
{
	// TODO: Add your control notification handler code here
	BYTE					byValue = 0;
	BYTE					byErrCode = 0;
	BYTE					byTotalOutlets = 0;
	BYTE					byTotalBanks = 0;
	BOOL					bPM10U = FALSE;
	int						nSize = 0;
	UINT					nNum = 0;
	CFile					cPDUInfoFile;
	CFileException			cFileEx;
	DWORD					dwTemp = 0;
	CString					cszLoad;
	CString					cszData;
	CFileFind				cFinder;
	CString					cszPDUInfoFilePath;
	BYTE					byDataBuffer[66] = {0};

	ShowPopupDlg(DLG_POPUP_CREATE);

	cszPDUInfoFilePath.Empty();
	cszPDUInfoFilePath = m_cszBasePath;
	cszPDUInfoFilePath += STR_PDU_INFO_FILE_NM;

	if (cFinder.FindFile(cszPDUInfoFilePath))
	{
		cFinder.Close();
		::DeleteFile(cszPDUInfoFilePath);
	}

	cPDUInfoFile.Open(cszPDUInfoFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &cFileEx);

	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_PDU_INFO_READ_TITLE);
	cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

	cszLoad.Empty();
	cszLoad = STR_NEXT_LINE;
	cszLoad += STR_UNDERLINE;
	cszLoad += STR_NEXT_LINE;
	cszLoad += STR_NEXT_LINE;
	cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

	for (nNum = 1; nNum <= PDU_PHASE_INP_CONN_TYPE; nNum++)
	{
		cszLoad.Empty();
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		ShowPopupDlg(DLG_POPUP_REFRESH);

		cszData.Empty();
		switch(nNum)
		{
			case PDU_MODEL_NM:
				{
					cszData.LoadString(IDS_STR_MODEL_NM_TITLE);
					cszData += STR_MOD_NM_EX_SPACE_8;
				}
				break;

			case PDU_SERIAL_NUM:
				{
					cszData.LoadString(IDS_STR_SER_NUM_TITLE);
					cszData += STR_SR_NUM_EX_SPACE_5;
				}
				break;

			case PDU_MODEL_NUM:
				{
					cszData.LoadString(IDS_STR_MODEL_NUM_TITLE);
					cszData += STR_MD_NUM_EX_SPACE_6;
				}
				break;

			case PDU_MANUFAC_DATE:
				{
					cszData.LoadString(IDS_STR_MANUFAC_DATE_TITLE);
					cszData += STR_MAN_DT_EX_SPACE_2;
				}
				break;

			case PDU_HOST_NM:
				{
					cszData.LoadString(IDS_STR_HOST_NM_TITLE);
					cszData += STR_HS_NM_EX_SPACE_9;
				}
				break;

			case PDU_MAC_ADDRS:
				{
					cszData.LoadString(IDS_STR_MAC_ADDR_TITLE);
					cszData += STR_MAC_EX_SPACE_7;
				}
				break;

			case PDU_STAGGER_ON_DELAY:
				{
					cszData.LoadString(IDS_STR_STAGGER_ON_DELAY_TITLE);
					cszData += STR_MAN_DT_EX_SPACE_2;
				}
				break;

			case PDU_STAGGER_OFF_DELAY:
				{
					cszData.LoadString(IDS_STR_STAGGER_OFF_DELAY_TITLE);
					cszData += STR_ST_OFF_EX_SPACE_1;
				}
				break;

			case PDU_FIRMWARE_VERSION:
				{
					cszData.LoadString(IDS_STR_FIRMWARE_VER_TITLE);
					cszData += STR_MAN_DT_EX_SPACE_2;
				}
				break;

			case PDU_TOTAL_OUTLETS:
				{
					cszData.LoadString(IDS_STR_TOT_NUM_OUTLETS_TITLE);
					cszData += STR_SR_NUM_EX_SPACE_5;
				}
				break;

			case PDU_TOTAL_BANKS:
				{
					cszData.LoadString(IDS_STR_TOT_BANKS_TITLE);
					cszData += STR_MAC_EX_SPACE_7;
				}
				break;

			case PDU_OUTLETS_PER_BANK:
				{
					cszData.LoadString(IDS_STR_OUTLETS_PER_BANK_TITLE);
					cszData += STR_MAN_DT_EX_SPACE_2;
				}
				break;

			case PDU_PHASE_INP_CONN_TYPE:
				{
					cszData.LoadString(IDS_STR_PHS_TYPE_TITLE);
					cszData += STR_MOD_NM_EX_SPACE_8;
				}
				break;

		} // switch

		cPDUInfoFile.Write(cszData, cszData.GetLength());

		//-------------------- READ ------------------------------
		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), nNum, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
		cszData.Empty();
		cszData.Format(_T("%s"), byDataBuffer);

		ShowPopupDlg(DLG_POPUP_REFRESH);

		if (nNum == PDU_MODEL_NUM)
		{	// FOR PM10U DEVICE
			CString		cszPM10U;
			CString		cszCheck;

			cszCheck.Empty();
			cszCheck = cszData;
			cszPM10U.Empty();
			cszPM10U = STR_PM10_U_VERS;

			if (cszPM10U.GetLength() <= cszCheck.GetLength())
			{
				cszCheck = cszCheck.Left(cszPM10U.GetLength());
				if (!cszCheck.CompareNoCase(cszPM10U))
				{
					bPM10U = TRUE;
				}
			}
		}
		else if (nNum == PDU_MAC_ADDRS)
		{
			cszLoad.Empty();
			cszLoad.Format(_T("%02x%02x%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3], byDataBuffer[4], byDataBuffer[5]);

			cszData.Empty();
			// Generate MAC Address values
			for (nSize = 0; nSize < cszLoad.GetLength(); nSize++)
			{
				cszData += cszLoad.GetAt(nSize);

				if (cszLoad.GetLength() == nSize + 1)
					break;

				if (nSize % 2)
				{
					cszData += ":";
				}
			}
		}
		else if ((nNum == PDU_STAGGER_ON_DELAY) || (nNum == PDU_STAGGER_OFF_DELAY))
		{
			cszData.Empty();
			cszData.Format(_T("%02x%02x%02x%02x"), byDataBuffer[0], byDataBuffer[1], byDataBuffer[2], byDataBuffer[3]);
			dwTemp = strtoul(cszData.GetBuffer(cszData.GetLength()), NULL, 16);
			
			cszData.Empty();
			cszData.Format(_T("%lu"), dwTemp);
		}
		else if (nNum == PDU_TOTAL_OUTLETS)
		{
			cszData.Empty();
			cszData.Format(_T("%u"), byDataBuffer[0]);
			byTotalOutlets = byDataBuffer[0];
		}
		else if  ((nNum == PDU_TOTAL_BANKS) || (nNum == PDU_OUTLETS_PER_BANK))
		{
			cszData.Empty();
			cszData.Format(_T("%u"), byDataBuffer[0]);
		}
		else if (nNum == PDU_PHASE_INP_CONN_TYPE)
		{
			cszLoad.Empty();
			cszLoad.Format(_T("%u"), byDataBuffer[0]);
			cszLoad += STR_NEXT_LINE;
			cszLoad += STR_NEXT_LINE;
			cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

			cszLoad.Empty();
			cszLoad.LoadString(IDS_STR_CONN_TYPE_TITLE);
			cszLoad += STR_MAN_DT_EX_SPACE_2;
			cszLoad += STR_ST_OFF_EX_SPACE_1;
			cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

			cszLoad.Empty();
			cszLoad.Format(_T("%c"), byDataBuffer[1]);
			cszData.Empty();
			cszData = cszLoad;
		}

		if  (nNum == PDU_TOTAL_BANKS)
		{
			byTotalBanks = byDataBuffer[0];
		}

		cszData += STR_NEXT_LINE;
		cszData += STR_NEXT_LINE;
		cPDUInfoFile.Write(cszData, cszData.GetLength());

		ShowPopupDlg(DLG_POPUP_REFRESH);

	} // for nNum

	// OUTLET NAMES
	cszData.Empty();
	if (bPM10U)
	{
		cszData.LoadString(IDS_STR_FOLL_CIR_NMS);
		byTotalOutlets = PM10U_MAX_OUTLETS;
	}
	else
	{
		cszData.LoadString(IDS_STR_OUTLET_NM_TXT);
	}

	cszData += STR_NEXT_LINE;
	cPDUInfoFile.Write(cszData, cszData.GetLength());

	for (nNum = 0; nNum < byTotalOutlets; nNum++)
	{
		ShowPopupDlg(DLG_POPUP_REFRESH);

		cszLoad.Empty();
		if (bPM10U)
		{
			cszLoad.LoadString(IDS_STR_CIRCUIT_NM);
		}
		else
		{
			cszLoad.LoadString(IDS_STR_OUTLET_NO);
		}

		cszData.Empty();
		cszData.Format(cszLoad, (nNum + 1));
		cszData += STR_SPACE_COLON;
		cszData += STR_HS_NM_EX_SPACE_9;

		cPDUInfoFile.Write(cszData, cszData.GetLength());

		cszLoad.Empty();
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_GET_SET_OUTLET_NAME, byDataBuffer, 0, FALSE, &byErrCode, TRUE, (nNum + 1));
		m_cszPDUAddr.ReleaseBuffer();

		// FOR SAFETY PURPOSE
		if (byDataBuffer[0] == 0xFF)
		{
			memset(byDataBuffer, 0, sizeof(byDataBuffer));
		}

		cszData.Empty();
		cszData.Format(_T("%s"), byDataBuffer);
		cszData += STR_NEXT_LINE;
		cPDUInfoFile.Write(cszData, cszData.GetLength());
	}

	// Write Phase Title Information
	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_PHAS_INFO_TIT);
	cszLoad = STR_NEXT_LINE + cszLoad; 
	cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

	// Read Product Group Name
	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_PROD_GROUP_NM);
	cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_PRODUCT_GROUP, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszData.Empty();
	cszData.Format(_T("%s"), byDataBuffer);
	cszData += STR_NEXT_LINE;
	cPDUInfoFile.Write(cszData, cszData.GetLength());

	// Read Bank Phase Info
	for (nNum = 1; nNum <= byTotalBanks; nNum++)
	{
		ShowPopupDlg(DLG_POPUP_REFRESH);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_PHS_ID_BANK);
		
		cszData.Empty();
		cszData.Format(cszLoad, nNum);

		cPDUInfoFile.Write(cszData, cszData.GetLength());

		cszLoad.Empty();
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_BANK_PHASE_ID, byDataBuffer, 0, FALSE, &byErrCode, TRUE, nNum);
		m_cszPDUAddr.ReleaseBuffer();

		// FOR SAFETY PURPOSE
		if (byDataBuffer[0] == 0xFF)
		{
			memset(byDataBuffer, 0, sizeof(byDataBuffer));
		}

		cszData.Empty();
		cszData.Format(_T("%s"), byDataBuffer);
		cszData += STR_NEXT_LINE;
		cPDUInfoFile.Write(cszData, cszData.GetLength());
	}

	// Read PDU Capacity
	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_CAP_PDU_TIT);
	cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_CAPACITY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszData.Empty();
	cszData.Format(_T("%u"), byDataBuffer[0]);
	cszData += STR_NEXT_LINE;
	cPDUInfoFile.Write(cszData, cszData.GetLength());

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// Read Bank Capacity
	for (nNum = 1; nNum <= byTotalBanks; nNum++)
	{
		ShowPopupDlg(DLG_POPUP_REFRESH);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_CAP_BANK_TIT);
		
		cszData.Empty();
		cszData.Format(cszLoad, nNum);

		cPDUInfoFile.Write(cszData, cszData.GetLength());

		cszLoad.Empty();
		memset(byDataBuffer, 0, sizeof(byDataBuffer));

		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_BANK_CAPACITY, byDataBuffer, 0, FALSE, &byErrCode, TRUE, nNum);
		m_cszPDUAddr.ReleaseBuffer();

		// FOR SAFETY PURPOSE
		if (byDataBuffer[0] == 0xFF)
		{
			memset(byDataBuffer, 0, sizeof(byDataBuffer));
		}

		cszData.Empty();
		cszData.Format(_T("%u"), byDataBuffer[0]);
		cszData += STR_NEXT_LINE;
		cPDUInfoFile.Write(cszData, cszData.GetLength());
	}

	// Read Outlet Capacity
	for (nNum = 1; nNum <= byTotalOutlets; nNum++)
	{
		ShowPopupDlg(DLG_POPUP_REFRESH);

		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_CAP_OUTLET_TIT);
		
		cszData.Empty();
		cszData.Format(cszLoad, nNum);

		cPDUInfoFile.Write(cszData, cszData.GetLength());

		memset(byDataBuffer, 0, sizeof(byDataBuffer));
		PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_OUTLET_CAPACITY, byDataBuffer, 0, FALSE, &byErrCode, TRUE, nNum);
		m_cszPDUAddr.ReleaseBuffer();

		// FOR SAFETY PURPOSE
		if (byDataBuffer[0] == 0xFF)
		{
			memset(byDataBuffer, 0, sizeof(byDataBuffer));
		}

		cszData.Empty();
		cszData.Format(_T("%u"), byDataBuffer[0]);
		cszData += STR_NEXT_LINE;
		cPDUInfoFile.Write(cszData, cszData.GetLength());
	}

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// Read MONITOR ONLY
	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_MONITOR_ONLY_TIT);
	cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

	cszData.Empty();
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_MONITOR_ONLY, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszData = STR_NO;
	if (byDataBuffer[0] == 0x01)
	{
		cszData.Empty();
		cszData = STR_YES;
	}
	cszData += STR_NEXT_LINE;
	cPDUInfoFile.Write(cszData, cszData.GetLength());

	ShowPopupDlg(DLG_POPUP_REFRESH);

	// Read FULL CONTROL
	cszLoad.Empty();
	cszLoad.LoadString(IDS_STR_FULL_CONTROL);
	cPDUInfoFile.Write(cszLoad, cszLoad.GetLength());

	cszData.Empty();
	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_FULL_CONTROL, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszData = STR_NO;
	if (byDataBuffer[0] == 0x01)
	{
		cszData.Empty();
		cszData = STR_YES;
	}
	cszData += STR_NEXT_LINE;
	cPDUInfoFile.Write(cszData, cszData.GetLength());

	ShowPopupDlg(DLG_POPUP_REFRESH);

	cPDUInfoFile.Close();

	ShowPopupDlg(DLG_POPUP_DESTROY);

	ShellExecute(this->m_hWnd, STR_OPEN_TITLE, cszPDUInfoFilePath, NULL, NULL, SW_SHOWNORMAL);
}

void CePower_MFGDlg::ReadFirmwareVer()
{
	CString					cszLoad;
	BYTE					byErrCode = 0;
	BYTE					byDataBuffer[66] = {0};

	memset(byDataBuffer, 0, sizeof(byDataBuffer));
	PduStaticInfoWriteAndRead(m_cszPDUAddr.GetBuffer(m_cszPDUAddr.GetLength()), PDU_FIRMWARE_VERSION, byDataBuffer, 0, FALSE, &byErrCode, TRUE);
	cszLoad.Empty();
	cszLoad.Format(_T("%s"), byDataBuffer);

	SetDlgItemText(IDC_EDIT_FIRM_VERS, cszLoad);
}

void CePower_MFGDlg::OnEnChangeStaggerOffDelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_OFF_DELAY);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_DELAY_SECS_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_STAGGER_OFF_DELAY, "");
		m_cEditStaggerOffDelay.SetFocus();
	}
}

void CePower_MFGDlg::OnEnChangeStaggerOnDelay()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	int			nValue = 0;
	CString		cszLoad;

	nValue = GetDlgItemInt(IDC_EDIT_STAGGER_ON_DELAY);

	if ((nValue < 0) || (nValue > 5000))
	{
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_STAG_DELAY_SECS_MSG);
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		SetDlgItemText(IDC_EDIT_STAGGER_ON_DELAY, "");
		m_cEditStaggerOnDelay.SetFocus();
	}
}

BOOL CePower_MFGDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if (pMsg->wParam == VK_RETURN)
	{
		pMsg->wParam = NULL;
	}
	else if (pMsg->wParam == VK_ESCAPE)
	{
		pMsg->wParam = NULL;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CePower_MFGDlg::OnCbnSelchangeDevice()
{
	// TODO: Add your control notification handler code here
	m_nDeviceType = m_cComboDevice.GetCurSel();

	LoadModelSubNum();
	GenerateModelNum();
}

void CePower_MFGDlg::OnBnClickedButtonLoad()
{
	// TODO: Add your control notification handler code here
	WriteCSVFile();
}

CString CePower_MFGDlg::RemoveSplCharFrmBuffer(CString cszBuffer)
{
	UINT		nNum = 0;
	CString		cszLoad;

	cszLoad = cszBuffer;
	for (nNum = 0; nNum < (UINT)m_cszSplCharsList.GetSize(); nNum++)
	{
		cszLoad.Replace(m_cszSplCharsList.ElementAt(nNum), _T(""));
	}

	return cszLoad;
}

void CePower_MFGDlg::OnEnChangeEditModelNm()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString				cszLoad;
	CString				cszTemp;
	CString				cszValue;

	cszValue.Empty();
	m_cEditModelNm.GetWindowText(cszValue);
	cszValue.TrimLeft();
	cszValue.TrimRight();

	if (CheckBufferWithSplChar(cszValue))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MODEL_NM_ERR_MSG);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);

		cszValue = RemoveSplCharFrmBuffer(cszValue);
		m_cEditModelNm.SetWindowText(cszValue);
		m_cEditModelNm.SetSel(cszValue.GetLength(), cszValue.GetLength());
	}

}

void CePower_MFGDlg::OnEnChangeEditSerNum()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString				cszLoad;
	CString				cszTemp;
	CString				cszValue;

	cszValue.Empty();
	m_cEditSerialNum.GetWindowText(cszValue);
	cszValue.TrimLeft();
	cszValue.TrimRight();

	if (CheckBufferWithSplChar(cszValue))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_SERIAL_NUM_ERR);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);

		cszValue = RemoveSplCharFrmBuffer(cszValue);
		m_cEditSerialNum.SetWindowText(cszValue);
		m_cEditSerialNum.SetSel(cszValue.GetLength(), cszValue.GetLength());
	}


}

void CePower_MFGDlg::OnEnChangeEditManuDate()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString				cszLoad;
	CString				cszTemp;
	CString				cszValue;

	cszValue.Empty();
	m_cEditManufacDate.GetWindowText(cszValue);
	cszValue.TrimLeft();
	cszValue.TrimRight();

	if (CheckBufferWithSplChar(cszValue))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_MANUF_DATE_ERR);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);

		cszValue = RemoveSplCharFrmBuffer(cszValue);
		m_cEditManufacDate.SetWindowText(cszValue);
		m_cEditManufacDate.SetSel(cszValue.GetLength(), cszValue.GetLength());
	}
}

void CePower_MFGDlg::OnEnChangeEditHostNm()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString				cszLoad;
	CString				cszTemp;
	CString				cszValue;

	cszValue.Empty();
	m_cEditHostNm.GetWindowText(cszValue);
	cszValue.TrimLeft();
	cszValue.TrimRight();

	if (CheckBufferWithSplChar(cszValue))
	{
		cszTemp.Empty();
		cszTemp.LoadString(IDS_STR_BUFFER_SP_ERR);
		cszLoad.Empty();
		cszLoad.LoadString(IDS_STR_HOST_NM_ERR);
		cszTemp += cszLoad;
		AfxMessageBox(cszTemp, MB_OK | MB_ICONSTOP);

		cszValue = RemoveSplCharFrmBuffer(cszValue);
		m_cEditHostNm.SetWindowText(cszValue);
		m_cEditHostNm.SetSel(cszValue.GetLength(), cszValue.GetLength());
	}


}

void CePower_MFGDlg::OnEnChangeEditMacAddr()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CString				cszValue;

	cszValue.Empty();
	m_cEditMACAddr.GetWindowText(cszValue);
	if (!cszValue.IsEmpty())
	{
		cszValue.Replace(_T("#"), _T(""));
		m_cEditMACAddr.SetWindowText(cszValue);
		m_cEditMACAddr.SetSel(cszValue.GetLength(), cszValue.GetLength());
	}
}

void CePower_MFGDlg::ShowPopupDlg(BYTE byType)
{
	if (byType == DLG_POPUP_CREATE)
	{
		m_cPopUpWnd = new CPopUp();

		if (m_cPopUpWnd)
		{
			m_cPopUpWnd->Create(IDD_DLG_POPUP, NULL);
			m_cPopUpWnd->AnimateWindow(600, AW_ACTIVATE | AW_BLEND);
			m_cPopUpWnd->ShowWindow(TRUE);
		}
	}
	else if (byType == DLG_POPUP_REFRESH)
	{
		MSG				msg;
		MSG				msg1;

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_cPopUpWnd)
			{
				if (!::IsDialogMessage(m_cPopUpWnd->m_hWnd, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

			if (!::IsDialogMessage(this->m_hWnd, &msg1))
			{
				TranslateMessage(&msg1);
				DispatchMessage(&msg1);
			}
		}
	}
	else if (byType == DLG_POPUP_DESTROY)
	{
		if (m_cPopUpWnd)
		{
			m_cPopUpWnd->AnimateWindow(600, AW_HIDE | AW_BLEND);
			m_cPopUpWnd->DestroyWindow();
			delete m_cPopUpWnd;
			m_cPopUpWnd = NULL;
		}
	}
}

void CePower_MFGDlg::OnBnClickedBtnRefresh()
{
	// TODO: Add your control notification handler code here
	RefreshPDUList();
}

void CePower_MFGDlg::RefreshPDUList()
{
	WORD						wPid = 0;
	WORD						wVid = 0;
	BYTE						byErrCode = 0;
	CString						cszLoad;
	BOOL 						bRetStatus = FALSE;

	m_bGreen = FALSE;
	cszLoad.Empty();

	if (!LoadVidPidValues())
	{
		cszLoad.Empty();
		cszLoad = _T("Unable to find the Config file CS_Device_Units.ini.");
		AfxMessageBox(cszLoad, MB_OK | MB_ICONSTOP);
		EndDialog(IDCANCEL);
		return;
	}

	m_cButReadPDU.EnableWindow(FALSE);

	if (PduEnumerateDevice(TRUE))
	{
		ReadFirmwareVer();

		m_bGreen = TRUE;
		SetDlgItemText(IDC_STATIC_PDU_DETECT, _T("PDU Device found"));
		m_cButWrite.EnableWindow(TRUE);
	}
	else
	{
		SetDlgItemText(IDC_EDIT_FIRM_VERS, _T(""));

		m_cButWrite.EnableWindow(FALSE);
		SetDlgItemText(IDC_STATIC_PDU_DETECT, _T("No PDU device found"));
	}
}