#define WIN32_LEAN_AND_MEAN  //解决头文件重定义
#define _WINSOCK_DEPRECATED_NO_WARNINGS //inet_ntoa 

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<vector>
#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项

std::vector<SOCKET> ClientSets;

enum DataType
{
	LOGIN,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT,
	NEW_USER_JOIN,
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
class NewUserJoin:public DataHeader
{
public:
	NewUserJoin() :sock(0)
	{
		dataLength = sizeof(NewUserJoin);
		type = NEW_USER_JOIN;
	}
	int sock;
};

//消息处理
int  proc(SOCKET cSock)
{
	DataHeader cmd{};
	int rlen = 0;

	//接受客户端请求recv
	rlen = recv(cSock, (char*)&cmd, sizeof(DataHeader), 0);
	if (rlen <= 0)
	{
		std::cout << "客户端: "<<cSock<<"已退出！" << std::endl;
		return -1;
	}
	//消息处理
	switch (cmd.type)
	{
	case LOGIN:
	{
		Login data;
		rlen = recv(cSock, (char*)&data + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
		std::cout << "客户端: " << cSock<<"请求! " << "数据长度：" << rlen << "|" << data.dataLength << " 请求登录的用户名：" << data.userName << std::endl;
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
		rlen = recv(cSock, (char*)&data + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
		std::cout << "客户端: " << cSock << "请求!" << "数据长度：" << rlen << "|" << data.dataLength << " 请求退出的用户名：" << data.userName << std::endl;
		//向客户端发送数据send
		LogoutResult result;
		send(cSock, (const char*)&result, sizeof(LogoutResult), 0);
	}
	break;
	default:
	{
		DataHeader data{ 0,WEEOR };
		send(cSock, (const char*)&data, sizeof(DataHeader), 0);
	}
	}
	return 0;
}

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//建立socket套节字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sock)
		std::cout << "服务端socket创建失败！" << std::endl;
	else
		std::cout << "服务端socket=" << sock << "创建成功！" << std::endl;

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

	while (true)
	{
		//创建文件描述符集
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExecept;

		//置零
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExecept);

		//将服务端socket添加到集合中
		FD_SET(sock, &fdRead);
		FD_SET(sock, &fdWrite);
		FD_SET(sock, &fdExecept);

		//将所有客户端socket添加到集合中
		for (int i =0; i <ClientSets.size(); i++)
			FD_SET(ClientSets[i], &fdRead);

		//非阻塞select
		timeval t = { 0,0 };
		int ret = select(sock+1, &fdRead, &fdWrite, &fdExecept, &t);
		if (ret < 0)
		{
			std::cout << "select 结束！" << std::endl;
			break;
		}

		//判断是否存在集合中
		if (FD_ISSET(sock, &fdRead))
		{
			//count归零
			FD_CLR(sock, &fdRead);
			sockaddr_in clientAddr = {};
			int addrlen = sizeof(clientAddr);
			SOCKET cSock = INVALID_SOCKET;

			//接收客户端连接accept
			cSock = accept(sock, (sockaddr*)&clientAddr, &addrlen);
			if (INVALID_SOCKET == cSock)
				std::cout << "新客户端连接请求失败！" << std::endl;
			else
			{
				ClientSets.push_back(cSock);
				std::cout << "新客户端 SOCKET ="<<cSock<<"连接成功！"<< std::endl;

				//转发告诉其他客户端有新客户加入
				for (int i = 0; i < ClientSets.size() - 1; i++)
				{
					NewUserJoin NewUser{};
					NewUser.sock = cSock;
					send(ClientSets[i],(const char *) & NewUser, NewUser.dataLength, 0);
				}
			}	
		}

		//处理请求
		for (int i = 0; i <fdRead.fd_count; i++)
			if (-1==proc(fdRead.fd_array[i]))
			{
				//在客户端集中删除退出的客户
				auto iter = std::find(ClientSets.begin(), ClientSets.end(), fdRead.fd_array[i]);
				if (iter != ClientSets.end())
					ClientSets.erase(iter);
			}
		/*
		* std::cout << "处理其他业务！" << std::endl;
		*/
	}
	//关闭服务端closesocket
	for (int i = ClientSets.size() - 1; i >= 0; i--)
		closesocket(ClientSets[i]);
	
	WSACleanup();
	return 0;
}