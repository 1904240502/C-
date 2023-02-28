#ifndef __CLIENT_H__
#define __CLIENT_H__

#define FD_SETSIZE 10001

#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 
#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#define BUF_SIZE 10240

class Client
{
public:
	Client(SOCKET sock= INVALID_SOCKET);
	virtual ~Client();
	SOCKET GetSocket();
	void SetSocket(const SOCKET& socket);
	int GetPos();
	void SetPos(const int& index);
	char * GetBufs();
private:
	SOCKET _sock;
	char *_BUFS;
	int _BUF_POS;
};

#endif //__CLIENT_H__