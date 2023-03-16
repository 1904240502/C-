#ifndef __TASK_H__
#define __TASK_H__

class Task
{
public:
	Task() {}
	virtual~Task() {}
	virtual void doTask(){}
};

#endif // !__TASK_H__