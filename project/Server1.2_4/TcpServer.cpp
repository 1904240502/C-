#include "TcpServer.h"

//#define S
//#define R_COUT


TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
	_RECV_COUNT = 0;
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

	if (isCon())
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
	if (!isCon())
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
	if (isCon())
	{
		//��֪����Ҫ�˳�
		for (auto& val : _ServerSet)
		{
			val->changeSocket();
		}
		//�ȴ�����������رշ����
		for (auto &val : _ServerSet)
		{
			val->GetThread()->join();
		}
		closesocket(_sock);
		_sock = INVALID_SOCKET;
		WSACleanup();
	}
}
void TcpServer::start()
{
	if (isCon())
	{
		//ͳ�ư���
		statistics();
		//�����ļ���������
		fd_set fdRead{};
		//����
		FD_ZERO(&fdRead);
		//�������socket��ӵ�������
		FD_SET(_sock, &fdRead);

		//������select
		timeval t = { 0,10 };
		int ret = select(_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			std::cout << "�ͻ� select ������" << std::endl;
			Close();
			return;
		}

		//�ж��Ƿ���ڼ�����
		if (FD_ISSET(_sock, &fdRead))
		{
			//count����
			FD_CLR(_sock, &fdRead);
			Accept();
			return;
		}
	}
}
void TcpServer::createServer(int SERVER_NUM)
{
	for (int i = 0; i < SERVER_NUM; i++)
	{
		//����������˶���
		_ServerSet.push_back(new ConsumerServer(this,_sock));
		//�����߳�
		_ServerSet[i]->start();
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
	onJoin(pClient);
}
void TcpServer::statistics()
{
	auto t = _Time.getElapsedSecond();
	if (t > 1.0) {
		int nim = 0;
		for (auto val : _ServerSet)
		{
			nim += val->GetClientNum();
			std::cout << val->GetClientNum() << std::endl;
		}
		std::cout << "����ʱ�䣺" << t << "����ͻ���Ŀ��" << _CLIENT_NUM  << "�������ݣ�" << (int)_RECV_COUNT / t << std::endl;
		_RECV_COUNT = 0;
		_Time.update();
	}
}
bool TcpServer::isCon()
{
	return INVALID_SOCKET != _sock;
}
void TcpServer::onLeave(Client* client)
{
	_CLIENT_NUM--;
}
void TcpServer::onJoin(Client* Plicent)
{
	_CLIENT_NUM++;
}
void TcpServer::onMsg(Client* pClient,const DataHeader* header)
{
	_RECV_COUNT++;
}