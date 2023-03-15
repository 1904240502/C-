#include "ConsumerServer.h"

ConsumerServer::ConsumerServer(ServerEvent *pEvent,SOCKET sock):_sock(sock)
{
	memset(_BUF, 0, sizeof(_BUF));
	_pThread = nullptr;
	_pEvent = pEvent;
}

ConsumerServer::~ConsumerServer()
{
	Close();
	_sock = INVALID_SOCKET;
	delete _pThread;
	std::cout << "����˴����˳���" << std::endl;
}

void ConsumerServer::start()
{
	_pThread = new std::thread(std::mem_fn(&ConsumerServer::Run), this);
}

void ConsumerServer::addClientToSet(Client* client)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_ClientSet.push_back(client);
}

int ConsumerServer::GetClientNum()
{
	return _Clients.size() + _ClientSet.size();
}

std::thread* ConsumerServer::GetThread()
{
	return _pThread;
}

void ConsumerServer::changeSocket()
{
	_sock = INVALID_SOCKET;
}

void ConsumerServer::Run()
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

		int ret = select(maxfd + 1, &fdRead, nullptr, nullptr, nullptr);
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

bool ConsumerServer::isCon()
{
	return INVALID_SOCKET != _sock;
}

void ConsumerServer::Close()
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

bool ConsumerServer::addToClients()
{
	if (_ClientSet.empty())
		return false;
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto val : _ClientSet)
		_Clients[val->GetSocket()] = val;
	_ClientSet.clear();
	return true;
}

bool ConsumerServer::RecvInfo(Client* client)
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
	memcpy(client->GetBufs() + client->GetPos(), _BUF, rlen);
	client->SetPos(client->GetPos() + rlen);

	//��������
	while (client->GetPos() >= len)
	{
		//������ݳ���
		DataHeader* header = (DataHeader*)client->GetBufs();
		//�������Ѿ��������İ�
		if (client->GetPos() >= header->dataLength)
		{
			int nlen = header->dataLength;
			//������Ϣ
			onMsg(client, header);
			//�ӻ�����ȡ����
			memcpy(client->GetBufs(), client->GetBufs() + nlen, client->GetPos() - nlen);
			client->SetPos(client->GetPos() - nlen);
		}
		else
			break;
	}

	return true;
}

void ConsumerServer::onMsg(Client * pClient, DataHeader* header)
{	
	_pEvent->onMsg(pClient,header);
}


