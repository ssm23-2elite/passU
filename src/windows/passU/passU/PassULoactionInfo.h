#pragma once


// CPassULoactionInfo

class CPassULoactionInfo : public CButton
{
	DECLARE_DYNAMIC(CPassULoactionInfo)

public:
	CPassULoactionInfo();
	virtual ~CPassULoactionInfo();

protected:
	DECLARE_MESSAGE_MAP()
public:
	int getStatus(void);
	int getPosition(void);
	void setStatus(int status);
	void setPosition(int position);
	int m_status;
	int m_position;
};


