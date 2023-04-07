#include "TcpClient.h"

TcpClient::TcpClient(SOCKET sock ):_sock(sock)
{
	_isConnect = false;
	_BUF_POS = 0;
	_BUF = new char[BUF_SIZE];
	_BUFS = new char[BUF_SIZE * 10];
}
TcpClient::~TcpClient()
{
	delete[] _BUF;
	delete[] _BUFS;
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
	 if (_sock==INVALID_SOCKET)
		 std::cout << "�ͻ���socket����ʧ�ܣ�" << std::endl;
	 else
		 ;
		//std::cout << "�ͻ��˴����ɹ���" << std::endl;
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
		std::cout << "socket=" << _sock << "��������http:\\\\" << IP << ":" << port << "ʧ�ܣ�" << std::endl;
	else
		_isConnect=true;// std::cout << "socket=" << _sock << "�ɹ�������http:\\\\" << IP << ":" << port << "�ɹ���" << std::endl;
}
void TcpClient::Close()
{
	if (isCon())
	{
		//std::cout << "�ͻ���" << _sock << "�˳���" << std::endl;
		closesocket(_sock);
		WSACleanup();
		_sock = INVALID_SOCKET;
		_isConnect = false;
	}
}
bool TcpClient::SendPro(){
	int rlen = 0;

	while (true)
	{
		//��������
		std::cin >> _BUF;

		//��������
		if (0 == strcmp(_BUF, "exit"))
			Close();
		else if (0 == strcmp(_BUF, "login")) {
			//����������Ϣ
			Login login;
			strcpy_s(login.userName, "zhang san");
			strcpy_s(login.passWord, "123456");
			SendData(&login);
		}
		else if (0 == strcmp(_BUF, "logout"))
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
	int len = sizeof(DataHeader);

	//�ӻ�����copy���ݵ���ת����
	int rlen = RecvData(_BUF, BUF_SIZE);
	//ת���ͻ����Խ�����
	memcpy(_BUFS + _BUF_POS, _BUF, rlen);
	_BUF_POS += rlen;

	//��������
	while (_BUF_POS>=len)
	{
		//������ݳ���
		DataHeader* header = (DataHeader*)_BUFS;
		//�������Ѿ��������İ�
		if (_BUF_POS >= header->dataLength)
		{
			int nlen = header->dataLength;
			//������Ϣ
			ExeCom(header);
			//�ӻ�����ȡ����
			memcpy(_BUFS, _BUFS + nlen, _BUF_POS- nlen);
			_BUF_POS -= nlen;
		}
		else
		{
			break;
		}
	}
	return true;
}
bool TcpClient::ExeCom(DataHeader* header)
{
	switch (header->type)
	{
	case LOGIN_RESULT:
	{
		//���յ�¼���
		LoginResult * res = (LoginResult *)header;
		if (res->result == 1)
			;// std::cout << "�ͻ��˵�¼�ɹ���" << std::endl;
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
	case HEART2C:
	{
		//���յ�¼���
		std::cout << "����˴��ڣ�" << std::endl;
	}
	break;
	default: std::cout << "�޷����������" << std::endl;
	}
	return false;
}
void TcpClient::start()
{
	fd_set fdRead{};
	FD_ZERO(&fdRead);
	FD_SET(_sock, &fdRead);


	timeval t = { 0,0 };
	int ret = select(_sock + 1, &fdRead, 0,0, &t);
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
	return INVALID_SOCKET !=_sock&&_isConnect;
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
	int rlen= recv(_sock, data, len, 0);
	//std::cout << rlen << std::endl;
	if (rlen <= 0)
	{
		std::cout << "�����˶Ͽ����ӣ�" << std::endl;
		Close();
	}
	return rlen;
}