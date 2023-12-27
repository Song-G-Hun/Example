// MyQueue.h: interface for the MyQueue class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYQUEUE_H__3D56DB46_4AFA_4C2E_B144_56896158866F__INCLUDED_)
#define AFX_MYQUEUE_H__3D56DB46_4AFA_4C2E_B144_56896158866F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define QUEUE_BUF_SIZE	0x1000

class MyQueue
{
public:
	DWORD GetSize(void);
	void InitQueue(void);
	BOOL IsEmpty(void);
	void PushData(BYTE ch);
	BYTE PopData(void);
	MyQueue();
	virtual ~MyQueue();

private:
	DWORD pPop;
	DWORD pPush;
	BYTE QBuffer[QUEUE_BUF_SIZE];
};

#endif // !defined(AFX_MYQUEUE_H__3D56DB46_4AFA_4C2E_B144_56896158866F__INCLUDED_)
