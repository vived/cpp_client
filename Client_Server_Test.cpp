//测试ServerSession部分

#include "stdafx.h"
#include "../ClientSession/ClientSession.h"
#include "../ServerSession/ServerSession.h"

#include <windows.h>
#include <process.h>
#include <vector>
using namespace std;

#if 0

static CAcceptor accepter(4006,"127.0.0.1");
static vector<HANDLE> lstThd;
HANDLE hThread_StartServer; 


int iErrorCode;

//开启server端
unsigned __stdcall RecServerThread(void* pArguments)
{
	using namespace std;
	CClientSession *pClientSession = (CClientSession*)pArguments;
	char pRecvBuf[100] = {0};
	while(1)
	{
		int iErr = 0;
		if (pClientSession->Recv(pRecvBuf, 100,iErr, 5) <= 0)
		{
			if (CLIENT_OPT_TIMEOUT == iErr)
			{
				continue;
			}
			pClientSession->CloseSession();
			break;
		}
		if (strcmp(pRecvBuf, "a") == 0)
		{
			pClientSession->Send("b", 2, iErr);
		}
		else if (strcmp(pRecvBuf, "b") == 0)
		{
			pClientSession->Send("c", 2, iErr);
		}
		else if (strcmp(pRecvBuf, "c") == 0)
		{
			pClientSession->Send("d", 2, iErr);
		}
		
	}
	delete pClientSession;
	return 0;

	
}

unsigned __stdcall StartServerThread(void* pArguments)
{
	int iAcpRet = 0;
	
	
	while(0 == iAcpRet)
	{
		CClientSession *pCltSession = new CClientSession(&accepter);
		iAcpRet = accepter.Accept(pCltSession); 
		if (0 == iAcpRet)
		{
			unsigned threadID_RecServer;
			HANDLE hThread_RecServer = (HANDLE)_beginthreadex( NULL, 0, &RecServerThread, (void*)pCltSession, 0, &threadID_RecServer );
			lstThd.push_back(hThread_RecServer);
		}
		
	}
	return 0;
}


//测试Acceptor中GetAcpAvailable的有效性，开启Server，开始测试ClientSession
TEST(Acceptor,GAAvailable)
{
	unsigned threadID_StartServer;
	ASSERT_TRUE(accepter.GetAcpAvailable());
	lstThd.clear();
	hThread_StartServer = (HANDLE)_beginthreadex( NULL, 0, &StartServerThread, NULL, 0, &threadID_StartServer );
}


 TEST(ServerSession,Block)
 {

	CServerSession svcSession1("127.0.0.1",4006), svcSession2("127.0.0.1",4006), svcSession3("127.0.0.1",4006);


	char bufRecv1[4] = {0};
	EXPECT_EQ(svcSession1.Connect(), 0);	
	svcSession1.Send("a", 2, iErrorCode);
	svcSession1.Recv(bufRecv1, 4, iErrorCode);	
	EXPECT_EQ(strcmp(bufRecv1, "b"), 0);

	

	char bufRecv2[4] = {0};
	EXPECT_EQ(svcSession2.Connect(), 0);	
	svcSession2.Send("b", 2, iErrorCode);
	svcSession2.Recv(bufRecv2, 4, iErrorCode);
	EXPECT_TRUE(svcSession2.IsOpen());
	EXPECT_EQ(strcmp(bufRecv2, "c"), 0);
	
	char bufRecv3[4] = {0};
	EXPECT_EQ(svcSession3.Connect(), 0);	
	svcSession3.Send("c", 2, iErrorCode);
	svcSession3.Recv(bufRecv3, 4, iErrorCode);
	EXPECT_EQ(strcmp(bufRecv3, "d"), 0);
	

	svcSession1.CloseSession();
	svcSession2.CloseSession();
	svcSession3.CloseSession();
	
	for (size_t szThd = 0; szThd < lstThd.size(); ++szThd)
	{
		WaitForSingleObject(lstThd[szThd], INFINITE);
	}

 }

// TEST(ServerSession,Close)
// {
// 	svcSession.CloseSession();
// 	WaitForSingleObject(hThread_StartServer, INFINITE);
// 	EXPECT_FALSE(cltSession.IsOpen());
// }

#endif