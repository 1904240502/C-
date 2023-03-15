#ifndef __EVENT_H__
#define __EVENT_H__
#include"Client.h"
#include"Package.h"

class ServerEvent
{
public:
	//客户离开事件
	virtual void onLeave(Client* client)  = 0;
	//客户加入事件
	virtual void onJoin(Client* client) = 0;
	//客户消息事件
	virtual void onMsg(Client *pClient, const DataHeader* header) = 0;
};

#endif //__EVENT_H__