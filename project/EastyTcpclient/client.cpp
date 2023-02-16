#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //windows ����������

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
	if (SOCKET_ERROR == connect(sock, (sockaddr *) & sin,sizeof(sockaddr_in)))
		std::cout << "���ӷ�����ʧ�ܣ�" << std::endl;
	else
		std::cout << "���ӷ������ɹ���" << std::endl;

	
	char buffer[1024] = {};
	int rlen = 0;

	
	//������Ϣ recv
	rlen = recv(sock, buffer, 1024, 0);
	if (rlen<=0)
		std::cout << "�ͻ���--������Ϣʧ�ܣ�" << std::endl;
	else {
		std::cout << "���ܵ��������Ϣ��" << std::endl;
		std::cout << "����˷���:"<<buffer << std::endl;
	}

	//�رշ����closesocket
	closesocket(sock);
	
	WSACleanup();
	getchar();
	return 0;
}