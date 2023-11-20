#pragma once

#include<iostream>
using namespace std;

namespace basicknowledge {
	namespace Template {
		namespace function {
			
			template<typename T>
			void func1( T var)
			{
				cout << "template<typename T> void func1(T var) : var = " << var << endl;
			}

			template<>
			void func1<int>(int var)
			{
				cout << "template<> void func1<int>(int var) : var = " << var<<endl;
			}

			template<typename T>
			void func1(T* var)
			{
				cout << "template<typename T> void func1(T* var) : *var = " << *var << endl;
			}

			template<typename T>
			using fun = void(*)(T);


			template<typename T,unsigned int L1>
			void NoTypeFun(const T (&C)[L1])
			{
				cout << C << endl;
			}

			void func1()
			{
				cout << "==========non-type Template ======begin" << endl;

				NoTypeFun<char>("template<typename T,unsigned int L1> void NoTypeFun(const T(&C)[L1])");

				cout << "==========non-type Template ======end" << endl;
			}

			void func2()
			{
				cout << "==========type Template ======begin" << endl;

				func1(1.2);
				func1<int>(2.2);
				func1(3);
				fun<int> FUN = func1;
				FUN(4);
				fun<double> FUN2 = func1;
				FUN2(4);

				int* var = new int(4);
				func1(var);
				delete var;

				cout << "==========type Template ======end" << endl;
			}

			void func()
			{
				cout << "==========function Template ======begin" << endl;
				func1();
				func2();
				cout << "==========function Template ======end" << endl;
			}
		}
	}
}
