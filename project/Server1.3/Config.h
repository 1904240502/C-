#ifndef __CONFIG_H__
#define __CONFIG_H__

//inet_ntoa 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
//���ͷ�ļ��ض���
#define WIN32_LEAN_AND_MEAN 
//select fd��С
#define FD_SETSIZE 3000
#include<Windows.h>
#include<WinSock2.h>

#include"Package.h"

//windows ����������
#pragma comment(lib,"ws2_32.lib") 



//���ջ����С
#define RECV_BUF_SIZE 1000
//���ͻ����С
#define SEND_BUF_SIZE 1000
//����ʱ�� /s
#define SURVIVAL_TIME 50
//��ʱ�������� /s
#define SEND_TIME 10

#endif //__CONFIG_H__


