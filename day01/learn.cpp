#include <iostream>

using namespace std;

int func1(int &c)
{
    return 0;
}
int func2(const int &c)
{
    return 0;
}
/*
constexpr int func2()
{
    // int a; //constexpr 中的变量必须初始化
    int a = 5;
    // for (int i{0}; i < 5; ++i)
    // cout << i << endl;

    return a;
}
 */
int main()
{
    /*
        int a = 3.5; // z自动类型转换
        int b{3.5};  // 报错
     */
    // auto 自动类型推断 编译期进行

    auto c = 1.5f;
    auto d = 1;

    // 引用 const

    int &e = d;
    // int &e = 1; //非常量的引用必须是左值
    const int &f = 1;
    // int &g = f;//丢弃了限定符
    int &g = (int &)f;
    g = 2;
    cout << "f value:" << f << endl;
    cout << "g value:" << g << endl;

    // constexpr int &f = 1; // error: cannot bind non-const lvalue reference of type 'int&' to an rvalue of type 'int'
    // 临时变量不能作为非const引用参数
    // func1(d + 1);
    // func1(1);
    func2(d + 1);

    // 临时变量
    /*参数以非const引用传入 默认将要修改值 并且返回后会使用*/
    /*
        1.临时变量通常在  函数参数  传递发生类型转换以及  函数返回值  时被创建
        2.C/C++不允许为非const的引用类型创建临时变量。
        3.以下三中情况会产生临时变量
            按值传递的时候
            参数为const的类型
            类型转换
    */

    // constexpr 编译时求值

    constexpr int var = 1;
    int var2 = 2;
    const int var3 = var2;
    // constexpr int var3 = var2;//var2 不是常量

    return 0;
}
