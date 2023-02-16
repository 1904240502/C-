#define WIN32_LEAN_AND_MEAN  //解决头文件重定义

#include<Windows.h>
#include<WinSock2.h>


//#pragma comment(lib,"ws2_32.lib") //windows 附加依赖项

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver,&dat);

	WSACleanup();
	return 0;
}