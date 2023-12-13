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
	virtual void func3() {}		 //����� ָ�� ������ַ�������ָ��ĵ�ַ


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
*  �Ǿ�̬��Ա����ռ�ö�����ڴ�ռ䣬ÿ���������Լ�������һ�ݡ�
*  ��̬��Ա������ռ�ö�����ڴ�ռ䣬���������࣬���ж�����ͬһ�ݾ�̬��Ա������
*  ��Ա�����;�̬��Ա����Ҳ��ռ�ö�����ڴ�ռ䣬���������࣬���ж�����ͬһ�ݳ�Ա�����;�̬��Ա������
*
*  ÿһ���麯�������л���һ��ָ��ָ���麯������Щָ���ŵĵط����麯����vtbl���У��麯���������ࡣ
*  ÿ��������һ���麯��ָ�� vptr���麯����ָ�룩��ָ��������������麯����
*  �麯�����������������麯���ĵ�ַ������������麯��ʱ������� vptr �ҵ��麯�����ж�Ӧ�ĺ�����ַ���е��á�
* 
*  ��̬�����麯�������
 */


/*
* 	������͵�һ���̳���ĵ�ַ��ͬ
* 
*  thisָ��ָ��ǰ����ִ�г�Ա�����Ķ���ĵ�ַ��
* 
*  ������һ����Ա����ʱ������������ʽ�ؽ���ǰ����ĵ�ַ��Ϊ�����������ݸ��ó�Ա�����������������thisָ�롣
* 
*  ͨ��ʹ��thisָ�룬��Ա�������Է��ʺͲ������ڵ�ǰ����ĳ�Ա������������Ա������
* 
*  �ڶ��ؼ̳е�����£�ÿ�����඼���Լ���thisָ�룬ָ����Ӧ������Ӷ�������������е�thisָ��ȡ���ڳ�Ա�����������ࡣ
*/

class obj3
{
public:
	Son son;
};
/*
* �ϳ�Ĭ�Ϲ��캯����
* 1������û���κι��캯�������ǰ���һ�������͵ĵĳ�Ա���ó�Ա������һ��ȱʡ�Ĺ��캯����Ĭ�Ϲ������������ó�ԱȱʡĬ�Ϲ��죩
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
* �ڵ��õ�������ʱ��include ��Ҫ���ã�c/c++>����>���Ӱ���Ŀ¼ ��include��
* �����������д�ģ���Ҫ���� ������>����>���ӿ�Ŀ¼��������>����>���������� | ͬһ�����������ͬ��Ŀ�� ���Բ�����������Դ������������ü��ɣ�lib��
* �����е�����Ҫ���� ��������>����>���� ��dll��
*/

::std::shared_ptr<A> foo()
{
	return A::create();
}

int main()												  
{
	//Son* var=new Son;
	//std::cout << sizeof(*var) << std::endl;				//  ����sizeof=1	��ĳ�Ա������ռ�����Ŀռ� 

	//var->ch = 'a';				    //HEX ���������������� 
	//var->Ivar = 256;		   //С�˴洢

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