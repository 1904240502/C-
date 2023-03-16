#ifndef __EVENT_SERVER_H__
#define __EVENT_SERVER_H__

#include"TaskSend.h"
#include"TaskServer.h"
#include"CELLTimestamp.hpp"
#include"Event.h"

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

class EventServer
{
public:
	EventServer(Event* pEvent,SOCKET sock = INVALID_SOCKET);
	~EventServer();

	void start();//创建线程
	void addClientToSet(Client* client);
	int GetClientNum();
	thread* GetThread();
	void changeSocket();
	void addSendTask(Client* pClient,  DataHeader* header);

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
	Event* _pEvent;//网络消息事件
	mutex _mutex;
	TaskServer _Tasks;//服务端任务
};

#endif //!__EVENT_SERVER_H__