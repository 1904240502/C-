#include<thread>
#include"TcpClient.h"

//const int nCount = FD_SETSIZE-1;
const int nCount = 5000;
void s(TcpClient *c)
{
	c->SendPro();
}

int main()
{
	TcpClient** c = new TcpClient *[nCount];
	for (int i = 0; i < nCount; i++)
	{
		c[i] = new TcpClient;
		c[i]->InitSocket();
		c[i]->Connect("127.0.0.1", 4567);
		std::cout << "¿Í»§£º" << i << std::endl;
	}
	//std::thread t1(s, &c1);
	//t1.detach();

	Login login;
	strcpy_s(login.userName, "zhang san");
	strcpy_s(login.passWord, "123456");


	while (true)
	{
		for (int i = 0; i < nCount; i++)
		{
			c[i]->SendData(&login);
			//c[i].start();
		}
	}
	for (int i = 0; i < nCount; i++)
	{
		c[i]->Close();
	}
	delete[] c;
	delete c;

	Sleep(2000);
	return 0;
}