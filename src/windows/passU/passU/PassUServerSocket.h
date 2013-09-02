#pragma once

// CPassUServerSocket 명령 대상입니다.

class CPassUServerSocket : public CSocket
{
public:
	CPassUServerSocket(HWND);
	CPassUServerSocket();
	virtual ~CPassUServerSocket();
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);

	HWND m_hWnd;
};


