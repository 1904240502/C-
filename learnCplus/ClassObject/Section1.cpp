#include<iostream>

class obj1
{
public:
	obj1()
	{
		std::cout << "obj 1 construct ,this = " << this << std::endl;	
	}
	virtual ~obj1(){}

	void func_obj1()
	{
		std::cout << "func_obj1 ,this = " << this << std::endl;
	}

	static void func2(){}			
	virtual void func3() {}		 //虚基表 指针 类对象地址是虚基表指针的地址


	char ch='\0';
	int Ivar=0;
	static int ST;
};

int obj1::ST = 1;

class obj2
{
public:
	obj2()
	{
		std::cout << "obj 2 construct ,this = " << this << std::endl;
	}

	void func_obj2()
	{
		std::cout << "func_obj2 ,this = " << this << std::endl;
	}
	void func1()
	{
		std::cout << "func1_obj2 ,this = " << this << std::endl;
	}
	virtual ~obj2(){}

private:

};

class  Son : public obj1,public obj2
{
public:
	Son() 
	{
		std::cout << "son construct ,this = " << this << std::endl;
	}

	void func_Son()
	{
		std::cout << "func_Son ,this = " << this << std::endl;
	}
	void func1()
	{
		std::cout << "func1_Son ,this = " << this << std::endl;
	}
};
 /*
*  非静态成员变量占用对象的内存空间，每个对象都有自己独立的一份。
*  静态成员变量不占用对象的内存空间，而是属于类，所有对象共享同一份静态成员变量。
*  成员函数和静态成员函数也不占用对象的内存空间，而是属于类，所有对象共享同一份成员函数和静态成员函数。
*
*  每一个虚函数在类中会有一个指针指向虚函数，这些指针存放的地方在虚函数表（vtbl）中，虚函数表属于类。
*  每个对象都有一个虚函数指针 vptr（虚函数表指针），指向它所属的类的虚函数表。
*  虚函数表包含该类的所有虚函数的地址，当对象调用虚函数时，会根据 vptr 找到虚函数表中对应的函数地址进行调用。
* 
*  多态是由虚函数表完成
 */


/*
* 	派生类和第一个继承类的地址相同
* 
*  this指针指向当前正在执行成员函数的对象的地址。
* 
*  当调用一个成员函数时，编译器会隐式地将当前对象的地址作为隐含参数传递给该成员函数，这个参数就是this指针。
* 
*  通过使用this指针，成员函数可以访问和操作属于当前对象的成员变量和其他成员函数。
* 
*  在多重继承的情况下，每个基类都有自己的this指针，指向相应基类的子对象。派生类对象中的this指针取决于成员函数所属的类。
*/

class obj3
{
public:
	Son son;
};
/*
* 合成默认构造函数：
* 1：该类没有任何构造函数，但是包含一个类类型的的成员，该成员的类有一个缺省的构造函数（默认构造是用来调用成员缺省默认构造）
*/


#include <memory>

class A {
public:
	static ::std::shared_ptr<A> create() {
		return ::std::make_shared<A>();
	}
	static std::shared_ptr<A> create()
	{
		struct make_shared_enabler : public A {};

		return std::make_shared<make_shared_enabler>();
	}

protected:
	A() {}
	A(const A&) = delete;
	const A& operator =(const A&) = delete;
};
/*
* 在调用第三方库时，include 需要配置：c/c++>常规>附加包含目录 （include）
* 如果是其他人写的，需要配置 链接器>常规>附加库目录，链接器>输入>附加依赖项 | 同一个解决方案不同项目的 可以不做配置在资源管理器添加引用即可（lib）
* 在运行调试需要配置 配置属性>调试>环境 （dll）
*/

::std::shared_ptr<A> foo()
{
	return A::create();
}

int main()												  
{
	//Son* var=new Son;
	//std::cout << sizeof(*var) << std::endl;				//  空类sizeof=1	类的成员函数不占类对象的空间 

	//var->ch = 'a';				    //HEX 对齐是最大的整数倍 
	//var->Ivar = 256;		   //小端存储

	//var->func_obj1();
	//var->func_obj2();
	//var->func_Son();
	//var->func1();
	//var->obj2::func1();

	//delete var;

	//obj3 var2;

	foo();
	return 0;
}