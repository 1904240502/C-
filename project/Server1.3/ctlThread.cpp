#include "ctlThread.h"

void ctlThread::Start(EventCall onCreate, EventCall onRun, EventCall onDestory)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (!_isRun)
	{
		_isRun = true;

		if (onCreate)
			_onCreate = onCreate;
		if (onRun)
			_onRun = onRun;
		if (onDestory)
			_onDestory = onDestory;

		//Ïß³Ì
		std::thread t(std::mem_fn(&ctlThread::OnWork), this);
		t.detach();
	}
}

void ctlThread::Close()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_isRun)
	{
		_isRun = false;
		_sem.wait();
	}
}

void ctlThread::Exit()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_isRun)
	{
		_isRun = false;
	}
}

bool ctlThread::isRun()
{
	return _isRun;
}

void ctlThread::OnWork()
{
	if (_onCreate)
		_onCreate(this);
	if (_onRun)
		_onRun(this);
	if (_onDestory)
		_onDestory(this);

	_sem.wakeup();
}
