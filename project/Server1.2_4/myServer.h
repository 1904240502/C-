#ifndef __MYSERVER_H__
#define __MYSERVER_H__
#include "TcpServer.h"
class myServer :
    public TcpServer
{
	//�ͻ��뿪�¼�
	void onLeave(Client* client);
	//�ͻ������¼�
	void onJoin(Client* client);
	//�ͻ���Ϣ�¼�
	void onMsg(Client* pClient, const DataHeader* header);
};

#endif //__MYSERVER_H__