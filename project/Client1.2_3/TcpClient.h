#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 
#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项


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

	//通信
	void InitSocket();
	void Connect(const char* IP, unsigned short port);
	void Close();

	int SendData(const DataHeader* data);

	//业务逻辑
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