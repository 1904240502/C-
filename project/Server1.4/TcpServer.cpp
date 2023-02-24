#include "TcpServer.h"

TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
}
TcpServer::~TcpServer()
{
	Close();
}
void TcpServer::InitServer()
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
		std::cout << "����˴����ɹ���" << std::endl;
}
void TcpServer::CreateServer(const char* IP, unsigned short port, int backlog)
{
	if (INVALID_SOCKET == _sock)
		InitServer();
	//�󶨿ͻ�������˿�
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	//sin.sin_addr.S_un.S_addr = inet_addr(IP);
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "�󶨶˿�ʧ�ܣ�" << std::endl;
	else
		std::cout << "�󶨶˿ڳɹ���" << std::endl;

	//����listen
	if (SOCKET_ERROR == listen(_sock, backlog))
		std::cout << "����˼���ʧ�ܣ�" << std::endl;
	else
		std::cout << "����˼����ɹ���" << std::endl;
}
void TcpServer::Close()
{
	if (isCon())
	{
		//�رշ����closesocket
		for (int i = _ClientSets.size() - 1; i >= 0; i--)
			closesocket(_ClientSets[i]);
		closesocket(_sock);
		WSACleanup();
		_sock = INVALID_SOCKET;
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

	//�����пͻ���socket��ӵ�������
	for (int i = 0; i < _ClientSets.size(); i++)
		FD_SET(_ClientSets[i], &fdRead);

	//������select
	timeval t = { 0,0 };
	int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExecept, &t);
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
		sockaddr_in clientAddr = {};
		int addrlen = sizeof(clientAddr);
		SOCKET cSock = INVALID_SOCKET;

		//���տͻ�������accept
		cSock = accept(_sock, (sockaddr*)&clientAddr, &addrlen);
		if (INVALID_SOCKET == cSock)
			std::cout << "�¿ͻ�����������ʧ�ܣ�" << std::endl;
		else
		{
			_ClientSets.push_back(cSock);
			std::cout << "�¿ͻ��� SOCKET =" << cSock << "���ӳɹ���" << std::endl;

			//ת�����������ͻ������¿ͻ�����
			for (int i = 0; i < _ClientSets.size() - 1; i++)
			{
				NewUserJoin NewUser{};
				NewUser.sock = cSock;
				SendData(i, &NewUser);
			}
		}
	}
	//��������
	for (int i = 0; i < fdRead.fd_count; i++)
		if (RecvInfo(fdRead.fd_array[i]))
		{
			//��������ɾ��
			DelClient(fdRead.fd_array[i]);
		}
}
bool TcpServer::isCon()
{
	return SOCKET_ERROR!=_sock;
}
bool TcpServer::RecvInfo(SOCKET cSock)
{
	DataHeader header{};
	int rlen = 0;

	//���ܿͻ�������recv
	rlen = RecvData(cSock, &header, header.dataLength);
	if (rlen <0)
	{
		std::cout << "�ͻ���: " << cSock << "���˳���" << std::endl;
		return true;
	}
	return ExeCom(cSock, &header);
}
bool TcpServer::ExeCom(SOCKET cSock, DataHeader* header)
{
	int rlen = 0;
	int point = sizeof(DataHeader);
	//��Ϣ����
	switch (header->type)
	{
	case LOGIN:
	{
		Login data;
		//rlen=RecvData(cSock, &data + point, data.dataLength - point);
		rlen = recv(cSock, (char *) & data + point, data.dataLength - point,0);
		std::cout << "�ͻ���: " << cSock << "�����¼! " << std::endl;
		/*
		* �û���Ϣ��֤
		*/
		//��ͻ��˷�������send
		LoginResult result;
		result.result = 1;
		SendData(cSock, &result);
		std::cout << "�ͻ���: " << cSock << "��¼�ɹ�! " << std::endl;
	}
	break;
	case LOGOUT:
	{
		Logout data;
		//rlen = RecvData(cSock, &data + point, data.dataLength - point);
		rlen = recv(cSock, (char*)&data + point, data.dataLength - point, 0);
		std::cout << "�ͻ���: " << cSock << "�����˳�!" << std::endl;
		//��ͻ��˷�������send
		LogoutResult result;
		result.result = 1;
		SendData(cSock, &result);
		std::cout << "�ͻ���: " << cSock << "�˳��ɹ�!" << std::endl;
		return true;
	}
	break;
	default:
	{
		DataHeader result;
		SendData(cSock, &result);
	}
	}
	return false;
}
void TcpServer::DelClient(SOCKET sock)
{
	//�ڿͻ��˼���ɾ���˳��Ŀͻ�
	auto iter = std::find(_ClientSets.begin(), _ClientSets.end(), sock);
	if (iter != _ClientSets.end())
		_ClientSets.erase(iter);
}
int TcpServer::SendData(int index, const DataHeader* data)
{
	return send(_ClientSets[index], (const char*)data, data->dataLength, 0);
}
int TcpServer::SendData(SOCKET cSock, const DataHeader* data)
{
	return send(cSock, (const char*)data, data->dataLength, 0);
}
int TcpServer::RecvData(SOCKET cSock, DataHeader* data, int len)
{
	return recv(cSock, (char *)data, len, 0);
}
