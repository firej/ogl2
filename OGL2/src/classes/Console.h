#pragma once
#include "./globals.h"
#include "./Text.h"
//#include "./ApplicationClass.h"
#include <string>
#include <vector>
#include <map>
#include <list>
using namespace std;

#define		CONCOL_DEFAULT		0
#define		CONCOL_HELP			1
#define		CONCOL_WARNING		2
#define		CONCOL_ERROR		3
#define		CONCOL_CURSOR		4
#define		CONCOL_BACKGROUND	5
#define		CONCOL_INPUTLINE	6
#define		CONCOL_OK			7
#define		CONCOL_MESSAGE		7

string	upcase_string	(	string	s	);
char	upcase			(	char	a	);

namespace	Console
{
	enum	Status
	{
		ST_VISIBLE			=	0,
		ST_INVISIBLE		=	1,
		ST_FADEIN			=	2,
		ST_FADEOUT			=	3
	};
	enum	FadeEffect						// Различные эффекты появления/исчезновения консоли
	{
		ALFA				=	0			// Эффект алфа-проявления
	};
	struct	ConString
	{
		//BYTE		chars;		// Количество использованных символов (для переноса)
		BYTE		c;
		string		s;			// Строка
	};
	struct	ListElement
	{
		ListElement	*	next;	// Указатель на следующий элемент списка
		ConString		data;	// Данные
	};

	enum	VarType					// Перечисление возможных типов переменных
	{
		VT_BYTE			=	14,
		VT_WORD			=	0,
		VT_DWORD		=	1,
		VT_QWORD		=	2,
		VT_CHAR			=	9,
		VT_INT16		=	3,
		VT_INT32		=	4,
		VT_INT64		=	5,
		VT_FLOAT32		=	6,
		VT_FLOAT64		=	7,
		VT_BOOLEAN		=	8,
		VT_STRING		=	10,
		VT_POINT2f		=	11,
		VT_POINT3f		=	12,
		VT_COLOR3f		=	12,
		VT_COLOR4f		=	13
	};
}

typedef void (*ConFunc)(string);					// Тип указателя на косольную функцию
typedef map <string,ConFunc,less<string>>	ConFuncsMap;		// Словарь для сопоставления имён и функций
typedef map <string,ConFunc,less<string>>::iterator	iCFuncsMap;	// Словарь для сопоставления имён и функций
struct	Var_t
{
	Console::VarType	type;
	void			*	var;
	BYTE				maxsize;
};
typedef map <string,Var_t>				ConVarsMap;	// Словарь для сопоставления имён и переменных
typedef	map <string,Var_t>::iterator	iCVarsMap;	// Словарь для сопоставления имён и переменных

class CConsole
{
//private:
protected:
	//BYTE			CurrSubString;					// Номер
	Console::ListElement*	list;					// Указатель на список строк консоли
									// (по совместительству указатель на текущую редактируемую строку)
	DWORD					chars;					// Количество букв в текущей строке
	DWORD					cursor;					// Позиция курсора в строке
	DWORD					ListElements;			// Количество элементов списка
	Point3f					Colors[64];				// Массив предопределённых цветов
	PFont					ConsoleFont;			// Указатель на консольный шрифт
	Console::Status			State;					// Состояние движения консоли
	double					persent;				// Проценты появления/исчезновения консоли
	double					pers_step;				// Шаг изменения состояния консоли
	double					transparency;			// Максимальная прозрачность (при максимальной видимости)
	double					height;					// Процент занимаемого на экране места
	double					lastTick;				// Послдее время курсора
	double					Period;					// Период мигания курсора
	bool					CurVisible;				// Видим ли курсор
	string					Commands[60];			// Список команд для повторения
	DWORD					CurCommand;				// Текущая выбранная команда для повторения
	unsigned short			iAComp;					// Количество слов для вывода в автодополнении
	std::list<string>			CommandsHistory;		// История введённых команд
	std::list<string>::iterator	iCndHistory;			// Итератор в истории
public:
	CConsole			(): ListElements(0),list(0)
	{}
	void	SetColors	(	Point3f	C_Def, Point3f C_Help, Point3f C_Warn, Point3f C_Err)
	{	Colors[0] = C_Def; Colors[1] = C_Help; Colors[2] = C_Warn; Colors[3] = C_Err;	}
	void	SetFont		(PFont	CFont)
	{	ConsoleFont	=	CFont;	}		// Выбор консольного шрифта	
	void	INIT		();
	void	DEINIT		();
	void	ProcessChar	(char	c);
	void	ProcessKey	(DWORD	key);
	void	OUT_STRING	(BYTE	c, const char *fmt, ...);
	void	Draw		();
	void	Show		();
	void	Hide		();
	void	ChangeState	();
	bool	Visible		();
// Работа с командами
private:
	ConFuncsMap			CFmap;		// Карта функций консоли
public:
	void	DoString	(string	s);					// Выполнение строки, введённой пользователем
	string	FindNextSym	(string	s);					// Найти следующий за этим символ в карте
	void	RegFunc		(string	s,ConFunc cf);		// Регистрация функции для возможности вызова
	void	RegFunc		(char*	s,ConFunc cf)
	{	RegFunc(string(s),cf);				}
	void	UnRegFunc	(string	s);					// Убирать регистрацию функции
	/*void	RegVar		(string	s,Console::VarType	type,void*	var,BYTE	maxsize);
	void	RegVar		(char*	s,Console::VarType	type,void*	var,BYTE	maxsize)
	{	RegVar		(string(s),type,var,maxsize);	}
	void	UnRegVar	(string	s);					// Убирать регистрацию переменной*/
	ConFuncsMap*	GetMap	(void)
	{	return &CFmap;	}
	void	DropHistory	(	void	);				// Очистка истории команд
};

extern CConsole	*	MainCon;

//void	Console_Set			(string	s);	// Установить значение зарегистрированной переменной
//void	Console_Get			(string	s);	// Вывести в консоль значение некоторой переменной
void	Console_Help		(string	s);	// Вывести справку вообще или по команде