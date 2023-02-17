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
	LoginResult() :result(1)
	{
		dataLength = sizeof(LoginResult);
		type = LOGIN_RESULT;
	}
	int result;
};
class Logout :public DataHeader
{
public:
	Logout() :userName("")
	{
		dataLength = sizeof(Logout);
		type = LOGOUT;
	}
	char userName[32];
};
class LogoutResult :public DataHeader
{
public:
	LogoutResult() :result(0)
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
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sock)
		std::cout << "�ͻ���socket����ʧ�ܣ�" << std::endl;
	else
		std::cout << "�ͻ���socket�����ɹ���" << std::endl;

	//���ӷ����connect
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "���ӷ�����ʧ�ܣ�" << std::endl;
	else
		std::cout << "���ӷ������ɹ���" << std::endl;

	
	char buffer[1024] = {};
	int rlen = 0;

	while (true)
	{
		//��������
		std::cout << "����";
		std::cin >> buffer;

		//��������
		if (0 == strcmp(buffer, "exit"))
		{
			std::cout << "�ͻ����˳���" << std::endl;
			break;
		}else if (0 == strcmp(buffer, "login")) {
			//����������Ϣ
			Login login;
			strcpy_s(login.userName, "zhang san");
			strcpy_s(login.passWord, "123456");
			//send(sock, (const char*)&login.type, sizeof(DataHeader), 0);
			send(sock, (const char*)&login, sizeof(Login), 0);
			//���յ�¼���
			LoginResult res;
			rlen=recv(sock, (char *)&res, sizeof(LoginResult), 0);
			if (res.result == 1)
				std::cout << "���ݳ��ȣ�" << rlen<<"|" << res.dataLength << " �ɹ�����\"1\": " << res.result << std::endl;
			else 
				std::cout << "�ͻ��˵�¼ʧ�ܣ�" << std::endl;
		}
		else if (0 == strcmp(buffer, "logout"))
		{
			//����������Ϣ
			
			Logout logout;
			strcpy_s(logout.userName, "li si");
			//send(sock, (const char*)&logout.type, sizeof(DataHeader), 0);
			send(sock, (const char*)&logout, sizeof(Logout), 0);
			//���յ�¼���
			LogoutResult res;
			rlen=recv(sock, (char*)&res, sizeof(LogoutResult), 0);
			if (res.result == 0)
				std::cout << "���ݳ��ȣ�" << rlen << "|" << res.dataLength <<  " �ɹ�����\"0\": " << res.result << std::endl;
			else
				std::cout << "�ͻ��˵�¼ʧ�ܣ�" << std::endl;
		}
		else
		{
			std::cout << "�����������" << std::endl;
		}
	}
	

	//�رշ����closesocket
	closesocket(sock);

	WSACleanup();
	getchar();
	return 0;
}