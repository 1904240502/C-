#include"TcpServer.h"

int main()
{
	TcpServer s1;
	s1.InitServer();
	s1.CreateServer("127.0.0.1", 4567, 20);

	while (true)
	{
		s1.start();
	}
	s1.Close();
	return 0;
}