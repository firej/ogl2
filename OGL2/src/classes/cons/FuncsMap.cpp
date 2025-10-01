/*==============================================================================================\\
|| Этот файл содержит данные необходимые для создания соответствий м/у символьными именами и    ||
|| реальными функциями.                                                                         ||
||  Функции должны иметь параметр string                                                        ||
||                                                                           ___________________||
|| copyright FJ ART, 2005                                                    |created 10.09.2005||
\\==============================================================================================*/

#include "FuncsMap.h"

ConsoleFuncsMap::ConsoleFuncsMap() {};
ConFunc ConsoleFuncsMap::GetFunc(string Name) {
    if (MMap.find(Name) != MMap.end()) {
        return MMap[Name];
    }
    return NULL;
};
bool ConsoleFuncsMap::AddFunc(string Name, ConFunc Func) {
    if (MMap.find(Name) == MMap.end())  // Если такого элемента ещё небыло
    {
        MMap[Name] = Func;  // Добавление в словарь
    } else {
        return false;
    }
    return true;
};

/*
bool	ConsoleFuncsMap::DelFunc (string Name, ConFunc Func)
{
    if (MMap.find(Name)!=MMap.end()) // Если такой элемент уже есть
    {
        MMap.erase(Name); // Удаление из словаря
    } else {return false;}
    return true;
}

int		ConsoleFuncsMap::GetQuantity(void)
{
    int Q;
    ConFuncsMap::iterator i;
    for (Q=0,i=MMap.begin();i!=MMap.end();i++,Q++){};
    return Q;
}
*/
ConsoleFuncsMap::~ConsoleFuncsMap() {};
