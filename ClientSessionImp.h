#pragma once
#include "boost/asio.hpp"

class CAcceptorImp;
class CClientSessionImp
{
public:
	CClientSessionImp(CAcceptorImp* pActImp);
	~CClientSessionImp(void);
public:
	boost::asio::ip::tcp::socket* GetSocket();
	bool IsOpen();
	int CloseSession();
	int Send( const char* pSendBuf, size_t szBufSize, int &iErrorCode, const unsigned short &usWaitsecs /*= 0*/ );
	int Recv(char *pRecvBuf, size_t szMaxRecvSize, int &iErrorCode, const unsigned short &usWaitsecs = 0);
private:	
	boost::asio::ip::tcp::socket *m_pClientSock;	
};

class CAcceptorImp
{
public:
	CAcceptorImp(unsigned short usListenPort, std::string strLocalAddress );
	~CAcceptorImp(void);
public:
	int Accept(CClientSessionImp *pSessionImp, unsigned int uiTimeWaits = 0);
	boost::asio::io_service* GetIOService();
	bool GetAcpAvail();
private:
	boost::asio::ip::tcp::acceptor *m_pAcceptor;
	boost::asio::io_service *m_pIoService;
	bool m_bAcpAvail;
};