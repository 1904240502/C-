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

	int SendData(const DataHeader* header);//数据存入缓冲

	int RecvData();//数据存入缓冲
	bool hasData();
	DataHeader* GetCommand();
	int SendDateReal();//发送数据
private:
	SOCKET _sock;
	//发送缓冲
	MsgBuffer _RECV_BUFS;
	//接收缓冲
	MsgBuffer _SEND_BUFS;
	//心跳计时
	time_t _tHeart;
	//上次发送时间
	time_t _tSend;
	int _cId;
public:
	int _sId;
};

#endif //!__CLIENT_H__