#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项

struct DataPackage
{
	int age;
	char name[32];
};

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//建立socket套节字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == sock)
		std::cout << "客户端socket创建失败！" << std::endl;
	else
		std::cout << "客户端socket创建成功！" << std::endl;

	//连接服务端connect
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "连接服务器失败！" << std::endl;
	else
		std::cout << "连接服务器成功！" << std::endl;

	
	char buffer[1024] = {};
	int rlen = 0;

	while (true)
	{
		//输入请求
		std::cout << "请求：";
		std::cin >> buffer;
		//处理请求
		if (0 == strcmp(buffer, "exit"))
		{
			std::cout << "客户端退出！" << std::endl;
			break;
		}else {
			//发送请求信息
			rlen=send(sock, buffer, sizeof(buffer)+1, 0);
			if (0 >= rlen)
				std::cout << "向服务端发送消息失败！" << std::endl;
			else
				std::cout << "向服务端发送消息成功！" << std::endl;
		}

		//接受信息 recv
		
		rlen = recv(sock,  buffer, 1024, 0);
		if (rlen <= 0)
			std::cout << "客户端--接受消息失败！" << std::endl;
		else {
			DataPackage* data = (DataPackage *)buffer;
			std::cout << "接受到服务端消息！" << std::endl;
			std::cout << "名字:" <<data->name<<"年龄:"<<data->age << std::endl;
		}
	}
	

	//关闭服务端closesocket
	closesocket(sock);

	WSACleanup();
	getchar();
	return 0;
}