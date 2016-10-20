// ClientSession.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ClientSession.h"
#include "ClientSessionImp.h"

CClientSession::CClientSession(CAcceptor *pAcceptor )
{
	m_pImp = new CClientSessionImp(pAcceptor->m_pAcpImp/*GetAcpImp()*/);
}

CClientSession::~CClientSession()
{
	if (NULL != m_pImp)
	{
		delete m_pImp;
		m_pImp = NULL;
	}
}

bool CClientSession::IsOpen()
{
	return (NULL == m_pImp) ? false : m_pImp->IsOpen();
}

int CClientSession::CloseSession()
{
	return (NULL == m_pImp) ? CLIENT_OPT_FAILED : m_pImp->CloseSession();
}

int CClientSession::Send( const char* pSendBuf, size_t szBufSize, int &iErrorCode, const unsigned short &usWaitsecs /*= 0*/ )
{
	return (NULL == m_pImp) ? CLIENT_OPT_FAILED : m_pImp->Send(pSendBuf, szBufSize, iErrorCode, usWaitsecs);
}

int CClientSession::Recv( char *pRecvBuf, size_t szMaxRecvSize, int &iErrorCode, const unsigned short &usWaitsecs /*= 0*/ )
{
	return (NULL == m_pImp) ? CLIENT_OPT_FAILED : m_pImp->Recv(pRecvBuf, szMaxRecvSize, iErrorCode, usWaitsecs);
}

int CAcceptor::Accept(CClientSession *pSession, unsigned int uiTimeWaits /*= 0*/ )
{
	if (NULL == m_pAcpImp)
	{
		return CLIENT_OPT_FAILED;
	}
	return (NULL == m_pAcpImp) ? CLIENT_OPT_FAILED : m_pAcpImp->Accept(pSession->m_pImp, uiTimeWaits);
}

CAcceptor::CAcceptor( unsigned short usListenPort, const char *strLocalAddress )
{
	m_pAcpImp = new CAcceptorImp(usListenPort, strLocalAddress);
}

CAcceptor::~CAcceptor()
{
	if (m_pAcpImp)
	{
		delete m_pAcpImp;
		m_pAcpImp = NULL;
	}
}

bool CAcceptor::GetAcpAvailable()
{
	return (NULL == m_pAcpImp) ? false : m_pAcpImp->GetAcpAvail();
}
