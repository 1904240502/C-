#ifndef __TCP_SERVER_FOR_ME_H__
#define __TCP_SERVER_FOR_ME_H__
#include "TcpServer.h"

class TcpServerForMe :
    public TcpServer
{
	//�ͻ��뿪�¼�
	virtual void onLeave(Client* client);
	//�ͻ������¼�
	virtual void onJoin(Client* client);
	//�ͻ���Ϣ�¼�
	virtual void onMsg(EventServer* pEventServer,Client* pClient, DataHeader* header);
	//virtual void onMsg( Client* pClient, DataHeader* header);
};

#endif //!__TCP_SERVER_FOR_ME_H__