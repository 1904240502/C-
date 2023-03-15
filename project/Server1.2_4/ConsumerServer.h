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

	void start();//�����߳�
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
	map<SOCKET,Client*> _Clients;//�ͻ���
	vector<Client*> _ClientSet;//����ͻ���
	char _BUF[BUF_SIZE]; //���ջ���
	thread* _pThread;//�߳�
	ServerEvent* _pEvent;//������Ϣ�¼�
	mutex _mutex;
};

#endif //__COMSUMER_SERVER_H__