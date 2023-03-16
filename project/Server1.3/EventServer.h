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

	void start();//�����߳�
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
	map<SOCKET,Client*> _Clients;//�ͻ���
	vector<Client*> _ClientSet;//����ͻ���
	char _BUF[BUF_SIZE]; //���ջ���
	thread* _pThread;//�߳�
	Event* _pEvent;//������Ϣ�¼�
	mutex _mutex;
	TaskServer _Tasks;//���������
};

#endif //!__EVENT_SERVER_H__