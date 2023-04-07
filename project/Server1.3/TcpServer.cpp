#include "TcpServer.h"

TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
	_MSG_COUNT = 0;
	_CLIENT_NUM = 0;
}
TcpServer::~TcpServer()
{
	Close();
	std::cout << "������˳���" << std::endl;
}
void TcpServer::InitSocket()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	if (INVALID_SOCKET != _sock)
		Close();
	//����socket�׽���
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
		std::cout << "�����socket����ʧ�ܣ�" << std::endl;
	else
		;//std::cout << "����˴����ɹ���" << std::endl;
}
void TcpServer::Bind(const char* IP, unsigned short port)
{
	if (INVALID_SOCKET == _sock)
		InitSocket();
	//�󶨿ͻ�������˿�
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	if (IP) {
		sin.sin_addr.S_un.S_addr = inet_addr(IP);
	}
	else {
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "�󶨶˿�ʧ�ܣ�" << std::endl;
	else
		;// std::cout << "�󶨶˿ڳɹ���" << std::endl;

}
void TcpServer::Listen(int backlog)
{
	//����listen
	if (SOCKET_ERROR == listen(_sock, backlog))
		std::cout << "����˼���ʧ�ܣ�" << std::endl;
	else
		;//std::cout << "����˼����ɹ���" << std::endl;
}
void TcpServer::Accept()
{
	sockaddr_in clientAddr = {};
	int addrlen = sizeof(clientAddr);
	SOCKET cSock = INVALID_SOCKET;

	//���տͻ�������accept
	cSock = accept(_sock, (sockaddr*)&clientAddr, &addrlen);
	if (INVALID_SOCKET == cSock)
		std::cout << "�¿ͻ�����������ʧ�ܣ�" << std::endl;
	else
	{
		//��ӵ��߳̿ͻ���
		selectServer(new Client(cSock));
	}
}
void TcpServer::Close()
{
	std::cout << "TCP Server Close! >>" << std::endl;
	_Thread.Close();
	if (INVALID_SOCKET != _sock)
	{
		//�ȴ�����������رշ����
		for (auto val : _ServerSet)
		{
			delete val;
		}
		_ServerSet.clear();
		closesocket(_sock);
		_sock = INVALID_SOCKET;
		WSACleanup();
	}
	std::cout << "TCP Server Close! <<" << std::endl;
}

void TcpServer::Start(int SERVER_NUM)
{
	std::cout << "TCP Server Start! >>" << std::endl;
	for (int i = 0; i < SERVER_NUM; i++)
	{
		//����������˶���,�����߳�
		_ServerSet.push_back(new EventServer(this, i + 1));
		_ServerSet[i]->Start();
	}
	_Thread.Start(
		nullptr,
		[this](ctlThread* pThread)
		{
			OnRun(pThread);
		},
		nullptr);
	std::cout << "TCP Server Start! <<" << std::endl;
}
void TcpServer::OnRun(ctlThread *pThread)
{
	while(_Thread.isRun())
	{
		//ͳ�ư���
		statistics();
		//�����ļ���������
		fd_set fdRead;
		//����
		FD_ZERO(&fdRead);
		//�������socket��ӵ�������
		FD_SET(_sock, &fdRead);

		//������select
		timeval t = { 0,1 };
		int ret = select((int)_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			std::cout << "�ͻ� select ������" << std::endl;
			_Thread.Exit();
			return;
		}

		//�ж��Ƿ���ڼ�����
		if (FD_ISSET(_sock, &fdRead))
		{
			//count����
			FD_CLR(_sock, &fdRead);
			Accept();
		}
	}
}

void TcpServer::selectServer(Client *pClient)
{
	auto ser = _ServerSet[0];
	auto num = _ServerSet[0]->GetClientNum();
	for (auto val : _ServerSet)
	{
		auto n = val->GetClientNum();
		if (n < num)
		{
			num = n;
			ser = val;
		}
	}
	//����ͻ�������
	ser->addClientToSet(pClient);
}
void TcpServer::statistics()
{
	auto t = _Time.getElapsedSecond();
	if (t > 1.0) {
		int nim = 0;
		/*for (auto val : _ServerSet)
		{
			nim += val->GetClientNum();
			std::cout << val->GetClientNum() << std::endl;
		}*/
		std::cout << "����ʱ�䣺" << t << "����ͻ���Ŀ��" << _CLIENT_NUM  << "�������ݰ���" << _MSG_COUNT  << std::endl;
		_MSG_COUNT = 0;
		_Time.update();
	}
}
void TcpServer::onLeave(Client* pClient)
{
	_CLIENT_NUM--;
}
void TcpServer::onJoin(Client* pClient)
{
	_CLIENT_NUM++;
}
void TcpServer::onMsg(EventServer* pEventServer , Client* pClient, DataHeader* header)
{
	_MSG_COUNT++;
}
void TcpServer::onRecv(Client* pClient)
{
	pClient->resetHeart();
}
//void TcpServer::onMsg(Client* pClient, DataHeader* header) {
//	_MSG_COUNT++;
//}