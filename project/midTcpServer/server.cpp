#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //windows ����������


enum DataType
{
	LOGIN,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT,
	WEEOR
};

struct DataHeader
{
	short dataLength;
	DataType type;
};

class Login :public DataHeader
{
public:
	Login() :userName(""), passWord("")
	{
		dataLength = sizeof(Login);
		type = LOGIN;
	}
	char userName[32];
	char passWord[32];
};
class LoginResult :public DataHeader
{
public:
	LoginResult():result(1)
	{
		dataLength = sizeof(LoginResult);
		type = LOGIN_RESULT;
	}
	int result;
};
class Logout :public DataHeader
{
public:
	Logout():userName("")
	{
		dataLength = sizeof(Logout);
		type = LOGOUT;
	}
	char userName[32];
};
class LogoutResult :public DataHeader
{
public:
	LogoutResult():result(0)
	{
		dataLength = sizeof(LogoutResult);
		type = LOGOUT_RESULT;
	}
	int result;
};



int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//����socket�׽���
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock)
		std::cout << "�����socket�����ɹ���" << std::endl;
	else
		std::cout << "�����socket�����ɹ���" << std::endl;

	//�󶨿ͻ�������˿�
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "�󶨶˿�ʧ�ܣ�" << std::endl;
	else
		std::cout << "�󶨶˿ڳɹ���" << std::endl;

	//����listen
	if (SOCKET_ERROR == listen(sock, 5))
		std::cout << "����˼���ʧ�ܣ�" << std::endl;
	else
		std::cout << "����˼����ɹ���" << std::endl;

	sockaddr_in clientAddr = {};
	int addrlen = sizeof(clientAddr);
	SOCKET cSock = INVALID_SOCKET;



	//���ܿͻ�������accept
	cSock = accept(sock, (sockaddr*)&clientAddr, &addrlen);
	if (INVALID_SOCKET == cSock)
		std::cout << "���ܵ��ͻ���ʧ�ܣ�" << std::endl;
	else
		std::cout << "���ܵ��ͻ��˳ɹ���" << std::endl;

	DataHeader cmd{};
	int rlen = 0;
	
	while (true)
	{
		//���ܿͻ�������recv
		rlen = recv(cSock, (char*)&cmd, sizeof(DataHeader), 0);
		if (rlen <= 0)
		{
			std::cout << "�ͻ���--������Ϣʧ�ܣ�" << std::endl;
			break;
		}	

		//��Ϣ����
		switch (cmd.type)
		{
            case LOGIN:
		    {
				Login data;
				rlen=recv(cSock, (char*)&data+sizeof(DataHeader), sizeof(Login)- sizeof(DataHeader), 0);
				std::cout << "���ݳ��ȣ�"<<rlen << "|" << data.dataLength <<" �����¼���û�����" << data.userName << std::endl;
				/*
				* �û���Ϣ��֤
				*/
				//��ͻ��˷�������send
				LoginResult result;
				send(cSock, (const char*)&result, sizeof(LoginResult), 0);
		    }
		    break;
			case LOGOUT:
			{
				Logout data;
				rlen=recv(cSock, (char*)&data+ sizeof(DataHeader), sizeof(Logout)- sizeof(DataHeader), 0);
				std::cout << "���ݳ��ȣ�" << rlen<<"|"<<data.dataLength << " �����˳����û�����" << data.userName << std::endl;
				//��ͻ��˷�������send
				LogoutResult result;
				send(cSock, (const char*)&result, sizeof(LogoutResult), 0);
			}
			break;
			default:
			{
				DataHeader data{0,WEEOR};
				send(cSock, (const char*)&data, sizeof(DataHeader) , 0);
			}
		}
	}
	//�رշ����closesocket
	closesocket(sock);

	WSACleanup();
	return 0;
}