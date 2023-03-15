#include "myServer.h"
void myServer::onJoin(Client* Pclient)
{
	_CLIENT_NUM++;
}
void myServer::onLeave(Client* pClient)
{
	 _CLIENT_NUM-- ;
}
void myServer::onMsg(Client* pClient, const DataHeader* header)
{
	_RECV_COUNT++;
	//��Ϣ����
	switch (header->type)
	{
	case LOGIN:
	{
		Login* data = (Login*)header;
		/*
		* �û���Ϣ��֤
		*/
		//��ͻ��˷�������send
		LoginResult result;
		result.result = 1;
		//SendData(cSock, &result);

	}
	break;
	case LOGOUT:
	{
		Logout* data = (Logout*)header;

		LogoutResult result;
		result.result = 1;
		//SendData(cSock, &result);
	}
	break;
	default:
	{
		//SendData(cSock, header);
	}
	}
}