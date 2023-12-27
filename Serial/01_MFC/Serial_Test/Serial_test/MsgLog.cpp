// MsgLog.cpp: implementation of the CMsgLog class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "framework.h"
#include "MsgLog.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMsgLog::CMsgLog()
{
	m_CFile.m_hFile = NULL;
}

CMsgLog::~CMsgLog()
{
	if(m_CFile.m_hFile != NULL)
		m_CFile.Close();
}

BOOL CMsgLog::InitLogFile(CString szFileName)
{
	if(!m_CFile.Open(szFileName,CFile::modeWrite|CFile::modeCreate|CFile::modeRead))
	{
		m_CFile.m_hFile = NULL;
		return FALSE;
	}
	return TRUE;
}

void CMsgLog::WriteLogFile(char *fmt, ...)
{
	va_list ap;
	char string[256];
	char string2[256];
	SYSTEMTIME st;
	
	if(m_CFile.m_hFile == NULL)
		return;

	GetLocalTime(&st);
	sprintf_s(string,"[%04d/%02d/%02d %02d:%02d:%02d] ",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	m_CFile.Write(string, strlen(string));	
	strcpy_s(string2,string);
	va_start(ap,fmt);
	vsprintf_s(string,fmt,ap);
	m_CFile.Write(string,strlen(string));
	strcpy_s(string2+strlen(string2),256- strlen(string2),string);
	va_end(ap);	
	m_CFile.Write("\r\n",2);
}
