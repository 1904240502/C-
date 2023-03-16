#include "EventServer.h"

EventServer::EventServer(Event* pEvent,SOCKET sock):_sock(sock)
{
	memset(_BUF, 0, sizeof(_BUF));
	_pThread = nullptr;
	_pEvent = pEvent;
}

EventServer::~EventServer()
{
	Close();
	_sock = INVALID_SOCKET;
	delete _pThread;
	std::cout << "����˴����˳���" << std::endl;
}

void EventServer::start()
{
	//�������տͻ���Ϣ
	_pThread = new std::thread(std::mem_fn(&EventServer::Run), this);
	//��������������ͻ�
	_Tasks.Start();
}

void EventServer::addClientToSet(Client* client)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_ClientSet.push_back(client);
}

int EventServer::GetClientNum()
{
	return (int)(_Clients.size() + _ClientSet.size());
}

std::thread* EventServer::GetThread()
{
	return _pThread;
}

void EventServer::changeSocket()
{
	_sock = INVALID_SOCKET;
}

void EventServer::addSendTask(Client* pClient, DataHeader* header)
{
	_Tasks.addTask(new TaskSend(pClient, header));
}

void EventServer::Run()
{
	fd_set cache{};
	FD_ZERO(&cache);

	bool isChange=false;
	SOCKET maxfd = 0;
	while (isCon())
	{
		//����ͻ���->�ͻ���
		if (addToClients())
			isChange = true;
		//û�пͻ�
		if (_Clients.empty())
		{
			//����ͻ��մ���
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}

		//�����ļ���������
		fd_set fdRead{};
		//����
		FD_ZERO(&fdRead);
		if (isChange)
		{
			maxfd = _Clients.begin()->first;
			for (auto& val : _Clients)
			{
				FD_SET(val.first, &fdRead);
				if (val.first > maxfd)
					maxfd = val.first;
			}
			memcpy(&cache, &fdRead, sizeof(fd_set));
			isChange = false;
		}
		else {
			memcpy(&fdRead, &cache, sizeof(fd_set));
		}

		int ret = select((int)maxfd + 1, &fdRead, nullptr, nullptr, nullptr);
		if (ret < 0)
		{
			std::cout << "��Ϣ select ������" << std::endl;
			Close();
			return;
		}
		else if (ret == 0)
		{
			continue;
		}

		//��������¼�
		vector<Client*> temp;
		for (auto val : _Clients)
		{
			if (FD_ISSET(val.first, &fdRead))
			{
				if (!RecvInfo(val.second))
				{
					//��server��ɾ���ͻ�
					if (_pEvent)
						_pEvent->onLeave(val.second);

					temp.push_back(val.second);
					isChange = true;
				}
			}
		}

		for (auto val : temp)
		{
			_Clients.erase(val->GetSocket());
			closesocket(val->GetSocket());
			delete val;
		}
	}
	//�߳��˳�
	
	if (!_Clients.empty())
	{
		/*
	    * ����������Ϣ�����Ƴ�
	    */
	}
	return;
}

bool EventServer::isCon()
{
	return INVALID_SOCKET != _sock;
}

void EventServer::Close()
{
	if (isCon())
	{
		//����رտͻ�����
		for (int i=0;i<_ClientSet.size();i++)
		{
			closesocket(_ClientSet[i]->GetSocket());
			delete _ClientSet[i];
		}
		_ClientSet.clear();
		for (int i = 0; i < _Clients.size(); i++)
		{
			closesocket(_Clients[i]->GetSocket());
			delete _Clients[i];
		}
		_Clients.clear();
	}
}

bool EventServer::addToClients()
{
	if (_ClientSet.empty())
		return false;
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto val : _ClientSet)
		_Clients[val->GetSocket()] = val;
	_ClientSet.clear();
	return true;
}

bool EventServer::RecvInfo(Client* client)
{
	//������ݳ���
	auto len = sizeof(DataHeader);

	//���տͻ�����
	auto rlen = recv(client->GetSocket(), _BUF, sizeof(_BUF), 0);
	if (rlen < 0)
	{
		//std::cout << "�ͻ���: " << client->GetSocket() << "���˳���" << std::endl;
		return false;
	}
	memcpy(client->GetRecvBufs() + client->GetRecvPos(), _BUF, rlen);
	client->SetRecvPos(client->GetRecvPos() + rlen);

	//��������
	while (client->GetRecvPos() >= len)
	{
		//������ݳ���
		DataHeader* header = (DataHeader*)client->GetRecvBufs();
		//�������Ѿ��������İ�
		if (client->GetRecvPos() >= header->dataLength)
		{
			int nlen = header->dataLength;
			//������Ϣ
			onMsg(client, header);
			//�ӻ�����ȡ����
			memcpy(client->GetRecvBufs(), client->GetRecvBufs() + nlen, client->GetRecvPos() - nlen);
			client->SetRecvPos(client->GetRecvPos() - nlen);
		}
		else
			break;
	}

	return true;
}

void EventServer::onMsg(Client * pClient, DataHeader* header)
{	
	_pEvent->onMsg(this,pClient,header);
	//_pEvent->onMsg(pClient,header);
}


