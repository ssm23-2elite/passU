#pragma once
class CClientInfo
{
public:
	CClientInfo(void);
	~CClientInfo(void);

	int status;
	int position;
	int clientID;
	CString m_address;

	int getStatus(void);
	int getPosition(void);
	void setStatus(int _stat);
	void setPosition(int _pos);
	int getID(void);
	void setID(int cID);
};

