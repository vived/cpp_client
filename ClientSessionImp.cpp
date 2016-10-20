#include "StdAfx.h"
#include "ClientSessionImp.h"
#include "ClientSession.h"

using namespace boost::asio::ip;

CClientSessionImp::CClientSessionImp(CAcceptorImp* pActImp )
{
	m_pClientSock = new boost::asio::ip::tcp::socket(*pActImp->GetIOService());
}

CClientSessionImp::~CClientSessionImp(void)
{
	if (m_pClientSock->is_open())
	{
		m_pClientSock->close();
	}
	delete m_pClientSock;
	m_pClientSock = NULL;
}

boost::asio::ip::tcp::socket* CClientSessionImp::GetSocket()
{
	return m_pClientSock;
}

bool CClientSessionImp::IsOpen()
{
	return m_pClientSock->is_open();
}

int CClientSessionImp::CloseSession()
{
	boost::system::error_code ec;	
	ec.clear();
	if (m_pClientSock->is_open())
	{
		m_pClientSock->close(ec);
		if (ec)
		{
			return ec.value();
		}		
	}
	return CLIENT_OPT_SUCCESS;
}

int CClientSessionImp::Send( const char* pSendBuf, size_t szBufSize, int &iErrorCode, const unsigned short &usWaitsecs /*= 0*/ )
{
	iErrorCode = 0;
	boost::system::error_code ec;
	int szSend = 0;
	if (0 == usWaitsecs)
	{
		szSend = m_pClientSock->write_some(boost::asio::buffer(pSendBuf, szBufSize), ec);
		iErrorCode = ec.value();		
	}
	else
	{
		if (!m_pClientSock->is_open())
		{
			m_pClientSock->open(boost::asio::ip::tcp::v4());
		}		

#ifdef WIN32
		m_pClientSock->non_blocking(true, ec);

		szSend = send(m_pClientSock->native_handle(), pSendBuf, szBufSize, 0);
		if (szSend == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
		{
			fd_set fdWrite;
			FD_ZERO(&fdWrite);
			FD_SET(m_pClientSock->native(), &fdWrite);
			timeval tv = {usWaitsecs};
			if (select(0, NULL, &fdWrite, NULL, &tv) <=0
				|| !FD_ISSET(m_pClientSock->native(), &fdWrite))
			{
				iErrorCode = CLIENT_OPT_TIMEOUT;
			}
			else
			{
				szSend = send(m_pClientSock->native_handle(), pSendBuf, szBufSize, 0);
				iErrorCode = WSAGetLastError();
			}	
		}
		else
		{
			iErrorCode = WSAGetLastError();
		}

		m_pClientSock->non_blocking(false,ec);	

#else
		return CLIENT_OPT_NOT_SUPPORT_PLATFORM;
#endif
	}
	return szSend;
}

int CClientSessionImp::Recv( char *pRecvBuf, size_t szMaxRecvSize, int &iErrorCode, const unsigned short &usWaitsecs /*= 0*/ )
{
	using namespace boost::asio::detail::socket_ops;
	iErrorCode = 0;
	boost::system::error_code ec;
	int szRecv = 0;
	if (0 == usWaitsecs)
	{
		szRecv = m_pClientSock->read_some(boost::asio::buffer(pRecvBuf, szMaxRecvSize), ec);
		iErrorCode = ec.value();		
	}
	else
	{
		if (!m_pClientSock->is_open())
		{
			m_pClientSock->open(boost::asio::ip::tcp::v4());
		}		

#ifdef WIN32
		m_pClientSock->non_blocking(true, ec);

		szRecv = recv(m_pClientSock->native_handle(), pRecvBuf, szMaxRecvSize, 0);
		iErrorCode = WSAGetLastError();
		if (szRecv < 0 && WSAEWOULDBLOCK == iErrorCode)
		{
			fd_set fdRead;
			FD_ZERO(&fdRead);
			FD_SET(m_pClientSock->native(), &fdRead);
			timeval tv = {usWaitsecs};
			if (select(0, &fdRead, NULL, NULL, &tv) <= 0
				|| !FD_ISSET(m_pClientSock->native(), &fdRead))
			{
				iErrorCode = CLIENT_OPT_TIMEOUT;
			}
			else
			{
				szRecv = recv(m_pClientSock->native_handle(), pRecvBuf, szMaxRecvSize, 0);	
				iErrorCode = WSAGetLastError();	
			}
		}		
		m_pClientSock->non_blocking(false, ec);
#else
		return CLIENT_OPT_NOT_SUPPORT_PLATFORM;
#endif
	}
	return szRecv;
}

int CAcceptorImp::Accept(CClientSessionImp *pSessionImp, unsigned int uiTimeWaits /*= 0*/ )
{
	boost::system::error_code ec;	
	ec.clear();

	if (0 == uiTimeWaits)
	{
		m_pAcceptor->accept(*(pSessionImp->GetSocket()), ec);
		if (ec)
		{
			return ec.value();
		}
	}
	else
	{
#ifdef WIN32		
		m_pAcceptor->native_non_blocking(true, ec);		
		boost::asio::detail::socket_addr_type  pAddrType;
		size_t szLen = sizeof(pAddrType);

		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(m_pAcceptor->native_handle(), &fdRead);
		timeval tv = {uiTimeWaits};
		if (select(0, &fdRead, 0, NULL, &tv) <=0
			|| !FD_ISSET(m_pAcceptor->native_handle(), &fdRead))
		{
			m_pAcceptor->native_non_blocking(false, ec);
			return CLIENT_OPT_TIMEOUT;
		}
		boost::asio::detail::socket_type new_Sock = boost::asio::detail::socket_ops::accept(m_pAcceptor->native_handle(), 0, &szLen, ec);				
		pSessionImp->GetSocket()->assign(m_pAcceptor->local_endpoint().protocol(), new_Sock, ec);
		if (ec)
		{
			closesocket(new_Sock);
			m_pAcceptor->native_non_blocking(false, ec);
			return ec.value();
		}		
		m_pAcceptor->native_non_blocking(false, ec);
#else
		return CLIENT_OPT_NOT_SUPPORT_PLATFORM;
#endif
	}
	return CLIENT_OPT_SUCCESS;
}

CAcceptorImp::CAcceptorImp( unsigned short usListenPort , std::string strLocalAddress )
{
	m_pIoService = new boost::asio::io_service();
	m_pAcceptor = NULL;
	m_bAcpAvail = true;
	try
	{
		if (strLocalAddress.empty())
		{
			m_pAcceptor = new boost::asio::ip::tcp::acceptor(*m_pIoService, tcp::endpoint(tcp::v4(), usListenPort));
		}
		else
		{
			m_pAcceptor = new boost::asio::ip::tcp::acceptor(*m_pIoService, 
				tcp::endpoint(boost::asio::ip::address::from_string(strLocalAddress), usListenPort));
		}
	}
	catch (...)
	{
		m_bAcpAvail = false;
	}
}

CAcceptorImp::~CAcceptorImp( void )
{
	if (NULL != m_pAcceptor)
	{
		if (m_pAcceptor->is_open())
		{
			m_pAcceptor->close();
		}
		delete m_pAcceptor;
		m_pAcceptor = NULL;
	}
	delete m_pIoService;
	m_pIoService = NULL;
	
}

boost::asio::io_service* CAcceptorImp::GetIOService()
{
	return m_pIoService;
}

bool CAcceptorImp::GetAcpAvail()
{
	return m_bAcpAvail;
}
