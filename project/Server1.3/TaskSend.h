#ifndef __TASK_SEND_H__
#define __TASK_SEND_H__

#include "Task.h"
#include"Package.h"
#include"Client.h"
class TaskSend :
    public Task
{
public:
    TaskSend(Client* pClient, DataHeader* header);
    virtual void doTask();
private:
    Client* _pClient;
    DataHeader* _header;
};

#endif // !__TASK_SEND_H__