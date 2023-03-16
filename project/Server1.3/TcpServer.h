#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include"EventServer.h"

class TcpServer:public Event
{
public :
	TcpServer();
	virtual ~TcpServer();

	//创建socket
	void InitSocket();
	//绑定端口
	void Bind(const char* IP, unsigned short port);
	//监听
	void Listen(int backlog);
	//关闭socket
	void Close();

	//开启server服务
	void start();
	//开启消息处理服务
	void createServer(int SERVER_NUM);
	
	//客户离开事件
	virtual void onLeave(Client* client) = 0;
	//客户加入事件
	virtual void onJoin(Client* client) = 0;
	//客户消息事件
	virtual void onMsg(EventServer* pEventServer, Client* pClient,  DataHeader* header) = 0;
	//virtual void onMsg(Client* pClient, DataHeader* header) = 0;
protected:
	//记录接收客户包数
    std::atomic_int _RECV_COUNT;
	//记录服务客户数目
	std::atomic_int _CLIENT_NUM;
private:
	//连接客户端
	void Accept();
	//判断服务端是否运行
	bool isCon();
	//选择加入客户消息处理的服务端
	void selectServer(Client * pClient);
	//统计运行情况
	void statistics();
private:
	SOCKET _sock;//服务端套接字
	vector<EventServer*> _ServerSet; //处理集
	CELLTimestamp _Time;
};
#endif //__TCP_SERVER_H__
