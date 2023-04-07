#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include<mutex>
class Semaphore
{
public:
	Semaphore();
	//阻塞当前线程
	void wait();
	//唤醒当前线程
	void wakeup();

private:
	//改变数据缓冲区时需要加锁
	std::mutex _mutex;
	//阻塞等待-条件变量
	std::condition_variable _cv;
	//等待计数
	int _wait;
	//唤醒计数
	int _wakeup;
};

#endif //__SEMAPHORE_H__