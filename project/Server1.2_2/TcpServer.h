#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 
#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<vector>

#include"Package.h"

class TcpServer
{
public :
	TcpServer();
	virtual ~TcpServer();

	void InitSocket();
	void Bind(const char* IP, unsigned short port);
	void Accetp(int backlog);
	void Close();

	void start();
	bool isCon();

	SOCKET _sock;
	std::vector<SOCKET> _ClientSets;
private:
	void DelClient(SOCKET sock);
	bool RecvInfo(SOCKET cSock);
	bool ExeCom(SOCKET cSock, DataHeader* header);
	int SendData(SOCKET cSock, const DataHeader* data);
	void SendDataToClients(DataHeader* data);
};
#endif __TCP_SERVER_H__
