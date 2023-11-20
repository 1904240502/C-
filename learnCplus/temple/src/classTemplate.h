#pragma once

#include<iostream>
#include<vector>
using namespace std;

namespace basicknowledge {
	namespace Template {
		namespace Class {

			template <unsigned Rows, unsigned Columns>
			class Matrix {
				
			};
			template <unsigned N, unsigned M, unsigned P>
			Matrix<N, P> operator*(Matrix<N, M> const& lhs, Matrix<M, P> const& rhs) {
				
			}

			template<typename U>
			class Template
			{
			public:
				Template(); 

				template<typename V>
				void Print(U u,V v)
				{
					cout << "template<typename U> Template template<typename V> void Print(U u, V v) u=" << u<<"  v ="<<v<<endl;
				}

				template<>
				void Print<string>(U u, string v)
				{
					cout << "template<typename U> Template template<> void Print<string>(U u, string v) u=" << u << "  v =" << v << endl;
				}

				void Print(U u)
				{
					cout << "template<typename U> Template void Print2(U u) u=" << u  << endl;
				}
			};

			template<>
			void Template<string>::Print(string u)
			{
				cout << "template<> Template void Print2(string u) u=" << u << endl;
			}

			template<typename U>
			Template<U>:: Template()
			{
				cout << "template<U> class Template Template()" << endl;
			}

			template<>
			class  Template<int>
			{
			public:
				Template() 
				{
					cout << "template<> class Template<int> Template()" << endl;
				}
				template<typename V>
				void Print(int u, V v);

			};

			template<typename V>
			inline void Template<int>::Print(int u, V v)
			{
				cout << "template<> Template<int> template<typename V> void Print(U u, V v) u=" << u << "  v =" << v << endl;
			}


			template<typename T,template<typename> typename U>
			class Templat2
			{
			public:			
				U<T>& getContainer();
				void Print()
				{
					for (auto var : Container)
						cout << var << endl;
				}

			private:
				U<T> Container;
			};

			template<typename T, template<typename> typename U>
			U<T>& Templat2<T, U>::getContainer()
			{
				return Container;
			}

			void func1()
			{
				cout << "==========non-type Template ======begin" << endl;

				Matrix<1, 2> m12;
				Matrix<2, 3> m23; /* ... */
				//auto m13 = m12 * m23; // OK
				//auto mX = m23 * m13;  // Compile Error!

				cout << "==========non-type Template ======end" << endl;
			}

			void func2()
			{
				cout << "==========type Template ======begin" << endl;
				cout << "==========VariableParameter ======begin" << endl;

				Template<string> tem1;
				tem1.Print<string>("1", "tem1");
				tem1.Print("tem1");
				Template<int> tem2;
				tem2.Print<string>(2,"tem2");
				Template<string> tem3;
				tem1.Print<int>("tem3", 3);

				Templat2<int, vector> tem;
				tem.getContainer().push_back(1);
				tem.Print();

                basicknowledge::Template::useIs::func();
                basicknowledge::Template::useHas::func();
                basicknowledge::Template::useTuple::func();
                cout << "==========VariableParameter ======end" << endl;
				cout << "==========type Template ======end" << endl;
			}

			void func()
			{
				cout << "==========class Template ======begin" << endl;
				func1();
				func2();
				cout << "==========class Template ======end" << endl;
			}
}
	}
}
