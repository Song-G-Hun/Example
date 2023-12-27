
// Serial_testDlg.h: 헤더 파일
//

#pragma once


// CSerialtestDlg 대화 상자
class CSerialtestDlg : public CDialogEx
{
// 생성입니다.
public:
	CSerialtestDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERIAL_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 시리얼포트 콤보
	CComboBox m_ComPort;
	afx_msg void OnDropdownCombo2();
	afx_msg void OnBnClickedBtnOpen();
	afx_msg void OnBnClickedBtnTest();
	CString m_LogStr;
	afx_msg void OnBnClickedBtnThread();

	HANDLE m_hReadProcessThread;
	DWORD m_dwThreadID;

	MyQueue m_Queue;
	afx_msg void OnBnClickedBtnQread();
};
