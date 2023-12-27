// MyQueue.cpp: implementation of the MyQueue class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "framework.h"
#include "MyQueue.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MyQueue::MyQueue()
{
	memset(QBuffer,0,QUEUE_BUF_SIZE);
	pPop = pPush = 0;
}

MyQueue::~MyQueue()
{
	;
}

BYTE MyQueue::PopData()
{
	BYTE ret;
	ret = QBuffer[pPop++];
	if(pPop == QUEUE_BUF_SIZE)
		pPop = 0;
	return ret;
}

void MyQueue::PushData(BYTE ch)
{
	QBuffer[pPush++] = ch;
	if(pPush == QUEUE_BUF_SIZE)
		pPush = 0;
}

BOOL MyQueue::IsEmpty()
{
	if(pPush == pPop)
		return TRUE;
	return FALSE;
}

void MyQueue::InitQueue()
{
	pPop = pPush = 0;
	memset(QBuffer,0,sizeof(QBuffer));
}

DWORD MyQueue::GetSize()
{
	return ((pPush - pPop + QUEUE_BUF_SIZE)%QUEUE_BUF_SIZE);
}
