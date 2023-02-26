#include "TcpClient.h"

TcpClient::TcpClient()
{
	_sock = INVALID_SOCKET;
}
TcpClient::~TcpClient()
{
		Close();
}
void TcpClient::InitSocket()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//�����������������Ͽ��������´���
	if (isCon())
	{
		std::cout << "socket=" << _sock << "�����ӹر�" << std::endl;
		Close();
	}
	//����socket�׽���
	 _sock= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!isCon())
		std::cout << "�ͻ���socket����ʧ�ܣ�" << std::endl;
	else
		std::cout << "�ͻ��˴����ɹ���" << std::endl;
}
void TcpClient::Connect(const char * IP,unsigned short port)
{
	if (!isCon())
	{
		InitSocket();
	}
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = inet_addr(IP);
	if (SOCKET_ERROR == connect(_sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout  <<"socket=" << _sock <<"��������http:\\\\"<<IP <<":"<< port <<"ʧ�ܣ�"<< std::endl;
	else
		std::cout << "socket=" << _sock << "�ɹ�������http:\\\\" << IP << ":" << port << "�ɹ���" << std::endl;
}
void TcpClient::Close()
{
	if (isCon())
	{
		closesocket(_sock);
		WSACleanup();
		std::cout << "�ͻ���"<<_sock<<"�˳���" << std::endl;
		_sock = INVALID_SOCKET;
	}
}
bool TcpClient::SendPro(){
	char buffer[1024] = {};
	int rlen = 0;

	while (true)
	{
		//��������
		std::cin >> buffer;

		//��������
		if (0 == strcmp(buffer, "exit"))
			Close();
		else if (0 == strcmp(buffer, "login")) {
			//����������Ϣ
			Login login;
			strcpy_s(login.userName, "zhang san");
			strcpy_s(login.passWord, "123456");
			SendData(&login);
		}
		else if (0 == strcmp(buffer, "logout"))
		{
			//����������Ϣ
			Logout logout;
			strcpy_s(logout.userName, "li si");
			SendData(&logout);
		}
		else
		{
			std::cout << "�����������" << std::endl;
		}
	}
	return false;
}
bool TcpClient::RecvInfo()
{
	
	char buffer[1024];
	int rlen;

	//���շ����ָ��
	rlen = RecvData(buffer,sizeof(DataHeader));
	DataHeader *header = (DataHeader *)buffer;
	if (rlen <= 0)
	{
		std::cout << "�����˶Ͽ����ӣ�" << std::endl;
		
		Close();
	}
	//std::cout << "ָ��ȣ�" << rlen << "|" << header->dataLength << " ָ������: " << header->type << std::endl;
	//��������
	rlen = RecvData(buffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
	//ִ��ָ��
	ExeCom((DataHeader *)buffer,rlen);

	return true;
}
bool TcpClient::ExeCom(DataHeader* header,int rlen)
{
	switch (header->type)
	{
	case LOGIN_RESULT:
	{
		//���յ�¼���
		LoginResult * res = (LoginResult *)header;
		if (res->result == 1)
			std::cout << "�ͻ��˵�¼�ɹ���" << std::endl;
		else
			std::cout << "�ͻ��˵�¼ʧ�ܣ�" << std::endl;
	}
	break;
	case LOGOUT_RESULT:
	{
		//���յ�¼���
		LogoutResult *res=(LogoutResult*)header;
		if (res->result == 1)
			Close();
		else
			std::cout << "�ͻ��˵�¼ʧ�ܣ�" << std::endl;
	}
	break;
	case NEW_USER_JOIN:
	{
		//���յ�¼���
		NewUserJoin* res = (NewUserJoin*)header;;
		if (res->type ==NEW_USER_JOIN)
			std::cout << "�¿ͻ���: " << res->sock << "�������Ⱥ�� ! " << std::endl;
		else
			std::cout << "�¿ͻ��˼���ʧ�ܣ�" << std::endl;
	}
	break;
	default:std::cout << "�޷����������" << std::endl;
	}
	return false;
}
void TcpClient::start()
{
	fd_set fdRead;
	fd_set fdWrite;
	fd_set fdExecept;

	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExecept);

	FD_SET(_sock, &fdRead);
	FD_SET(_sock, &fdWrite);
	FD_SET(_sock, &fdExecept);

	timeval t = { 0,0 };
	int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExecept, &t);
	if (ret < 0)
	{
		std::cout << "select ������" << std::endl;
		Close();
		return;
	}

	if (FD_ISSET(_sock, &fdRead))
	{
		FD_CLR(_sock, &fdRead);
		if (!RecvInfo())
		{
			std::cout << "select���������" << std::endl;
			return;
		}
	}
}
bool TcpClient::isCon()
{
	return INVALID_SOCKET !=_sock;
}
int TcpClient::SendData(const DataHeader *data)
{
	if (isCon() && data)
	{
		return send(_sock, (const char*)data, data->dataLength, 0);
	}
	return SOCKET_ERROR;
}
int TcpClient::RecvData(char* data, int len)
{
	return recv(_sock, data, len, 0);
}