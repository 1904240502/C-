#include"TcpServerForMe.h"

bool RUN = true;
void cmd()
{
	char buf[512];
	while (true)
	{
		//��������
		std::cin >> buf;

		//��������
		if (0 == strcmp(buf, "exit"))
		{
			RUN = false;
			std::cout << "�˳�cmd�߳�" << std::endl;
			break;
		}
		else {
			std::cout << "�����������" << std::endl;
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
	//�������� ���մ�����Ϣ
	s1.createServer(4);

	//���տͻ�
	while (RUN)
	{
		s1.start();
	}
	s1.Close();
	return 0;
}