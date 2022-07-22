#include <afxsock.h>            // MFC �׽�����չ
#include <afxtempl.h>

class CCourseInfo
{
public:
	CString  strSchNo;				//ѧУ����
	CString  strSchName;			//ѧУ����
	CString  strClassID;			//����ID
	CString  strClassName;			//����ID
	CString  strRoads;				//¼�ƻ����͵�·��
	CString	 strTeacherID;			//��ʦID
	CString	 strTeacherName;		//��ʦ����
	CString  strTheme;				//����
	CString  strRemark;				//��ע
	CString  strRecordType;			//¼�����ͣ�1¼����Ļ��2¼�ƽ�ʦ��3¼��ѧ����ֵ������1��3��4��6��,
	CString  strCaptionName;		//ֱ��(���߹㲥)����
	CString	 strType;				//ֱ��(���߹㲥)���ͣ�1����ʦֱ����2���ƶ�ֱ����3�����ӽ�Ŀ��
	CString	 strServerIP;			//¼��������IP
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
	int iStatic;		//1 ��ʾ�ļ��Ѿ��� ��0 ��ʾ�ļ��Ѿ��ر�
	int iSendCount;		//iSendCount �����������Ĵ��������͵Ĵ���������һ�����ֱ�ʾ���ӶϿ������Socket
	int iBeginTime;
	bool isBroadCast;	//�ж����ǹ㲥��ѧ����ֱ����ѧ

	//long lProcessID[2]; 
	CArray<long,long&> lPushProID;
	CArray<long,long&> lRecordProID;
	CArray<CSocket*,CSocket*&> pskSend;
	CCourseInfo CourseInfo;
	CString strStoragePath;
	CString strFullPath;
	CString strFileName;
};