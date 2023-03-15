#include "TcpServer.h"

//#define S
//#define R_COUT


TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
	_RECV_COUNT = 0;
	_CLIENT_NUM = 0;
}
TcpServer::~TcpServer()
{
	Close();
	std::cout << "服务端退出！" << std::endl;
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
		;//std::cout << "服务端创建成功！" << std::endl;
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
		;// std::cout << "绑定端口成功！" << std::endl;

}
void TcpServer::Listen(int backlog)
{
	//监听listen
	if (SOCKET_ERROR == listen(_sock, backlog))
		std::cout << "服务端监听失败！" << std::endl;
	else
		;//std::cout << "服务端监听成功！" << std::endl;
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
		//添加到线程客户集
		selectServer(new Client(cSock));
	}
}
void TcpServer::Close()
{
	if (isCon())
	{
		//告知服务要退出
		for (auto& val : _ServerSet)
		{
			val->changeSocket();
		}
		//等待服务处理结束关闭服务端
		for (auto &val : _ServerSet)
		{
			val->GetThread()->join();
		}
		closesocket(_sock);
		_sock = INVALID_SOCKET;
		WSACleanup();
	}
}
void TcpServer::start()
{
	if (isCon())
	{
		//统计包数
		statistics();
		//创建文件描述符集
		fd_set fdRead{};
		//置零
		FD_ZERO(&fdRead);
		//将服务端socket添加到集合中
		FD_SET(_sock, &fdRead);

		//非阻塞select
		timeval t = { 0,10 };
		int ret = select(_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			std::cout << "客户 select 结束！" << std::endl;
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
	}
}
void TcpServer::createServer(int SERVER_NUM)
{
	for (int i = 0; i < SERVER_NUM; i++)
	{
		//创建服务处理端对象
		_ServerSet.push_back(new ConsumerServer(this,_sock));
		//启动线程
		_ServerSet[i]->start();
	}
}

void TcpServer::selectServer(Client *pClient)
{
	auto ser = _ServerSet[0];
	auto num = _ServerSet[0]->GetClientNum();
	for (auto val : _ServerSet)
	{
		auto n = val->GetClientNum();
		if (n < num)
		{
			num = n;
			ser = val;
		}
	}
	//加入客户缓冲中
	ser->addClientToSet(pClient);
	onJoin(pClient);
}
void TcpServer::statistics()
{
	auto t = _Time.getElapsedSecond();
	if (t > 1.0) {
		int nim = 0;
		for (auto val : _ServerSet)
		{
			nim += val->GetClientNum();
			std::cout << val->GetClientNum() << std::endl;
		}
		std::cout << "运行时间：" << t << "加入客户数目：" << _CLIENT_NUM  << "接收数据：" << (int)_RECV_COUNT / t << std::endl;
		_RECV_COUNT = 0;
		_Time.update();
	}
}
bool TcpServer::isCon()
{
	return INVALID_SOCKET != _sock;
}
void TcpServer::onLeave(Client* client)
{
	_CLIENT_NUM--;
}
void TcpServer::onJoin(Client* Plicent)
{
	_CLIENT_NUM++;
}
void TcpServer::onMsg(Client* pClient,const DataHeader* header)
{
	_RECV_COUNT++;
}