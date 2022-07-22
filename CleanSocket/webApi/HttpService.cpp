#include "stdafx.h"
#include "UrlCode.h"
#include "RegisterEx.h"
#include "HttpService.h"



#include <time.h>
#include <sys/stat.h>
#include <atlstr.h>
#include <io.h>
#include <fcntl.h>


#define  NORMAL_CONNECT             INTERNET_FLAG_KEEP_CONNECTION
#define  SECURE_CONNECT             NORMAL_CONNECT | INTERNET_FLAG_SECURE
#define  NORMAL_REQUEST             INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE 
#define  SECURE_REQUEST             NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

CUrlCode Curl;
void g_TcharToChar(const TCHAR* tcStr, char* cStr)
{
	int ilength ;
	ilength = WideCharToMultiByte(CP_ACP, 0, tcStr, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tcStr, -1, cStr, ilength, NULL, NULL);
	cStr[ilength] = '\0';
}

void g_charToTCHAR(const char* strIn, TCHAR* strOut)			//char转TCHAR
{
	int len = MultiByteToWideChar(CP_ACP,0,strIn, -1,NULL,0);   
	MultiByteToWideChar(CP_ACP,0,strIn,-1,strOut,len);   
	strOut[len]='\0';   
}

void g_StringToChar(std::string sStr, char* cStr)
{
	int ilength;
	ilength = sStr.size();
	memcpy(cStr, sStr.c_str(), ilength);
	cStr[ilength] = '\0';
}

CString WINAPI GetLocalIP()
{
	CString strIP = TEXT("");
	//1.初始化wsa  
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		return strIP;
	}

	//2.获取主机名 
	char hostname[256];
	ret = gethostname(hostname, sizeof(hostname));
	if (ret == SOCKET_ERROR)
	{
		return strIP;
	}

	//3.获取主机ip  
	HOSTENT* host = gethostbyname(hostname);
	if (host == NULL)
	{
		return strIP;
	}
	
	//4.转化为char*并拷贝返回 
	USES_CONVERSION;
	strIP.Format(TEXT("%s"), A2T(inet_ntoa(*(in_addr*)*host->h_addr_list)));
	return strIP;
}

int GetIPbyName(const std::string& name, std::string& ip) {
    WSADATA wsaData;
   
    struct hostent* host;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return -1;

    host = gethostbyname(name.c_str());
    if (!host) return -1;

    // 默认取第一个
    if (host->h_addr_list[0])
        ip = inet_ntoa(*(struct in_addr*)host->h_addr_list[0]);
    //std::string ip_str[5];
    //for (i = 0; host->h_addr_list[i]; i++)
    //    ip_str[i] = inet_ntoa(*(struct in_addr*)host->h_addr_list[i]);

    WSACleanup();
    return 0;
}

BOOL isIpFormatRight(LPTSTR ipAddress)
{
	int a, b, c, d;
	if ((swscanf(ipAddress, L"%d.%d.%d.%d", &a, &b, &c, &d) == 4)
		&& (a >= 0 && a <= 255)
		&& (b >= 0 && b <= 255)
		&& (c >= 0 && c <= 255)
		&& (d >= 0 && d <= 255))
	{
		return TRUE;
	}
	return FALSE;
}

bool WINAPI PingServer(CString strIP)
{
	CPing objPing;
	PingReply reply;

	USES_CONVERSION;
	char cParam[MAX_PATH];
	strcpy(cParam, T2A(strIP.GetBuffer()));

	for (int i = 0; i< 3; i++)
	{
		if (objPing.Ping(cParam, &reply, 500) == TRUE)
		{
			return true;
		}
	}

	return false;
}

int WriteLog(char *fmt, ...)
{
	int n, fd;
	time_t i;
	char buffer[5120],strDate[12],strTime[12],strTemp[32];
	va_list ap;
	struct stat sbuf;
	struct tm *tim;
	char fileName[100];    

	TCHAR szFileToFind[MAX_PATH];
	WIN32_FIND_DATA wfd;
	BOOL rValue = FALSE;

	TCHAR tcModule[MAX_PATH] = TEXT("");
	GetModuleFileName(NULL,tcModule, MAX_PATH); //得到当前模块路径
	CString strModule;
	strModule = tcModule;
	int iFind = strModule.ReverseFind('\\');
	strModule = strModule.Left(iFind);
	strModule += TEXT("\\Log\\");
	CreateDirectory(strModule, NULL);
	strModule += TEXT("\\资源删除");
	_tcscpy(szFileToFind, strModule.GetString());
	HANDLE hFind = FindFirstFile(szFileToFind, &wfd);
	if((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		rValue = TRUE;
	}
	FindClose(hFind);
	if(rValue == FALSE)
	{
		SECURITY_ATTRIBUTES  attrib;
		attrib.bInheritHandle = FALSE;
		attrib.lpSecurityDescriptor = NULL;
		attrib.nLength = sizeof(SECURITY_ATTRIBUTES);
		if(FALSE == ::CreateDirectory(szFileToFind, &attrib))
			return -1;
	}
	time(&i);
	tim=localtime(&i);

	char cFileToFind[MAX_PATH];
	g_TcharToChar(szFileToFind, cFileToFind);
	sprintf(fileName,"%s\\%04d%02d%02d.log",cFileToFind,tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
	va_start(ap, fmt);
	char tmplog[5000]={0};
	SYSTEMTIME st;
	GetLocalTime(&st);
	sprintf(tmplog,"%04d-%02d-%02d %02d:%02d:%02d.%03d[WebAPI]: %s", st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, fmt);
	n = vsprintf(buffer, tmplog, ap);
	va_end(ap);

	stat(fileName, &sbuf);
	fd = _open(fileName, O_CREAT | O_RDWR, 0600);

	if (fd < 0)
	{
		return -1;
	}

	_lseek(fd,0,SEEK_END);

	memset(strDate,0x00,sizeof(strDate));
	memset(strTime,0x00,sizeof(strTime));
	memset(strTemp,0x00,sizeof(strTemp));

	_write(fd, buffer, strlen(buffer));
	_write(fd, "\n", 1);

	_close(fd);

	return 0;
}

DWORD SetstrValueToRegedit(LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD dwSize, LPCTSTR strValue)
{
	CRegisterEx Regedit;

	CString strTemp = TEXT("");

	DWORD ret = -1;

	ret = Regedit.SetRegValue(HKEY_LOCAL_MACHINE, lpSubKey, lpValueName, REG_SZ, dwSize, (PVOID)strValue);

	return ret;
}
CString GetstrValueFromRegedit(LPCTSTR lpSubKey, LPCTSTR lpValueName)
{
	CRegisterEx Regedit;

	CString strTemp = TEXT("");
	DWORD TypeOfKey;
	DWORD SizeOfKey;
	PVOID pValueOfKey;

	int ret;

	ret = Regedit.GetRegValue(HKEY_LOCAL_MACHINE, lpSubKey, lpValueName, &TypeOfKey, &SizeOfKey, &pValueOfKey);

	if (ret == ERROR_SUCCESS)
	{
		strTemp = (PWCHAR)pValueOfKey;
	}
	return strTemp;
}
CHttpService::CHttpService(LPCTSTR strAgent)
{
    m_pSession = new CInternetSession(strAgent);
    m_pConnection = NULL;
    m_pFile = NULL;
}


CHttpService::~CHttpService(void)
{
    Clear();
    if(NULL != m_pSession)
    {
        m_pSession->Close();
        delete m_pSession;
        m_pSession = NULL;
    }
}

void CHttpService::Clear()
{
    if(NULL != m_pFile)
    {
        m_pFile->Close();
        delete m_pFile;
        m_pFile = NULL;
    }

    if(NULL != m_pConnection)
    {
        m_pConnection->Close();
        delete m_pConnection;
        m_pConnection = NULL;
    }
}

int CHttpService::ExecuteRequest(LPCTSTR strMethod, LPCTSTR strUrl, LPCTSTR strPostData, string &strResponse)  
{  
    CString strServer;  
    CString strObject;  
    DWORD dwServiceType;  
    INTERNET_PORT nPort;  
    strResponse = "";  
  
    AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);  
  
    if(AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)  
    {  
        return FAILURE;  
    }  
  
    try  
    {  
        m_pConnection = m_pSession->GetHttpConnection(strServer,  
            dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,  
            nPort);  
        m_pFile = m_pConnection->OpenRequest(strMethod, strObject,   
            NULL, 1, NULL, NULL,   
            (dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));  
		
		m_pFile->AddRequestHeaders(_T("Accept: *, */*")); 
		m_pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));   
		m_pFile->AddRequestHeaders(_T("Content-Type: text/xml; charset=utf-8"));  
		m_pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded"));  
  
        m_pFile->SendRequest(NULL, 0, (LPVOID)(LPCTSTR)strPostData, strPostData == NULL ? 0 : _tcslen(strPostData));  
  
        char szChars[BUFFER_SIZE + 1] = {0};  
        string strRawResponse = "";  
        UINT nReaded = 0;  
        while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)  
        {  
            szChars[nReaded] = '\0';  
            strRawResponse += szChars;  
            memset(szChars, 0, BUFFER_SIZE + 1);  
        }  

        strResponse = strRawResponse;  
        Clear();  
    }  
    catch (CInternetException* e)  
    {  
        Clear();  
        DWORD dwErrorCode = e->m_dwError;  
        e->Delete();  
  
        DWORD dwError = GetLastError();  
  
        WriteLog("dwError = %d", dwError);  
  
        if (ERROR_INTERNET_TIMEOUT == dwErrorCode)  
        {  
            return OUTTIME;  
        }  
        else  
        {  
            return FAILURE;  
        }  
    }  
    return SUCCESS;  
}
 
int CHttpService::ExecuteRequestJson(LPCTSTR strMethod, LPCTSTR strUrl, LPCTSTR strPostData, string &strResponse)
{
    CString strServer;
    CString strObject;
    DWORD dwServiceType;
    INTERNET_PORT nPort;
    strResponse = "";

    AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

	std::string str;
	USES_CONVERSION;
	str = T2A(strPostData);
	Curl.UrlGB2312((char*)str.c_str());
	strPostData = A2T(str.c_str());

    if (AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
    {
        return FAILURE;
    }

    try
    {
        m_pConnection = m_pSession->GetHttpConnection(strServer,
            dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,
            nPort);
        m_pFile = m_pConnection->OpenRequest(strMethod, strObject,
            NULL, 1, NULL, NULL,
            (dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));
		m_pFile->AddRequestHeaders(_T("HTTP/1.1"));
        //m_pFile->AddRequestHeaders(_T("Accept: application/json"));
        //m_pFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
        m_pFile->AddRequestHeaders(_T("Content-Type: application/json"));
       // m_pFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded"));

        m_pFile->SendRequest(NULL, 0, (LPVOID)(LPCTSTR)strPostData, (LPCTSTR)strPostData == NULL ? 0 : _tcslen(strPostData));

        char szChars[BUFFER_SIZE + 1] = { 0 };
        string strRawResponse = "";
        UINT nReaded = 0;
        while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)
        {
            szChars[nReaded] = '\0';
            strRawResponse += szChars;
            memset(szChars, 0, BUFFER_SIZE + 1);
        }

        strResponse = strRawResponse;

        Clear();
    }
    catch (CInternetException* e)
    {
        Clear();
        DWORD dwErrorCode = e->m_dwError;
        e->Delete();

        DWORD dwError = GetLastError();

        WriteLog("dwError = %d", dwError);

        if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
        {
            return OUTTIME;
        }
        else
        {
            return FAILURE;
        }
    }
    return SUCCESS;
}

int CHttpService::HttpGet(LPCTSTR strUrl, char* cResponse)
{
	string strResponse;
    int iRet = ExecuteRequest(_T("GET"), strUrl, NULL, strResponse);

	int nCount = 0;
	for (; iRet != 0; )
	{
		iRet = ExecuteRequest(_T("GET"), strUrl, NULL, strResponse);
		if (++nCount >= 5)
		{
			nCount = 0;
			WriteLog("基础平台，E00数据库获取错误！");
			return -1;
		}
		Sleep(1000 * 30);
	}

	string cOut;
	cOut = m_urlcode.UrlUTF8Decode(strResponse.c_str());

	if (cOut.size() > 1024 * 1024 - 1)
	{
		WriteLog("返回Json的字符串大于1024*1024,内存超出!");
		return -1;
	}
	strcpy(cResponse, cOut.c_str());
	char cLog[1024] = "";
	int iLen = cOut.length();
	if (iLen > 1023)
	{
		iLen = 1023;
	}
	memcpy(cLog, cOut.c_str(), iLen);
	WriteLog("返回Json解析后为：%s", cLog);
	return iRet;
}

int CHttpService::HttpGet(LPCTSTR strUrl, std::string& sResponse)
{
	string strResponse;
	int iRet = ExecuteRequest(_T("GET"), strUrl, NULL, strResponse);
	int nCount = 0;
	for (; iRet != 0; )
	{
		iRet = ExecuteRequest(_T("GET"), strUrl, NULL, strResponse);
		if (++nCount >= 3)
		{
			nCount = 0;
			return -1;
		}
		Sleep(1000 * 30);
	}
	sResponse = m_urlcode.UrlUTF8Decode(strResponse.c_str());

	char cLog[1024] = "";
	int iLen = sResponse.length();
	if (iLen > 1023)
	{
		iLen = 1023;
	}
	memcpy(cLog, sResponse.c_str(), iLen);
	//WriteLog("返回Json解析后为：%s", cLog);
	return iRet;
}

int CHttpService::HttpGetJson(LPCTSTR strUrl, std::string& sResponse)
{
    string strResponse;
    int iRet = ExecuteRequestJson(_T("GET"), strUrl, NULL, strResponse);
    int nCount = 0;
    for (; iRet != 0; )
    {
        iRet = ExecuteRequestJson(_T("GET"), strUrl, NULL, strResponse);
        if (++nCount >= 600)
        {
            nCount = 0;
            WriteLog("基础平台，E00数据库获取错误！");
        }
        Sleep(1000 * 30);
    }
    sResponse = m_urlcode.UrlUTF8Decode(strResponse.c_str());

    char cLog[1024] = "";
    int iLen = sResponse.length();
    if (iLen > 1023)
    {
        iLen = 1023;
    }
    memcpy(cLog, sResponse.c_str(), iLen);
   // WriteLog("返回Json解析后为：%s", cLog);
    return iRet;
}

int CHttpService::HttpPost(LPCTSTR strUrl, LPCTSTR strPostData, char* cResponse)
{
	char cLog[BUFFER_SIZE * 10];
	g_TcharToChar(strUrl, cLog);
	//WriteLog("HttpPost: 传入URL：%s", cLog);

	g_TcharToChar(strPostData, cLog);

	string strResponse;
	CString strMethod;
	strMethod = _T("POST");
	CString strServer;
	CString strObject;
	DWORD dwServiceType;
	INTERNET_PORT nPort;
	strResponse = "";

	AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort);

	if (AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		return FAILURE;
	}

	try
	{
		m_pConnection = m_pSession->GetHttpConnection(strServer,
			dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_CONNECT : SECURE_CONNECT,
			nPort);
		m_pFile = m_pConnection->OpenRequest(strMethod, strObject,
			NULL, 1, NULL, NULL,
			(dwServiceType == AFX_INET_SERVICE_HTTP ? NORMAL_REQUEST : SECURE_REQUEST));

		m_pFile->AddRequestHeaders(_T("HTTP/1.1"));
		m_pFile->AddRequestHeaders(_T("Content-Type: application/json"));

		m_pFile->SendRequest(NULL, 0, (LPVOID)strPostData, strPostData == NULL ? 0 : _tcslen(strPostData));


		char szChars[BUFFER_SIZE + 1] = { 0 };
		string strRawResponse = "";
		UINT nReaded = 0;
		while ((nReaded = m_pFile->Read((void*)szChars, BUFFER_SIZE)) > 0)
		{
			szChars[nReaded] = '\0';
			strRawResponse += szChars;
			memset(szChars, 0, BUFFER_SIZE + 1);
		}

		strResponse = strRawResponse;

		Clear();
	}
	catch (CInternetException* e)
	{
		Clear();
		DWORD dwErrorCode = e->m_dwError;
		e->Delete();

		DWORD dwError = GetLastError();

		//CLog::GetInstance().Logf("dwError = %d", dwError);  

		if (ERROR_INTERNET_TIMEOUT == dwErrorCode)
		{
			return OUTTIME;
		}
		else
		{
			return FAILURE;
		}
	}
	string cOut;
	cOut = m_urlcode.UrlUTF8DecodeV2(strResponse.c_str());
	strcpy(cResponse, cOut.c_str());
	ULONGLONG iLen = cOut.length();
	if (iLen > 1024)
	{
		iLen = 1024 - 2;
	}
	memset(cLog, 0x00, 1024);
	memcpy(cLog, cResponse, (size_t)iLen);
	strcat(cLog, "\0");
	//CLog::GetInstance().Logf("返回Json解析后为：%s", cLog);
	return SUCCESS;
}


long CHttpService::ParseResponse(char* buf, Json::Value& json_data)
{
	tinyxml2::XMLDocument doc;
	json_data = Json::Value::null;
	if (doc.Parse(buf) != tinyxml2::XML_SUCCESS) {
		return -1;
	}

	// 首节点不为数组
	ConvertElement(doc.RootElement(), json_data, false);

	return 0;
}
void CHttpService::ConvertElement(tinyxml2::XMLElement* node, Json::Value& json_data, bool root_is_array) {
	if (!node) {
		json_data = Json::Value::null;
		return;
	}

	const char* node_name = node->Name();
	tinyxml2::XMLElement* child_node = node->FirstChildElement();
	if (child_node) {
		// 比较前两个节点名称，判断其子节点是否为数组
		std::string child_node_name = child_node->Name();
		tinyxml2::XMLElement* next_node = child_node->NextSiblingElement();
		if (next_node) {
			bool is_array = false;
			if (child_node_name == next_node->Name()) {
				is_array = true;
			}

			unsigned int index = 0;
			tinyxml2::XMLElement* i_node = node->FirstChildElement();
			for (; i_node != NULL; i_node = i_node->NextSiblingElement()) {
				const char* i_node_name = i_node->Name();
				if (is_array) {
					ConvertElement(i_node, json_data[node_name][i_node_name][index++],
						true);
				}
				else {
					if (root_is_array) {
						ConvertElement(i_node, json_data, false);
					}
					else {
						ConvertElement(i_node, json_data[node_name], false);
					}
				}
			}
		}
		else {
			// 该node只有一个子节点
			if (root_is_array) {
				ConvertElement(child_node, json_data, false);
			}
			else {
				ConvertElement(child_node, json_data[node_name], false);
			}
		}
	}
	else {
		// 独立节点，无子节点
		if (root_is_array) {
			if (node->GetText()) json_data = node->GetText();
			/*				json_data =
			(node->GetText() != NULL) ? node->GetText() : Json::Value::null;*/
		}
		else {
			if (node->GetText()) json_data[node_name] = node->GetText();
			//json_data[node_name] =
			//	(node->GetText() != NULL) ? node->GetText() : Json::Value::null;
		}
	}
}

//获取本机MAC信息
bool CHttpService::GetMacByGetAdaptersInfo(std::string& macOUT)
{
	bool ret = false;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
		return false;
	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL)
			return false;
	}

	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR)
	{
		for (PIP_ADAPTER_INFO pAdapter = pAdapterInfo; pAdapter != NULL; pAdapter = pAdapter->Next)
		{
			// 确保是以太网
			if (pAdapter->Type != MIB_IF_TYPE_ETHERNET)
				continue;
			// 确保MAC地址的长度为 00-00-00-00-00-00
			if (pAdapter->AddressLength != 6)
				continue;
			if(strstr(pAdapter->Description, "PCI") <= 0)
				continue;
			char acMAC[32];
			sprintf(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X",
				int(pAdapter->Address[0]),
				int(pAdapter->Address[1]),
				int(pAdapter->Address[2]),
				int(pAdapter->Address[3]),
				int(pAdapter->Address[4]),
				int(pAdapter->Address[5]));
			macOUT = acMAC;
			ret = true;
			break;
		}
	}

	free(pAdapterInfo);
	return ret;
}

