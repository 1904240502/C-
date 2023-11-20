#pragma once

#include<iostream>
using namespace std;

namespace basicknowledge {
	namespace Template {
		namespace useTuple {

			template<int mycount, int mymaxcount, typename...T>
			class myclass
			{
			public:
				static void myfunc(const tuple<T...>& t)
				{
					cout << "value = " << get<mycount>(t) << endl;
					myclass<mycount + 1, mymaxcount, T...>::myfunc(t);
				}
			};

			template<int mymaxcount, typename...T>
			class myclass<mymaxcount, mymaxcount,T...>
			{
			public:
				static void myfunc(const tuple<T...>& t)
				{
					cout << "myclass<int mymaxcount, typename...T>" << endl;
				}
			};

			template<typename ...T>
			void myfunc(const tuple<T...>& t)
			{
				cout << "myfunc<typename ...T>" << endl;
				myclass<0, sizeof...(T), T...>::myfunc(t);
			}

			void func()
			{
				cout << "==========use tuple ======begin" << endl;
				tuple<string, int, float> item("value", 28, 18.8);
				myfunc(item);
				cout << "==========use tuple ======end" << endl;
			}
		}
	}
}

