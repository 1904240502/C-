#pragma once

#include<iostream>
using namespace std;

namespace basicknowledge {
	namespace Template {
		namespace useHas {
			template<typename...T>  //Generic
			class myclass
			{
			public:
				myclass()
				{
					cout << "myclass<typename...T>  execute , this = " << this << endl;
				}
			};

			template<>   //Specialization
			class myclass<>
			{
			public:
				myclass()
				{
					cout << "myclass<>  execute , this = " << this << endl;
				}
			};

			template<typename FT, typename...T>//partial specialization
			class myclass<FT, T...> //:private myclass<T...>
			{
			public:
				myclass() : m_f(0)
				{
					cout << "myclass<typename FT, typename...T>  execute , this = " << this << endl;
				}

				myclass(FT ft, T... t) :m_f(ft), m_c(t...)
				{
					cout << "myclass<typename FT, typename...T> ::myclass(FT ft, T... t) execute , this = " << this << "  m_f = " << m_f << endl;
				}
				FT m_f;
				myclass<T...> m_c;
			};

			void func()
			{
				cout << "==========use has ======begin" << endl;
				myclass<string, int, float> item("value", 28, 18.8);
				cout << "==========use has ======end" << endl;
			}
		}
	}
}

