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
	//���ݳ���
	int dataLen = header->dataLength;
	const char* data = (const char*)header;
	int dataPos = 0;
	while (true)
	{
		//������ó���
		int availdLen = BUF_SIZE - _SEND_BUF_POS;
		if (availdLen > dataLen)
		{
			//������ȫ����
			memcpy(_SEND_BUFS + _SEND_BUF_POS, data, dataLen);
			_SEND_BUF_POS += dataLen;
			break;
		}
		else
		{
			//��������ȫ����
			memcpy(_SEND_BUFS + _SEND_BUF_POS, data+dataPos, availdLen);
			//���ͻ�������
			int ret = send(_sock, _SEND_BUFS, BUF_SIZE, 0);
			//����ͳ�ƻ�����
			_SEND_BUF_POS = 0;
			//����ͳ�����ݳ���
			dataLen -= availdLen;
			//����ͳ�ƻ���λ��
			dataPos += availdLen;
			if (SOCKET_ERROR == ret)
			{
				return;
			}
		}
	}
}


