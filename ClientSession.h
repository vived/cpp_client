#pragma once

/*
	����:ClientSession.h
	����ܣ��˿��ɷ����ʹ�ã�һ�����Ӷ�Ӧһ��ClientSession����ClientSession��Ч��
			ֱ�ӽ��н��ջ��߷�������
	�汾��1.0
	����:zhangfang 20140220
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

/*��Ҫ����Acceptorʵ�����м���*/
class DllImportOrExport CAcceptor
{
	friend class CClientSession;
public:
	/*	Param1:ָ���˿ں�
		Param2:�󶨱��ص�ַ����Ϊ�գ���ʹ��Ĭ�ϵ�ַ���м���
	*/
	CAcceptor(unsigned short usListenPort, const char *strLocalAddress);
	~CAcceptor();
public:
	/*��������
		Param1:����һ��CClientSession�������Ӻ��CClientSession��Ч���������շ���
		Param2:�ȴ�ʱ�䣬0Ϊ�����ȴ�����λ:��
		ʹ�ô˺���ǰ��ý�����Ч���ж�
	*/
	int Accept(CClientSession *pSession, unsigned int uiTimeWaits = 0);
	/*
		�ж�Acceptor�Ƿ���Ч����Ϊ����Ķ˿ڻ��ߵ�ַ���ܲ���ȷ
	*/
	bool GetAcpAvailable();
private:
	CAcceptorImp *m_pAcpImp;
};

/*�����ӳɹ������SessionΪ����״̬�������շ�����*/
class DllImportOrExport CClientSession
{
	friend class CAcceptor;
public:
	CClientSession(CAcceptor *pAcceptor);
	~CClientSession();
public:
	/*�ж�Socket�Ƿ��*/
	bool IsOpen();
	/*�ر�Socket*/
	int CloseSession();
	/*iErrorCode��ʾ���ش����룬usWaitsecsΪ�ȴ�������0Ϊ����*/
	int Send( const char* pSendBuf, size_t szBufSize, int &iErrorCode, const unsigned short &usWaitsecs = 0 );
	/*iErrorCode��ʾ���ش����룬usWaitsecsΪ�ȴ�������0Ϊ����*/
	int Recv(char *pRecvBuf, size_t szMaxRecvSize, int &iErrorCode, const unsigned short &usWaitsecs = 0);
private:
	CClientSessionImp *m_pImp;
};
