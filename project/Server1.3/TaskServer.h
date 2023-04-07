#ifndef __TASK_SERVER_H__
#define __TASK_SERVER_H__

#include "Task.h"
#include"ctlThread.h"

#include<iostream>
#include<list>

class TaskServer 
{
public:
    //服务端任务缓冲集
    std::list<Task*> _ServerTaskCache;
    //服务端任务集
    std::list<Task*>_ServerTasks;
    //锁
    std::mutex _Mutex;

private:
    ctlThread _Thread;
    int _id;
public:
    TaskServer(int id=0);
    ~TaskServer();

	//添加任务
	void addTask(Task* task);
	//启动工作线程
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
* 服务端：
* Server：服务端入口
* Client：客户信息，消息
* CELLTimeStamp:精准时间
* Package:消息包
* 
*客户=>生产者
* TcpServer：继承ServerEvent 接收客户
* myServer：继承TcpServer 处理事件
* 
* ServerEvent：接口
* 
* 客户=>消费者  任务=>生产者
* ConsumerServer：接收客户消息，处理任务
* 
* Task：接口
* 
* 任务=>消费者
* ServerTask：接收任务
* SendTask：继承Task 处理任务
*/
/*
* 客户端：
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