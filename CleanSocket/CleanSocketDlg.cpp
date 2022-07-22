
// CleanSocketDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CleanSocket.h"
#include "CleanSocketDlg.h"
#include "afxdialogex.h"
#include "Logger.h"
#include "webApi/BaseService.h"
#include <shlwapi.h>

#include "webApi/HttpService.h"
#pragma comment(lib,"Shlwapi.lib") //如果没有这行，会出现link错误


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();


	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CCleanSocketDlg 对话框



CCleanSocketDlg::CCleanSocketDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLEANSOCKET_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCleanSocketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCleanSocketDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CCleanSocketDlg 消息处理程序

BOOL CCleanSocketDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	
	//接收到消息后，开始通过接口获取目录进行本地删除

	CString strSvPort;
	strSvPort = L"9819";
	socServerPort = _ttoi(strSvPort);
	InitSocket();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CCleanSocketDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCleanSocketDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	ShowWindow(SW_HIDE);
}
CString GetExePath()
{
	CString strExePath;
	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	strExePath = szFilePath;
	return strExePath;
}
void CCleanSocketDlg::Log(CString strType, CString strMsg, CString strLogType)
{
	CString strPath;
	strPath = GetExePath() + L"Log\\";
	CreateDirectory(strPath, NULL);
	strPath.Append(L"手动录制");
	fstream _file;
	_file.open(strPath, ios::in);
	if (!_file)
	{
		CreateDirectory(strPath, NULL);//创建c:/lgftp
	}
	//增加日期
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	char date[100];
	char time[100];
	sprintf_s(date, "%d%02d%02d", st.wYear, st.wMonth, st.wDay);
	sprintf_s(time, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);

	USES_CONVERSION;
	CString strDate;
	strDate = A2T(date);

	if (strLogType == SCHEDULE_RECORD)
	{
		strPath.Append(L"\\录播");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == MANUE_RECORD)
	{
		strPath.Append(L"\\手动删除");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == CONFERENCE_RECORD)
	{
		strPath.Append(L"\\会议");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == AIROOM_RECORD)
	{
		strPath.Append(L"\\智能考场");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == FFRECORD)
	{
		strPath.Append(L"\\ffrecord");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == MONITOR_RECORD)
	{
		strPath.Append(L"\\监控");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == DIRECTOR_RECORD)
	{
		strPath.Append(L"\\导播");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == HardWare_RECORD)
	{
		strPath.Append(L"\\硬件录制");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == TEXT("Error"))
	{
		strPath.Append(L"\\Error");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == TEXT("RtmpServer"))
	{
		strPath.Append(L"\\RtmpServer");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == SOCKET_MESSAGE)
	{
		strPath.Append(L"\\Socket");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}
	if (strLogType == VideoEdit_RECORD)
	{
		strPath.Append(L"\\课后导播");
		strPath.Append(strDate);
		strPath.Append(L".log");
	}

	CStringA strPathEx(strPath);
	const char *cPath;
	cPath = strPathEx.GetString();

	CStringA strTypeEx(strType);
	CStringA strMsgEx(strMsg);
	const char *cType;
	cType = strTypeEx.GetString();
	const char *cMsg;
	cMsg = strMsgEx.GetString();

	FILE *m_pfLogFile = NULL;
	if (NULL != m_pfLogFile)
	{
		fclose(m_pfLogFile);
	}
	errno_t err;
	if (err = fopen_s(&m_pfLogFile, cPath, "at+"))
	{
		return;
	}

	RcLogInfo rl;
	if (strlen(time) + strlen(cType) + strlen(cMsg) > sizeof(rl.m_cInfo))
	{
		return;
	}
	rl.SetLogFile(m_pfLogFile);

	sprintf_s(rl.m_cInfo, "%s %s%s\n", time, cType, cMsg);

	if (rl.WriteLogInfo(rl.m_cInfo) == 0)
	{
		fclose(m_pfLogFile);
	}
}

void CCleanSocketDlg::InitSocket()
{
	CHECK_TRUE_DO_RETURN((NULL != socServer), Log(TEXT("StartSocket: StartSocket Has started ！"), TEXT(""), MANUE_RECORD));
	CHECK_TRUE_DO_RETURN((!AfxSocketInit()), Log(TEXT("StartSocket: AfxSocketInit false!"), TEXT(""), MANUE_RECORD));

	socServer = new CMySocket(this);
	BOOL bRet = socServer->Create(socServerPort, SOCK_STREAM);
	bRet = socServer->Listen();
	CHECK_TRUE_DO((!bRet), Log(TEXT("StartSocket: Listen() false!"), TEXT(""), MANUE_RECORD));
}
void CCleanSocketDlg::OnReceive(int iID, char* cMsg, long nLen)
{
	USES_CONVERSION;
	rcvMsg = TEXT("");
	rcvMsg.Format(_T("%s"), A2T(cMsg));
	Log(TEXT("接收到消息成功，rcvMsg="), rcvMsg, MANUE_RECORD);
	if (rcvMsg == "deleteManually")
	{
		CRegKey key;
		TCHAR WebBaseServiceIP[128];
		TCHAR WebBaseServicePort[128];
		char char_WebBaseServiceIP[128];
		char char_WebBaseServicePort[128];
		if ((key.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\CloudClassroom\\WebService"), KEY_READ)) != ERROR_SUCCESS)
		{
			WriteLog("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\CloudClassroom\\WebService注册表打开失败");
		}
		else
		{
			WriteLog("注册表打开成功");
			DWORD len1 = 128 * sizeof(TCHAR);
			
			if(key.QueryStringValue(L"WebBaseServiceIP", WebBaseServiceIP, &len1) != ERROR_SUCCESS)
			{
				WriteLog("WebBaseServiceIP读取错误");
			}
			DWORD len2 = 128 * sizeof(TCHAR);
			
			if (key.QueryStringValue(L"WebBaseServicePort", WebBaseServicePort, &len1) != ERROR_SUCCESS)
			{
				WriteLog("WebBaseServicePort读取错误");
			}
		}
		key.Close();
		//接收到消息后，开始通过接口获取目录进行本地删除
		vector<DeleteInfo> delete_info;
		CBaseService c_base_service;
		//c_base_service.DeleteLocalDir("172.16.52.194", "10102", delete_info);
		g_TcharToChar(WebBaseServiceIP, char_WebBaseServiceIP);
		g_TcharToChar(WebBaseServicePort, char_WebBaseServicePort);
		c_base_service.DeleteLocalDir(char_WebBaseServiceIP, char_WebBaseServicePort, delete_info);
	}
}
int CCleanSocketDlg::GetSocketID()
{
	bool bRet = false;

	for (int i = 1; i < 1000; i++)
	{
		//判断i是否已经存在map之中
		bRet = false;
		for (int j = 0; j < m_pRecServer.GetCount(); j++)
		{
			if (i == m_pRecServer.GetAt(j)->iID)
			{
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			return i;
		}
	}
	return -1;
}

void CCleanSocketDlg::OnAccept(int nErrorCode)
{
	BOOL bRet = FALSE;
	if (socServer != NULL)
	{
		//CMySocket* pRecServer = NULL;
		CMySocket* pRecServer = new CMySocket(this);
		// 
		bRet = socServer->Accept(*pRecServer);
		if (bRet)
		{
			pRecServer->iID = GetSocketID();

			if (pRecServer->iID < 0)
			{
				Log(TEXT("OnAccept: GetSocketID < 0！"), TEXT(""), MANUE_RECORD);
				return;
			}
			m_pRecServer.Add(pRecServer);
			Log(TEXT("OnAccept: 生成新接收信息服务成功！"), TEXT(""), MANUE_RECORD);
		}
	}
}
void CCleanSocketDlg::OnSend(int nErrorCode)
{

}
void CCleanSocketDlg::OnClose(int iSocketID)
{
	int b = m_pRecServer.GetCount();
	for (int i = 0; i < b; i++)
	{
		if (iSocketID == m_pRecServer.GetAt(i)->iID)
		{
			m_pRecServer.GetAt(i)->Close();
			delete  m_pRecServer.GetAt(i);
			m_pRecServer.RemoveAt(i);
			break;
		}
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCleanSocketDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

