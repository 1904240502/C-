#include<iostream>
#include<thread>

class MyThread
{
public:
	MyThread(int n) :m_n(n) 
	{
		std::cout << "MyThread : " << m_n << "  this thread ID: " << std::this_thread::get_id() << std::endl;
	};
	~MyThread() {};

	void operator()(int n)
	{
		m_n = n;
		std::cout << m_n << std::endl;
	}

	MyThread(const MyThread& other) : m_n(other.m_n)
	{
		std::cout << "Copy Constructor this thread ID: " <<std::this_thread::get_id()<< std::endl;
	}
private:
	int m_n;

};

int main()
{
	std::cout << "main ID: " << std::this_thread::get_id()<<std::endl;
	//MyThread v(1);
	//std::thread var1(std::cref(v), 2);
	//var1.join();

	std::thread var1(MyThread(1), 2);
	var1.detach();
	return 0;
}