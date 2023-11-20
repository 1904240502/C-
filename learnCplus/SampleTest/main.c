//#include<iostream>
//using namespace std;
//
////#include"Template.h"
////#include"initList.h"
//#include"ObjectOfClass.h"
//#include<filesystem>
//
//#include<future>
//
////Thread1
//int AddThread(int a, int b)
//{
//	std::cout << "Add Thread" << std::endl;
//	return a + b;
//}
//
////Thread2
//int MulThread(int a, int b)
//{
//	std::cout << "Mul Thread" << std::endl;
//	return a * b;
//}
//int main()
//{
//
//	/*cout << "==========Template ======begin" << endl;
//	basicknowledge::Template::function::func();
//	basicknowledge::Template::Class::func();
//	cout << "==========Template ======end" << endl;*/
//
//	//std::string path = std::filesystem::current_path().string();
//	//std::cout << path << std::endl;
//	
//    std::cout << "Main thread to display the data" << std::endl;
//
//    //Using async to run the AddThread
//    std::future<int>result1 = std::async(AddThread, 3, 3);
//
//    //wait is blocking call, it will ONLY wait for function to be finished.
//    //if deferref policy then wait will also trigger the function call.
//    result1.wait();
//
//    //we can call wait more than one time, second call of get() will
//    //give run time error.
//    result1.wait();
//
//    //get is blocking call + get the result or exception
//    std::cout << "Add result:" << result1.get() << std::endl;
//
//    //We can also use auto instead of future.
//    //call asyn with async policy
//    auto result2 = std::async(std::launch::async, MulThread, 3, 3);
//
//    //wait_for zero, mean continously polling
//    if (result2.wait_for(std::chrono::seconds(0)) != std::future_status::deferred)
//    {
//        while (result2.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
//        {
//            std::cout << "Waiting" << std::endl;
//        }
//        std::cout << "Ready" << std::endl;
//    }
//
//    std::cout << "Mul Result:" << result2.get() << std::endl;
//
//	//fun();
//	return 0;
//}


//#include <iostream>
//#include <thread>
//
//using namespace std;
//
//class A
//{
//public:
//    mutable int m_i;
//
//    // 类型转换构造函数，可以把一个“int”转换为一个“类对象”
//    A(int a) : m_i(a) { cout << "[A::A (int a) 构造函数执行]" << ' ' << this << ' ' << "thread_id = " << std::this_thread::get_id() << endl; }
//    A(const A& a) : m_i(a.m_i) { cout << "[A::A(const A)拷贝构造函数执行]" << ' ' << this << ' ' << "thread_id = " << std::this_thread::get_id() << endl; }
//    ~A() { cout << "[A::~A()析构函数执行]" << ' ' << this << ' ' << "thread_id = " << std::this_thread::get_id() << endl; }
//
//    void thread_work(int num) { cout << "[子线程thread_work执行]" << ' ' << this << ' ' << "thread_id = " << std::this_thread::get_id() << endl; }
//    void operator() (int num) { cout << "[子线程()执行]" << ' ' << this << ' ' << "thread_id = " << std::this_thread::get_id() << endl; }
//};
//
//void go(unique_ptr<int> pzn)
//{
//    cout << "子线程go的参数地址是" << ' ' << "thread_id = " << std::this_thread::get_id() << endl;
//}
//
//void myprint(const int i, const A& pmybuf)
//{
//    // 打印一下pmybuff对象的地址
//    cout << &pmybuf << endl;
//    return;
//}
//
//int main()
//{
//    //A myobj(10);
//    //std::thread mytobj(&A::thread_work, std::ref(myobj), 15);
//    //mytobj.join();
//    ////mytobj.detach();
//
//    A myobj(10);
//    std::thread o(myobj, 12);
//    o.join();
//
//    //int mvar = 1;
//    //int mysecondpar = 12;
//    //thread mytobj2(myprint, mvar, A(mysecondpar));
//    //mytobj2.detach();
//    //mytobj2.join();
//
//                                                      
//
//    return 0;
//}


//#include <iostream>
//#include <vector>
//#include <thread>
//#include<mutex>
//#include <list>
//
//using namespace std;
//
//class A
//{
//public:
//
//    // 把收到的消息（玩家命令）放入到一个队列的线程
//    void inMsgRecvQueue()
//    {
//        for (int i = 0; i < 100; i++)
//        {
//            std::lock_guard<mutex> lock(m_mutex);
//            cout << "inMsgRecvQueue()执行，插入一个元素" << i << endl;
//            msgRecvQueue.push_back(i); // 假设这个数字i就是收到的命令
//            // 直接把它弄到消息队列中 
//        }
//    }
//
//    // 把数据从消息队列中取出的线程
//    void outMsgRecvQueue()
//    {
//        for (int i = 0; i < 100; i++)
//        {
//            if (!msgRecvQueue.empty())
//            {
//                // 消息不为空
//                std::lock_guard<mutex> lock(m_mutex);
//                int command = msgRecvQueue.front(); // 返回第一个元素
//                cout << "outMsgRecvQueue()执行，删除一个元素" << command << endl;
//                msgRecvQueue.pop_front();           // 移除第一个元素
//            }
//            // 如果消息队列为空
//            else cout << "outMsgRecvQueue()执行，但是目前消息队列中为空" << i << endl;
//
//            //cout << "end" << endl;
//        }
//    }
//
//private:
//    std::list<int> msgRecvQueue; // 容器(消息队列)，专门用于代表玩家给我们发送过来的命令
//    std::mutex m_mutex;
//};
//
//
//int main()
//{
//    A myobj_a;
//
//    std::thread myOutMsgObj(&A::outMsgRecvQueue, std::ref(myobj_a));
//    std::thread myInMsgObj(&A::inMsgRecvQueue, std::ref(myobj_a));
//
//    myInMsgObj.join();
//    myOutMsgObj.join();
//
//    return 0;
//}
//
//#include<mutex>
//#include<condition_variable>
//#include<iostream>
//#include<thread>
//#include<list>
//
//class CSingleten
//{
//public:
//	static  CSingleten* Instance()
//	{
//		if (!m_instace)
//		{
//			std::unique_lock<std::mutex> lock(m_mutex);
//			if (!m_instace)
//			{
//				m_instace = new CSingleten;
//				static freeSingleton freeSingle;
//			}
//		}
//		return m_instace;
//	}
//
//	void inQueue(int num)
//	{
//		for (int i = 0; i < num; ++i)
//		{
//			std::lock_guard<std::mutex> lock(m_queue_mutex);
//			queue.push_back(i);
//			std::cout << "inQueue()收到消息 ：" << i << std::endl;
//			condition.notify_all();
//		}
//	}
//
//	void outQueue(int num)
//	{
//		for (int i = 0; i < num; ++i)
//		{
//			if (!queue.empty())
//			{
//				std::lock_guard<std::mutex> lock(m_queue_mutex);
//				if (!queue.empty())
//				{
//					//std::lock_guard<std::mutex> lock(m_queue_mutex);  /*只有一个out 不改变上述代码的执行状态*/
//					auto command = queue.front();
//					queue.pop_front();
//					std::cout << "outQueue()取出消息 ：" << command << std::endl;
//				}
//				else
//				{
//					std::cout << "outQueue()没有消息" << std::endl;
//				}
//			}
//			else
//			{
//				std::cout << "outQueue()没有消息" << std::endl;
//			}
//		}
//	}
//	void outOneCommand()
//	{
//		while (true)
//		{
//			std::unique_lock<std::mutex> lock(m_queue_mutex);
//
//			condition.wait(lock, [this]()
//				{
//					if (!queue.empty())
//					{
//						return true;
//					}
//					else {
//						return false;
//					}
//				});
//
//			/*获取了锁，有资源可以获取*/
//			auto command = queue.front();
//			queue.pop_front();
//			std::cout << "outOneCommand()取出消息 ：" << command << std::endl;
//		}
//	}
//private:
//	CSingleten() { std::cout << "call once"<<std::endl;; };
//	static CSingleten* m_instace;
//	static std::mutex m_mutex;
//
//	std::list<int> queue;
//	std::mutex m_queue_mutex;
//	std::condition_variable condition;
//	
//
//	class freeSingleton
//	{
//	public:
//		~freeSingleton()
//		{
//			if (m_instace)
//			{
//				delete m_instace;
//				m_instace = nullptr;
//			}
//		}
//	};
//	
//};
//
//CSingleten* CSingleten::m_instace = nullptr;
//std::mutex CSingleten::m_mutex = std::mutex();
//
//void mythread(int flag,int num)
//{
//	if (!flag)
//	{
//		CSingleten::Instance()->inQueue(num);
//	}
//	else if(flag==1)
//	{
//		CSingleten::Instance()->outQueue(num);
//	}
//	else
//	{
//		CSingleten::Instance()->outOneCommand();
//	}
//}
//
//int main()
//{
//	//CSingleten* ptr = CSingleten::Instance();
//	int num = 100;
//	//std::thread  thread1(&CSingleten::inQueue,CSingleten::Instance(),num);
//	//std::thread  thread2(&CSingleten::outQueue,CSingleten::Instance(),num);
//
//	std::thread thread1(mythread, 0, num);
//	std::thread thread2(mythread, 1, num);
//	std::thread thread3(mythread, 2, num);
//
//	//std::thread thread1(m);
//	//std::thread thread2(m);
//
//	thread1.join();
//	thread2.join();
//	thread3.join();
//}


#include<stdio.h>
#include"a.c"

extern int var;

int main()
{
	//std::cout << var << std::endl;
	printf("%d\n", var);
	return 0;
}