#include "TcpClient.h"

TcpClient::TcpClient(SOCKET sock ):_sock(sock)
{
	_isConnect = false;
	_BUF_POS = 0;
	_BUF = new char[BUF_SIZE];
	_BUFS = new char[BUF_SIZE * 10];
}
TcpClient::~TcpClient()
{
	delete[] _BUF;
	delete[] _BUFS;
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
	 if (_sock==INVALID_SOCKET)
		 std::cout << "客户端socket创建失败！" << std::endl;
	 else
		 ;
		//std::cout << "客户端创建成功！" << std::endl;
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
		std::cout << "socket=" << _sock << "错误！连接http:\\\\" << IP << ":" << port << "失败！" << std::endl;
	else
		_isConnect=true;// std::cout << "socket=" << _sock << "成功！连接http:\\\\" << IP << ":" << port << "成功！" << std::endl;
}
void TcpClient::Close()
{
	if (isCon())
	{
		//std::cout << "客户端" << _sock << "退出！" << std::endl;
		closesocket(_sock);
		WSACleanup();
		_sock = INVALID_SOCKET;
		_isConnect = false;
	}
}
bool TcpClient::SendPro(){
	int rlen = 0;

	while (true)
	{
		//输入请求
		std::cin >> _BUF;

		//处理请求
		if (0 == strcmp(_BUF, "exit"))
			Close();
		else if (0 == strcmp(_BUF, "login")) {
			//发送请求信息
			Login login;
			strcpy_s(login.userName, "zhang san");
			strcpy_s(login.passWord, "123456");
			SendData(&login);
		}
		else if (0 == strcmp(_BUF, "logout"))
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
	int len = sizeof(DataHeader);

	//从缓冲中copy数据到中转缓冲
	int rlen = RecvData(_BUF, BUF_SIZE);
	//转到客户端自建缓冲
	memcpy(_BUFS + _BUF_POS, _BUF, rlen);
	_BUF_POS += rlen;

	//接收数据
	while (_BUF_POS>=len)
	{
		//获得数据长度
		DataHeader* header = (DataHeader*)_BUFS;
		//缓冲中已经有完整的包
		if (_BUF_POS >= header->dataLength)
		{
			int nlen = header->dataLength;
			//处理消息
			ExeCom(header);
			//从缓冲中取出包
			memcpy(_BUFS, _BUFS + nlen, _BUF_POS- nlen);
			_BUF_POS -= nlen;
		}
		else
		{
			break;
		}
	}
	return true;
}
bool TcpClient::ExeCom(DataHeader* header)
{
	switch (header->type)
	{
	case LOGIN_RESULT:
	{
		//接收登录结果
		LoginResult * res = (LoginResult *)header;
		if (res->result == 1)
			;// std::cout << "客户端登录成功！" << std::endl;
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
	case HEART2C:
	{
		//接收登录结果
		std::cout << "服务端存在！" << std::endl;
	}
	break;
	default: std::cout << "无法解析的命令！" << std::endl;
	}
	return false;
}
void TcpClient::start()
{
	fd_set fdRead{};
	FD_ZERO(&fdRead);
	FD_SET(_sock, &fdRead);


	timeval t = { 0,0 };
	int ret = select(_sock + 1, &fdRead, 0,0, &t);
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
	return INVALID_SOCKET !=_sock&&_isConnect;
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
	int rlen= recv(_sock, data, len, 0);
	//std::cout << rlen << std::endl;
	if (rlen <= 0)
	{
		std::cout << "与服务端断开连接！" << std::endl;
		Close();
	}
	return rlen;
}