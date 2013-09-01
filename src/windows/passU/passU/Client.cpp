// Client.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "Client.h"
#include "afxdialogex.h"


// CClient 대화 상자입니다.

IMPLEMENT_DYNAMIC(CClient, CDialogEx)

CClient::CClient(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClient::IDD, pParent)
{

}

CClient::~CClient()
{
}

void CClient::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CClient, CDialogEx)
END_MESSAGE_MAP()


// CClient 메시지 처리기입니다.
