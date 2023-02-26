#include "TcpClient.h"

TcpClient::TcpClient()
{
	_sock = INVALID_SOCKET;
}
TcpClient::~TcpClient()
{
		Close();
}
void TcpClient::InitSocket()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//已连接其他服务器断开连接重新创建
	if (isCon())
	{
		std::cout << "socket=" << _sock << "旧连接关闭" << std::endl;
		Close();
	}
	//建立socket套节字
	 _sock= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (!isCon())
		std::cout << "客户端socket创建失败！" << std::endl;
	else
		std::cout << "客户端创建成功！" << std::endl;
}
void TcpClient::Connect(const char * IP,unsigned short port)
{
	if (!isCon())
	{
		InitSocket();
	}
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = inet_addr(IP);
	if (SOCKET_ERROR == connect(_sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout  <<"socket=" << _sock <<"错误！连接http:\\\\"<<IP <<":"<< port <<"失败！"<< std::endl;
	else
		std::cout << "socket=" << _sock << "成功！连接http:\\\\" << IP << ":" << port << "成功！" << std::endl;
}
void TcpClient::Close()
{
	if (isCon())
	{
		closesocket(_sock);
		WSACleanup();
		std::cout << "客户端"<<_sock<<"退出！" << std::endl;
		_sock = INVALID_SOCKET;
	}
}
bool TcpClient::SendPro(){
	char buffer[1024] = {};
	int rlen = 0;

	while (true)
	{
		//输入请求
		std::cin >> buffer;

		//处理请求
		if (0 == strcmp(buffer, "exit"))
			Close();
		else if (0 == strcmp(buffer, "login")) {
			//发送请求信息
			Login login;
			strcpy_s(login.userName, "zhang san");
			strcpy_s(login.passWord, "123456");
			SendData(&login);
		}
		else if (0 == strcmp(buffer, "logout"))
		{
			//发送请求信息
			Logout logout;
			strcpy_s(logout.userName, "li si");
			SendData(&logout);
		}
		else
		{
			std::cout << "命令输入错误！" << std::endl;
		}
	}
	return false;
}
bool TcpClient::RecvInfo()
{
	
	char buffer[1024];
	int rlen;

	//接收服务端指令
	rlen = RecvData(buffer,sizeof(DataHeader));
	DataHeader *header = (DataHeader *)buffer;
	if (rlen <= 0)
	{
		std::cout << "与服务端断开连接！" << std::endl;
		
		Close();
	}
	//std::cout << "指令长度：" << rlen << "|" << header->dataLength << " 指令类型: " << header->type << std::endl;
	//接收数据
	rlen = RecvData(buffer + sizeof(DataHeader), header->dataLength - sizeof(DataHeader));
	//执行指令
	ExeCom((DataHeader *)buffer,rlen);

	return true;
}
bool TcpClient::ExeCom(DataHeader* header,int rlen)
{
	switch (header->type)
	{
	case LOGIN_RESULT:
	{
		//接收登录结果
		LoginResult * res = (LoginResult *)header;
		if (res->result == 1)
			std::cout << "客户端登录成功！" << std::endl;
		else
			std::cout << "客户端登录失败！" << std::endl;
	}
	break;
	case LOGOUT_RESULT:
	{
		//接收登录结果
		LogoutResult *res=(LogoutResult*)header;
		if (res->result == 1)
			Close();
		else
			std::cout << "客户端登录失败！" << std::endl;
	}
	break;
	case NEW_USER_JOIN:
	{
		//接收登录结果
		NewUserJoin* res = (NewUserJoin*)header;;
		if (res->type ==NEW_USER_JOIN)
			std::cout << "新客户端: " << res->sock << "请求加入群聊 ! " << std::endl;
		else
			std::cout << "新客户端加入失败！" << std::endl;
	}
	break;
	default:std::cout << "无法解析的命令！" << std::endl;
	}
	return false;
}
void TcpClient::start()
{
	fd_set fdRead;
	fd_set fdWrite;
	fd_set fdExecept;

	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExecept);

	FD_SET(_sock, &fdRead);
	FD_SET(_sock, &fdWrite);
	FD_SET(_sock, &fdExecept);

	timeval t = { 0,0 };
	int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExecept, &t);
	if (ret < 0)
	{
		std::cout << "select 结束！" << std::endl;
		Close();
		return;
	}

	if (FD_ISSET(_sock, &fdRead))
	{
		FD_CLR(_sock, &fdRead);
		if (!RecvInfo())
		{
			std::cout << "select处理结束！" << std::endl;
			return;
		}
	}
}
bool TcpClient::isCon()
{
	return INVALID_SOCKET !=_sock;
}
int TcpClient::SendData(const DataHeader *data)
{
	if (isCon() && data)
	{
		return send(_sock, (const char*)data, data->dataLength, 0);
	}
	return SOCKET_ERROR;
}
int TcpClient::RecvData(char* data, int len)
{
	return recv(_sock, data, len, 0);
}