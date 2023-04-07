#ifndef __EVENT_SERVER_H__
#define __EVENT_SERVER_H__

#include"TaskSend.h"
#include"TaskServer.h"
#include"CELLTimestamp.hpp"
#include"Event.h"

#include<map>
#include<vector>


class EventServer
{
public:
	EventServer(Event* pEvent,int id = -1);
	~EventServer();

	void Start();//创建线程
	void addClientToSet(Client* client);
	int GetClientNum();
	void addSendTask(Client* pClient,  DataHeader* header);

private:
	void OnRun(ctlThread * pThread);
	void ClearClients();
	void Close();
	bool addToClients();
	bool RecvInfo(Client* client);
	void onMsg(Client* pClient, DataHeader* header);
	void ReadData(const fd_set& fd);
	void WriterData(const fd_set& fd);
	void ClientLeave(Client* client);
	void CheckClient();
private:
	std::map<SOCKET,Client*> _Clients;//客户集
	std::vector<Client*> _ClientSet;//缓冲客户集
	ctlThread _thread; 
	Event* _pEvent;//网络消息事件
	std::mutex _mutex; //缓冲队列的锁
	TaskServer* _Tasks;//服务端任务
	time_t _oldTime;//记录上次检测心跳时间
	int _id;
	bool _isChange;
	fd_set _old_fdSet;
	SOCKET _maxfd ;
};

#endif //!__EVENT_SERVER_H__