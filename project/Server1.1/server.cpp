#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>

#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项


enum DataType
{
	LOGIN,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT,
	WEEOR
};

struct DataHeader
{
	short dataLength;
	DataType type;
};

class Login :public DataHeader
{
public:
	Login() :userName(""), passWord("")
	{
		dataLength = sizeof(Login);
		type = LOGIN;
	}
	char userName[32];
	char passWord[32];
};
class LoginResult :public DataHeader
{
public:
	LoginResult():result(1)
	{
		dataLength = sizeof(LoginResult);
		type = LOGIN_RESULT;
	}
	int result;
};
class Logout :public DataHeader
{
public:
	Logout():userName("")
	{
		dataLength = sizeof(Logout);
		type = LOGOUT;
	}
	char userName[32];
};
class LogoutResult :public DataHeader
{
public:
	LogoutResult():result(0)
	{
		dataLength = sizeof(LogoutResult);
		type = LOGOUT_RESULT;
	}
	int result;
};



int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//建立socket套节字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
	if (SOCKET_ERROR == listen(sock, 5))
		std::cout << "服务端监听失败！" << std::endl;
	else
		std::cout << "服务端监听成功！" << std::endl;

	sockaddr_in clientAddr = {};
	int addrlen = sizeof(clientAddr);
	SOCKET cSock = INVALID_SOCKET;



	//接受客户端连接accept
	cSock = accept(sock, (sockaddr*)&clientAddr, &addrlen);
	if (INVALID_SOCKET == cSock)
		std::cout << "接受到客户端失败！" << std::endl;
	else
		std::cout << "接受到客户端成功！" << std::endl;

	DataHeader cmd{};
	int rlen = 0;
	
	while (true)
	{
		//接受客户端请求recv
		rlen = recv(cSock, (char*)&cmd, sizeof(DataHeader), 0);
		if (rlen <= 0)
		{
			std::cout << "客户端--接受消息失败！" << std::endl;
			break;
		}	

		//消息处理
		switch (cmd.type)
		{
            case LOGIN:
		    {
				Login data;
				rlen=recv(cSock, (char*)&data+sizeof(DataHeader), sizeof(Login)- sizeof(DataHeader), 0);
				std::cout << "数据长度："<<rlen << "|" << data.dataLength <<" 请求登录的用户名：" << data.userName << std::endl;
				/*
				* 用户信息认证
				*/
				//向客户端发送数据send
				LoginResult result;
				send(cSock, (const char*)&result, sizeof(LoginResult), 0);
		    }
		    break;
			case LOGOUT:
			{
				Logout data;
				rlen=recv(cSock, (char*)&data+ sizeof(DataHeader), sizeof(Logout)- sizeof(DataHeader), 0);
				std::cout << "数据长度：" << rlen<<"|"<<data.dataLength << " 请求退出的用户名：" << data.userName << std::endl;
				//向客户端发送数据send
				LogoutResult result;
				send(cSock, (const char*)&result, sizeof(LogoutResult), 0);
			}
			break;
			default:
			{
				DataHeader data{0,WEEOR};
				send(cSock, (const char*)&data, sizeof(DataHeader) , 0);
			}
		}
	}
	//关闭服务端closesocket
	closesocket(sock);

	WSACleanup();
	return 0;
}