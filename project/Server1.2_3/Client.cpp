#include "Client.h"

Client::Client(SOCKET sock):_sock(sock)
{
	_BUF_POS = 0;
	_BUFS = new char[BUF_SIZE * 10];
}

Client::~Client()
{
	_BUF_POS = 0;
}

SOCKET Client::GetSocket()
{
	return _sock;
}

void Client::SetSocket(const SOCKET& socket)
{
	_sock = socket;
}

int Client::GetPos()
{
	return _BUF_POS;
}

void Client::SetPos(const int& index)
{
	_BUF_POS = index;
}

char* Client::GetBufs()
{
	return _BUFS;
}
