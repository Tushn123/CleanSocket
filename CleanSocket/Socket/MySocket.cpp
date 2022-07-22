//#include "MySocket.h"
#include "stdafx.h"
#include "../Resource.h"
#include "../CleanSocketDlg.h"

CMySocket::CMySocket(CWnd* pPerantDlg) : CSocket()
{
	m_pPerantDlg = pPerantDlg;
	iID = -1;
	iStatic = 0;
	iSendCount = 0;
	iBeginTime = 0;
}

void CMySocket::OnAccept(int nErrorCode)
{
	CCleanSocketDlg *pDlg = (CCleanSocketDlg *)m_pPerantDlg;

	pDlg->OnAccept(nErrorCode);

	CSocket::OnAccept(nErrorCode);
}

void CMySocket::OnReceive(int nErrorCode)
{
	CCleanSocketDlg *pDlg = (CCleanSocketDlg *)m_pPerantDlg;

	int rec_size = 0;
	//-------------½ÓÊÕµ½×Ö·û´®------------------
	const int BUF_SIZE = 1024;
	//-----------byte----------------
	//BYTE* pbufRecv = new BYTE[BUF_SIZE];  
	//-----------char---------------
	char* pbufRecv = new char[BUF_SIZE];  
	//------------------------------

	ZeroMemory(pbufRecv, BUF_SIZE);  

	rec_size = this->Receive(pbufRecv,BUF_SIZE);

	pDlg->OnReceive(iID, pbufRecv, rec_size);
	//pDlg->OnReceive(pbufRecv, rec_size);

	AsyncSelect(FD_READ);

	CSocket::OnReceive(nErrorCode);
}

void CMySocket::OnSend(int nErrorCode)
{
	CCleanSocketDlg *pDlg = (CCleanSocketDlg *)m_pPerantDlg;

	pDlg->OnSend(nErrorCode);

	CSocket::OnSend(nErrorCode);
}

void CMySocket::OnClose(int nErrorCode)
{
	CCleanSocketDlg *pDlg = (CCleanSocketDlg *)m_pPerantDlg;

	pDlg->OnClose(iID);

	CSocket::OnClose(nErrorCode);
}
