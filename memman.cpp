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
		//for (MemMan::ptrTable tmp : table)	// Если на этот адрес уже кто-то ссылается
		//{
		//	if (tmp.ptr == ptr->getPtr())
		//	{
		//		tmp.counter++;
		//		tmp.mPtr.push_back(ptr);
		//		return;
		//	}
		//}
		for (int i = 0; i < table.size(); i++)	// Если на этот адрес уже кто-то ссылается
		{
			if (table[i].ptr == ptr->getPtr())
			{
				table[i].counter++;
				table[i].mPtr.push_back(ptr);
				return;
			}
		}

		MemMan::ptrTable tmp;	// Если на этот адрес еще никто не ссылался
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
	//for (MemMan::ptrTable tmp : table)	// Поиск нужного адреса
	//{
	//	if (tmp.ptr == ptr->getPtr())
	//	{
	//		tmp.counter--;
	//		if (tmp.counter == 0)
	//		{
	//			table.erase(it);
	//			MemMan::freeAlloc(*ptr);
	//			if (table.size() == 0)	// Если не осталось указателей
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
	for (int i = 0; i < table.size(); i++)	// Поиск нужного адреса
	{
		if (table[i].ptr == ptr->getPtr())
		{
			table[i].counter--;
			if (table[i].counter == 0)
			{
				table.erase(it);
				MemMan::freeAlloc(*ptr);
				if (table.size() == 0)	// Если не осталось указателей
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
	//for (MemMan::ptrTable tmp : table)	// Поиск нужного адреса
	//{
	//	if (tmp.ptr == ptr)
	//		return tmp.counter;
	//	it++;
	//}
	for (int i = 0; i < table.size(); i++)	// Поиск нужного адреса
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
	for (int i = 0; i < table.size(); i++)	// Поиск нужного адреса
	{
		if (table[i].ptr == mPtr->getPtr())
		{
			table[i].ptr = ptr;
			mIt = table[i].mPtr.begin();
			for (int j = 0; j < table[i].mPtr.size(); j++)
				table[i].mPtr[j]->setPtr(ptr);
			table.erase(it);
			if (table.size() == 0)	// Если не осталось указателей
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
	//for (MemMan::ptrTable tmp : table)	// Поиск нужного адреса
	//{
	//	if (tmp.ptr == ptr)
	//	{
	//		mIt = tmp.mPtr.begin();
	//		for (ManPtr* mPtr : tmp.mPtr)
	//			mPtr->setPtr(nullptr);
	//		table.erase(it);
	//		if (table.size() == 0)	// Если не осталось указателей
	//			MemMan::flush();
	//		return;
	//	}
	//	it++;
	//}
	for (int i = 0; i < table.size(); i++)	// Поиск нужного адреса
	{
		if (table[i].ptr == ptr)
		{
			mIt = table[i].mPtr.begin();
			for (int j = 0; j < table[i].mPtr.size(); j++)
				table[i].mPtr[j]->setPtr(nullptr);
			table.erase(it);
			if (table.size() == 0)	// Если не осталось указателей
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
	if (ptr.getPtr() != nullptr)	// Если этот указатель уже на что-то указывает
		if (MemMan::pointers(ptr.getPtr()) == 1)
			MemMan::freeAlloc(ptr);
		else
			MemMan::delLoc(&ptr);
	if (systemMemory == nullptr)	// Если память еще не выделялась
	{
		cout << "System memory were allocated" << endl;
		memoryQuantity = freeMemory = STEP_SYSTEM_SIZE;
		systemMemory = malloc(memoryQuantity);
		for (int i = 0; i < memoryQuantity; i++)
			((char*)systemMemory)[i] = 0;
	}
	while (freeMemory + 5 < len)	// Если свободной памяти недостаточно
	{
		systemMemory = realloc(systemMemory, memoryQuantity * 2);
		for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
			((char*)systemMemory)[i] = 0;
		freeMemory += memoryQuantity;
		memoryQuantity *= 2;
	}
	for (int i = 0; i < memoryQuantity; i++)	// Поиск свободного участка памяти
	{
		if (((char*)systemMemory)[i] == 0)	// Если найдена свободная память
		{
			for (int j = i; j <= len + 4; j++)	// Проверка на достаточность найденной свободной памяти
				if (((char*)systemMemory)[i] != 0)	// Если памяти недостаточно
				{
					flag = true;
					break;
				}
		}
		else    // Если участок был занят, то перемещаемся на следующий участок
		{
			unsigned int tmpLen;
			i++;
			tmpLen = *(unsigned int*)((char*)systemMemory + i);
			i += (4 + tmpLen - 1);
			if (i + 1 >= memoryQuantity)	// Если не нашлось доступного участка памяти
			{
				systemMemory = realloc(systemMemory, memoryQuantity * 2);
				for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
					((char*)systemMemory)[i] = 0;
				freeMemory += memoryQuantity;
				memoryQuantity *= 2;
			}
			continue;
		}
		if (flag)	// Если памяти недостаточно
		{
			flag = false;
			unsigned int tmpLen;
			i++;
			tmpLen = *(unsigned int*)((char*)systemMemory + i);
			i += (4 + tmpLen - 1);
			if (i + 1 >= memoryQuantity)	// Если не нашлось доступного участка памяти
			{
				systemMemory = realloc(systemMemory, memoryQuantity * 2);
				for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
					((char*)systemMemory)[i] = 0;
				freeMemory += memoryQuantity;
				memoryQuantity *= 2;
			}
			continue;
		}
		((char*)systemMemory)[i++] = 1;	// Инициализируем память
		*(unsigned int*)((char*)systemMemory + i) = len;	// Заносим размер
		index = i + 4;	// Индексируем выделенный участок
		break;
	}
	freeMemory -= len + 5;	// Уменьшаем количество свободной памяти
	cout << "Allocated address is: " << (void*)((char*)systemMemory + index) << endl;
	ptr.setPtr((char*)systemMemory + index);	// Передаём указатель на выделенную память
	MemMan::addLoc(&ptr);	// Заносим в таблицу
	return true;
}

bool MemMan::reallocation(ManPtr& ptr, unsigned int newLen)
{
	if (newLen == 0)
		return false;
	char *pointer = (char*)ptr.getPtr();
	if (pointer == nullptr || pointer < systemMemory || pointer >((char*)systemMemory + memoryQuantity))	// Если указатель указывает на другую область памяти
		return false;
	unsigned int len;
	pointer -= 4;
	len = *(unsigned int*)pointer;	// Считываем длину выделенной памяти
	if (len > newLen)	// Если перевыделяемая новая длина меньше старой
	{
		unsigned int diff = len - newLen;	// Находим разницу в памяти
		*(unsigned int*)pointer = newLen;	// Обновляем длину
		pointer += newLen;	// Переходим на старый участок памяти
		for (int i = 0; i < diff; i++)	// Очищаем старый участок памяти
			pointer[i] = 0;
		cout << "Memory was updated" << endl;
		return true;
	}
	else if (len < newLen)	// Если перевыделяемая новая длина больше старой
	{
		bool flag = false;
		unsigned int diff = newLen - len;	// Находим разницу в памяти
		if (pointer[len - 1])	// Если следующий фрагмент памяти доступен, проверяем его длину
		{
			char *tmp = pointer + len - 1;
			for (int i = 0; i < diff; i++)
				if (tmp[i] != 0)	// Если памяти недостаточно
				{
					flag = true;
					break;
				}
			if (flag)	// Если следующий фрагмента памяти недостаточно, ищем новый, очищаем старый
			{
				bool flag = false;
				unsigned int index = 0;
				for (int i = 0; i < memoryQuantity; i++)	// Поиск свободного участка памяти
				{
					if (((char*)systemMemory)[i] == 0)	// Если найдена свободная память
					{
						for (int j = i; j <= newLen + 4; j++)	// Проверка на достаточность найденной свободной памяти
							if (((char*)systemMemory)[i] != 0)	// Если памяти недостаточно
							{
								flag = true;
								break;
							}
					}
					if (flag)	// Если памяти недостаточно
					{
						flag = false;
						i += newLen + 4;
						if (i + 1 >= memoryQuantity)	// Если не нашлось доступного участка памяти
						{
							systemMemory = realloc(systemMemory, memoryQuantity * 2);
							for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
								((char*)systemMemory)[i] = 0;
							freeMemory += memoryQuantity;
							memoryQuantity *= 2;
						}
						continue;
					}
					((char*)systemMemory)[i++] = 1;	// Инициализируем память
					*(unsigned int*)((char*)systemMemory + i) = len;	// Заносим размер
					index = i + 4;	// Индексируем выделенный участок
					break;
				}
				for (int i = 0, j = index; i < len; i++, j++)	// Перезаписываем участок памяти
					((char*)systemMemory)[index] = pointer[i + 4];
				for (int i = 0; i < len + 5; i++)	// Освобождаем старый участок памяти
					pointer[i - 1] = 0;
				MemMan::changeAll((char*)systemMemory + index, &ptr);
				cout << "Memory was reallocated from: " << ptr.getPtr() << " to: " << (void*)((char*)systemMemory + index) << endl;
				freeMemory -= (newLen - len);	// Уменьшаем количество свободной памяти
			}
			else    // Если памяти достаточно, то обновляем количество
			{
				*(unsigned int*)pointer = newLen;	// Обновляем длину
				for (int i = 0; i < diff; i++)
					tmp[i] = 0;
				cout << "Memory was updated" << endl;
				freeMemory -= (newLen - len);	// Уменьшаем количество свободной памяти
			}
		}
		else    // Если следующий фрагмент памяти недоступен, ищем новый, очищаем старый
		{
			bool flag = false;
			unsigned int index = 0;
			for (int i = 0; i < memoryQuantity; i++)	// Поиск свободного участка памяти
			{
				if (((char*)systemMemory)[i] == 0)	// Если найдена свободная память
				{
					for (int j = i; j <= newLen + 4; j++)	// Проверка на достаточность найденной свободной памяти
						if (((char*)systemMemory)[i] != 0)	// Если памяти недостаточно
						{
							flag = true;
							break;
						}
				}
				if (flag)	// Если памяти недостаточно
				{
					flag = false;
					i += newLen + 4;
					if (i + 1 >= memoryQuantity)	// Если не нашлось доступного участка памяти
					{
						systemMemory = realloc(systemMemory, memoryQuantity * 2);
						for (int i = memoryQuantity; i < memoryQuantity * 2; i++)
							((char*)systemMemory)[i] = 0;
						freeMemory += memoryQuantity;
						memoryQuantity *= 2;
					}
					continue;
				}
				((char*)systemMemory)[i++] = 1;	// Инициализируем память
				*(unsigned int*)((char*)systemMemory + i) = len;	// Заносим размер
				index = i + 4;	// Индексируем выделенный участок
				break;
			}
			for (int i = 0, j = index; i < len; i++, j++)	// Перезаписываем участок памяти
				((char*)systemMemory)[index] = pointer[i + 4];
			for (int i = 0; i < len + 5; i++)	// Освобождаем старый участок памяти
				pointer[i - 1] = 0;
			cout << "Memory was reallocated from: " << ptr.getPtr() << " to: " << (void*)((char*)systemMemory + index) << endl;
			MemMan::changeAll((char*)systemMemory + index, &ptr);
			freeMemory -= (newLen - len);	// Уменьшаем количество свободной памяти
		}
	}
	return true;
}

bool MemMan::freeAlloc(ManPtr& ptr)
{
	char *pointer = (char*)ptr.getPtr();
	if (pointer == nullptr || pointer < systemMemory || pointer > ((char*)systemMemory + memoryQuantity))	// Если указатель указывает на другую область памяти
		return false;
	unsigned int len;
	pointer -= 4;
	len = *(unsigned int*)pointer;	// Считываем длину выделенной памяти
	if (len == 0)
		return true;
	pointer--;
	for (int i = pointer - systemMemory; i < (pointer - systemMemory) + len + 5; i++)	// Освобождаем выделенный участок памяти
		((char*)systemMemory)[i] = 0;
	cout << "Allocated memory: " << ptr.getPtr() << " was free" << endl;
	MemMan::gc(ptr.getPtr());
	freeMemory += len + 5;	// Увеличиваем количество свободной памяти
	return true;
}