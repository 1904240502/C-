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
	LoginResult() :result(1)
	{
		dataLength = sizeof(LoginResult);
		type = LOGIN_RESULT;
	}
	int result;
};
class Logout :public DataHeader
{
public:
	Logout() :userName("")
	{
		dataLength = sizeof(Logout);
		type = LOGOUT;
	}
	char userName[32];
};
class LogoutResult :public DataHeader
{
public:
	LogoutResult() :result(0)
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
		}else if (0 == strcmp(buffer, "login")) {
			//发送请求信息
			Login login;
			strcpy_s(login.userName, "zhang san");
			strcpy_s(login.passWord, "123456");
			//send(sock, (const char*)&login.type, sizeof(DataHeader), 0);
			send(sock, (const char*)&login, sizeof(Login), 0);
			//接收登录结果
			LoginResult res;
			rlen=recv(sock, (char *)&res, sizeof(LoginResult), 0);
			if (res.result == 1)
				std::cout << "数据长度：" << rlen<<"|" << res.dataLength << " 成功返回\"1\": " << res.result << std::endl;
			else 
				std::cout << "客户端登录失败！" << std::endl;
		}
		else if (0 == strcmp(buffer, "logout"))
		{
			//发送请求信息
			
			Logout logout;
			strcpy_s(logout.userName, "li si");
			//send(sock, (const char*)&logout.type, sizeof(DataHeader), 0);
			send(sock, (const char*)&logout, sizeof(Logout), 0);
			//接收登录结果
			LogoutResult res;
			rlen=recv(sock, (char*)&res, sizeof(LogoutResult), 0);
			if (res.result == 0)
				std::cout << "数据长度：" << rlen << "|" << res.dataLength <<  " 成功返回\"0\": " << res.result << std::endl;
			else
				std::cout << "客户端登录失败！" << std::endl;
		}
		else
		{
			std::cout << "命令输入错误！" << std::endl;
		}
	}
	

	//关闭服务端closesocket
	closesocket(sock);

	WSACleanup();
	getchar();
	return 0;
}