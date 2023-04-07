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


	//������Ϣ�������
	void Start(int SERVER_NUM);
	//ִ������
	void OnRun(ctlThread *pThread);

	//�ͻ��뿪�¼�
	virtual void onLeave(Client* pClient);
	//�ͻ������¼�
	virtual void onJoin(Client* pClient);
	//�ͻ���Ϣ�¼�
	virtual void onMsg(EventServer* pEventServer, Client* pClient,  DataHeader* header);
	//virtual void onMsg(Client* pClient, DataHeader* header) = 0;
	//recv�¼�
	virtual void onRecv(Client* pClient);
protected:
	//��¼���տͻ�����
    std::atomic_int _MSG_COUNT;
	//��¼����ͻ���Ŀ
	std::atomic_int _CLIENT_NUM;
private:
	//���ӿͻ���
	void Accept();
	//�ر�socket
	void Close();
	//ѡ�����ͻ���Ϣ����ķ����
	void selectServer(Client * pClient);
	//ͳ���������
	void statistics();
private:
	SOCKET _sock;//������׽���
	std::vector<EventServer*> _ServerSet; //����
	CELLTimestamp _Time;
	ctlThread _Thread;
};
#endif //__TCP_SERVER_H__
