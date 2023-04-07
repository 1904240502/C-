#include "TaskServer.h"

TaskServer::TaskServer(int id):_id(id)
{
}

TaskServer::~TaskServer()
{
	Close();
}

void TaskServer::addTask(Task* task)
{
	std::lock_guard<std::mutex> lock(_Mutex);
	_ServerTaskCache.push_back(task);
}

void TaskServer::Start()
{
	std::cout << "Task Server Start!  >>" << _id << std::endl;
	//�߳�
	_Thread.Start(
		nullptr, 
		[this](ctlThread* pThread) {
			OnRun(pThread);
		},
		nullptr
	);
	std::cout << "Task Server Start!  <<" << _id << std::endl;
}

void TaskServer::Close()
{
	std::cout << "Task Server Close!  >>" << _id << std::endl;
	_Thread.Close();
	std::cout << "Task Server Close!  <<" << _id << std::endl;
}

void TaskServer::addToTasks()
{
	if (_ServerTaskCache.empty())
		return;
	std::lock_guard<std::mutex> lock(_Mutex);
	for (auto val : _ServerTaskCache)
		_ServerTasks.push_back(val);
	_ServerTaskCache.clear();
}
bool TaskServer::TaskIsEmpty()
{
	if (_ServerTasks.empty())
	{
		//����ͻ��մ���
		std::chrono::milliseconds t(1);
		std::this_thread::sleep_for(t);
		return true;
	}
	return false;
}
//void TaskServer::OnRun()
//{
//	while (_isRun)
//	{
//		addToTasks();
//		if (TaskIsEmpty())
//			continue;
//		/*for (auto &val : _ServerTasks)
//		{
//			val->doTask();
//			delete val;
//		}*/
//		while (!_ServerTasks.empty())
//		{
//			auto iter = _ServerTasks.begin();
//			//std::cout << "������һ������" << std::endl;
//			(*iter)->doTask();
//			delete *iter;
//			_ServerTasks.erase(iter);
//		}
//		_ServerTasks.clear();
//	}
//	
//}

//��������
void TaskServer::OnRun(ctlThread* pThread)
{
	while (pThread->isRun())
	{
		addToTasks();
		if (TaskIsEmpty())
			continue;
		while (!_ServerTasks.empty())
		{
			auto iter = _ServerTasks.begin();
			//std::cout << "������һ������" << std::endl;
			(*iter)->doTask();
			delete* iter;
			_ServerTasks.erase(iter);
		}
		_ServerTasks.clear();
	}
	//����������
	while (!_ServerTaskCache.empty())
	{
		auto iter = _ServerTaskCache.begin();
		//std::cout << "������һ������" << std::endl;
		(*iter)->doTask();
		delete* iter;
		_ServerTaskCache.erase(iter);
	}
	_ServerTaskCache.clear();
}