#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include"Client.h"
#include"Package.h"
#include"CELLTimestamp.hpp"
#include<vector>


class TcpServer
{
public :
	TcpServer();
	virtual ~TcpServer();

	void InitSocket();
	void Bind(const char* IP, unsigned short port);
	void Listen(int backlog);
	void Accept();
	void Close();

	void start();
	bool isCon();

private:
	void DelClient(Client* client);
	bool RecvInfo(Client* client);
	bool ExeCom(SOCKET cSock, DataHeader* header);
	int SendData(SOCKET cSock, const DataHeader* data);
	void SendDataToClients(DataHeader* data);

	SOCKET _sock;
	std::vector<Client*> _ClientSets;
	char _BUF[BUF_SIZE];
	CELLTimestamp _Time;
	int _RECV_COUNT;
};
#endif //__TCP_SERVER_H__
