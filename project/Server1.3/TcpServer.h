#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include"EventServer.h"

class TcpServer:public Event
{
public :
	TcpServer();
	virtual ~TcpServer();

	//����socket
	void InitSocket();
	//�󶨶˿�
	void Bind(const char* IP, unsigned short port);
	//����
	void Listen(int backlog);
	//�ر�socket
	void Close();

	//����server����
	void start();
	//������Ϣ�������
	void createServer(int SERVER_NUM);
	
	//�ͻ��뿪�¼�
	virtual void onLeave(Client* client) = 0;
	//�ͻ������¼�
	virtual void onJoin(Client* client) = 0;
	//�ͻ���Ϣ�¼�
	virtual void onMsg(EventServer* pEventServer, Client* pClient,  DataHeader* header) = 0;
	//virtual void onMsg(Client* pClient, DataHeader* header) = 0;
protected:
	//��¼���տͻ�����
    std::atomic_int _RECV_COUNT;
	//��¼����ͻ���Ŀ
	std::atomic_int _CLIENT_NUM;
private:
	//���ӿͻ���
	void Accept();
	//�жϷ�����Ƿ�����
	bool isCon();
	//ѡ�����ͻ���Ϣ����ķ����
	void selectServer(Client * pClient);
	//ͳ���������
	void statistics();
private:
	SOCKET _sock;//������׽���
	vector<EventServer*> _ServerSet; //����
	CELLTimestamp _Time;
};
#endif //__TCP_SERVER_H__
