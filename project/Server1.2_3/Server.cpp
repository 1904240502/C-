#include"TcpServer.h"

int main()
{
	TcpServer s1;
	s1.InitSocket();
	s1.Bind(nullptr, 4567);
	s1.Listen(4567);

	while (true)
	{
		s1.start();
	}
	s1.Close();
	return 0;
}