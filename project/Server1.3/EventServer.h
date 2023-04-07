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

	void Start();//�����߳�
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
	std::map<SOCKET,Client*> _Clients;//�ͻ���
	std::vector<Client*> _ClientSet;//����ͻ���
	ctlThread _thread; 
	Event* _pEvent;//������Ϣ�¼�
	std::mutex _mutex; //������е���
	TaskServer* _Tasks;//���������
	time_t _oldTime;//��¼�ϴμ������ʱ��
	int _id;
	bool _isChange;
	fd_set _old_fdSet;
	SOCKET _maxfd ;
};

#endif //!__EVENT_SERVER_H__