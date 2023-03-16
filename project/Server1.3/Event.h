#ifndef __EVENT_H__
#define __EVENT_H__

#include"Client.h"
//����
class EventServer;
class Event
{
public:
	//�ͻ��뿪�¼�
	virtual void onLeave(Client* client)  = 0;
	//�ͻ������¼�
	virtual void onJoin(Client* client) = 0;
	//�ͻ���Ϣ�¼�
	virtual void onMsg(EventServer* pEventServer, Client *pClient, DataHeader* header) = 0;
	//virtual void onMsg( Client* pClient, DataHeader* header) = 0;
};

#endif //!__EVENT_H__