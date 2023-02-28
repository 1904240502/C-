#include "TcpServer.h"

//#define S
//#define R_COUT
TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
	_RECV_COUNT = 0;
}
TcpServer::~TcpServer()
{
	Close();
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
		std::cout << "����˴����ɹ���" << std::endl;
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
		std::cout << "�󶨶˿ڳɹ���" << std::endl;

}
void TcpServer::Listen(int backlog)
{
	//����listen
	if (SOCKET_ERROR == listen(_sock, backlog))
		std::cout << "����˼���ʧ�ܣ�" << std::endl;
	else
		std::cout << "����˼����ɹ���" << std::endl;
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
		Client* client = new Client(cSock);
		_ClientSets.push_back(client);
		//std::cout << "�¿ͻ��� SOCKET =" << cSock << "���ӳɹ���" <<"�û���"<<_ClientSets.size() << std::endl;

		NewUserJoin NewUser;
		NewUser.sock = cSock;

#ifdef S
		SendDataToClients(&NewUser);
#endif // S

		
	}
}
void TcpServer::Close()
{
	if (isCon())
	{
		//�رշ����closesocket
		for (int i = _ClientSets.size() - 1; i >= 0; i--)
		{
			closesocket(_ClientSets[i]->GetSocket());
			delete _ClientSets[i];
		}
		closesocket(_sock);
		WSACleanup();
		_sock = INVALID_SOCKET;

		_ClientSets.clear();
	}
}
void TcpServer::start()
{
	//�����ļ���������
	fd_set fdRead;
	fd_set fdWrite;
	fd_set fdExecept;

	//����
	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExecept);

	//�������socket��ӵ�������
	FD_SET(_sock, &fdRead);
	FD_SET(_sock, &fdWrite);
	FD_SET(_sock, &fdExecept);

	SOCKET maxfd = _sock;
	
	//�����пͻ���socket��ӵ�������
	for (auto client:_ClientSets)
	{
		FD_SET(client->GetSocket(), &fdRead);
		if (maxfd < client->GetSocket())
			maxfd = client->GetSocket();
	}
		
	//������select
	timeval t = { 0,0 };
	int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdExecept, &t);
	if (ret < 0)
	{
		std::cout << "select ������" << std::endl;
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
	//��������
	for (auto client:_ClientSets)
		if (FD_ISSET(client->GetSocket(),&fdRead))
			if (RecvInfo(client))
			{
				//��������ɾ��
				DelClient(client);
			}
		
}
bool TcpServer::isCon()
{
	return INVALID_SOCKET !=_sock;
}
bool TcpServer::RecvInfo(Client* client)
{
	int len = sizeof(DataHeader);
	 
	//���ܿͻ�������recv
	int rlen =recv(client->GetSocket(), _BUF, BUF_SIZE, 0);
	if (rlen < 0)
	{
		//std::cout << "�ͻ���: " << client->GetSocket() << "���˳���" << std::endl;
		return true;
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
			ExeCom(client->GetSocket(), header);
			//�ӻ�����ȡ����
			memcpy(client->GetBufs(), client->GetBufs() + nlen, client->GetPos() - nlen);
			client->SetPos(client->GetPos() - nlen);
		}
		else
			break;
	}
	return false;
}
bool TcpServer::ExeCom(SOCKET cSock, DataHeader * header)
{
	_RECV_COUNT++;
	auto t = _Time.getElapsedSecond();
	if ( t> 1.0) {
		std::cout << "����ʱ�䣺" << t << "����ͻ���Ŀ��"<<_ClientSets.size()<<"�ͻ���" << cSock << "�������ݣ�" << _RECV_COUNT << std::endl;
		_RECV_COUNT = 0;
		_Time.update();
	}
	//��Ϣ����
	switch (header->type)
	{
	case LOGIN:
	{
		Login* data = (Login *)header;
#ifdef R_COUT
		std::cout << "�ͻ���: " << cSock << "�����¼! " << data->userName << std::endl;
#endif // R_COUT

		/*
		* �û���Ϣ��֤
		*/
		//��ͻ��˷�������send
#ifdef S
		LoginResult result;
		result.result = 1;
		SendData(cSock, &result);
#endif // S
#ifdef R_COUT
		std::cout << "�ͻ���: " << cSock << "��¼�ɹ�! " << std::endl;
#endif // R_COUT
		
	}
	break;
	case LOGOUT:
	{
		Logout *data=(Logout *)header;
#ifdef R_COUT
		std::cout << "�ͻ���: " << cSock << "�����˳�!" << data->userName << std::endl;
#endif // R_COUT
		//��ͻ��˷�������send
#ifdef S
		LogoutResult result;
		result.result = 1;
		SendData(cSock, &result);
#endif // S
#ifdef R_COUT
		std::cout << "�ͻ���: " << cSock << "�˳��ɹ�!" << std::endl;;
#endif // R_COUT
		
		return true;
	}
	break;
	default:
	{
#ifdef S
		SendData(cSock, header);
#endif // S
	}
	}
	return false;
}
void TcpServer::DelClient(Client* client)
{
	//�ڿͻ��˼���ɾ���˳��Ŀͻ�
	auto iter = std::find(_ClientSets.begin(), _ClientSets.end(), client);
	if (iter != _ClientSets.end())
	{
		delete client;
		_ClientSets.erase(iter);
	}
		
}
int TcpServer::SendData(SOCKET cSock, const DataHeader* data)
{
	if (isCon() && data)
	{
		return send(cSock, (const char*)data, data->dataLength, 0);
	}
	return SOCKET_ERROR;
}
void TcpServer::SendDataToClients(DataHeader* data)
{
	//ת�����������ͻ������¿ͻ�����
	for (int i = 0; i < _ClientSets.size() - 1; i++)
	{
		SendData(_ClientSets[i]->GetSocket(), data);
	}
}
