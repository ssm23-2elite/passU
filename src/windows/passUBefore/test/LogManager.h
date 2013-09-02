#pragma once

#if !defined(_LOGMANAGER_H_)
#define _LOGMANAGER_H_
#endif

#include "afx.h"

#define LOG LogManager::GetInstance()->Log

class LogManager{
public:
	~LogManager(void);
	void Log(LPCSTR  mesg, ... );
	static LogManager * GetInstance();

protected:
	LogManager();
	CStdioFile * Open();
	void	Close();

	static LogManager * me;
	static BOOL instanceFlag;
	CStdioFile * logFile;
};
