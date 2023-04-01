// 内存池.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//#include"Alloctor.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<stdio.h>
#include"ngx_mem_pool.h"

#include"myalloctor.hpp"
#include<vector>
struct Data
{
    char* ptr;
};
void func1(void *p)
{
    char* p1 = static_cast<char*>(p);
    free(p1);
}


int main()
{
 /*   ngx_pool pool;
    pool.ngx_create_pool(512);
    pool.ngx_palloc(128);
    Data* p2 = static_cast<Data*> (pool.ngx_palloc(512));
    p2->ptr = static_cast<char*>(malloc(12));
    strcpy(p2->ptr, "hello world");


    ngx_pool_cleanup_s* c1 = pool.ngx_pool_cleanup_add(sizeof(char*));
    c1->handler = func1;
    c1->data = p2->ptr;

    pool.ngx_destroy_pool();*/

    std::vector<int, myalloctor<int>> v;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
