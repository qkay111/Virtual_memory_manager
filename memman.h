// memman.h

#pragma once

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

namespace memMan
{
	class MemMan;
	class ManPtr;

	class MemMan
	{
	public:
		struct ptrTable
		{
			void * ptr;
			vector<ManPtr*> mPtr;
			unsigned int counter;
		};
	private:
		static void * systemMemory;
		static vector<ptrTable> table;
		static unsigned int memoryQuantity;
		static unsigned int freeMemory;
		MemMan() {};
		static void addLoc(ManPtr *);
		static void delLoc(ManPtr *);
		static unsigned int pointers(void *);
		static void changeAll(void *, ManPtr *);
		static void gc(void *);
		static void flush();
	public:
		static bool allocation(ManPtr&, unsigned int);
		static bool reallocation(ManPtr&, unsigned int);
		static bool freeAlloc(ManPtr&);
		friend class ManPtr;
	};

	class ManPtr
	{
	private:
		void * ptr = nullptr;
		void setPtr(void *pointer) { ptr = pointer; }
	public:
		void * getPtr() { return ptr; }
		ManPtr& operator=(ManPtr& ptr) {
			if (this == &ptr || this->getPtr() == ptr.getPtr())	// Проверка на самоприсваивание
				return *this;
			if (this->getPtr() != ptr.getPtr() && this->getPtr() == nullptr)
			{
				this->setPtr(ptr.getPtr());
				MemMan::addLoc(this);
			}
			else if (this->getPtr() != ptr.getPtr() && ptr.getPtr() == nullptr)
			{
				MemMan::delLoc(this);
				this->setPtr(nullptr);
			}
			else if (this->getPtr() != ptr.getPtr())
			{
				MemMan::delLoc(this);
				this->setPtr(ptr.getPtr());
				MemMan::addLoc(this);
			}
			return *this;
		}
		~ManPtr() { MemMan::delLoc(this); }
		friend class MemMan;
	};
};
