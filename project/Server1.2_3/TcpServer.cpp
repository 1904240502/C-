#include "TcpServer.h"

//#define S
//#define R_COUT
TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
	_RECV_COUNT = 0;
}
TcpServer::~TcpServer()
{
	Close();
}
void TcpServer::InitSocket()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	if (isCon())
		Close();
	//建立socket套节字
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
		std::cout << "服务端socket创建失败！" << std::endl;
	else
		std::cout << "服务端创建成功！" << std::endl;
}
void TcpServer::Bind(const char* IP, unsigned short port)
{
	if (!isCon())
		InitSocket();
	//绑定客户端网络端口
	sockaddr_in sin = {};
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	if (IP) {
		sin.sin_addr.S_un.S_addr = inet_addr(IP);
	}
	else {
		sin.sin_addr.S_un.S_addr = INADDR_ANY;
	}
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&sin, sizeof(sockaddr_in)))
		std::cout << "绑定端口失败！" << std::endl;
	else
		std::cout << "绑定端口成功！" << std::endl;

}
void TcpServer::Listen(int backlog)
{
	//监听listen
	if (SOCKET_ERROR == listen(_sock, backlog))
		std::cout << "服务端监听失败！" << std::endl;
	else
		std::cout << "服务端监听成功！" << std::endl;
}
void TcpServer::Accept()
{
	sockaddr_in clientAddr = {};
	int addrlen = sizeof(clientAddr);
	SOCKET cSock = INVALID_SOCKET;

	//接收客户端连接accept
	cSock = accept(_sock, (sockaddr*)&clientAddr, &addrlen);
	if (INVALID_SOCKET == cSock)
		std::cout << "新客户端连接请求失败！" << std::endl;
	else
	{
		Client* client = new Client(cSock);
		_ClientSets.push_back(client);
		//std::cout << "新客户端 SOCKET =" << cSock << "连接成功！" <<"用户："<<_ClientSets.size() << std::endl;

		NewUserJoin NewUser;
		NewUser.sock = cSock;

#ifdef S
		SendDataToClients(&NewUser);
#endif // S

		
	}
}
void TcpServer::Close()
{
	if (isCon())
	{
		//关闭服务端closesocket
		for (int i = _ClientSets.size() - 1; i >= 0; i--)
		{
			closesocket(_ClientSets[i]->GetSocket());
			delete _ClientSets[i];
		}
		closesocket(_sock);
		WSACleanup();
		_sock = INVALID_SOCKET;

		_ClientSets.clear();
	}
}
void TcpServer::start()
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
	FD_SET(_sock, &fdRead);
	FD_SET(_sock, &fdWrite);
	FD_SET(_sock, &fdExecept);

	SOCKET maxfd = _sock;
	
	//将所有客户端socket添加到集合中
	for (auto client:_ClientSets)
	{
		FD_SET(client->GetSocket(), &fdRead);
		if (maxfd < client->GetSocket())
			maxfd = client->GetSocket();
	}
		
	//非阻塞select
	timeval t = { 0,0 };
	int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdExecept, &t);
	if (ret < 0)
	{
		std::cout << "select 结束！" << std::endl;
		Close();
		return;
	}

	//判断是否存在集合中
	if (FD_ISSET(_sock, &fdRead))
	{
		//count归零
		FD_CLR(_sock, &fdRead);
		Accept();
		return;
	}
	//处理请求
	for (auto client:_ClientSets)
		if (FD_ISSET(client->GetSocket(),&fdRead))
			if (RecvInfo(client))
			{
				//请求处理完删除
				DelClient(client);
			}
		
}
bool TcpServer::isCon()
{
	return INVALID_SOCKET !=_sock;
}
bool TcpServer::RecvInfo(Client* client)
{
	int len = sizeof(DataHeader);
	 
	//接受客户端请求recv
	int rlen =recv(client->GetSocket(), _BUF, BUF_SIZE, 0);
	if (rlen < 0)
	{
		//std::cout << "客户端: " << client->GetSocket() << "已退出！" << std::endl;
		return true;
	}
	memcpy(client->GetBufs() + client->GetPos(), _BUF, rlen);
	client->SetPos(client->GetPos() + rlen);
	

	//接收数据
	while (client->GetPos() >= len)
	{
		//获得数据长度
		DataHeader* header = (DataHeader*)client->GetBufs();
		//缓冲中已经有完整的包
		if (client->GetPos() >= header->dataLength)
		{
			int nlen = header->dataLength;
			//处理消息
			ExeCom(client->GetSocket(), header);
			//从缓冲中取出包
			memcpy(client->GetBufs(), client->GetBufs() + nlen, client->GetPos() - nlen);
			client->SetPos(client->GetPos() - nlen);
		}
		else
			break;
	}
	return false;
}
bool TcpServer::ExeCom(SOCKET cSock, DataHeader * header)
{
	_RECV_COUNT++;
	auto t = _Time.getElapsedSecond();
	if ( t> 1.0) {
		std::cout << "运行时间：" << t << "加入客户数目："<<_ClientSets.size()<<"客户：" << cSock << "接收数据：" << _RECV_COUNT << std::endl;
		_RECV_COUNT = 0;
		_Time.update();
	}
	//消息处理
	switch (header->type)
	{
	case LOGIN:
	{
		Login* data = (Login *)header;
#ifdef R_COUT
		std::cout << "客户端: " << cSock << "请求登录! " << data->userName << std::endl;
#endif // R_COUT

		/*
		* 用户信息认证
		*/
		//向客户端发送数据send
#ifdef S
		LoginResult result;
		result.result = 1;
		SendData(cSock, &result);
#endif // S
#ifdef R_COUT
		std::cout << "客户端: " << cSock << "登录成功! " << std::endl;
#endif // R_COUT
		
	}
	break;
	case LOGOUT:
	{
		Logout *data=(Logout *)header;
#ifdef R_COUT
		std::cout << "客户端: " << cSock << "请求退出!" << data->userName << std::endl;
#endif // R_COUT
		//向客户端发送数据send
#ifdef S
		LogoutResult result;
		result.result = 1;
		SendData(cSock, &result);
#endif // S
#ifdef R_COUT
		std::cout << "客户端: " << cSock << "退出成功!" << std::endl;;
#endif // R_COUT
		
		return true;
	}
	break;
	default:
	{
#ifdef S
		SendData(cSock, header);
#endif // S
	}
	}
	return false;
}
void TcpServer::DelClient(Client* client)
{
	//在客户端集中删除退出的客户
	auto iter = std::find(_ClientSets.begin(), _ClientSets.end(), client);
	if (iter != _ClientSets.end())
	{
		delete client;
		_ClientSets.erase(iter);
	}
		
}
int TcpServer::SendData(SOCKET cSock, const DataHeader* data)
{
	if (isCon() && data)
	{
		return send(cSock, (const char*)data, data->dataLength, 0);
	}
	return SOCKET_ERROR;
}
void TcpServer::SendDataToClients(DataHeader* data)
{
	//转发告诉其他客户端有新客户加入
	for (int i = 0; i < _ClientSets.size() - 1; i++)
	{
		SendData(_ClientSets[i]->GetSocket(), data);
	}
}
