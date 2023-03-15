#ifndef __MYSERVER_H__
#define __MYSERVER_H__
#include "TcpServer.h"
class myServer :
    public TcpServer
{
	//客户离开事件
	void onLeave(Client* client);
	//客户加入事件
	void onJoin(Client* client);
	//客户消息事件
	void onMsg(Client* pClient, const DataHeader* header);
};

#endif //__MYSERVER_H__