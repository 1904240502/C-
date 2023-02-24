#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 
#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#include"Package.h"


class TcpClient
{
public:

	TcpClient();
	virtual ~TcpClient();

	//通信
	void InitSocket();
	void Connect(const char* IP, unsigned short port);
	void Close();

	//业务逻辑
	bool SendPro();
	void start();
	bool isCon();
	
	SOCKET _sock;
private:
	bool RecvInfo();
	bool ExeCom(DataHeader* header, int rlen);
	int SendData(const DataHeader *data);
	int RecvData(char * data,int len);
	
};

#endif __TCP_CLIENT_H__