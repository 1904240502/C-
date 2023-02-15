#include <iostream>

using namespace std;

int main()
{
    int var1 = 10;
    char var2[10];
    int *ip; // 指针变量的声明

    cout << "var1 address: ";
    cout << &var1 << endl;

    cout << "var2 address: ";
    cout << &var2 << endl;

    ip = &var1; // 在指针变量中存储 var 的地址

    // 输出在指针变量中存储的地址
    cout << "Address stored in ip variable: ";
    cout << ip << endl;

    // 访问指针中地址的值
    cout << "Value of *ip variable: ";
    cout << *ip << endl;

    cout << "Value of var variable: ";
    cout << var1 << endl;

    return 0;
}
