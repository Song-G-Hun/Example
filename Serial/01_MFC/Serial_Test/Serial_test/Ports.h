// Ports.h: interface for the CPorts class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PORTS_H__E73E7CD1_D9DB_407D_AF9D_6B26EB3707BA__INCLUDED_)
#define AFX_PORTS_H__E73E7CD1_D9DB_407D_AF9D_6B26EB3707BA__INCLUDED_

#include "MyQueue.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*for port add from windows registry*/
#define MAX_KEY_LENGTH		255
#define MAX_VALUE_LENGTH	16383
#define REG_SERIALPORT		_T("HARDWARE\\DEVICEMAP\\SERIALCOMM")			
#define REG_PARAPORT		_T("HARDWARE\\DEVICEMAP\\PARALLEL PORTS")		

class CPorts  
{
public:
	BOOL GetPacketFormUSB();
	HANDLE m_hRead;
	BOOL bUsbI_interface;
	int m_dwStop;
	DWORD m_dwThreadId;
	DWORD AddPortFromRegToCboBox(BYTE ucType,CComboBox* pCbo);
	BYTE m_csUsbInterfaceName[1024];
	BYTE m_hwPgmType;
	HWND m_hDlgWnd;
	HANDLE m_hThreadWatchComm;
	MyQueue m_Queue;
	DWORD ReadPort(BYTE* pBuff, DWORD dwToRead);
	volatile BOOL m_bConnected;
	volatile BOOL m_bRead;
	OVERLAPPED m_osRead;
	OVERLAPPED m_osWrite;
	DWORD WritePort(BYTE* pBuff, DWORD dwToWrite);
	int m_dwDevice;
	HANDLE m_hPort;			/*Opened port handle*/
	int m_dwFlowCtl;		/*flow control type in COM port*/
	int m_dwData;			/*data lenth in COM port*/
	int m_dwParity;			/*parity bit in COM port*/
	BOOL Close();			/*port close*/
	CString m_csPortName;	/*control port name*/
	int m_dwBaud;			/*baudrate in COM port*/
	BOOL Open();			/*port open by member vars setting*/
	CPorts();
	virtual ~CPorts();
};

DWORD	ThreadWatchComm(CPorts* pMyPort);
/*DWORD	ThreadWatchComm2(CPorts* pMyPort);*/

#endif // !defined(AFX_PORTS_H__E73E7CD1_D9DB_407D_AF9D_6B26EB3707BA__INCLUDED_)
