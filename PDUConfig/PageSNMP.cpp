// PageSNMP.cpp : implementation file
//

#include "stdafx.h"
#include "PDUConfig.h"
#include "PageSNMP.h"


// CPageSNMP dialog

IMPLEMENT_DYNAMIC(CPageSNMP, CDialog)

CPageSNMP::CPageSNMP(CWnd* pParent /*=NULL*/)
	: CDialog(CPageSNMP::IDD, pParent)
{
	m_ctrlDateHired = NULL;
	m_ctrlTimeHired = NULL;
}

CPageSNMP::~CPageSNMP()
{
	if (m_ctrlDateHired)
	{
		delete m_ctrlDateHired;
		m_ctrlDateHired = NULL;
	}

	if (m_ctrlTimeHired)
	{
		delete m_ctrlTimeHired;
		m_ctrlTimeHired = NULL;
	}
}

void CPageSNMP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SNMP_TRAP, m_cChkSNMPTrap);
	DDX_Control(pDX, IDC_IPADD_TRAP_IP, m_cReceIPAddr);
	DDX_Control(pDX, IDC_EDIT_SNMP_TRAP_PORT, m_cEditSnmpTrapPortNum);
	DDX_Control(pDX, IDC_CHECK_ENB_NTP_DT_TM, m_cBtnEnbNTPDateTime);
	DDX_Control(pDX, IDC_EDIT_DT_TM_SERVERS, m_cEditDateTimeServers);
	DDX_Control(pDX, IDC_COMBO_TIME_ZONE, m_cComboTimeZone);
}


BEGIN_MESSAGE_MAP(CPageSNMP, CDialog)
	ON_BN_CLICKED(IDC_CHECK_SNMP_TRAP, &CPageSNMP::OnBnClickedCheckSnmpTrap)
	ON_BN_CLICKED(IDC_CHECK_ENB_NTP_DT_TM, &CPageSNMP::OnBnClickedCheckEnbNtpDtTm)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPageSNMP message handlers

BOOL CPageSNMP::PreTranslateMessage(MSG* pMsg)
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

void CPageSNMP::OnBnClickedCheckSnmpTrap()
{
	// TODO: Add your control notification handler code here
	if (m_cChkSNMPTrap.GetCheck() == BST_CHECKED)
	{
		EnableSNMPTrapSettings(TRUE);
	}
	else
	{
		EnableSNMPTrapSettings(FALSE);
	}
}

void CPageSNMP::EnableSNMPTrapSettings(BOOL bEnabled)
{
	GetDlgItem(IDC_STATIC_RECEIVER_IP_ADD)->EnableWindow(bEnabled);
	GetDlgItem(IDC_IPADD_TRAP_IP)->EnableWindow(bEnabled);
	GetDlgItem(IDC_STATIC_PORT)->EnableWindow(bEnabled);
	GetDlgItem(IDC_EDIT_SNMP_TRAP_PORT)->EnableWindow(bEnabled);
}

BOOL CPageSNMP::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_cChkSNMPTrap.SetCheck(BST_CHECKED);
	OnBnClickedCheckSnmpTrap();

	m_ctrlDateHired = new CDateTimeCtrl;
	m_ctrlTimeHired = new CDateTimeCtrl;

    m_ctrlDateHired->Create(WS_CHILD | WS_VISIBLE, CRect(155, 348, 344, 371), this, IDC_DATE_HIRED);
	m_ctrlTimeHired->Create(WS_CHILD | WS_VISIBLE | DTS_TIMEFORMAT, CRect(354, 348, 422, 371), this, IDC_TIME_HIRED);

#if 0	// FIND EXACT POSITION		TIME --> 236, 214
	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_EDIT7);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(&rect);
#endif

	m_ctrlDateHired->SetMonthCalColor(MCSC_TITLEBK, RGB(0, 0, 128));
	m_ctrlDateHired->SetMonthCalColor(MCSC_MONTHBK, RGB(70, 170, 255));
	m_ctrlDateHired->SetMonthCalColor(MCSC_TEXT, RGB(250, 240, 50));
	m_ctrlDateHired->SetMonthCalColor(MCSC_TITLETEXT, RGB(255, 255, 0));
	m_ctrlDateHired->SetMonthCalColor(MCSC_BACKGROUND, RGB(190, 225, 240));
	m_ctrlDateHired->SetMonthCalColor(MCSC_TRAILINGTEXT, RGB(150, 200, 255));

	m_ctrlDateHired->SetFormat(_T("MM-dd-yyyy"));
	m_ctrlTimeHired->SetFormat(_T("HH:mm"));

	AddAllTimeZones();
	m_cComboTimeZone.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPageSNMP::OnBnClickedCheckEnbNtpDtTm()
{
	// TODO: Add your control notification handler code here
	BOOL			bEnable = FALSE;

	if (m_cBtnEnbNTPDateTime.GetCheck() == BST_CHECKED)
	{
		bEnable = TRUE;
	}

	GetDlgItem(IDC_STATIC_NTP_SERVER)->EnableWindow(bEnable);
	m_cEditDateTimeServers.EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_TIME_ZONE)->EnableWindow(bEnable);
	m_cComboTimeZone.EnableWindow(bEnable);

	GetDlgItem(IDC_STATIC_SET_DT_TM)->EnableWindow(!bEnable);
	m_ctrlDateHired->EnableWindow(!bEnable);
	m_ctrlTimeHired->EnableWindow(!bEnable);
}

void CPageSNMP::AddAllTimeZones()
{
	CString			cszLoad;

	cszLoad.Empty();
	cszLoad = _T("pool.ntp.org");
	cszLoad += _T("\r\n");
	cszLoad += _T("nist1.symmetricom.com");
	cszLoad += _T("\r\n");
	cszLoad += _T("nist1-sj.ustiming.org");
	m_cEditDateTimeServers.SetWindowText(cszLoad);

	m_cComboTimeZone.ResetContent();

	m_cComboTimeZone.AddString(_T("GMT -12:00 Eniwetok, Kwajalein"));
	m_cComboTimeZone.AddString(_T("GMT -11:00 Midway Island, Samoa"));
	m_cComboTimeZone.AddString(_T("GMT -10:00 Hawaii"));
	m_cComboTimeZone.AddString(_T("GMT -9:00 Alaska"));
	m_cComboTimeZone.AddString(_T("GMT -8:00 Pacific Time (US & Canada)"));
	m_cComboTimeZone.AddString(_T("GMT -7:00 Mountain Time (US & Canada)"));
	m_cComboTimeZone.AddString(_T("GMT -6:00 Central Time (US & Canada), Mexico City"));
	m_cComboTimeZone.AddString(_T("GMT -5:00 Eastern Time (US & Canada), Bogota, Lima"));
	m_cComboTimeZone.AddString(_T("GMT -4:00 Atlantic Time (Canada), La Paz, Santiago"));
	m_cComboTimeZone.AddString(_T("GMT -3:30 Newfoundland"));
	m_cComboTimeZone.AddString(_T("GMT -3:00 Brazil, Buenos Aires, Georgetown"));
	m_cComboTimeZone.AddString(_T("GMT -2:00 Mid-Atlantic"));
	m_cComboTimeZone.AddString(_T("GMT -1:00 Azores, Cape Verde Islands"));
	m_cComboTimeZone.AddString(_T("GMT Western Europe Time, London, Lisbon, Casablanca, Greenwich"));
	m_cComboTimeZone.AddString(_T("GMT +1:00 Brussels, Copenhagen, Madrid, Paris"));
	m_cComboTimeZone.AddString(_T("GMT +2:00 Kaliningrad, South Africa, Cairo"));
	m_cComboTimeZone.AddString(_T("GMT +3:00 Baghdad, Riyadh, Moscow, St. Petersburg"));
	m_cComboTimeZone.AddString(_T("GMT +3:30 Tehran"));
	m_cComboTimeZone.AddString(_T("GMT +4:00 Abu Dhabi, Muscat, Yerevan, Baku, Tbilisi"));
	m_cComboTimeZone.AddString(_T("GMT +4:30 Kabul"));
	m_cComboTimeZone.AddString(_T("GMT +5:00 Ekaterinburg, Islamabad, Karachi, Tashkent"));
	m_cComboTimeZone.AddString(_T("GMT +5:30 Mumbai, Kolkata, Chennai, New Delhi"));
	m_cComboTimeZone.AddString(_T("GMT +5:45 Kathmandu"));
	m_cComboTimeZone.AddString(_T("GMT +6:00 Almaty, Dhaka, Colombo"));
	m_cComboTimeZone.AddString(_T("GMT +7:00 Bangkok, Hanoi, Jakarta"));
	m_cComboTimeZone.AddString(_T("GMT +8:00 Beijing, Perth, Singapore, Hong Kong"));
	m_cComboTimeZone.AddString(_T("GMT +9:00 Tokyo, Seoul, Osaka, Sapporo, Yakutsk"));
	m_cComboTimeZone.AddString(_T("GMT +9:30 Adelaide, Darwin"));
	m_cComboTimeZone.AddString(_T("GMT +10:00 Eastern Australia, Guam, Vladivostok"));
	m_cComboTimeZone.AddString(_T("GMT +11:00 Magadan, Solomon Islands, New Caledonia"));
	m_cComboTimeZone.AddString(_T("GMT +12:00 Auckland, Wellington, Fiji, Kamchatka"));
	m_cComboTimeZone.AddString(_T("GMT +13:00 Samoa"));
	m_cComboTimeZone.AddString(_T("GMT +14:00 Kiribati"));
}

HBRUSH CPageSNMP::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
	if (pWnd)
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_NOTE)
		{
			pDC->SetTextColor(RGB(0, 0, 255));
		}
	}

	// TODO:  Return a different brush if the default is not desired
	return hbr;
}

void CPageSNMP::CopyDateTime(CString cszDateTimeStr)
{
	TCHAR	nChar = 0;
	int		nDate = 0;
	int		nMonth = 0;
	int		nYear = 0;
	int		nHour = 0;
	int		nMinute = 0;

	//cszDateTimeStr = _T("%m-%d-%Y %H:%M");
	sscanf_s(cszDateTimeStr.GetBuffer(cszDateTimeStr.GetLength()), _T("%u %c %d %c %d %d %c %d"), &nMonth, &nChar, sizeof(nChar), &nDate, &nChar, sizeof(nChar), &nYear, &nHour, &nChar, sizeof(nChar), &nMinute);
	cszDateTimeStr.ReleaseBuffer();

	m_ctrlDateHired->SetTime(COleDateTime(nYear, nMonth, nDate, nHour, nMinute, 0));
	m_ctrlTimeHired->SetTime(COleDateTime(nYear, nMonth, nDate, nHour, nMinute, 0));
}