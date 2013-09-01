#pragma once


// CClient 대화 상자입니다.

class CClient : public CDialogEx
{
	DECLARE_DYNAMIC(CClient)

public:
	CClient(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CClient();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CLIENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
