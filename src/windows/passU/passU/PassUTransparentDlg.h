#pragma once


// CPassUTransparentDlg 대화 상자입니다.

class CPassUTransparentDlg : public CDialog
{
	DECLARE_DYNAMIC(CPassUTransparentDlg)

public:
	CPassUTransparentDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPassUTransparentDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TRANSPARENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
