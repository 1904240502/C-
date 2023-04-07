#include "MsgBuffer.h"

MsgBuffer::MsgBuffer(int nSize)
{
	_nSize = nSize;
	_pBuff = new char[_nSize];
}

MsgBuffer::~MsgBuffer()
{
	if (_pBuff)
	{
		delete[] _pBuff;
		_pBuff = nullptr;
	}
}

char* MsgBuffer::data()
{
	return _pBuff;
}

bool MsgBuffer::push(const char* pData, int nLen)
{
	if (_nLast + nLen <= _nSize)
	{
		//将要发送的数据 拷贝到发送缓冲区尾部
		memcpy(_pBuff + _nLast, pData, nLen);
		//计算数据尾部位置
		_nLast += nLen;

		if (_nLast == SEND_BUF_SIZE)
		{
			++_fullCount;
		}
		return true;
	}
	else {
		++_fullCount;
		std::cout << "发送缓冲溢出:" << _fullCount << std::endl;
	}

	return false;
}

void MsgBuffer::pop(int nLen)
{
	int n = _nLast - nLen;
	if (n > 0)
	{
		memcpy(_pBuff, _pBuff + nLen, n);
	}
	_nLast = n;
	if (_fullCount > 0)
		--_fullCount;
}

int MsgBuffer::write2socket(SOCKET sockfd)
{
	int ret = 0;
	//缓冲区有数据
	if (_nLast > 0 && INVALID_SOCKET != sockfd)
	{
		//发送数据
		ret = send(sockfd, _pBuff, _nLast, 0);
		//数据尾部位置清零
		_nLast = 0;
		//
		_fullCount = 0;
	}
	return ret;
}

int MsgBuffer::read4socket(SOCKET sockfd)
{
	if (_nSize - _nLast > 0)
	{
		//接收客户端数据
		char* szRecv = _pBuff + _nLast;
		int nLen = (int)recv(sockfd, szRecv, _nSize - _nLast, 0);
		if (nLen <= 0)
		{
			return nLen;
		}
		//消息缓冲区的数据尾部位置后移
		_nLast += nLen;
		return nLen;
	}
	return 0;
}

bool MsgBuffer::hasMsg()
{
	////判断消息缓冲区的数据长度大于消息头DataHeader长度
		//if (_nLast >= sizeof(DataHeader))
		//{
		//	//这时就可以知道当前消息的长度
		//	DataHeader* header = (DataHeader*)_pBuff;
		//	//判断消息缓冲区的数据长度大于消息长度
		//	return _nLast >= header->dataLength;
		//}
		//return false;
	return _nLast >= sizeof(DataHeader) ? true : false;
}

bool MsgBuffer::needWrite()
{
	return _nLast > 0;
}
