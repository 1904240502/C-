#include "TcpServerForMe.h"
void TcpServerForMe::onJoin(Client* pClient)
{
	TcpServer::onJoin(pClient);
}
void TcpServerForMe::onLeave(Client* pClient)
{
	TcpServer::onLeave(pClient);
}
void TcpServerForMe::onMsg(EventServer* pEventServer,Client* pClient,DataHeader* header)
{
	TcpServer::onMsg(pEventServer,pClient, header);
	//��Ϣ����
	switch (header->type)
	{
	case LOGIN:
	{
		Login* data = (Login*)header;

		//cout << "�յ���Ϣ" << endl;
		/*
		* �û���Ϣ��֤
		*/
		//����һ����Ϣ
		LoginResult* result = new LoginResult(1);
		//�߳����ӷ�������
		pEventServer->addSendTask(pClient, result);
	}
	break;
	case LOGOUT:
	{
		Logout* data = (Logout*)header;

		LogoutResult* result = new LogoutResult(1);
		//pClient->SendData(&result);
	}
	break;
	case HEART2S:
	{
		pClient->resetHeart();
		Heart2Client* result = new Heart2Client();
		pEventServer->addSendTask(pClient, result);
	}
	break;
	default:
	{
		//pClient->SendData(header);
	}
	}
}
//void TcpServerForMe::onMsg(Client* pClient, DataHeader* header) {
//	//TcpServer::onMsg( pClient, header);
//}