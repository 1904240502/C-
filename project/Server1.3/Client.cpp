#include "Client.h"

Client::Client(SOCKET sock):_sock(sock)
{
	_RECV_BUF_POS = 0;
	_SEND_BUF_POS = 0;
	_RECV_BUFS = new char[BUF_SIZE];
	_SEND_BUFS = new char[BUF_SIZE];
}

Client::~Client()
{
	delete _RECV_BUFS;
	delete _SEND_BUFS;
	_RECV_BUF_POS = 0;
	_SEND_BUF_POS = 0;
}

SOCKET Client::GetSocket()
{
	return _sock;
}

void Client::SetSocket(const SOCKET& socket)
{
	_sock = socket;
}

int Client::GetRecvPos()
{
	return _RECV_BUF_POS;
}

void Client::SetRecvPos(const int& index)
{
	_RECV_BUF_POS = index;
}

char* Client::GetRecvBufs()
{
	return _RECV_BUFS;
}

int Client::GetSendPos()
{
	return _SEND_BUF_POS;
}

void Client::SetSendPos(const int& index)
{
	_SEND_BUF_POS = index;
}
void Client::SendData(const DataHeader* header)
{
	//数据长度
	int dataLen = header->dataLength;
	const char* data = (const char*)header;
	int dataPos = 0;
	while (true)
	{
		//缓冲可用长度
		int availdLen = BUF_SIZE - _SEND_BUF_POS;
		if (availdLen > dataLen)
		{
			//可以完全存入
			memcpy(_SEND_BUFS + _SEND_BUF_POS, data, dataLen);
			_SEND_BUF_POS += dataLen;
			break;
		}
		else
		{
			//不可以完全存入
			memcpy(_SEND_BUFS + _SEND_BUF_POS, data+dataPos, availdLen);
			//发送缓冲数据
			int ret = send(_sock, _SEND_BUFS, BUF_SIZE, 0);
			//重新统计缓冲标记
			_SEND_BUF_POS = 0;
			//重新统计数据长度
			dataLen -= availdLen;
			//重新统计缓冲位置
			dataPos += availdLen;
			if (SOCKET_ERROR == ret)
			{
				return;
			}
		}
	}
}


