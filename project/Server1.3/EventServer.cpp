#include "EventServer.h"

EventServer::EventServer(Event* pEvent,int id):_id(id)
{
	_pEvent = pEvent;
	_oldTime = 0;
	_isChange = false;
	_maxfd = 0;
	FD_ZERO(&_old_fdSet);
}

EventServer::~EventServer()
{
	Close();
}

void EventServer::Start()
{
	std::cout << "Event Server Start!  >" << _id << std::endl;
	//����������
	_Tasks = new TaskServer(_id);
	_Tasks->Start();
	//��ʼ���н��տͻ���Ϣ
	_thread.Start(
		nullptr,
		[this](ctlThread* pThread) {
			OnRun(pThread);
		},
		[this](ctlThread* pThread) {
			ClearClients();
		}
		);
	std::cout << "Event Server Start!  <" << _id << std::endl;
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

void EventServer::addSendTask(Client* pClient, DataHeader* header)
{
	_Tasks->addTask(new TaskSend(pClient, header));
}

void EventServer::OnRun(ctlThread* pThread)
{
	while (_thread.isRun())
	{
		//����ͻ���->�ͻ���
		if (addToClients())
			_isChange = true;
		//û�пͻ�
		if (_Clients.empty())
		{
			//����ͻ��մ���
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			_oldTime = cellTime::getTimeInSec();
			continue;
		}

		//�����ļ���������
		fd_set fdRead;
		fd_set fdWrite;
		//����
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);

		if (_isChange)
		{
			_maxfd = _Clients.begin()->first;
			for (auto& val : _Clients)
			{
				FD_SET(val.first, &fdRead);
				if (val.first > _maxfd)
					_maxfd = val.first;
			}
			memcpy(&_old_fdSet, &fdRead, sizeof(fd_set));
			_isChange = false;
		}
		else {
			memcpy(&fdRead, &_old_fdSet, sizeof(fd_set));
		}
		memcpy(&fdWrite, &_old_fdSet, sizeof(fd_set));

		timeval t{ 0,1 };
		int ret = select((int)_maxfd + 1, &fdRead, &fdWrite, nullptr, &t);
		if (ret < 0)
		{
			std::cout << "��Ϣ select ������" << std::endl;
			pThread->Exit(); //����
			return;
		}
		else if (ret == 0)
		{
			continue;
		}
		ReadData(fdRead);
		WriterData(fdWrite);
		CheckClient();
	}
	return;
}

void EventServer::ClearClients()
{
	//����رտͻ�����
	for (int i = 0; i < _ClientSet.size(); i++)
	{
		delete _ClientSet[i];
	}
	_ClientSet.clear();
	for (int i = 0; i < _Clients.size(); i++)
	{
		delete _Clients[i];
	}
	_Clients.clear();
}

void EventServer::Close()
{
	std::cout << "Event Server Exit!  >"<< _id << std::endl;
	_thread.Close();
	delete _Tasks;
	std::cout << "Event Server Exit!  <" << _id << std::endl;
}

bool EventServer::addToClients()
{
	if (_ClientSet.empty())
		return false;
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto val : _ClientSet)
	{
		val->_sId = _id;
		_Clients[val->GetSocket()] = val;
		
		if (_pEvent)
			_pEvent->onJoin(val);
	}
	_ClientSet.clear();
	return true;
}

bool EventServer::RecvInfo(Client* client)
{
	//���տͻ�����
	auto rlen = client->RecvData();
	if (rlen < 0)
	{
		//std::cout << "�ͻ���: " << client->GetSocket() << "���˳���" << std::endl;
		return false;
	}
	_pEvent->onRecv(client);
	//������Ϣ

	while (client->hasData())
	{
		onMsg(client, client->GetCommand());
	}
	
	return true;
}

void EventServer::onMsg(Client * pClient, DataHeader* header)
{	
	_pEvent->onMsg(this,pClient,header);
	//_pEvent->onMsg(pClient,header);
}

void EventServer::ClientLeave(Client* client)
{
	if (_pEvent)
		_pEvent->onLeave(client);

	_isChange = true;
	delete client;
}

void EventServer::ReadData(const fd_set& fd)
{
	if (fd.fd_count == 0)
		return;
	for (auto iter = _Clients.begin(); iter != _Clients.end();)
	{
		if (FD_ISSET(iter->first, &fd))
		{
			if (!RecvInfo(iter->second))
			{
				//��server��ɾ���ͻ�
				ClientLeave(iter->second);
				iter = _Clients.erase(iter);
				continue;
			}
		}
		iter++;
	}
}

void EventServer::WriterData(const fd_set& fd)
{
	if (fd.fd_count == 0)
		return;
	for (auto iter = _Clients.begin(); iter != _Clients.end();)
	{
		if (FD_ISSET(iter->first, &fd))
		{
			if (iter->second->SendDateReal()==-1)
			{
				//��server��ɾ���ͻ�
				ClientLeave(iter->second);
				iter = _Clients.erase(iter);
				continue;
			}
		}
		iter++;
	}
}

void EventServer::CheckClient()
{
	auto nowTime = cellTime::getTimeInSec();
	auto survival_time = nowTime - _oldTime;
	_oldTime = nowTime;
	for (auto iter = _Clients.begin(); iter != _Clients.end();)
	{
		//�������
		if (iter->second->checkHeart(survival_time))
		{
			//��server��ɾ���ͻ�
			ClientLeave(iter->second);
			iter = _Clients.erase(iter);
			continue;
		}
		//��ʱ���ͼ��
		if (iter->second->checkSend(survival_time))
		{
			//����������ݷ���
			iter->second->SendDateReal();
		}
		iter++;
	}
}

