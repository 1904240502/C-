#ifndef __CLIENT_H__
#define __CLIENT_H__

#include<iostream>
#include"MsgBuffer.h"

class Client
{
public:
	Client(SOCKET sock= INVALID_SOCKET);
	virtual ~Client();

	SOCKET GetSocket();
    void SetSocket(const SOCKET& socket);

	void resetHeart();
	bool checkHeart(const time_t& t);
	void resetSendTime();
	bool checkSend(const time_t& t);

	int SendData(const DataHeader* header);//���ݴ��뻺��

	int RecvData();//���ݴ��뻺��
	bool hasData();
	DataHeader* GetCommand();
	int SendDateReal();//��������
private:
	SOCKET _sock;
	//���ͻ���
	MsgBuffer _RECV_BUFS;
	//���ջ���
	MsgBuffer _SEND_BUFS;
	//������ʱ
	time_t _tHeart;
	//�ϴη���ʱ��
	time_t _tSend;
	int _cId;
public:
	int _sId;
};

#endif //!__CLIENT_H__