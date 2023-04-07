#ifndef __TASK_SERVER_H__
#define __TASK_SERVER_H__

#include "Task.h"
#include"ctlThread.h"

#include<iostream>
#include<list>

class TaskServer 
{
public:
    //��������񻺳弯
    std::list<Task*> _ServerTaskCache;
    //���������
    std::list<Task*>_ServerTasks;
    //��
    std::mutex _Mutex;

private:
    ctlThread _Thread;
    int _id;
public:
    TaskServer(int id=0);
    ~TaskServer();

	//�������
	void addTask(Task* task);
	//���������߳�
	void Start();
    void Close();
protected:
	//void OnRun();
    void OnRun(ctlThread* pThread);
private:
    void addToTasks();
    bool TaskIsEmpty();
};

#endif // !__TASK_SERVER_H__
/*
* ����ˣ�
* Server����������
* Client���ͻ���Ϣ����Ϣ
* CELLTimeStamp:��׼ʱ��
* Package:��Ϣ��
* 
*�ͻ�=>������
* TcpServer���̳�ServerEvent ���տͻ�
* myServer���̳�TcpServer �����¼�
* 
* ServerEvent���ӿ�
* 
* �ͻ�=>������  ����=>������
* ConsumerServer�����տͻ���Ϣ����������
* 
* Task���ӿ�
* 
* ����=>������
* ServerTask����������
* SendTask���̳�Task ��������
*/
/*
* �ͻ��ˣ�
* Client:use Package 
* 
* TaskSend:is a Task & hava a Client & hava a Package
* TaskServer:have a Task(TaskSend)
* 
* Event:hava a Client & have a Package & have a EventServer
* EventServer:have a Event(TcpServerForMe) & have a TaskServer & have a Client 
* TcpServer:is a Event & have a EventServer
* TcpServerForMe:is a TcpServer
* 
* Servre: use TcpServerForMe 
*/