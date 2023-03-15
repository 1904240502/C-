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
	std::cout << "服务端处理退出！" << std::endl;
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
		//缓冲客户集->客户集
		if (addToClients())
			isChange = true;
		//没有客户
		if (_Clients.empty())
		{
			//避免客户空处理
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}

		//创建文件描述符集
		fd_set fdRead{};
		//置零
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
			std::cout << "消息 select 结束！" << std::endl;
			Close();
			return;
		}
		else if (ret == 0)
		{
			continue;
		}

		//处理监听事件
		vector<Client*> temp;
		for (auto val : _Clients)
		{
			if (FD_ISSET(val.first, &fdRead))
			{
				if (!RecvInfo(val.second))
				{
					//在server中删除客户
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
	//线程退出
	
	if (!_Clients.empty())
	{
		/*
	    * 接收完整消息后再推出
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
		//清理关闭客户连接
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
	//获得数据长度
	auto len = sizeof(DataHeader);

	//接收客户数据
	auto rlen = recv(client->GetSocket(), _BUF, sizeof(_BUF), 0);
	if (rlen < 0)
	{
		//std::cout << "客户端: " << client->GetSocket() << "已退出！" << std::endl;
		return false;
	}
	memcpy(client->GetBufs() + client->GetPos(), _BUF, rlen);
	client->SetPos(client->GetPos() + rlen);

	//接收数据
	while (client->GetPos() >= len)
	{
		//获得数据长度
		DataHeader* header = (DataHeader*)client->GetBufs();
		//缓冲中已经有完整的包
		if (client->GetPos() >= header->dataLength)
		{
			int nlen = header->dataLength;
			//处理消息
			onMsg(client, header);
			//从缓冲中取出包
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


