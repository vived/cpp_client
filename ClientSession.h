#pragma once

/*
	名称:ClientSession.h
	库介绍：此库由服务端使用，一个连接对应一个ClientSession，当ClientSession有效后
			直接进行接收或者发送数据
	版本：1.0
	作者:zhangfang 20140220
*/

#ifdef CLIENTSESSION_CPP
#define DllImportOrExport __declspec(dllexport)
#else
#define DllImportOrExport __declspec(dllimport)
#endif

const int CLIENT_OPT_TIMEOUT = -255;
const int CLIENT_OPT_SUCCESS = 0;
const int CLIENT_OPT_FAILED	=	-1;
const int CLIENT_OPT_NOT_SUPPORT_PLATFORM = -256;

class CClientSessionImp;
class CClientSession;
class CAcceptorImp;

/*需要声明Acceptor实例进行监听*/
class DllImportOrExport CAcceptor
{
	friend class CClientSession;
public:
	/*	Param1:指定端口号
		Param2:绑定本地地址，如为空，则使用默认地址进行监听
	*/
	CAcceptor(unsigned short usListenPort, const char *strLocalAddress);
	~CAcceptor();
public:
	/*接收连接
		Param1:传入一个CClientSession，有连接后此CClientSession有效进行数据收发。
		Param2:等待时间，0为阻塞等待，单位:秒
		使用此函数前最好进行有效性判断
	*/
	int Accept(CClientSession *pSession, unsigned int uiTimeWaits = 0);
	/*
		判断Acceptor是否有效，因为传入的端口或者地址可能不正确
	*/
	bool GetAcpAvailable();
private:
	CAcceptorImp *m_pAcpImp;
};

/*有连接成功，则此Session为可用状态，可以收发数据*/
class DllImportOrExport CClientSession
{
	friend class CAcceptor;
public:
	CClientSession(CAcceptor *pAcceptor);
	~CClientSession();
public:
	/*判断Socket是否打开*/
	bool IsOpen();
	/*关闭Socket*/
	int CloseSession();
	/*iErrorCode提示返回错误码，usWaitsecs为等待秒数，0为阻塞*/
	int Send( const char* pSendBuf, size_t szBufSize, int &iErrorCode, const unsigned short &usWaitsecs = 0 );
	/*iErrorCode提示返回错误码，usWaitsecs为等待秒数，0为阻塞*/
	int Recv(char *pRecvBuf, size_t szMaxRecvSize, int &iErrorCode, const unsigned short &usWaitsecs = 0);
private:
	CClientSessionImp *m_pImp;
};
