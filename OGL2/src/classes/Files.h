/*=[ Файлы: ]===================================================================================\\
||	1) Конфигурационный файл
||	2) Лог файл
||
\\==============================================================================================*/
#pragma once

//#pragma warning(disable:4996)
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <string.h>
#include <windows.h>

void CleanString(char *s);						// Очистка строк от табов и лишних пробелов (и подобной муры)

class File 
{
protected:
	unsigned int FilenameLenght;				// Длина строки с именем файла (для удаления)
	FILE*		f;								// Указатель на идентификатор файла
    char		type;							// Тип файла		(t|b)
	char		mode;							// Способ открытия	(r|w|a)
	bool		Extended;						// Аналог + для open
	char		modestr[4];						// Строка содержащая тип файла и способ открытия
	bool		isOpen;							// Определяет, открыт ли файл
public:
	File		(void);							// Конструктор по умолчанию
	File		(	char*	Fname,			  	// Конструктор с указанием нужных данных
					char	Omode	=	'r',	//		Файл автоматически открывается
					char	Ftype	=	't',		
					bool	isExtended = true);
	~File		(void);							// Деструктор стандартный

	bool Open	(	char*	Fname,				// Открытие файла
					char	Omode	=	'r',
					char	Ftype	=	't',
					bool	isExtended = true);
	void Close	( void );						// Закрытие файла	
};

/*=[ Файлы: ]===================================================================================\\
||	Логит в виде:
||	Заголовок: <Message>: Session started at hh:mm:ss dd.mm.yyyy
||	(hh:mm:ss)	<<Block>>::<Message>
||	Конец - 2 варианта - нормальный и ненормальный
||	1) нормальный - <Message>: Session ended at hh:mm:ss dd.mm.yyyy
||	2) ненормальный - <Message>: Fatal error at hh:mm:ss dd.mm.yyyy
\\==============================================================================================*/

#define FJC_NEED_TIME	0x0001
#define FJC_NEED_DATE	0x0002
//#define MAX_TIME_PRESISION		// Определяет надоли указывать доли секунды в логе (не работает пока)

void FJCGetTD(char mode,char* str);				// Находим время и/или дату для вывода в файл

class LogFile: public File
{
// Класс мой и я не собираюсь его мучать особыми проверками на ошибки поскольку надеюсь их не делать :=)
protected:
	char	msg [256];							// Сообщение начальное
	char	buf	[256];							// Буфер для поготовки сообщений в лог
	char	buf2[256];							// 2й Буфер для поготовки сообщений в лог
	unsigned int msgLength;						// Длина сообщения (для удаления)
public:
	bool	Start	(char* message);			// Начало лога
	bool	Logf	(char* block, char* str, ...);						// Форматированая запись в лог
	bool	Log		(char* block, char* msg, bool ok = true);			// Запись в лог
	bool	Msg		(char* msg);				// Просто сообщение без указания времени
	bool	End		(bool is_normal);			// Конец лога
};

class CFGFile: public File
{
protected:
public:
	char*	Gets	(char* string);				// Вытаскивает построчно данные из файла (комментарии пропускаются)
};
