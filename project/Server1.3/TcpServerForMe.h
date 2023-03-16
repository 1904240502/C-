#ifndef __TCP_SERVER_FOR_ME_H__
#define __TCP_SERVER_FOR_ME_H__
#include "TcpServer.h"

class TcpServerForMe :
    public TcpServer
{
	//客户离开事件
	virtual void onLeave(Client* client);
	//客户加入事件
	virtual void onJoin(Client* client);
	//客户消息事件
	virtual void onMsg(EventServer* pEventServer,Client* pClient, DataHeader* header);
	//virtual void onMsg( Client* pClient, DataHeader* header);
};

#endif //!__TCP_SERVER_FOR_ME_H__