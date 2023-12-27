// Ports.cpp: implementation of the CPorts class.
//
//////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "framework.h"
#include "Ports.h"
//#include "DlgInput.h"
//#include "usbprintsys.h"
//#include "Descriptior.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPorts::CPorts()
{
	m_csPortName = "";
	m_dwBaud = -1;
	m_dwData = -1;
	m_dwFlowCtl = -1;
	m_dwParity = -1;
	m_hPort = INVALID_HANDLE_VALUE;
	m_bConnected = FALSE;
	m_hThreadWatchComm = INVALID_HANDLE_VALUE;
	m_hDlgWnd = NULL;
	m_csUsbInterfaceName[0] = NULL;
	m_hRead = INVALID_HANDLE_VALUE;
}

CPorts::~CPorts()
{
	m_hDlgWnd = NULL;
	Close();
}

BOOL CPorts::Open()
{
//	DWORD	dwThreadID;

	m_csPortName.MakeUpper();
	if(m_csPortName.Left(3) == "COM")
	{
		if(m_csPortName == "")
			return FALSE;
		if(m_dwBaud == -1)
			return FALSE;
		if(m_dwData == -1)
			return FALSE;
		if(m_dwFlowCtl == -1)
			return FALSE;
		if(m_dwParity == -1)
			return FALSE;	

		DCB				dcb;
		COMMTIMEOUTS	timeouts;
		/*DWORD	dwThreadID;*/
		if (m_hPort != INVALID_HANDLE_VALUE)
		{			
			CloseHandle(m_hPort);
			myLog.WriteLogFile("CPorts::Open() CloseHandle run\r\n");
			Sleep(1000);
		}

		m_hPort = CreateFile("\\\\.\\"+m_csPortName, 
			GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
			NULL);

		if (m_hPort == INVALID_HANDLE_VALUE)
		{
			myLog.WriteLogFile("CPorts::Open() CreateFile error\r\n");
			return FALSE;
			//CloseHandle(m_hPort);
		}

		SetupComm(m_hPort,4086,4086);
		ZeroMemory(&timeouts, sizeof(timeouts));

		timeouts.ReadIntervalTimeout = 0;
		timeouts.ReadTotalTimeoutConstant = 0;
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutMultiplier=100;
		timeouts.WriteTotalTimeoutConstant=0;

		if(!SetCommTimeouts(m_hPort, &timeouts))
		{	
			myLog.WriteLogFile("CPorts::Open() SetComTimeouts failed.\r\n");
			CloseHandle(m_hPort);
			return FALSE;
		}

		ZeroMemory(&dcb, sizeof(dcb));
		if(!GetCommState(m_hPort, &dcb))//--> 현재 설정된 값 중에서..
		{
			myLog.WriteLogFile("CPorts::Open() GetCommState failed.\r\n");
			CloseHandle(m_hPort);
			return FALSE;
		}

		//--> 인자에 의한 포트 설정
		dcb.BaudRate = m_dwBaud;		//Baud Rate
			
		dcb.ByteSize = (BYTE)m_dwData;		//Data Bit

		dcb.StopBits = (BYTE)m_dwStop;		//add 1.5 or 2 stop bit possible 2008.5.19

		dcb.Parity = (BYTE)m_dwParity;		//Parity

		//only use none flow control
		dcb.fOutxCtsFlow = FALSE;
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fDsrSensitivity=FALSE;

		if(!SetCommState(m_hPort, &dcb))
		{
			myLog.WriteLogFile("CPorts::Open() SetCommstate failed(%d).\r\n",GetLastError());
			CloseHandle(m_hPort);
			return FALSE;
		}
		else
		{	
			PurgeComm(m_hPort,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		}

		//수신 스레드 실행 
	/*	m_hThreadWatchComm = CreateThread( NULL, 0, 
			(LPTHREAD_START_ROUTINE)ThreadWatchComm, this, 0, &m_dwThreadId);
		if (!m_hThreadWatchComm)
		{
			//m_hThreadWatchComm
			this->Close();
			return FALSE;
		}*/
	}
	
	m_bConnected = TRUE;
	m_bRead = TRUE;
	return TRUE;
}

BOOL CPorts::Close()
{
	if(m_hRead != INVALID_HANDLE_VALUE){
		m_bConnected = FALSE;
		WaitForSingleObject(m_hRead,INFINITE);
		CloseHandle(m_hRead);
		m_hRead = INVALID_HANDLE_VALUE;
	}

	if(m_hThreadWatchComm && m_hThreadWatchComm != INVALID_HANDLE_VALUE){
		//WaitForSingleObject(m_hThreadWatchComm,INFINITE);
		m_bConnected = FALSE;		
		CloseHandle(m_hThreadWatchComm);
		m_hThreadWatchComm = INVALID_HANDLE_VALUE;
	}

	m_bRead = FALSE;

	if(m_hPort != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hPort);
		m_hPort = /*NULL*/INVALID_HANDLE_VALUE;
		m_bConnected = FALSE;
//		m_hDlgWnd = NULL;
			
		return TRUE;
	}
	
//	myLog.WriteLogFile("CPorts::Close() Invalid Handle\r\n");
	return FALSE;
}

DWORD CPorts::WritePort(BYTE *pBuff, DWORD dwToWrite)
{
	DWORD	dwWritten, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--> 포트가 연결되지 않은 상태이면..
	if(m_hPort ==INVALID_HANDLE_VALUE)		
		return 0;
	
	if(m_csPortName.Left(3) == "COM" )/*if COM port*/
	{
		//--> 인자로 들어온 버퍼의 내용을 nToWrite 만큼 쓰고.. 쓴 갯수를.,dwWrite 에 넘김.
		if( !WriteFile( m_hPort, pBuff, dwToWrite, &dwWritten, &m_osWrite))
		{
			//--> 아직 전송할 문자가 남았을 경우..
			dwError = GetLastError();
			if (dwError == ERROR_IO_PENDING)
			{
				// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의
				// 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
				//timeouts에 정해준 시간만큼 기다려준다.
				while (!GetOverlappedResult(m_hPort, &m_osWrite, &dwWritten, TRUE))
				{				
					dwError = GetLastError();
					if(dwError != ERROR_IO_INCOMPLETE)
					{
						myLog.WriteLogFile("WriteFile Time-out!");
						ClearCommError( m_hPort, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				myLog.WriteLogFile("WriteFile get error result [%d]",dwError);
				dwWritten = 0;
				ClearCommError( m_hPort, &dwErrorFlags, &comstat);
			}
		}
	}

	//--> 실제 포트로 쓰여진 갯수를 리턴..
	return dwWritten;
}

DWORD CPorts::ReadPort(BYTE *pBuff, DWORD dwToRead)
{
	DWORD	dwRead, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--- system queue에 도착한 byte수만 미리 읽는다.
	ClearCommError( m_hPort, &dwErrorFlags, &comstat);

	//--> 시스템 큐에서 읽을 거리가 있으면..
	dwRead = comstat.cbInQue;
	
	if(dwRead > 0)
	{
		//--> 버퍼에 일단 읽어들이는데.. 만일..읽어들인값이 없다면..
		if( !ReadFile( m_hPort, pBuff, dwRead, &dwToRead, &m_osRead) )
		{
			//--> 읽을 거리가 남았으면..
			DWORD errNo = GetLastError();
			if (errNo == ERROR_IO_PENDING)
			{
				//--------- timeouts에 정해준 시간만큼 기다려준다.
				while (! GetOverlappedResult( m_hPort, &m_osRead, &dwRead, TRUE))
				{
					dwError = GetLastError();
					if (dwError != ERROR_IO_INCOMPLETE)
					{
						ClearCommError( m_hPort, &dwErrorFlags, &comstat);
						break;
					}
				}
			}
			else
			{
				dwRead = 0;
				ClearCommError( m_hPort, &dwErrorFlags, &comstat);
			}
		}
	}

	//--> 실제 읽어들인 갯수를 리턴.
	return dwRead;
}

DWORD	ThreadWatchComm(CPorts* pMyPort)
{
	DWORD		dwEvent;
	OVERLAPPED	os;
	BOOL		bOk = TRUE;
	BYTE		buff[2048];	 // 읽기 버퍼
	DWORD		dwRead;	 // 읽은 바이트수.


	/*DeviceIoControl()*/
	memset( &os, 0, sizeof(OVERLAPPED));
	os.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL);
	if (!os.hEvent)
		bOk = FALSE;
	if (! SetCommMask( pMyPort->m_hPort, EV_RXCHAR))
	{
		DWORD err = GetLastError();
		bOk = FALSE;
	}
	if (! bOk)
	{
		AfxMessageBox("수신 스레드 실행 실패 "+ pMyPort->m_csPortName);
		return FALSE;
	}

	// 포트를 감시하는 루프.
	while (pMyPort->m_bConnected && pMyPort->m_bRead)
	{
		dwEvent = 0;

		// 포트에 읽을 거리가 올때까지 기다린다.
		WaitCommEvent( pMyPort->m_hPort, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			// 포트에서 읽을 수 있는 만큼 읽는다.
			do	
			{
				dwRead = pMyPort->ReadPort( buff, 2048);
				
				if (QUEUE_BUF_SIZE - pMyPort->m_Queue.GetSize() > (int)dwRead)
				{
					for ( DWORD i = 0; i < dwRead; i++)
						pMyPort->m_Queue.PushData(buff[i]);
				}
				else
					AfxMessageBox("버퍼가 가득참");
			  
			} while (dwRead);
			//	읽어 가도록 메시지를 보낸다.
			//::SendMessage(pMyPort->m_hDlgWnd,WM_USER_COM_RCV, NULL, NULL);
		}
	}	
	
	// 포트가 ClosePort에 의해 닫히면 m_bConnected 가 FALSE가 되어 종료.

	CloseHandle( os.hEvent);
	//CloseHandle(pMyPort->m_hThreadWatchComm);

	return TRUE;
}

DWORD CPorts::AddPortFromRegToCboBox(BYTE ucType, CComboBox *pCbo)/*ucType 0 : serial, 1 : parallel*/
{
	DWORD cnt = 0;
	CString BaseKey0(REG_SERIALPORT);
    HKEY tempKeyHnd;

    LONG Result, ret;   
	int  idx = 0;    
	TCHAR sVal[MAX_KEY_LENGTH];
	BYTE sData[MAX_VALUE_LENGTH];
	CString strtmp;
	memset(sVal, 0, MAX_KEY_LENGTH);
	memset(sData, 0, MAX_VALUE_LENGTH);

	DWORD dwValSize = MAX_KEY_LENGTH;
	DWORD dwDataSize = MAX_VALUE_LENGTH;

	if(ucType == 0)
    {
		Result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, BaseKey0, 0, KEY_READ, &tempKeyHnd);
		if(Result != ERROR_SUCCESS) 
		{
			myLog.WriteLogFile("Registry open fail");
			return 0;
		}
		
		// Device Key 얻어온다.
		while(1)
		{
			dwValSize = MAX_KEY_LENGTH;
			dwDataSize = MAX_VALUE_LENGTH;
			sVal[0] = '\0';
			sData[0] = '\0';

			ret = RegEnumValue(
				tempKeyHnd,
				idx,
				sVal,
				&dwValSize,
				0,
				NULL,
				sData,
				&dwDataSize
				);
			if (ret == ERROR_SUCCESS)
			{
				strtmp.Format("%s",sData);
				pCbo->AddString(strtmp);
				cnt++;
			}
			else
				break;
			idx++;		
		}
	}
	
	return cnt;
}
