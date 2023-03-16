#include "TaskServer.h"

void TaskServer::addTask(Task* task)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_ServerTaskCache.push_back(task);
}

void TaskServer::Start()
{
	//线程
	std::thread t(std::mem_fn(&TaskServer::OnRun), this);
	t.detach();
}

void TaskServer::addToTasks()
{
	if (_ServerTaskCache.empty())
		return;
	std::lock_guard<std::mutex> lock(_mutex);
	for (auto val : _ServerTaskCache)
		_ServerTasks.push_back(val);
	_ServerTaskCache.clear();
}
bool TaskServer::TaskIsEmpty()
{
	if (_ServerTasks.empty())
	{
		//避免客户空处理
		std::chrono::milliseconds t(1);
		std::this_thread::sleep_for(t);
		return true;
	}
	return false;
}
void TaskServer::OnRun()
{
	while (true)
	{
		addToTasks();
		if (TaskIsEmpty())
			continue;
		for (auto val : _ServerTasks)
		{
			val->doTask();
			delete val;
		}
		_ServerTasks.clear();
	}
	
}
