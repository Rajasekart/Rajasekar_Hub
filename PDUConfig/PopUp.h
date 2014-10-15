#pragma once


// CPopUp dialog

class CPopUp : public CDialog
{
	DECLARE_DYNAMIC(CPopUp)

public:
	CPopUp(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPopUp();

// Dialog Data
	enum { IDD = IDD_DLG_POPUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
