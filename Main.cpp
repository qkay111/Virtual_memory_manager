// Main.cpp

#include "memman.h"
#include <conio.h>

using namespace memMan;

int main()
{
	ManPtr ptr_1;
	MemMan::allocation(ptr_1, 2);

	ManPtr ptr_2;
	MemMan::allocation(ptr_2, 30);

	ManPtr ptr_3;
	MemMan::allocation(ptr_3, 15);

	MemMan::freeAlloc(ptr_1);
	MemMan::allocation(ptr_2, 15);

	ManPtr ptr_4;
	ptr_4 = ptr_2;
	ManPtr ptr_5;
	ptr_5 = ptr_2;
	cout << "ptr_1: " << ptr_1.getPtr() << endl;
	cout << "ptr_2: " << ptr_2.getPtr() << endl;
	cout << "ptr_3: " << ptr_3.getPtr() << endl;
	cout << "ptr_4: " << ptr_4.getPtr() << endl;
	cout << "ptr_5: " << ptr_5.getPtr() << endl;
	MemMan::freeAlloc(ptr_4);
	cout << endl << endl << "ptr_1: " << ptr_1.getPtr() << endl;
	cout << "ptr_2: " << ptr_2.getPtr() << endl;
	cout << "ptr_3: " << ptr_3.getPtr() << endl;
	cout << "ptr_4: " << ptr_4.getPtr() << endl;
	cout << "ptr_5: " << ptr_5.getPtr() << endl;

	ptr_4 = ptr_5 = ptr_3;
	cout << endl << "ptr_3: " << ptr_3.getPtr() << endl;
	cout << "ptr_4: " << ptr_4.getPtr() << endl;
	cout << "ptr_5: " << ptr_5.getPtr() << endl;
	MemMan::allocation(ptr_4, 50);
	cout << endl << "ptr_3: " << ptr_3.getPtr() << endl;
	cout << "ptr_4: " << ptr_4.getPtr() << endl;
	cout << "ptr_5: " << ptr_5.getPtr() << endl;

	MemMan::reallocation(ptr_3, 3);
	cout << "ptr_1: " << ptr_1.getPtr() << endl;
	cout << "ptr_2: " << ptr_2.getPtr() << endl;
	cout << "ptr_3: " << ptr_3.getPtr() << endl;
	cout << "ptr_4: " << ptr_4.getPtr() << endl;
	cout << "ptr_5: " << ptr_5.getPtr() << endl;

	_getch();
	return 0;
}