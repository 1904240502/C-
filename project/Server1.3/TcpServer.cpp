#include "TcpServer.h"

TcpServer::TcpServer()
{
	_sock = INVALID_SOCKET;
	_MSG_COUNT = 0;
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

	if (INVALID_SOCKET != _sock)
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
	if (INVALID_SOCKET == _sock)
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
	std::cout << "TCP Server Close! >>" << std::endl;
	_Thread.Close();
	if (INVALID_SOCKET != _sock)
	{
		//等待服务处理结束关闭服务端
		for (auto val : _ServerSet)
		{
			delete val;
		}
		_ServerSet.clear();
		closesocket(_sock);
		_sock = INVALID_SOCKET;
		WSACleanup();
	}
	std::cout << "TCP Server Close! <<" << std::endl;
}

void TcpServer::Start(int SERVER_NUM)
{
	std::cout << "TCP Server Start! >>" << std::endl;
	for (int i = 0; i < SERVER_NUM; i++)
	{
		//创建服务处理端对象,启动线程
		_ServerSet.push_back(new EventServer(this, i + 1));
		_ServerSet[i]->Start();
	}
	_Thread.Start(
		nullptr,
		[this](ctlThread* pThread)
		{
			OnRun(pThread);
		},
		nullptr);
	std::cout << "TCP Server Start! <<" << std::endl;
}
void TcpServer::OnRun(ctlThread *pThread)
{
	while(_Thread.isRun())
	{
		//统计包数
		statistics();
		//创建文件描述符集
		fd_set fdRead;
		//置零
		FD_ZERO(&fdRead);
		//将服务端socket添加到集合中
		FD_SET(_sock, &fdRead);

		//非阻塞select
		timeval t = { 0,1 };
		int ret = select((int)_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			std::cout << "客户 select 结束！" << std::endl;
			_Thread.Exit();
			return;
		}

		//判断是否存在集合中
		if (FD_ISSET(_sock, &fdRead))
		{
			//count归零
			FD_CLR(_sock, &fdRead);
			Accept();
		}
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
}
void TcpServer::statistics()
{
	auto t = _Time.getElapsedSecond();
	if (t > 1.0) {
		int nim = 0;
		/*for (auto val : _ServerSet)
		{
			nim += val->GetClientNum();
			std::cout << val->GetClientNum() << std::endl;
		}*/
		std::cout << "运行时间：" << t << "加入客户数目：" << _CLIENT_NUM  << "处理数据包：" << _MSG_COUNT  << std::endl;
		_MSG_COUNT = 0;
		_Time.update();
	}
}
void TcpServer::onLeave(Client* pClient)
{
	_CLIENT_NUM--;
}
void TcpServer::onJoin(Client* pClient)
{
	_CLIENT_NUM++;
}
void TcpServer::onMsg(EventServer* pEventServer , Client* pClient, DataHeader* header)
{
	_MSG_COUNT++;
}
void TcpServer::onRecv(Client* pClient)
{
	pClient->resetHeart();
}
//void TcpServer::onMsg(Client* pClient, DataHeader* header) {
//	_MSG_COUNT++;
//}