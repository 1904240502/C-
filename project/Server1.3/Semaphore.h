#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include<mutex>
class Semaphore
{
public:
	Semaphore();
	//������ǰ�߳�
	void wait();
	//���ѵ�ǰ�߳�
	void wakeup();

private:
	//�ı����ݻ�����ʱ��Ҫ����
	std::mutex _mutex;
	//�����ȴ�-��������
	std::condition_variable _cv;
	//�ȴ�����
	int _wait;
	//���Ѽ���
	int _wakeup;
};

#endif //__SEMAPHORE_H__