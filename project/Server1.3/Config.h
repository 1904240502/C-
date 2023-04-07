#ifndef __CONFIG_H__
#define __CONFIG_H__

//inet_ntoa 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
//解决头文件重定义
#define WIN32_LEAN_AND_MEAN 
//select fd大小
#define FD_SETSIZE 3000
#include<Windows.h>
#include<WinSock2.h>

#include"Package.h"

//windows 附加依赖项
#pragma comment(lib,"ws2_32.lib") 



//接收缓冲大小
#define RECV_BUF_SIZE 1000
//发送缓冲大小
#define SEND_BUF_SIZE 1000
//心跳时常 /s
#define SURVIVAL_TIME 50
//定时发送数据 /s
#define SEND_TIME 10

#endif //__CONFIG_H__


