#include<thread>
#include"TcpClient.h"


void s(TcpClient *c)
{
	c->SendPro();
}

int main()
{
	TcpClient c1;
	c1.InitSocket();
	c1.Connect("127.0.0.1",4567);

	std::thread t1(s, &c1);
	t1.detach();

	while (c1.isCon())
	{
		//c1.SendPro();
		c1.start();
	}
	c1.Close();

	Sleep(2000);
	return 0;
}