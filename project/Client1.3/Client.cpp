#include<thread>
#include"TcpClient.h"
#include<thread>
//const int nCount = FD_SETSIZE-1;
const int nCount = 10000;
const int tCount = 4;
TcpClient* clients[nCount];

bool RUN = true;
void cmd()
{
	char buf[512];
	while (RUN)
	{
		//输入请求
		std::cin >> buf;

		//处理请求
		if (0 == strcmp(buf, "exit"))
		{
			RUN = false;
			std::cout << "退出cmd线程" << std::endl;
			break;
		}
		else {
			std::cout << "命令输入错误！" << std::endl;
		}
	}
}

void func_p(int i)
{
	int size = nCount / tCount;
	
	int begin = i * size;
	int end = begin + size;
	for (int i = begin; i < end; i++)
	{
		clients[i] = new TcpClient();
	}
	for (int j = begin;j < end; j++)
	{
		clients[j]->Connect("127.0.0.1",4567);
		std::cout << "线程：" << i + 1 << "客户：" <<j << std::endl;
	}


	//等待
	std::chrono::milliseconds t(1000);
	std::this_thread::sleep_for(t);

	Login login;
	strcpy_s(login.userName, "zhang san");
	strcpy_s(login.passWord, "123456");

	//发送包
	while (RUN)
	{
		for (int i = begin; i < end; i++)
		{
			clients[i]->SendData(&login);
			clients[i]->start();
		}
	}
	for (int i = begin; i < end; i++)
	{
		clients[i]->Close();
		delete clients[i];
	}
}

int main()
{
	std::thread out(cmd);
	out.detach();

	for (int i = 0; i < tCount; i++)
	{
		std::thread th=std::thread(func_p, i );
		th.detach();
	}
	while (RUN)
	{
		Sleep(50);
	}
	return 0;
}