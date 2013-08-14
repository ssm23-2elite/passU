#pragma once

// CPassUListen 명령 대상입니다.

#pragma once

#include "PassUSocket.h"
#include "PassUThread.h"

class CPassUListen : public CAsyncSocket
{
public:
	CPassUListen();
	virtual ~CPassUListen();

	virtual void OnAccept(int nErrorCode);
};


