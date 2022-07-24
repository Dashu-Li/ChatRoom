
// ChatRoomDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatRoom.h"
#include "ChatRoomDlg.h"
#include "afxdialogex.h"
#include "Inc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatRoomDlg �Ի���



CChatRoomDlg::CChatRoomDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChatRoomDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	bShowAll = FALSE;
	m_ListenSock = INVALID_SOCKET;
	m_ConnectSock = INVALID_SOCKET;
	m_hListenThread = NULL;
	m_hConnectThread = NULL;
	m_bIsServer = -1;
	bShutDown = FALSE;
}

void CChatRoomDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STOP_CLIENT, m_StopClientbtn);
	DDX_Control(pDX, IDC_SHOW_MSG, m_MsgEdit);
}

BEGIN_MESSAGE_MAP(CChatRoomDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_SERVER, &CChatRoomDlg::OnBnClickedRadioServer)
	ON_BN_CLICKED(IDC_RADIO_CLIENT, &CChatRoomDlg::OnBnClickedRadioClient)
	ON_BN_CLICKED(IDC_NETSET, &CChatRoomDlg::OnBnClickedNetset)
	ON_BN_CLICKED(IDC_START_SERVER, &CChatRoomDlg::OnBnClickedStartServer)
	ON_BN_CLICKED(IDC_SENDMSG, &CChatRoomDlg::OnBnClickedSendmsg)
	ON_BN_CLICKED(IDC_STOP_SERVER, &CChatRoomDlg::OnBnClickedStopServer)
	ON_BN_CLICKED(IDC_START_CLIENT, &CChatRoomDlg::OnBnClickedStartClient)
	ON_EN_CHANGE(IDC_INPUT_MSG, &CChatRoomDlg::OnEnChangeInputMsg)
	ON_BN_CLICKED(IDC_STOP_CLIENT, &CChatRoomDlg::OnBnClickedStopClient)
	ON_BN_CLICKED(IDC_CANCEL, &CChatRoomDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_OTHER, &CChatRoomDlg::OnBnClickedOther)
	ON_COMMAND(ID_MENU_32771, &CChatRoomDlg::OnMenu32771)
	ON_MESSAGE(WM_TRAYICON_MSG, OnTrayCallBackMsg)
	ON_COMMAND(ID_MENU_SHOW, &CChatRoomDlg::OnMenuShow)
	ON_COMMAND(ID_MENU_EXIT, &CChatRoomDlg::OnMenuExit)
END_MESSAGE_MAP()


// CChatRoomDlg ��Ϣ�������

BOOL CChatRoomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	DlgAllInit();
	ExtendDialog(FALSE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChatRoomDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChatRoomDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChatRoomDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CChatRoomDlg::EnableWindow(UINT uID, BOOL bEnable)
{
	return GetDlgItem(uID)->EnableWindow(bEnable);
}

void CChatRoomDlg::DlgAllInit()
{
	//m_StopClientbtn.EnableWindow(FALSE);
	EnableWindow(IDC_STOP_CLIENT, FALSE);
	EnableWindow(IDC_LISTEN_PORT, FALSE);
	EnableWindow(IDC_START_SERVER, FALSE);
	EnableWindow(IDC_STOP_SERVER, FALSE);
	EnableWindow(IDC_STATIC_LISTEN, FALSE);
	EnableWindow(IDC_SENDMSG, FALSE);
	CheckRadioButton(IDC_RADIO_CLIENT, IDC_RADIO_SERVER, IDC_RADIO_CLIENT);
	hostent *host;
	char hostname[20];
	gethostname(hostname, 20);
	host = gethostbyname(hostname);
	CString strIPAddr;
	strIPAddr = inet_ntoa(*(struct in_addr*)host->h_addr);
	SetDlgItemText(IDC_IP_ADDR, strIPAddr);
	SetDlgItemText(IDC_CONNECT_PORT, L"5050");
	SetDlgItemText(IDC_LISTEN_PORT, L"5050");
}

void CChatRoomDlg::ExtendDialog(BOOL bShow)
{
	static CRect m_DlgRectLarge(0, 0, 0, 0);
	static CRect m_DlgRectSmall(0, 0, 0, 0);
	static CRect m_GroupRectLarge(0, 0, 0, 0);
	static CRect m_GroupRectSmall(0, 0, 0, 0);
	if (m_DlgRectLarge.IsRectNull())
	{
		GetWindowRect(&m_DlgRectLarge);
		m_DlgRectSmall = m_DlgRectLarge;
		m_DlgRectSmall.right -= 325;
		::GetWindowRect(GetDlgItem(IDC_FRAME)->GetSafeHwnd(), &m_GroupRectLarge);
		m_GroupRectSmall = m_GroupRectLarge;
		m_GroupRectSmall.right -= 325;
	}
	if (bShow)
	{
		bShowAll = TRUE;
		SetWindowPos(NULL, m_DlgRectLarge.left, m_DlgRectLarge.top, m_DlgRectLarge.Width(), m_DlgRectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);
		::SetWindowPos(GetDlgItem(IDC_FRAME)->GetSafeHwnd(), NULL, m_GroupRectLarge.left, m_GroupRectLarge.top, m_GroupRectLarge.Width(), m_GroupRectLarge.Height(), SWP_NOZORDER | SWP_NOMOVE);
	}
	else
	{
		bShowAll = FALSE;
		SetWindowPos(NULL, m_DlgRectSmall.left, m_DlgRectSmall.top, m_DlgRectSmall.Width(), m_DlgRectSmall.Height(), SWP_NOZORDER | SWP_NOMOVE);
		::SetWindowPos(GetDlgItem(IDC_FRAME)->GetSafeHwnd(), NULL, m_GroupRectSmall.left, m_GroupRectSmall.top, m_GroupRectSmall.Width(), m_GroupRectSmall.Height(), SWP_NOZORDER | SWP_NOMOVE);
	}
}

void CChatRoomDlg::ShowMsg(CString strMsg)
{
	m_MsgEdit.SetSel(m_MsgEdit.GetWindowTextLengthW(), m_MsgEdit.GetWindowTextLengthW());
	m_MsgEdit.ReplaceSel(strMsg + L"\r\n\r\n");
}

void CChatRoomDlg::RemoveClientFromArray(CClientItem in_Item)
{
	for (int idx = 0; idx < m_ClientArray.GetCount(); idx++)
	{
		CClientItem tItem = m_ClientArray.GetAt(idx);
		if (tItem.m_Socket == in_Item.m_Socket&&tItem.hThread == in_Item.hThread&&tItem.m_strIp == in_Item.m_strIp)
		{
			m_ClientArray.RemoveAt(idx);
			break;
		}
	}
}

void CChatRoomDlg::SendClientMsg(CString strMsg, CClientItem *pNotSend)
{
	wchar_t szBuf[MAX_BUF_SIZE] = { 0 };
	_tcscpy_s(szBuf, MAX_BUF_SIZE, strMsg);
	for (INT_PTR idx = 0; idx < m_ClientArray.GetCount(); idx++)
		if (!pNotSend || pNotSend->m_Socket != m_ClientArray.GetAt(idx).m_Socket || 
			pNotSend->hThread != m_ClientArray.GetAt(idx).hThread || 
			pNotSend->m_strIp != m_ClientArray.GetAt(idx).m_strIp)
			send(m_ClientArray.GetAt(idx).m_Socket, (char*)szBuf, _tcslen(szBuf)*sizeof(wchar_t), 0);
}

void CChatRoomDlg::StopClient()
{
	bShutDown = TRUE;
	DWORD dwRet = WaitForSingleObject(m_hConnectThread, 1000);
	if (dwRet != WAIT_OBJECT_0)
	{
		TerminateThread(m_hConnectThread, -1);
		closesocket(m_ConnectSock);
	}
	m_hConnectThread = NULL;
	m_ConnectSock = INVALID_SOCKET;
	m_bIsServer = -1;
	bShutDown = FALSE;
}

void CChatRoomDlg::StopServer()
{
	UINT nCount = m_ClientArray.GetCount();
	HANDLE *m_pHandles = new HANDLE[nCount + 1];
	m_pHandles[0] = m_hListenThread;
	for (int idx = 0; idx < nCount; idx++)
		m_pHandles[idx + 1] = m_ClientArray.GetAt(idx).hThread;
	bShutDown = TRUE;
	DWORD dwRet = WaitForMultipleObjects(nCount + 1, m_pHandles, TRUE, 1000);
	if (dwRet != WAIT_OBJECT_0)
	{
		for (INT_PTR i = 0; i < m_ClientArray.GetCount(); i++)
		{
			TerminateThread(m_ClientArray.GetAt(i).hThread, -1);
			closesocket(m_ClientArray.GetAt(i).m_Socket);
		}
		TerminateThread(m_hListenThread, -1);
		closesocket(m_ListenSock);
	}
	delete [] m_pHandles;
	m_hListenThread = NULL;
	m_ListenSock = INVALID_SOCKET;
	m_bIsServer = -1;
	bShutDown = FALSE;
}

BOOL CChatRoomDlg::TrayMyIcon(BOOL bAdd)
{
	BOOL bRet = FALSE;
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = GetSafeHwnd();
	tnd.uID = IDR_MAINFRAME;
	if (bAdd == TRUE)
	{
		tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		tnd.uCallbackMessage = WM_TRAYICON_MSG;
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
		_tcscpy_s(tnd.szTip, sizeof(tnd.szTip), L"������");
		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_HIDE);
		bRet = Shell_NotifyIcon(NIM_ADD, &tnd);
	}
	else
	{
		ShowWindow(SW_SHOWNA);
		SetForegroundWindow();
		bRet = Shell_NotifyIcon(NIM_DELETE, &tnd);
	}
	return bRet;
}

LRESULT CChatRoomDlg::OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam)
{
	switch (lparam)
	{
	case WM_RBUTTONUP:
		{
			CMenu mMenu, *pMenu = NULL;
			CPoint pt;
			mMenu.LoadMenu(IDR_MENU2);
			pMenu = mMenu.GetSubMenu(0);
			GetCursorPos(&pt);
			SetForegroundWindow();
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
			break;
		}
	case WM_LBUTTONUP:
		ShowWindow(SW_RESTORE);
		TrayMyIcon(FALSE);
		break;
	default:
		break;
	}
	return NULL;
}

void CChatRoomDlg::OnBnClickedRadioServer()
{
	if (m_bIsServer != TRUE)
		EnableWindow(IDC_START_SERVER);
	else
		EnableWindow(IDC_STOP_SERVER);
	EnableWindow(IDC_LISTEN_PORT);
	EnableWindow(IDC_STATIC_LISTEN);
	if (m_bIsServer != FALSE)
		EnableWindow(IDC_START_CLIENT, FALSE);
	else
		EnableWindow(IDC_STOP_CLIENT, FALSE);
	EnableWindow(IDC_IP_ADDR, FALSE);
	EnableWindow(IDC_CONNECT_PORT, FALSE);
	EnableWindow(IDC_STATIC_IP, FALSE);
	EnableWindow(IDC_STATIC_CONNECT, FALSE);
}


void CChatRoomDlg::OnBnClickedRadioClient()
{
	if (m_bIsServer != FALSE)
		EnableWindow(IDC_START_CLIENT);
	else
		EnableWindow(IDC_STOP_CLIENT);
	EnableWindow(IDC_IP_ADDR);
	EnableWindow(IDC_CONNECT_PORT);
	EnableWindow(IDC_STATIC_IP);
	EnableWindow(IDC_STATIC_CONNECT);
	if (m_bIsServer != TRUE)
		EnableWindow(IDC_START_SERVER, FALSE);
	else
		EnableWindow(IDC_STOP_SERVER, FALSE);
	EnableWindow(IDC_LISTEN_PORT, FALSE);
	EnableWindow(IDC_STATIC_LISTEN, FALSE);
}


void CChatRoomDlg::OnBnClickedNetset()
{
	if (bShowAll)
		ExtendDialog(FALSE);
	else
		ExtendDialog(TRUE);
}


void CChatRoomDlg::OnBnClickedStartServer()
{
	if (m_bIsServer != -1)
	{
		AfxMessageBox(L"���ѽ��������ң������ظ����룡");
		return;
	}
	//���߳�
	m_hListenThread = CreateThread(NULL, 0, ListenThreadFunc, this, 0, NULL);
}


void CChatRoomDlg::OnBnClickedStopServer()
{
	StopServer();
	EnableWindow(IDC_START_SERVER);
	EnableWindow(IDC_STOP_SERVER, FALSE);
	ShowMsg(L"ϵͳ��Ϣ���رշ������ɹ���");
}


void CChatRoomDlg::OnBnClickedStartClient()
{
	if (m_bIsServer != -1)
	{
		AfxMessageBox(L"���ѽ��������ң������ظ����룡");
		return;
	}
	//���߳�
	m_hConnectThread = CreateThread(NULL, 0, ConnectThreadFunc, this, 0, NULL);
}


void CChatRoomDlg::OnBnClickedStopClient()
{
	StopClient();
	EnableWindow(IDC_START_CLIENT);
	EnableWindow(IDC_STOP_CLIENT, FALSE);
	ShowMsg(L"ϵͳ��Ϣ���Ͽ�������������ӳɹ���");
}


void CChatRoomDlg::OnBnClickedSendmsg()
{
	CString strMsg;
	GetDlgItemText(IDC_INPUT_MSG, strMsg);
	if (m_bIsServer == TRUE)
	{
		strMsg = L"��������>\r\n" + strMsg;
		ShowMsg(strMsg);
		SendClientMsg(strMsg);
	}
	else if (m_bIsServer == FALSE)
	{
		CString strTmp = L"���ؿͻ��ˣ�>\r\n" + strMsg;
		ShowMsg(strTmp);
		int iSend = send(m_ConnectSock, (char*)strMsg.GetBuffer(), strMsg.GetLength()*sizeof(wchar_t), 0);
		strMsg.ReleaseBuffer();
	}
	else
		AfxMessageBox(L"�����Ƚ��������ң�");
	SetDlgItemText(IDC_INPUT_MSG,L"");
	EnableWindow(IDC_SENDMSG, FALSE);
}


void CChatRoomDlg::OnEnChangeInputMsg()
{
	CString strMsg;
	GetDlgItemText(IDC_INPUT_MSG, strMsg);
	if (strMsg.IsEmpty())
		EnableWindow(IDC_SENDMSG, FALSE);
	else
		EnableWindow(IDC_SENDMSG);
}


void CChatRoomDlg::OnBnClickedCancel()
{
	if (m_bIsServer == TRUE)
		StopServer();
	else if (m_bIsServer == FALSE)
		StopClient();
	OnCancel();
}


void CChatRoomDlg::OnBnClickedOther()
{
	CPoint pt;
	CRect mRect;
	CMenu mMenu, *pMenu = NULL;
	GetDlgItem(IDC_OTHER)->GetWindowRect(&mRect);
	pt = mRect.BottomRight();
	pt.y = mRect.top + 10;
	mMenu.LoadMenu(IDR_MENU1);
	pMenu = mMenu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, pt.x, pt.y, this);
}


void CChatRoomDlg::OnMenu32771()
{
	TrayMyIcon();
}


void CChatRoomDlg::OnMenuShow()
{
	ShowWindow(SW_RESTORE);
	TrayMyIcon(FALSE);
}


void CChatRoomDlg::OnMenuExit()
{
	TrayMyIcon(FALSE);
	OnBnClickedCancel();
}


BOOL CChatRoomDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
		return TRUE;
	return CDialogEx::PreTranslateMessage(pMsg);
}
