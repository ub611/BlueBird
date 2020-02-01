
// TestThreadDlg.cpp : implementation file
//spy��
//postThreadMessage ���� threadId�� ������� �����ٰ���. -> 
//1.msg���Ź��� thread����
//2.ThreadID Lib�� ���� => Lib�ʱ�ȭ�Լ�(init)��* *�Ķ����* ��.
//3.Lib ����� -> lib���� (Uninit() ��...ȣ��)

//showWindowȣ�� -> return���� �ڵ鰪���˷���. -> destroy��Ű�� 


#include "Resource.h"
#include "stdafx.h"
#include "TestThread.h"
#include "TestThreadDlg.h"
#include <map>
#include <vector>
#include<Windows.h>
#include<tchar.h>
#include<iostream>	
#include<MonFuncStaticLib.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#define WM_DIALOG_SHOW			WM_USER + 1  //���̾�αװ��߱�/���̱�
#define WM_APP_EXIT             WM_USER + 2  //���̾�α�����
#define WM_TRAYICON             WM_USER + 3  //Ʈ���̾����ܸ޽���
#define WM_MAIN_TRAY			WM_USER + 4
#endif

bool g_bActive = false;
HINSTANCE global;
HINSTANCE g_Instance =NULL;
HHOOK g_hHook = NULL;
HWND hWnd;



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestThreadDlg dialog




CTestThreadDlg::CTestThreadDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTestThreadDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CAPTION, m_stCaption);
}

BEGIN_MESSAGE_MAP(CTestThreadDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CREATETHREAD, &CTestThreadDlg::OnBnClickedButtonCreatethread)
	ON_BN_CLICKED(IDC_BUTTON_THREAD_HELLO, &CTestThreadDlg::OnBnClickedButtonThreadHello)
	ON_BN_CLICKED(IDC_BUTTON_QUITTHREAD, &CTestThreadDlg::OnBnClickedButtonQuitthread)
	ON_MESSAGE(WM_APP+2,&CTestThreadDlg::OnThreadQuitFunction)
	ON_MESSAGE(WM_APP + 3, &CTestThreadDlg::OnThreadCheckFunction)
	ON_MESSAGE(WM_MAIN_TRAY, &CTestThreadDlg::OnMainTray)
	ON_COMMAND(ID_32773, &CTestThreadDlg::OnPopOpen)
	ON_COMMAND(ID_32774, &CTestThreadDlg::OnPopDown)
	ON_STN_CLICKED(IDC_STATIC_CAPTION, &CTestThreadDlg::OnStnClickedStaticCaption)
END_MESSAGE_MAP()


// CTestThreadDlg message handlers

BOOL CTestThreadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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
		return TRUE;
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_stCaption.SetWindowTextW(L"No Thread Running");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestThreadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}

	// ��ܿ� x ��ư�� ��������
	else if (nID == SC_CLOSE)
	{
	}

	else if (nID == SC_MINIMIZE)
	{
		CreateTray();

		NOTIFYICONDATA  nid;
		ZeroMemory(&nid, sizeof(nid));

		Shell_NotifyIcon(NIM_SETFOCUS, &nid);


	}

	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CTestThreadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestThreadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD threadProc(LPVOID lParam)
{

	MSG msg;
	while(GetMessage(&msg,0,0,0))
	{
		if (msg.message == WM_APP + 1)
		{
			MessageBoxA(NULL, "Hello", "From Thread", MB_OK);
		}
		DispatchMessage(&msg);
	}

	MessageBoxA(NULL,"Thread will closed On pressing OK","From Thread",MB_OK);

	AfxGetApp()->m_pMainWnd->PostMessageW(WM_APP+2,0,0);
	return 0;
}




void CTestThreadDlg::OnBnClickedButtonCreatethread()
{
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadProc,0,0,&m_idThread);
	CString strTemp;
	Monitoring::MonitoringFuncs::InitMon(m_idThread);
	strTemp.Format(L"Thread Started with id#%d",m_idThread);	//�� ������� ������ �����ֱ�
	m_stCaption.SetWindowTextW(strTemp);
}

void CTestThreadDlg::OnBnClickedButtonThreadHello()
{
	CString tmp;		//editText�� �ִ� ���ڿ�
	CString comp;		//���� ���ڿ� ���� ����
	comp = "1234";
	tmp = "Start Mon";

	GetDlgItem(IDC_EDIT1)->GetWindowText(tmp);

	if (comp.Compare(tmp) == 0){				//���� data ��, ������ hello ����
		PostThreadMessage(m_idThread, WM_APP + 1, 0, 0);
	}

	else {
		
		WNDCLASSEX wcex1;
		memset(&wcex1, 0, sizeof(wcex1));

		wcex1.cbSize = sizeof(wcex1);
		wcex1.style = CS_HREDRAW | CS_VREDRAW;
		wcex1.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wcex1.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex1.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex1.hInstance = g_Instance;
		wcex1.cbClsExtra = 0;
		wcex1.cbWndExtra = 0;
		wcex1.hIconSm = NULL;
		wcex1.lpszMenuName = NULL;
		wcex1.lpfnWndProc = Wndproc;
		wcex1.lpszClassName = L"beatmapOn";

		RegisterClassEx(&wcex1);

		hWnd = CreateWindow(L"beatmapOn", L"beatmapOn",
			WS_VISIBLE | WS_THICKFRAME | WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, 900, 600, NULL, NULL, g_Instance, NULL);

		::ShowWindow(hWnd, 3);


		MSG msg;
		memset(&msg, 0, sizeof(msg));
	
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	}
	
	///
}



void CTestThreadDlg::OnBnClickedButtonQuitthread()
{

	Monitoring::MonitoringFuncs::stopMon();
	Monitoring::MonitoringFuncs::UnInitMon();
	PostThreadMessage(m_idThread,WM_QUIT,0,0);
}

LRESULT CTestThreadDlg::OnThreadQuitFunction(WPARAM wParam, LPARAM lParam)
{	
	CString strTemp;
	strTemp.Format(L"Thread Closed with id#%d",m_idThread);
	m_stCaption.SetWindowTextW(strTemp);
	return LRESULT(0);
}


afx_msg LRESULT CTestThreadDlg::OnMainTray(WPARAM wParam, LPARAM lParam)
{
	switch (lParam){
	case WM_LBUTTONDBLCLK:
		MoveWindow(500, 300, 800, 330, 1);
		break;
	case WM_RBUTTONDOWN:
		SetCapture();
		break;
	case WM_RBUTTONUP:
		CMenu menu, *pMenu = NULL;
		CPoint pt;

		//menu.LoadMenu(IDR_MENU_TRAY_POPUP);
		pMenu = menu.GetSubMenu(0);
		GetCursorPos(&pt);
		SetForegroundWindow();
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
		::PostMessage(m_hWnd, WM_NULL, NULL, NULL);
		ReleaseCapture();

		break;
	}
	return 0;
}

afx_msg LRESULT CTestThreadDlg::OnThreadCheckFunction(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


void CTestThreadDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// CDialog::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	// TODO:  ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CTestThreadDlg::CreateTray()
{
	NOTIFYICONDATA  nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.hWnd = m_hWnd; // �����������ڵ�
	nid.uID = IDR_MAINFRAME;  // �����ܸ��ҽ�ID
	nid.uCallbackMessage = WM_MAIN_TRAY;
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; // �÷��׼���
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // �����ܷε�
	CString Title = _T("test_Thread");   // Ʈ��������Ŀ�����ö�����ǥ�õǴ����α׷���
	lstrcpy(nid.szTip, Title);
	Shell_NotifyIcon(NIM_ADD, &nid);	//Ʈ���� ������ �߰�

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	MoveWindow(-100, -100, 0, 0, 1);
}

//ON_MESSAGE(WM_MAIN_TRAY, &CHighAvailabilityDlg::OnMainTray)

LONG CTestThreadDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	switch (lParam){
	case WM_LBUTTONDBLCLK:
		MoveWindow(500, 300, 800, 330, 1);
		break;
	case WM_RBUTTONDOWN:
		SetCapture();
		break;
	case WM_RBUTTONUP:
		CMenu menu, *pMenu = NULL;
		CPoint pt;

		//menu.LoadMenu(IDR_MENU_TRAY_POPUP);
		pMenu = menu.GetSubMenu(0);
		GetCursorPos(&pt);
		SetForegroundWindow();
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
		::PostMessage(m_hWnd, WM_NULL, NULL, NULL);
		ReleaseCapture();

		break;
	}
	return 0;
}

void CTestThreadDlg::OnPopOpen()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow(SW_SHOW);
}


void CTestThreadDlg::OnPopDown()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	ShowWindow(SW_SHOW);

}
LRESULT CALLBACK Wndproc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam){

	switch (Message){
	case WM_CREATE:{
		g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, Disablekeys, g_Instance, 0);
		g_hHook = SetWindowsHookEx(WH_MOUSE_LL, DisableMouse, g_Instance, 0);
	
	}break;

	case WM_ACTIVATEAPP:{
		g_bActive = (bool)wParam;
	}break;

	case WM_DESTROY:{
		UnhookWindowsHookEx(g_hHook);
		g_hHook = NULL;
		PostQuitMessage(0);
	}break;
	}
	return(DefWindowProc(hWnd, Message, wParam, lParam));
}

LRESULT CALLBACK Disablekeys(int nCode, WPARAM wparam, LPARAM lparam){
	
			KBDLLHOOKSTRUCT *pkh = (KBDLLHOOKSTRUCT*)lparam;

			if (pkh->vkCode == VK_ESCAPE&&VK_CONTROL){
				return 1;
			}
			else if (pkh->vkCode == VK_TAB&&pkh->flags&LLKHF_ALTDOWN){
				return 1;
			}
			else if (pkh->vkCode == VK_ESCAPE&&pkh->flags&LLKHF_ALTDOWN){
				return 1;
			}
			else if (pkh->vkCode == VK_LWIN || pkh->vkCode == VK_RWIN){
				return 1;
			}
			else if (pkh->vkCode == VK_F4&&pkh->flags&LLKHF_ALTDOWN){
				return 1;
			}
			else {
				return 1;
			}
			
			
		
	
	return CallNextHookEx(g_hHook, nCode, wparam, lparam);
}
LRESULT CALLBACK DisableMouse(int nCode, WPARAM wparam, LPARAM lparam){
	MOUSEHOOKSTRUCT *pkh = (MOUSEHOOKSTRUCT*)lparam;
	switch (pkh->wHitTestCode){
	case WM_LBUTTONDOWN:
		return 1;

	case WM_RBUTTONDOWN:
		return 1;

	default:
		break;
	}

}


void CTestThreadDlg::OnStnClickedStaticCaption()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
