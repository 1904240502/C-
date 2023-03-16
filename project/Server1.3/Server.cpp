#include"TcpServerForMe.h"

bool RUN = true;
void cmd()
{
	char buf[512];
	while (true)
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
int main()
{
	std::thread out(cmd);
	out.detach();

	TcpServerForMe s1;
	s1.InitSocket();
	s1.Bind(nullptr,4567);
	s1.Listen(5);
	//创建服务 接收处理消息
	s1.createServer(4);

	//接收客户
	while (RUN)
	{
		s1.start();
	}
	s1.Close();
	return 0;
}