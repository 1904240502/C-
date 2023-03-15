#ifndef __EVENT_H__
#define __EVENT_H__
#include"Client.h"
#include"Package.h"

class ServerEvent
{
public:
	//�ͻ��뿪�¼�
	virtual void onLeave(Client* client)  = 0;
	//�ͻ������¼�
	virtual void onJoin(Client* client) = 0;
	//�ͻ���Ϣ�¼�
	virtual void onMsg(Client *pClient, const DataHeader* header) = 0;
};

#endif //__EVENT_H__