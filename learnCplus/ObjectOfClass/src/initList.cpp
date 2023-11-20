#include"initList.h"
basicknowledge::Object::initList::myclass::myclass()
{
	cout << "myclass()" << endl;
}

basicknowledge::Object::initList::myclass::myclass(int i)
{
	cout << "myclass(int i)" << endl;
}

basicknowledge::Object::initList::myclass::myclass(initializer_list<int> v)
{
	cout << "myclass(initializer_list<int> v)" << endl;
}

basicknowledge::Object::initList::myclass& basicknowledge::Object::initList::myclass::operator=(const myclass& var)
{
	cout << "myclass operator=(const myclass& var)" << endl;

	return *this; 
}
