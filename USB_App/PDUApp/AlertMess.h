#pragma once


// CAlertMess dialog

class CAlertMess : public CDialog
{
	DECLARE_DYNAMIC(CAlertMess)

public:
	CAlertMess(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAlertMess();

// Dialog Data
	enum { IDD = IDD_ALERT_POPUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
