#include "Client.h"


Client::Client(SOCKET sock):_sock(sock), _RECV_BUFS(RECV_BUF_SIZE),_SEND_BUFS(SEND_BUF_SIZE)
{
	static int n = 1;
	_cId = n++;
	_sId = -1;
	_tHeart = 0;
	_tSend = 0;
}
Client::~Client()
{
	std::cout << "属于："<<_sId<<"线程的："<<_cId<<"号客户退出！" << std::endl;
	if (SOCKET_ERROR != _sock)
	{
		closesocket(_sock);
		_sock = INVALID_SOCKET;
	}	
}

SOCKET Client::GetSocket()
{
	return _sock;
}
void Client::SetSocket(const SOCKET& socket)
{
	_sock = socket;
}

void Client::resetHeart()
{
	_tHeart = 0;
}
bool Client::checkHeart(const time_t& t)
{
	//std::cout << "客户未检测时常: " <<_tHeart<< std::endl;
	return (_tHeart += t) >= SURVIVAL_TIME ? true : false;
}
void Client::resetSendTime()
{
	_tSend = 0;
}
bool Client::checkSend(const time_t& t)
{
	return (_tSend += t) >= SEND_TIME ? true : false;
}

int Client::SendData(const DataHeader* header)
{
	if (_SEND_BUFS.push(reinterpret_cast<const char*> (header), header->dataLength))
		return header->dataLength;
	return -1;
}
int Client::RecvData()
{
	return _RECV_BUFS.read4socket(_sock);
}
bool Client::hasData()
{
	return _RECV_BUFS.hasMsg();
}
DataHeader* Client::GetCommand()
{
	DataHeader* command;
	if (_RECV_BUFS.hasMsg())
	{
		command = reinterpret_cast<DataHeader*>(_RECV_BUFS.data());
		int len = command->dataLength;
		_RECV_BUFS.pop(len);

	}
	else
	{
		command = new DataHeader;
		command->type = WEEOR;
	}
	return command;
}
int Client::SendDateReal()
{
	//std::cout << "定时数据发送数据！" << std::endl;
	resetSendTime();
	return _SEND_BUFS.write2socket(_sock);
}


