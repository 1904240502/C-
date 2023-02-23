#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<vector>
#pragma comment(lib,"ws2_32.lib") //windows ����������

std::vector<SOCKET> ClientSets;

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
class NewUserJoin:public DataHeader
{
public:
	NewUserJoin() :sock(0)
	{
		dataLength = sizeof(NewUserJoin);
		type = NEW_USER_JOIN;
	}
	int sock;
};

//��Ϣ����
int  proc(SOCKET cSock)
{
	DataHeader cmd{};
	int rlen = 0;

	//���ܿͻ�������recv
	rlen = recv(cSock, (char*)&cmd, sizeof(DataHeader), 0);
	if (rlen <= 0)
	{
		std::cout << "�ͻ���: "<<cSock<<"���˳���" << std::endl;
		return -1;
	}
	//��Ϣ����
	switch (cmd.type)
	{
	case LOGIN:
	{
		Login data;
		rlen = recv(cSock, (char*)&data + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
		std::cout << "�ͻ���: " << cSock<<"����! " << "���ݳ��ȣ�" << rlen << "|" << data.dataLength << " �����¼���û�����" << data.userName << std::endl;
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
		rlen = recv(cSock, (char*)&data + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
		std::cout << "�ͻ���: " << cSock << "����!" << "���ݳ��ȣ�" << rlen << "|" << data.dataLength << " �����˳����û�����" << data.userName << std::endl;
		//��ͻ��˷�������send
		LogoutResult result;
		send(cSock, (const char*)&result, sizeof(LogoutResult), 0);
	}
	break;
	default:
	{
		DataHeader data{ 0,WEEOR };
		send(cSock, (const char*)&data, sizeof(DataHeader), 0);
	}
	}
	return 0;
}

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//����socket�׽���
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock)
		std::cout << "�����socket����ʧ�ܣ�" << std::endl;
	else
		std::cout << "�����socket=" << sock << "�����ɹ���" << std::endl;

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

	while (true)
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
		FD_SET(sock, &fdRead);
		FD_SET(sock, &fdWrite);
		FD_SET(sock, &fdExecept);

		//�����пͻ���socket��ӵ�������
		for (int i =0; i <ClientSets.size(); i++)
			FD_SET(ClientSets[i], &fdRead);

		//������select
		timeval t = { 0,0 };
		int ret = select(sock+1, &fdRead, &fdWrite, &fdExecept, &t);
		if (ret < 0)
		{
			std::cout << "select ������" << std::endl;
			break;
		}

		//�ж��Ƿ���ڼ�����
		if (FD_ISSET(sock, &fdRead))
		{
			//count����
			FD_CLR(sock, &fdRead);
			sockaddr_in clientAddr = {};
			int addrlen = sizeof(clientAddr);
			SOCKET cSock = INVALID_SOCKET;

			//���տͻ�������accept
			cSock = accept(sock, (sockaddr*)&clientAddr, &addrlen);
			if (INVALID_SOCKET == cSock)
				std::cout << "�¿ͻ�����������ʧ�ܣ�" << std::endl;
			else
			{
				ClientSets.push_back(cSock);
				std::cout << "�¿ͻ��� SOCKET ="<<cSock<<"���ӳɹ���"<< std::endl;

				//ת�����������ͻ������¿ͻ�����
				for (int i = 0; i < ClientSets.size() - 1; i++)
				{
					NewUserJoin NewUser{};
					NewUser.sock = cSock;
					send(ClientSets[i],(const char *) & NewUser, NewUser.dataLength, 0);
				}
			}	
		}

		//��������
		for (int i = 0; i <fdRead.fd_count; i++)
			if (-1==proc(fdRead.fd_array[i]))
			{
				//�ڿͻ��˼���ɾ���˳��Ŀͻ�
				auto iter = std::find(ClientSets.begin(), ClientSets.end(), fdRead.fd_array[i]);
				if (iter != ClientSets.end())
					ClientSets.erase(iter);
			}
		/*
		* std::cout << "��������ҵ��" << std::endl;
		*/
	}
	//�رշ����closesocket
	for (int i = ClientSets.size() - 1; i >= 0; i--)
		closesocket(ClientSets[i]);
	
	WSACleanup();
	return 0;
}