// memman.cpp

#include "memman.h"

#define STEP_SYSTEM_SIZE 4096

using namespace memMan;

void * MemMan::systemMemory;
unsigned int MemMan::memoryQuantity;
unsigned int MemMan::freeMemory;
vector<MemMan::ptrTable> MemMan::table;

void MemMan::addLoc(ManPtr *ptr)
{
	if (table.size() == 0)
	{
		MemMan::ptrTable tmp;
		tmp.ptr = ptr->getPtr();
		tmp.mPtr.push_back(ptr);
		tmp.counter = 1;
		table.push_back(tmp);
	}
	else
	{
		//for (MemMan::ptrTable tmp : table)	// ���� �� ���� ����� ��� ���-�� ���������
		//{
		//	if (tmp.ptr == ptr->getPtr())
		//	{
		//		tmp.counter++;
		//		tmp.mPtr.push_back(ptr);
		//		return;
		//	}
		//}
		for (int i = 0; i < table.size(); i++)	// ���� �� ���� ����� ��� ���-�� ���������
		{
			if (table[i].ptr == ptr->getPtr())
			{
				table[i].counter++;
				table[i].mPtr.push_back(ptr);
				return;
			}
		}

		MemMan::ptrTable tmp;	// ���� �� ���� ����� ��� ����� �� ��������
		tmp.ptr = ptr->getPtr();
		tmp.mPtr.push_back(ptr);
		tmp.counter = 1;
		table.push_back(tmp);
	}
}

void MemMan::delLoc(ManPtr *ptr)
{
	if (ptr->getPtr() == nullptr)
		return;
	vector<MemMan::ptrTable>::iterator it = table.begin();
	vector<ManPtr*>::iterator mIt;
	//for (MemMan::ptrTable tmp : table)	// ����� ������� ������
	//{
	//	if (tmp.ptr == ptr->getPtr())
	//	{
	//		tmp.counter--;
	//		if (tmp.counter == 0)
	//		{
	//			table.erase(it);
	//			MemMan::freeAlloc(*ptr);
	//			if (table.size() == 0)	// ���� �� �������� ����������
	//				MemMan::flush();
	//			return;
	//		}
	//		mIt = tmp.mPtr.begin();
	//		for (ManPtr* mPtr : tmp.mPtr)
	//		{
	//			if (mPtr != ptr)
	//				mIt++;
	//			else
	//				break;
	//		}
	//		tmp.mPtr.erase(mIt);
	//		return;
	//	}
	//	it++;
	//}
	for (int i = 0; i < table.size(); i++)	// ����� ������� ������
	{
		if (table[i].ptr == ptr->getPtr())
		{
			table[i].counter--;
			if (table[i].counter == 0)
			{
				table.erase(it);
				MemMan::freeAlloc(*ptr);
				if (table.size() == 0)	// ���� �� �������� ����������
					MemMan::flush();
				return;
			}
			mIt = table[i].mPtr.begin();
			for (int j = 0; j < table[i].mPtr.size(); j++)
			{
				if (table[i].mPtr[j] != ptr)
					mIt++;
				else
					break;
			}
			table[i].mPtr.erase(mIt);
			return;
		}
		it++;
	}
}

unsigned int MemMan::pointers(void *ptr)
{
	vector<MemMan::ptrTable>::iterator it = table.begin();
	//for (MemMan::ptrTable tmp : table)	// ����� ������� ������
	//{
	//	if (tmp.ptr == ptr)
	//		return tmp.counter;
	//	it++;
	//}
	for (int i = 0; i < table.size(); i++)	// ����� ������� ������
	{
		if (table[i].ptr == ptr)
			return table[i].counter;
		it++;
	}
	return 0;
}

void MemMan::changeAll(void *ptr, ManPtr *mPtr)
{
	vector<MemMan::ptrTable>::iterator it = table.begin();
	vector<ManPtr*>::iterator mIt;
	for (int i = 0; i < table.size(); i++)	// ����� ������� ������
	{
		if (table[i].ptr == mPtr->getPtr())
		{
			table[i].ptr = ptr;
			mIt = table[i].mPtr.begin();
			for (int j = 0; j < table[i].mPtr.size(); j++)
				table[i].mPtr[j]->setPtr(ptr);
			table.erase(it);
			if (table.size() == 0)	// ���� �� �������� ����������
				MemMan::flush();
			return;
		}
		it++;
	}
}

void MemMan::gc(void *ptr)
{
	vector<MemMan::ptrTable>::iterator it = table.begin();
	vector<ManPtr*>::iterator mIt;
	//for (MemMan::ptrTable tmp : table)	// ����� ������� ������
	//{
	//	if (tmp.ptr == ptr)
	//	{
	//		mIt = tmp.mPtr.begin();
	//		for (ManPtr* mPtr : tmp.mPtr)
	//			mPtr->setPtr(nullptr);
	//		table.erase(it);
	//		if (table.size() == 0)	// ���� �� �������� ����������
	//			MemMan::flush();
	//		return;
	//	}
	//	it++;
	//}
	for (int i = 0; i < table.size(); i++)	// ����� ������� ������
	{
		if (table[i].ptr == ptr)
		{
			mIt = table[i].mPtr.begin();
			for (int j = 0; j < table[i].mPtr.size(); j++)
				table[i].mPtr[j]->setPtr(nullptr);
			table.erase(it);
			if (table.size() == 0)	// ���� �� �������� ����������
				MemMan::flush();
			return;
		}
		it++;
	}
}

void MemMan::flush()
{
	cout << "All memory was free" << endl;
	free(systemMemory);
	systemMemory = nullptr;
	memoryQuantity = 0;
	freeMemory = 0;
}

bool MemMan::allocation(ManPtr& ptr, unsigned int len)
{
	if (len == 0)
		return false;
	bool flag = false;
	unsigned int index = 0;
	if (ptr.getPtr() != nullptr)	// ���� ���� ��������� ��� �� ���-�� ���������
		if (MemMan::pointers(ptr.getPtr()) == 1)
			MemMan::freeAlloc(ptr);
		else
			MemMan::delLoc(&ptr);
	if (systemMemory == nullptr)	// ���� ������ ��� �� ����������
	{
		cout << "System memory were allocated" << endl;
		memoryQuantity = freeMemory = STEP_SYSTEM_SIZE;
		systemMemory = malloc(memoryQuantity);
		for (int i = 0; i < memoryQuantity; i++)
			((char*)systemMemory)[i] = 0;
	}
	while (freeMemory + 5 < len)	// ���� ��������� ������ ������������
	{
		systemMemory = realloc(systemMemory, memoryQuantity * 2);
		for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
			((char*)systemMemory)[i] = 0;
		freeMemory += memoryQuantity;
		memoryQuantity *= 2;
	}
	for (int i = 0; i < memoryQuantity; i++)	// ����� ���������� ������� ������
	{
		if (((char*)systemMemory)[i] == 0)	// ���� ������� ��������� ������
		{
			for (int j = i; j <= len + 4; j++)	// �������� �� ������������� ��������� ��������� ������
				if (((char*)systemMemory)[i] != 0)	// ���� ������ ������������
				{
					flag = true;
					break;
				}
		}
		else    // ���� ������� ��� �����, �� ������������ �� ��������� �������
		{
			unsigned int tmpLen;
			i++;
			tmpLen = *(unsigned int*)((char*)systemMemory + i);
			i += (4 + tmpLen - 1);
			if (i + 1 >= memoryQuantity)	// ���� �� ������� ���������� ������� ������
			{
				systemMemory = realloc(systemMemory, memoryQuantity * 2);
				for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
					((char*)systemMemory)[i] = 0;
				freeMemory += memoryQuantity;
				memoryQuantity *= 2;
			}
			continue;
		}
		if (flag)	// ���� ������ ������������
		{
			flag = false;
			unsigned int tmpLen;
			i++;
			tmpLen = *(unsigned int*)((char*)systemMemory + i);
			i += (4 + tmpLen - 1);
			if (i + 1 >= memoryQuantity)	// ���� �� ������� ���������� ������� ������
			{
				systemMemory = realloc(systemMemory, memoryQuantity * 2);
				for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
					((char*)systemMemory)[i] = 0;
				freeMemory += memoryQuantity;
				memoryQuantity *= 2;
			}
			continue;
		}
		((char*)systemMemory)[i++] = 1;	// �������������� ������
		*(unsigned int*)((char*)systemMemory + i) = len;	// ������� ������
		index = i + 4;	// ����������� ���������� �������
		break;
	}
	freeMemory -= len + 5;	// ��������� ���������� ��������� ������
	cout << "Allocated address is: " << (void*)((char*)systemMemory + index) << endl;
	ptr.setPtr((char*)systemMemory + index);	// ������� ��������� �� ���������� ������
	MemMan::addLoc(&ptr);	// ������� � �������
	return true;
}

bool MemMan::reallocation(ManPtr& ptr, unsigned int newLen)
{
	if (newLen == 0)
		return false;
	char *pointer = (char*)ptr.getPtr();
	if (pointer == nullptr || pointer < systemMemory || pointer >((char*)systemMemory + memoryQuantity))	// ���� ��������� ��������� �� ������ ������� ������
		return false;
	unsigned int len;
	pointer -= 4;
	len = *(unsigned int*)pointer;	// ��������� ����� ���������� ������
	if (len > newLen)	// ���� �������������� ����� ����� ������ ������
	{
		unsigned int diff = len - newLen;	// ������� ������� � ������
		*(unsigned int*)pointer = newLen;	// ��������� �����
		pointer += newLen;	// ��������� �� ������ ������� ������
		for (int i = 0; i < diff; i++)	// ������� ������ ������� ������
			pointer[i] = 0;
		cout << "Memory was updated" << endl;
		return true;
	}
	else if (len < newLen)	// ���� �������������� ����� ����� ������ ������
	{
		bool flag = false;
		unsigned int diff = newLen - len;	// ������� ������� � ������
		if (pointer[len - 1])	// ���� ��������� �������� ������ ��������, ��������� ��� �����
		{
			char *tmp = pointer + len - 1;
			for (int i = 0; i < diff; i++)
				if (tmp[i] != 0)	// ���� ������ ������������
				{
					flag = true;
					break;
				}
			if (flag)	// ���� ��������� ��������� ������ ������������, ���� �����, ������� ������
			{
				bool flag = false;
				unsigned int index = 0;
				for (int i = 0; i < memoryQuantity; i++)	// ����� ���������� ������� ������
				{
					if (((char*)systemMemory)[i] == 0)	// ���� ������� ��������� ������
					{
						for (int j = i; j <= newLen + 4; j++)	// �������� �� ������������� ��������� ��������� ������
							if (((char*)systemMemory)[i] != 0)	// ���� ������ ������������
							{
								flag = true;
								break;
							}
					}
					if (flag)	// ���� ������ ������������
					{
						flag = false;
						i += newLen + 4;
						if (i + 1 >= memoryQuantity)	// ���� �� ������� ���������� ������� ������
						{
							systemMemory = realloc(systemMemory, memoryQuantity * 2);
							for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
								((char*)systemMemory)[i] = 0;
							freeMemory += memoryQuantity;
							memoryQuantity *= 2;
						}
						continue;
					}
					((char*)systemMemory)[i++] = 1;	// �������������� ������
					*(unsigned int*)((char*)systemMemory + i) = len;	// ������� ������
					index = i + 4;	// ����������� ���������� �������
					break;
				}
				for (int i = 0, j = index; i < len; i++, j++)	// �������������� ������� ������
					((char*)systemMemory)[index] = pointer[i + 4];
				for (int i = 0; i < len + 5; i++)	// ����������� ������ ������� ������
					pointer[i - 1] = 0;
				MemMan::changeAll((char*)systemMemory + index, &ptr);
				cout << "Memory was reallocated from: " << ptr.getPtr() << " to: " << (void*)((char*)systemMemory + index) << endl;
				freeMemory -= (newLen - len);	// ��������� ���������� ��������� ������
			}
			else    // ���� ������ ����������, �� ��������� ����������
			{
				*(unsigned int*)pointer = newLen;	// ��������� �����
				for (int i = 0; i < diff; i++)
					tmp[i] = 0;
				cout << "Memory was updated" << endl;
				freeMemory -= (newLen - len);	// ��������� ���������� ��������� ������
			}
		}
		else    // ���� ��������� �������� ������ ����������, ���� �����, ������� ������
		{
			bool flag = false;
			unsigned int index = 0;
			for (int i = 0; i < memoryQuantity; i++)	// ����� ���������� ������� ������
			{
				if (((char*)systemMemory)[i] == 0)	// ���� ������� ��������� ������
				{
					for (int j = i; j <= newLen + 4; j++)	// �������� �� ������������� ��������� ��������� ������
						if (((char*)systemMemory)[i] != 0)	// ���� ������ ������������
						{
							flag = true;
							break;
						}
				}
				if (flag)	// ���� ������ ������������
				{
					flag = false;
					i += newLen + 4;
					if (i + 1 >= memoryQuantity)	// ���� �� ������� ���������� ������� ������
					{
						systemMemory = realloc(systemMemory, memoryQuantity * 2);
						for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
							((char*)systemMemory)[i] = 0;
						freeMemory += memoryQuantity;
						memoryQuantity *= 2;
					}
					continue;
				}
				((char*)systemMemory)[i++] = 1;	// �������������� ������
				*(unsigned int*)((char*)systemMemory + i) = len;	// ������� ������
				index = i + 4;	// ����������� ���������� �������
				break;
			}
			for (int i = 0, j = index; i < len; i++, j++)	// �������������� ������� ������
				((char*)systemMemory)[index] = pointer[i + 4];
			for (int i = 0; i < len + 5; i++)	// ����������� ������ ������� ������
				pointer[i - 1] = 0;
			cout << "Memory was reallocated from: " << ptr.getPtr() << " to: " << (void*)((char*)systemMemory + index) << endl;
			MemMan::changeAll((char*)systemMemory + index, &ptr);
			freeMemory -= (newLen - len);	// ��������� ���������� ��������� ������
		}
	}
	return true;
}

bool MemMan::freeAlloc(ManPtr& ptr)
{
	char *pointer = (char*)ptr.getPtr();
	if (pointer == nullptr || pointer < systemMemory || pointer > ((char*)systemMemory + memoryQuantity))	// ���� ��������� ��������� �� ������ ������� ������
		return false;
	unsigned int len;
	pointer -= 4;
	len = *(unsigned int*)pointer;	// ��������� ����� ���������� ������
	if (len == 0)
		return true;
	pointer--;
	for (int i = pointer - systemMemory; i < (pointer - systemMemory) + len + 5; i++)	// ����������� ���������� ������� ������
		((char*)systemMemory)[i] = 0;
	cout << "Allocated memory: " << ptr.getPtr() << " was free" << endl;
	MemMan::gc(ptr.getPtr());
	freeMemory += len + 5;	// ����������� ���������� ��������� ������
	return true;
}