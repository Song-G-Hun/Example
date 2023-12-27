// MsgLog.h: interface for the CMsgLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSGLOG_H__53527D1D_C4A4_4A8B_B432_DD7E95645176__INCLUDED_)
#define AFX_MSGLOG_H__53527D1D_C4A4_4A8B_B432_DD7E95645176__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMsgLog  
{
public:
	void WriteLogFile(char *fmt,...);
	BOOL InitLogFile(CString szFileName);
	CFile m_CFile;
	CMsgLog();
	virtual ~CMsgLog();

};

#endif // !defined(AFX_MSGLOG_H__53527D1D_C4A4_4A8B_B432_DD7E95645176__INCLUDED_)
