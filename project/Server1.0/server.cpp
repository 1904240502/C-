#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//建立socket套节字
	SOCKET sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock)
		std::cout << "服务端socket创建成功！" << std::endl;
	else
		std::cout << "服务端socket创建成功！" << std::endl;

	//绑定客户端网络端口
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(4567);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "绑定端口失败！" << std::endl;
	else
		std::cout << "绑定端口成功！" << std::endl;

	//监听listen
	if (SOCKET_ERROR == listen(sock,5))
		std::cout << "服务端监听失败！" << std::endl;
	else
		std::cout << "服务端监听成功！" << std::endl;
	
	sockaddr_in clientAddr = {};
	int addrlen = sizeof(clientAddr);
	SOCKET cSock = INVALID_SOCKET;
	char buffer[] = "l'm server. ";

	while (true)
	{
		//接受客户端连接accept
		cSock = accept(sock, (sockaddr*)&clientAddr, &addrlen);
		if (INVALID_SOCKET == cSock)
			std::cout << "接受到客户端失败！" << std::endl;
		else
			std::cout << "接受到客户端成功！" << std::endl;

		std::cout << "新客户端IP:"<< inet_ntoa(clientAddr.sin_addr)<< std::endl;
		//向客户端发送数据send
		if (0>=send(cSock, buffer, strlen(buffer) + 1, 0))
			std::cout << "向客户端发送消息失败！" << std::endl;
		else
			std::cout << "向客户端发送消息成功！" << std::endl;
	}

	//关闭服务端closesocket
	closesocket(sock);

	WSACleanup();
	return 0;
}