#include "stdafx.h"
#include "Resource.h"
#include "ChatRoomDlg.h"
#include "Inc.h"

DWORD WINAPI ConnectThreadFunc(LPVOID pParam)
{
	CChatRoomDlg *pChatRoom = (CChatRoomDlg*)pParam;
	ASSERT(pChatRoom != NULL);
	pChatRoom->m_ConnectSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pChatRoom->m_ConnectSock == INVALID_SOCKET)
	{
		AfxMessageBox(L"新建Socket失败！");
		return FALSE;
	}
	CString strServIp;
	pChatRoom->GetDlgItemText(IDC_IP_ADDR, strServIp);
	int iPort = pChatRoom->GetDlgItemInt(IDC_CONNECT_PORT);
	if (iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(L"请输入合适的端口：1 - 65535");
		goto __Error_End;
	}
	char szIpAddr[16] = { 0 };
	USES_CONVERSION;
	strcpy_s(szIpAddr, 16, T2A(strServIp));
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr(szIpAddr);
	if (connect(pChatRoom->m_ConnectSock, (struct sockaddr*)&server, sizeof(struct sockaddr)) == SOCKET_ERROR)
	{
		AfxMessageBox(L"连接失败，请重试！");
		goto __Error_End;
	}
	pChatRoom->m_bIsServer = FALSE;
	pChatRoom->EnableWindow(IDC_START_CLIENT, FALSE);
	pChatRoom->EnableWindow(IDC_STOP_CLIENT);
	pChatRoom->ShowMsg(L"系统信息：连接服务器成功！");
	while (!(pChatRoom->bShutDown))
	{
		if (SOCKET_Select(pChatRoom->m_ConnectSock))
		{
			wchar_t szBuf[MAX_BUF_SIZE] = { 0 };
			int iRet = recv(pChatRoom->m_ConnectSock, (char*)szBuf, MAX_BUF_SIZE, 0);
			if (iRet > 0)
				pChatRoom->ShowMsg(szBuf);
			else
			{
				pChatRoom->EnableWindow(IDC_STOP_CLIENT, FALSE);
				pChatRoom->EnableWindow(IDC_START_CLIENT);
				pChatRoom->m_bIsServer = -1;
				pChatRoom->ShowMsg(L"聊天服务器已关闭，请重新进行连接！");
				break;
			}
		}
		Sleep(500);
	}
__Error_End:
	closesocket(pChatRoom->m_ConnectSock);
	return TRUE;
}