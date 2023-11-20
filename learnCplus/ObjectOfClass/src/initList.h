#include<iostream>
#include <initializer_list>
using namespace std;

namespace basicknowledge {
	namespace Object {
		namespace initList {

			class  myclass
			{
			public:
				myclass();
				explicit myclass(int i);
				explicit myclass(initializer_list<int> v);
				myclass& operator=(const myclass& var);
			};

			void func()
			{
				cout << "==========class Template ======begin" << endl;
				
				myclass x=myclass(1);
				//myclass y{ 1 };
				//myclass z{ 1,2 };
				//myclass p = { 1 };
				//p.myclass::myclass({ 1,2 });

				cout << "==========class Template ======end" << endl;
			}
		}
	}
}