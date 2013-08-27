#pragma once

#include <afxsock.h>
#include "MySocket.h"
#include "MyListen.h"
#include "packet.h"
// ServerConf 대화 상자입니다.

class ServerConf : public CDialogEx
{
	DECLARE_DYNAMIC(ServerConf)

public:
	ServerConf(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~ServerConf();
	
	int m_deviceFlag; // 모니터인가, 스마트폰 그림인가를 판별하는 플래그 ( -1 : 초기값, 0 : 모니터, 1 : 스마트폰 )
	bool m_bDragFlag; // 드래그 판별 플래그
	bool m_applyFlag; // apply 눌렀나 판별하는 플래그
	bool m_startFlag; // 서버 구동중인가 아닌가를 판별하는 플래그
	int m_settingFlag[9]; // 버튼에 모니터 or 스마트폰 or 빈 칸인지 판별하는 플래그 ( -1 : 초기값, 0 : 모니터, 1 : 스마트폰 )
	int m_whereisPoint; // 마우스가 어느 위치에 있느냐? 기본값 : 5, 1 ~ 9 까지있음 , 이 변수의 -1 한 값이 아래 배열의 인덱스가 됨(클라이언트ID)
	
	int nSocket; // 클라이언트가 몇 명 들어왔는지 판별하는 변수
	int client_id[9]; // 각 버튼마다 몇 번의 클라이언트 id가 저장되어 있는지 저장하는 배열
	// 인덱스 + 1 : 버튼번호, 값 : 클라이언트 ID 

	//typedef CTypedPtrArray<CObArray, CMySocket *> CSocketList;
	
	CPoint m_ptltemText; // 특정 아이템 텍스트의 좌표
	
	HINSTANCE hinstDLL; // DLL 로딩
	HHOOK hHook; // HHOOK

	BOOL m_keyBoardHook;
	BOOL m_mouseHook;

	CMyListen listen;

	WINDOWPLACEMENT getCoord[11];

	CWnd *btnControl[9];
	CWnd *picControl[2];


	typedef struct tagHEVENT{ // DLL 파일에서 받아오는 구조체 ( 키코드 포함 )
		int type;
		int keyCode;
		WPARAM data;
		LPARAM lParam;
	}HEVENT;


	KPACKET keyP;
	MPACKET mouseP;
	CPACKET clientP;
	DPACKET dataP;
	
	CMyThread *cThread;
	CObList *tmp;

	virtual BOOL PreTranslateMessage(MSG* pMsg);
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedCancel();
	CButton m_CButton_one;
	CButton m_CButton_two;
	CButton m_CButton_three;
	CButton m_CButton_four;
	CButton m_CButton_five;
	CButton m_CButton_six;
	CButton m_CButton_seven;
	CButton m_CButton_eight;
	CButton m_CButton_nine;
	CString m_serverPortEdit;
	afx_msg void OnBnClickedPortApply();
	CButton m_CButton_portApply;
	afx_msg void OnBnClickedPortCancel();
	CButton m_CButton_portCancel;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CEdit m_portEditControl;
	CString serverIPAddress;
	void initServer(int nPort);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	//afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	void closeClient(CMySocket *s);
	void receiveData(CMySocket *s);
	CButton m_CBtn_Start;
	CButton m_CBtn_stop;
	void initFlag(void);
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	KPACKET packMessage(int msgType, int sendDev, int recvDev, int devType, int relativeField, int updownFlag, int pad1, int keyCode, int pad2, int pad3);
	void unpackMessage(KPACKET p, CMySocket *s);
	//afx_msg BOOL OnCopyData(CWnd *pWnd, COPYDATASTRUCT *pCopyDataStruct);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnBnClickedButton14();
};
