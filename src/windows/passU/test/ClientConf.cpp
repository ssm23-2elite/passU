// ClientConf.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "test.h"
#include "ClientConf.h"
#include "afxdialogex.h"
	
#include <afxsock.h> // sock 함수 포함하는 라이브러리


// ClientConf 대화 상자입니다.

IMPLEMENT_DYNAMIC(ClientConf, CDialogEx)

ClientConf::ClientConf(CWnd* pParent /*=NULL*/)
	: CDialogEx(ClientConf::IDD, pParent)
	, m_PortNum(_T(""))
{
	AfxSocketInit();
}

ClientConf::~ClientConf()
{

}

void ClientConf::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Text(pDX, IDC_EDIT1, m_PortNum);
	DDV_MaxChars(pDX, m_PortNum, 5);
}


BEGIN_MESSAGE_MAP(ClientConf, CDialogEx)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_IPADDRESS1, &ClientConf::OnIpnFieldchangedIpaddress)
	ON_BN_CLICKED(IDOK, &ClientConf::OnBnClickedOk)
END_MESSAGE_MAP()


// ClientConf 메시지 처리기입니다.


void ClientConf::OnIpnFieldchangedIpaddress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);

		*pResult = 0;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void ClientConf::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BYTE ipFirst, ipSecond, ipThird, ipForth;
	

	UpdateData();

	m_ip.GetAddress(ipFirst, ipSecond, ipThird, ipForth);
	m_address.Format(_T("%d.%d.%d.%d"), ipFirst, ipSecond, ipThird, ipForth);

	int nPort;


	nPort = _ttoi(m_PortNum);

	//AfxMessageBox(nPort);
	AfxMessageBox(m_address);


	//flag = 1;
	clientSock.Create();
	
	clientSock.Connect(m_address, nPort);


	CDialogEx::OnOK();

}