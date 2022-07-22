
// CleanSocketDlg.h : 头文件
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

// CCleanSocketDlg 对话框
class CCleanSocketDlg : public CDialogEx
{
// 构造
public:
	CCleanSocketDlg(CWnd* pParent = NULL);	// 标准构造函数
	CString rcvMsg;//接收到的命令参数
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
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLEANSOCKET_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
