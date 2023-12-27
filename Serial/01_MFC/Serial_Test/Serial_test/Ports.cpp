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
		if(!GetCommState(m_hPort, &dcb))//--> ���� ������ �� �߿���..
		{
			myLog.WriteLogFile("CPorts::Open() GetCommState failed.\r\n");
			CloseHandle(m_hPort);
			return FALSE;
		}

		//--> ���ڿ� ���� ��Ʈ ����
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

		//���� ������ ���� 
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

	//--> ��Ʈ�� ������� ���� �����̸�..
	if(m_hPort ==INVALID_HANDLE_VALUE)		
		return 0;
	
	if(m_csPortName.Left(3) == "COM" )/*if COM port*/
	{
		//--> ���ڷ� ���� ������ ������ nToWrite ��ŭ ����.. �� ������.,dwWrite �� �ѱ�.
		if( !WriteFile( m_hPort, pBuff, dwToWrite, &dwWritten, &m_osWrite))
		{
			//--> ���� ������ ���ڰ� ������ ���..
			dwError = GetLastError();
			if (dwError == ERROR_IO_PENDING)
			{
				// ���� ���ڰ� ���� �ְų� ������ ���ڰ� ���� ���� ��� Overapped IO��
				// Ư���� ���� ERROR_IO_PENDING ���� �޽����� ���޵ȴ�.
				//timeouts�� ������ �ð���ŭ ��ٷ��ش�.
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

	//--> ���� ��Ʈ�� ������ ������ ����..
	return dwWritten;
}

DWORD CPorts::ReadPort(BYTE *pBuff, DWORD dwToRead)
{
	DWORD	dwRead, dwError, dwErrorFlags;
	COMSTAT	comstat;

	//--- system queue�� ������ byte���� �̸� �д´�.
	ClearCommError( m_hPort, &dwErrorFlags, &comstat);

	//--> �ý��� ť���� ���� �Ÿ��� ������..
	dwRead = comstat.cbInQue;
	
	if(dwRead > 0)
	{
		//--> ���ۿ� �ϴ� �о���̴µ�.. ����..�о���ΰ��� ���ٸ�..
		if( !ReadFile( m_hPort, pBuff, dwRead, &dwToRead, &m_osRead) )
		{
			//--> ���� �Ÿ��� ��������..
			DWORD errNo = GetLastError();
			if (errNo == ERROR_IO_PENDING)
			{
				//--------- timeouts�� ������ �ð���ŭ ��ٷ��ش�.
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

	//--> ���� �о���� ������ ����.
	return dwRead;
}

DWORD	ThreadWatchComm(CPorts* pMyPort)
{
	DWORD		dwEvent;
	OVERLAPPED	os;
	BOOL		bOk = TRUE;
	BYTE		buff[2048];	 // �б� ����
	DWORD		dwRead;	 // ���� ����Ʈ��.


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
		AfxMessageBox("���� ������ ���� ���� "+ pMyPort->m_csPortName);
		return FALSE;
	}

	// ��Ʈ�� �����ϴ� ����.
	while (pMyPort->m_bConnected && pMyPort->m_bRead)
	{
		dwEvent = 0;

		// ��Ʈ�� ���� �Ÿ��� �ö����� ��ٸ���.
		WaitCommEvent( pMyPort->m_hPort, &dwEvent, NULL);
		if ((dwEvent & EV_RXCHAR) == EV_RXCHAR)
		{
			// ��Ʈ���� ���� �� �ִ� ��ŭ �д´�.
			do	
			{
				dwRead = pMyPort->ReadPort( buff, 2048);
				
				if (QUEUE_BUF_SIZE - pMyPort->m_Queue.GetSize() > (int)dwRead)
				{
					for ( DWORD i = 0; i < dwRead; i++)
						pMyPort->m_Queue.PushData(buff[i]);
				}
				else
					AfxMessageBox("���۰� ������");
			  
			} while (dwRead);
			//	�о� ������ �޽����� ������.
			//::SendMessage(pMyPort->m_hDlgWnd,WM_USER_COM_RCV, NULL, NULL);
		}
	}	
	
	// ��Ʈ�� ClosePort�� ���� ������ m_bConnected �� FALSE�� �Ǿ� ����.

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
		
		// Device Key ���´�.
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
