#ifndef __MESSAGE_BUFFER_H__
#define __MESSAGE_BUFFER_H__

#include<memory>
#include<iostream>
#include"Config.h"
class MsgBuffer
{
public:
	MsgBuffer(int nSize = 8192);
	~MsgBuffer();

	char* data();

	bool push(const char* pData, int nLen);
	
	void pop(int nLen);

	int write2socket(SOCKET sockfd);

	int read4socket(SOCKET sockfd);

	bool hasMsg();

	bool needWrite();
private:
	//�ڶ������� ���ͻ�����
	char* _pBuff = nullptr;
	//�������������������������ݿ�
	//list<char*> _pBuffList;
	//������������β��λ�ã��������ݳ���
	int _nLast = 0;
	//�������ܵĿռ��С���ֽڳ���
	int _nSize = 0;
	//������д����������
	int _fullCount = 0;
};

#endif //__MESSAGE_BUFFER_H__