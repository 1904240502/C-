#ifndef __CLIENT_H__
#define __CLIENT_H__

#define FD_SETSIZE 3000

#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 
#pragma comment(lib,"ws2_32.lib") //windows ����������

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include"Package.h"
#define BUF_SIZE 1024

class Client
{
public:
	Client(SOCKET sock= INVALID_SOCKET);
	virtual ~Client();
	SOCKET GetSocket();
	void SetSocket(const SOCKET& socket);
	int GetRecvPos();
	void SetRecvPos(const int& index);
	char * GetRecvBufs();
	void SendData(const DataHeader* header);
	int GetSendPos();
	void SetSendPos(const int& index);
private:
	SOCKET _sock;
	//���ͻ���
	char *_RECV_BUFS;
	int _RECV_BUF_POS;
	//���ջ���
	char *_SEND_BUFS;
	int _SEND_BUF_POS;
};

#endif //!__CLIENT_H__