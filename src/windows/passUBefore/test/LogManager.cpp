#include "StdAfx.h"
#include "LogManager.h"
#include "afxinet.h"

BOOL LogManager::instanceFlag = false;
LogManager * LogManager::me = NULL;

LogManager * LogManager::GetInstance()
{
	if(! instanceFlag)
	{
		me = new LogManager();
		instanceFlag = true;
	}
	return me;
}

LogManager::LogManager(){
	this->logFile = NULL;
}

LogManager::~LogManager(void){
	instanceFlag = false;
	this->Close();
}

void LogManager::Log(LPCSTR  mesg, ...)
{
	CStdioFile * pFile = this->logFile;
	CString buffer, buffer2, format;
	CTime t;
	va_list argptr;

	format.Format(_T("%s"), mesg);

	if ( pFile == NULL ) {
		pFile = LogManager::Open();
		if ( pFile == NULL ) {
			AfxMessageBox(_T("Cannot Open Log!!"));
			return;
		}
		this->logFile = pFile;
	}

	t = CTime::GetCurrentTime();
	buffer.Format(_T("%s\t"), t.Format(_T("%Y-%m-%d %H:%M:%S")));
	va_start( argptr, mesg );
	for(int i = 0; i < format.GetLength(); i++){
		if(format.GetAt(i) != '%'){
			buffer2 = " ";
			buffer2.SetAt(0, format.GetAt(i));
			buffer += buffer2;
		} else {
			i++;
			switch(format.GetAt(i)){
				case 'd':
					buffer2.Format(_T("%d"), va_arg(argptr,int) );
					buffer += buffer2;
					break;
				case 's':
					buffer2.Format(_T("%s"), va_arg(argptr, char*) );
					buffer += buffer2;
					break;
				case 'c':
					buffer2.Format(_T("%c"), va_arg(argptr, char) );
					buffer += buffer2;
					break;
			}
		}
	}
	va_end( argptr );
	if(buffer.Find(_T("\r\n")) < 0)
		buffer += _T("\r\n");

	TRACE(buffer);

	pFile->WriteString(buffer);
	pFile->Flush();
	pFile->Close();
}

CStdioFile * LogManager::Open()
{
	CStdioFile * fp = new CStdioFile();
	CFileException e;
	
	CTime t;
	t = CTime::GetCurrentTime();
	CString filePath;
	CString fileName;
	CString progName;
	char csRet[255];   
	ZeroMemory(csRet,sizeof(csRet));

	int ret = ::GetPrivateProfileString( _T("FTP"), _T("LOG_FLD"), NULL, (LPWSTR)csRet, 255, _T(".\\ProgIni.ini") );

	if(ret > 0){
		filePath.Format(_T("%s\\"), csRet);
	} else {
		filePath.Format(_T(".\\\\LOG\\\\"));
	}
	::CreateDirectory(filePath, NULL);
	CWnd * pMain = ::AfxGetApp()->m_pMainWnd;
	pMain->GetWindowText( progName );
	fileName.Format(_T("%s %s.log"), AfxGetAppName(), t.Format(_T("%Y%m%d")));

	::CreateDirectory(filePath, NULL);

	if ( !fp->Open(filePath + fileName,  CFile::modeCreate | 
		                                 CFile::modeNoTruncate | 
		                                 CFile::modeWrite | 
										 CFile::shareDenyNone, &e))
	{
		//Error!!
//		if(e.m_cause == CFileException::sharingViolation)
			if(!fp->Open(filePath + fileName,  CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite, &e)){
				TCHAR szError[1024];
				e.GetErrorMessage(szError, 1024);
				CString buffer;
				buffer.Format( _T("Failed Open Log!! (%d)\n%s\n"), e.m_cause, szError );
//				AfxMessageBox( buffer );
				TRACE( buffer );
				return NULL;
			}
	}

	fp->SeekToEnd();

	return fp;
}

void LogManager::Close(){
	this->logFile->Flush();
	this->logFile->Close();
	delete this->logFile;
}