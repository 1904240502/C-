#ifndef __CONSUMER_SERVER_H__
#define __COMSUMER_SERVER_H__

#include"CELLTimestamp.hpp"
#include"ServerEvent.h"

#include<map>
#include<vector>
#include<mutex>
#include <functional>

using std::vector;
using std::map;
using std::thread;
using std::mutex;
using std::cout;
using std::endl;

class ConsumerServer
{
public:
	ConsumerServer(ServerEvent* pEvent,SOCKET sock = INVALID_SOCKET);
	~ConsumerServer();

	void start();//创建线程
	void addClientToSet(Client* client);
	int GetClientNum();
	thread* GetThread();
	void changeSocket();

private:
	void Run();
	bool isCon();
	void Close();
	bool addToClients();
	bool RecvInfo(Client* client);
	void onMsg(Client* pClient, DataHeader* header);
private:
	SOCKET _sock;
	map<SOCKET,Client*> _Clients;//客户集
	vector<Client*> _ClientSet;//缓冲客户集
	char _BUF[BUF_SIZE]; //接收缓冲
	thread* _pThread;//线程
	ServerEvent* _pEvent;//网络消息事件
	mutex _mutex;
};

#endif //__COMSUMER_SERVER_H__