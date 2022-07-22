
// CleanSocketDlg.h : ͷ�ļ�
//
#include "Socket/MySocket.h"
#include <fstream>
#pragma once

#define SCHEDULE_EXE            "ScheduleRecord.exe"
#define MONITOR_EXE             "MoniterRecord.exe"
#define MANUE_EXE               "ManuClassRecord.exe"
#define AIROOM_EXE              "AiRoomRecord.exe"

#define MONITOR_RECORD         TEXT("Monitor")
#define SCHEDULE_RECORD        TEXT("RecordServer")
#define MANUE_RECORD           TEXT("ManuRecord")
#define CONFERENCE_RECORD      TEXT("Conference")
#define AIROOM_RECORD          TEXT("AiRoom")
#define FFRECORD               TEXT("FFrecord")
#define SOCKET_MESSAGE         TEXT("Socket")
#define DIRECTOR_RECORD        TEXT("DirectorRecord")

#define HardWare_RECORD        TEXT("HardWareRecord")

#define VideoEdit_RECORD         TEXT("VideoEditRecord")

#define CHECK_TRUE_DO(result, fun) if(TRUE == result) {\
	fun;\
	}

#define CHECK_TRUE_DO_RETURN(result, fun) if(TRUE == result) {\
	fun;\
	return;}

// CCleanSocketDlg �Ի���
class CCleanSocketDlg : public CDialogEx
{
// ����
public:
	CCleanSocketDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CString rcvMsg;//���յ����������
	UINT socServerPort;	//Socket Server Port
	CMySocket* socServer;
	CArray<CMySocket*, CMySocket*&> m_pRecServer;
	void Log(CString strType, CString strMsg, CString strLogType);
	void InitSocket();
	int GetSocketID();
	void OnSend(int nErrorCode);
	void OnAccept(int nErrorCode);
	void OnClose(int iSocketID);
	void OnReceive(int iID, char* cMsg, long nLen);
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLEANSOCKET_DIALOG };
#endif

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
};
