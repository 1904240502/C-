#include"TcpServerForMe.h"


int main()
{
	TcpServerForMe s1;
	s1.InitSocket();
	s1.Bind(nullptr,4567);
	s1.Listen(5);


	//�������� ���մ�����Ϣ
	s1.Start(4);
	while (true)
	{
		char buf[512];
		//��������
		std::cin >> buf;

		//��������
		if (0 == strcmp(buf, "exit"))
		{
			std::cout << "�˳�cmd�߳�" << std::endl;
			break;
		}
		else {
			std::cout << "�����������" << std::endl;
		}
	}
	return 0;
}