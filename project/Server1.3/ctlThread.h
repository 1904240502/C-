#ifndef __CONTROL_THREAD_H__
#define __CONTROL_THREAD_H__

#include"Semaphore.h"
#include<functional>

class ctlThread
{
private:
	typedef std::function<void(ctlThread*)> EventCall;
public:
	//�����߳�
	void Start(
		EventCall onCreate = nullptr,
		EventCall onRun = nullptr,
		EventCall onDestory = nullptr);

	//�ر��߳�
	void Close();
	//�ڹ����������˳�
	//����Ҫʹ���ź����������ȴ�
	//���ʹ�û�����
	void Exit();

	//�߳��Ƿ���������״̬
	bool isRun();
protected:
	//�̵߳�����ʱ�Ĺ�������
	void OnWork();
private:
	EventCall _onCreate;
	EventCall _onRun;
	EventCall _onDestory;
	//��ͬ�߳��иı�����ʱ��Ҫ����
	std::mutex _mutex;
	//�����̵߳���ֹ���˳�
	Semaphore _sem;
	//�߳��Ƿ�����������
	bool	_isRun = false;
};

#endif //__CONTROL_THREAD_H__