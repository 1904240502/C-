#define WIN32_LEAN_AND_MEAN  //���ͷ�ļ��ض���

#include<Windows.h>
#include<WinSock2.h>


//#pragma comment(lib,"ws2_32.lib") //windows ����������

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver,&dat);

	WSACleanup();
	return 0;
}