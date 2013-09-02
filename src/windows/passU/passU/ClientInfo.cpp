#include "stdafx.h"
#include "ClientInfo.h"


CClientInfo::CClientInfo(void)
{
}


CClientInfo::~CClientInfo(void)
{
}


int CClientInfo::getStatus(void)
{
	return status;
}


int CClientInfo::getPosition(void)
{
	return position;
}


void CClientInfo::setStatus(int _status)
{
	status = _status;
}


void CClientInfo::setPosition(int _pos)
{
	position = _pos;
}


int CClientInfo::getID(void)
{
	return clientID;
}


void CClientInfo::setID(int _cID)
{
	clientID = _cID;
}
