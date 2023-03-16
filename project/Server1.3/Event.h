#ifndef __EVENT_H__
#define __EVENT_H__

#include"Client.h"
//声明
class EventServer;
class Event
{
public:
	//客户离开事件
	virtual void onLeave(Client* client)  = 0;
	//客户加入事件
	virtual void onJoin(Client* client) = 0;
	//客户消息事件
	virtual void onMsg(EventServer* pEventServer, Client *pClient, DataHeader* header) = 0;
	//virtual void onMsg( Client* pClient, DataHeader* header) = 0;
};

#endif //!__EVENT_H__