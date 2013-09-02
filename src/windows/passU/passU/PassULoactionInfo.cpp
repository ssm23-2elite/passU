// PassULoactionInfo.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PassU.h"
#include "PassULoactionInfo.h"


// CPassULoactionInfo

IMPLEMENT_DYNAMIC(CPassULoactionInfo, CWnd)

CPassULoactionInfo::CPassULoactionInfo()
{

}

CPassULoactionInfo::~CPassULoactionInfo()
{
}


BEGIN_MESSAGE_MAP(CPassULoactionInfo, CWnd)
END_MESSAGE_MAP()



// CPassULoactionInfo 메시지 처리기입니다.


int CPassULoactionInfo::getStatus(void)
{
	return m_status;
}


int CPassULoactionInfo::getPosition(void)
{
	return m_position;
}


void CPassULoactionInfo::setStatus(int status)
{
	m_status = status;
}


void CPassULoactionInfo::setPosition(int position)
{
	m_position = position;
}
