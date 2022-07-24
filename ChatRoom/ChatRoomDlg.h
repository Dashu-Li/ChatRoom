
// ChatRoomDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "Inc.h"


// CChatRoomDlg �Ի���
class CChatRoomDlg : public CDialogEx
{
// ����
public:
	CChatRoomDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHATROOMS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void DlgAllInit();
	CButton m_StopClientbtn;
	BOOL EnableWindow(UINT uID, BOOL bEnable = TRUE);
	BOOL bShutDown;
	void ExtendDialog(BOOL bShow);
	BOOL bShowAll;
	SOCKET m_ListenSock;
	SOCKET m_ConnectSock;
	HANDLE m_hListenThread;
	HANDLE m_hConnectThread;
	CArray <CClientItem, CClientItem> m_ClientArray;
	int m_bIsServer;
	void ShowMsg(CString strMsg);
	void RemoveClientFromArray(CClientItem in_Item);
	void SendClientMsg(CString strMsg, CClientItem *pNotSend = NULL);
	void StopClient();
	void StopServer();
	BOOL TrayMyIcon(BOOL bAdd = TRUE);
	LRESULT OnTrayCallBackMsg(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedRadioServer();
	afx_msg void OnBnClickedRadioClient();
	afx_msg void OnBnClickedNetset();
	afx_msg void OnBnClickedStartServer();
	CEdit m_MsgEdit;
	afx_msg void OnBnClickedSendmsg();
	afx_msg void OnBnClickedStopServer();
	afx_msg void OnBnClickedStartClient();
	afx_msg void OnEnChangeInputMsg();
	afx_msg void OnBnClickedStopClient();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOther();
	afx_msg void OnMenu32771();
	afx_msg void OnMenuShow();
	afx_msg void OnMenuExit();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
