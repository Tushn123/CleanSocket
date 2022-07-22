#include <afxsock.h>            // MFC 套接字扩展
#include <afxtempl.h>

class CCourseInfo
{
public:
	CString  strSchNo;				//学校代号
	CString  strSchName;			//学校名称
	CString  strClassID;			//教室ID
	CString  strClassName;			//教室ID
	CString  strRoads;				//录制或推送的路数
	CString	 strTeacherID;			//教师ID
	CString	 strTeacherName;		//教师名称
	CString  strTheme;				//主题
	CString  strRemark;				//备注
	CString  strRecordType;			//录制类型（1录制屏幕、2录制教师、3录制学生，值可以是1、3、4、6）,
	CString  strCaptionName;		//直播(或者广播)名称
	CString	 strType;				//直播(或者广播)类型（1：教师直播，2：云端直播，3：电视节目）
	CString	 strServerIP;			//录播服务器IP
};


class CMySocket : public CSocket
{
public:
	CMySocket(CWnd* pPerantDlg);

	virtual void OnAccept(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnClose(int nErrorCode);
public:
	CWnd* m_pPerantDlg;
	int iID;
	int iStatic;		//1 表示文件已经打开 ，0 表示文件已经关闭
	int iSendCount;		//iSendCount 发送心跳包的次数，发送的次数超过了一定数字表示连接断开，清除Socket
	int iBeginTime;
	bool isBroadCast;	//判断是是广播教学还是直播教学

	//long lProcessID[2]; 
	CArray<long,long&> lPushProID;
	CArray<long,long&> lRecordProID;
	CArray<CSocket*,CSocket*&> pskSend;
	CCourseInfo CourseInfo;
	CString strStoragePath;
	CString strFullPath;
	CString strFileName;
};