#include "Semaphore.h"
#include<iostream>
Semaphore::Semaphore():_wait(0),_wakeup(0)
{
}
//P，V同步 锁 互斥                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
void Semaphore::wait()  
{
	std::unique_lock<std::mutex> lock(_mutex);
	if (--_wait < 0)//P操作
	{
		//阻塞等待
		_cv.wait(lock, [this]()->bool {
			return _wakeup > 0; //避免虚假唤醒
			});
		--_wakeup;
	}
}

void Semaphore::wakeup() 
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (++_wait <= 0)//V操作
	{
		++_wakeup;
		_cv.notify_one();
	}
}
