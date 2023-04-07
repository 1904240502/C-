#include"TcpServerForMe.h"


int main()
{
	TcpServerForMe s1;
	s1.InitSocket();
	s1.Bind(nullptr,4567);
	s1.Listen(5);


	//创建服务 接收处理消息
	s1.Start(4);
	while (true)
	{
		char buf[512];
		//输入请求
		std::cin >> buf;

		//处理请求
		if (0 == strcmp(buf, "exit"))
		{
			std::cout << "退出cmd线程" << std::endl;
			break;
		}
		else {
			std::cout << "命令输入错误！" << std::endl;
		}
	}
	return 0;
}