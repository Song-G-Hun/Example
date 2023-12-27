
// Serial_testDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Serial_test.h"
#include "Serial_testDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSerialtestDlg 대화 상자



CSerialtestDlg::CSerialtestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERIAL_TEST_DIALOG, pParent)
	, m_LogStr(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_ComPort);
	DDX_Text(pDX, IDC_EDIT_LOG, m_LogStr);
}

BEGIN_MESSAGE_MAP(CSerialtestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_DROPDOWN(IDC_COMBO2, &CSerialtestDlg::OnDropdownCombo2)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CSerialtestDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_TEST, &CSerialtestDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BTN_THREAD, &CSerialtestDlg::OnBnClickedBtnThread)
	ON_BN_CLICKED(IDC_BTN_QREAD, &CSerialtestDlg::OnBnClickedBtnQread)
END_MESSAGE_MAP()


// CSerialtestDlg 메시지 처리기

BOOL CSerialtestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_ComPort.ResetContent();
	DWORD dwPortCnt = myPort.AddPortFromRegToCboBox(0,&m_ComPort);
	if (dwPortCnt > 0)
		m_ComPort.SetCurSel(0); //첫번째 리스트를 선택
	else
		AfxMessageBox("시리얼포트가 없습니다.");

	m_Queue.InitQueue();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CSerialtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSerialtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSerialtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSerialtestDlg::OnDropdownCombo2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ComPort.ResetContent();
	DWORD dwPortCnt = myPort.AddPortFromRegToCboBox(0, &m_ComPort);
	if (dwPortCnt > 0)
		m_ComPort.SetCurSel(0); //첫번째 리스트를 선택
	else
		AfxMessageBox("시리얼포트가 없습니다.");

}


void CSerialtestDlg::OnBnClickedBtnOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	myPort.m_dwBaud = 115200; //bps
	myPort.m_dwData = 8; //bit
	myPort.m_dwParity = 0; //없음
	myPort.m_dwStop = ONESTOPBIT;
	myPort.m_dwFlowCtl = 0; //없음

	GetDlgItemText(IDC_COMBO2, myPort.m_csPortName); // 포트번호

	if (myPort.Open() != TRUE)
	{
		AfxMessageBox("시리얼포트 OPEN 실패!");
		return;
	}
	UpdateData(TRUE);
	m_LogStr = "포트 열림!\n";
	UpdateData(FALSE);
}


void CSerialtestDlg::OnBnClickedBtnTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	BYTE baData[] = "1234ABCE";
	myPort.WritePort(baData, sizeof(baData)-1);
	Sleep(100);
	BYTE baRead[100] = { 0, };
	/*int len = myPort.ReadPort(baRead, sizeof(baRead));

	
	UpdateData(TRUE);
	CString tmpStr = "";

	m_LogStr += "\r\n데이터 읽기 : ";
	for (int i = 0; i < len; i++) {
		tmpStr.Format("%c", baRead[i]);
		m_LogStr += tmpStr;
	}
	UpdateData(FALSE);*/

}

void Func_SerialReadThead(LPVOID pParam)
{
	CSerialtestDlg* pCls = (CSerialtestDlg*)pParam;

	while (1)
	{
		//read를 실행

		BYTE baRead[100] = { 0, };
		int recvLen = myPort.ReadPort(baRead, sizeof(baRead));
		
		if (recvLen > 0)
		{
			
			for (int i = 0; i < recvLen; i++) {
				pCls->m_Queue.PushData(baRead[i]);
			
			}
			recvLen = recvLen;

			//SendMessage 
		}
		
	}
}

void CSerialtestDlg::OnBnClickedBtnThread()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_hReadProcessThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Func_SerialReadThead,this,0,&m_dwThreadID);
	if (m_hReadProcessThread == NULL)
	{
		AfxMessageBox("스레드 생성 실패!");
	}
	else {

	}
}


void CSerialtestDlg::OnBnClickedBtnQread()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다. 
	if (!m_Queue.IsEmpty())
	{
		int Qsize = m_Queue.GetSize();
		UpdateData(TRUE);
		CString tmpStr = "";

		m_LogStr += "\r\n큐 데이터 읽기 : ";
		for (int i = 0; i < Qsize; i++) {
			tmpStr.Format("%c", m_Queue.PopData());
			m_LogStr += tmpStr;
		}
		UpdateData(FALSE); 		
	}
}
