#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 
#pragma comment(lib,"ws2_32.lib") //windows ����������


#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#include"Package.h"

#define BUF_SIZE 10240

class TcpClient
{
public:

	TcpClient(SOCKET sock = INVALID_SOCKET);
	 virtual ~TcpClient();

	//ͨ��
	void InitSocket();
	void Connect(const char* IP, unsigned short port);
	void Close();

	int SendData(const DataHeader* data);

	//ҵ���߼�
	bool SendPro();
	void start();
	bool isCon();
	
private:
	bool RecvInfo();
	bool ExeCom(DataHeader* header);
	int RecvData(char * data,int len);

	
	SOCKET _sock;
	char* _BUFS;
	char* _BUF;
	int _BUF_POS;
};

#endif //__TCP_CLIENT_H__