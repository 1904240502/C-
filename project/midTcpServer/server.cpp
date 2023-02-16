#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //windows ����������

struct DataPackage
{
	int age;
	char name[32];
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

	char reBuf[1024] = {};
	int rlen = 0;
	

	while (true)
	{
		//���ܿͻ�������recv
		rlen = recv(cSock, reBuf, 1024, 0);
		if (rlen <= 0)
		{
			std::cout << "�ͻ���--������Ϣʧ�ܣ�" << std::endl;
			break;
		}	

		//��Ϣ����
		if (0 == strcmp(reBuf, "getinfo"))
		{
			//��ͻ��˷�������send
			DataPackage data = {12,"zhang san"};
			rlen = send(cSock, (const char*)&data, sizeof(data), 0);
			if (0 >=rlen )
				std::cout << "��ͻ��˷�����Ϣʧ�ܣ�" << std::endl;
			else
				std::cout << "��ͻ��˷�����Ϣ�ɹ���" << std::endl;
		}else {
			//��ͻ��˷�������send
			char sBuf[] = "???";
			rlen = send(cSock, sBuf, strlen(sBuf) + 1, 0);
			if (0 >= rlen)
				std::cout << "��ͻ��˷�����Ϣʧ�ܣ�" << std::endl;
			else
				std::cout << "�û����������ͻ��˷�����Ϣ�ɹ���" << std::endl;	
		}
	}

	//�رշ����closesocket
	closesocket(sock);

	WSACleanup();
	return 0;
}