/*==============================================================================================\\
|| Этот файл содержит данные необходимые для создания соответствий м/у символьными именами и    ||
|| реальными функциями                                                                          ||
||  Функции должны иметь параметр string                                                        ||
||                                                                           ___________________||
|| copyright FJ ART, 2005                                                    |created 10.09.2005||
\\==============================================================================================*/

#pragma once

#include <string>
#include <map>

using namespace std;

typedef void (*ConFunc)(string);							// Тип указателя на косольную функцию
typedef map <string,ConFunc,less <string> > ConFuncsMap;	// Словарь для сопоставления имён и функций

class ConsoleFuncsMap							// Класс для идентификации функций по символьным именам
{
private:
	ConFuncsMap MMap;										// Словарь содержащий данные
	ConFuncsMap::iterator i;								// Итератор для путешествий по словарю

public:
	ConsoleFuncsMap();										// Конструктор по-умолчанию
	~ConsoleFuncsMap();										// Деструктор по-умолчанию
	ConFunc GetFunc	(string Name);							// Поиск функции по имени
	bool AddFunc	(string Name, ConFunc Func);			// Добавление функции в словарь
	bool AddVar		(string Name, void * p);				// Добавление переменной в словарь
//	bool DelFunc	(string Name, ConFunc Func=NULL);		// Удаление из словаря
//	int GetQuantity	(void);									// Можно узнать количество функций
	bool Exec		(string Name);
};