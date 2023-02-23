#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<thread>
#pragma comment(lib,"ws2_32.lib") //windows ����������

enum DataType
{
	LOGIN,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT,
	NEW_USER_JOIN,
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
class NewUserJoin :public DataHeader
{
public:
	NewUserJoin() :sock(0)
	{
		dataLength = sizeof(NewUserJoin);
		type = NEW_USER_JOIN;
	}
	int sock;
};

int proc(SOCKET sock)
{
	DataHeader cmd{};
	int rlen = 0;

	//���տͻ�������recv
	rlen = recv(sock, (char*)&cmd, sizeof(DataHeader), 0);
	if (rlen <= 0)
	{
		std::cout << "�����˶Ͽ����ӣ�" << std::endl;
		return -1;
	}
	//��Ϣ����
	switch (cmd.type)
	{
	case LOGIN_RESULT:
	{
		//���յ�¼���
		LoginResult res;
		rlen = recv(sock, (char*)&res+sizeof(DataHeader), sizeof(LoginResult)-sizeof(DataHeader), 0);
		if (res.result == 1)
			std::cout << "���ݳ��ȣ�" << rlen << "|" << res.dataLength << " �ɹ�����\"1\": " << res.result << std::endl;
		else
			std::cout << "�ͻ��˵�¼ʧ�ܣ�" << std::endl;
	}
	break;
	case LOGOUT_RESULT:
	{
		//���յ�¼���
		LogoutResult res;
		rlen = recv(sock, (char*)&res + sizeof(DataHeader), sizeof(LogoutResult) - sizeof(DataHeader), 0);
		if (res.result == 0)
			std::cout << "���ݳ��ȣ�" << rlen << "|" << res.dataLength << " �ɹ�����\"0\": " << res.result << std::endl;
		else
			std::cout << "�ͻ��˵�¼ʧ�ܣ�" << std::endl;
	}
	break;
	case NEW_USER_JOIN:
	{
		//���յ�¼���
		NewUserJoin res;
		rlen = recv(sock, (char*)&res + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
		if (rlen != 0)
			std::cout <<  "�¿ͻ���: " << res.sock << "�������Ⱥ�� ! " << std::endl;
		else
			std::cout << "�¿ͻ��˼���ʧ�ܣ�" << std::endl;
	}
	break;
	default:std::cout << "�޷����������" << std::endl;
	}
	return 0;
}

//�ͻ����������룬����
bool isRun = true;
void cmdThread(SOCKET sock)
{
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
			isRun = false;
			std::cout << "�߳��˳���" << std::endl;
			break;
		}
		else if (0 == strcmp(buffer, "login")) {
			//����������Ϣ
			Login login;
			strcpy_s(login.userName, "zhang san");
			strcpy_s(login.passWord, "123456");
			send(sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp(buffer, "logout"))
		{
			//����������Ϣ
			Logout logout;
			strcpy_s(logout.userName, "li si");
			send(sock, (const char*)&logout, sizeof(Logout), 0);
		}
		else
		{
			std::cout << "�����������" << std::endl;
		}
	}

}
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
		std::cout << "�ͻ���socket="<<sock<<"�����ɹ���" << std::endl;

	//���ӷ����connect
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr= inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "���ӷ�����ʧ�ܣ�" << std::endl;
	else
		std::cout << "���ӷ������ɹ���" << std::endl;

	std::thread t(cmdThread,sock);
	t.detach();

	while (isRun)
	{
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExecept;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExecept);

		FD_SET(sock, &fdRead);
		FD_SET(sock, &fdWrite);
		FD_SET(sock, &fdExecept);

		timeval t = { 1,0 };
		int ret = select(sock+1, &fdRead, &fdWrite, &fdExecept, &t);
		if (ret < 0)
		{
			std::cout << "select ������" << std::endl;
			break;
		}

		if (FD_ISSET(sock, &fdRead))
		{
			FD_CLR(sock, &fdRead);
			if (proc(sock))
			{
				std::cout << "���̴��������" << std::endl;
				break;
			}
		}

		//std::cout << "��������ҵ��" << std::endl;
	}


	//�رշ����closesocket
	closesocket(sock);
	WSACleanup();
	std::cout << "�ͻ����˳���" << std::endl;
	return 0;
}