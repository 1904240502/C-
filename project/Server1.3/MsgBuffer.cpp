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
		//��Ҫ���͵����� ���������ͻ�����β��
		memcpy(_pBuff + _nLast, pData, nLen);
		//��������β��λ��
		_nLast += nLen;

		if (_nLast == SEND_BUF_SIZE)
		{
			++_fullCount;
		}
		return true;
	}
	else {
		++_fullCount;
		std::cout << "���ͻ������:" << _fullCount << std::endl;
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
	//������������
	if (_nLast > 0 && INVALID_SOCKET != sockfd)
	{
		//��������
		ret = send(sockfd, _pBuff, _nLast, 0);
		//����β��λ������
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
		//���տͻ�������
		char* szRecv = _pBuff + _nLast;
		int nLen = (int)recv(sockfd, szRecv, _nSize - _nLast, 0);
		if (nLen <= 0)
		{
			return nLen;
		}
		//��Ϣ������������β��λ�ú���
		_nLast += nLen;
		return nLen;
	}
	return 0;
}

bool MsgBuffer::hasMsg()
{
	////�ж���Ϣ�����������ݳ��ȴ�����ϢͷDataHeader����
		//if (_nLast >= sizeof(DataHeader))
		//{
		//	//��ʱ�Ϳ���֪����ǰ��Ϣ�ĳ���
		//	DataHeader* header = (DataHeader*)_pBuff;
		//	//�ж���Ϣ�����������ݳ��ȴ�����Ϣ����
		//	return _nLast >= header->dataLength;
		//}
		//return false;
	return _nLast >= sizeof(DataHeader) ? true : false;
}

bool MsgBuffer::needWrite()
{
	return _nLast > 0;
}
