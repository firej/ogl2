#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux includes
#include <string>
#include <OpenGL/gl.h>
using namespace std;
#endif

#include "./ConsoleFunctions.h"
#include "./Console.h"
#include "./Time.h"
//mmmmmmmmmmmmmmmmmmmm Здесь перечислены функции которые можно вызывать из консоли
void	Exit			(string	s);		// Выход
void	FPS				(string	s);		// Показывает текучее значение FPS
void	CLS				(string	s);		// Очистка экрана консоли
void	Echo			(string	s);		// Повторить вывод в консоль
void	Lighting		(string	s);		// Управление освещением
void	Lighting2Side	(string	s);
void	DropHistory		(string s);

void	INIT_CONSOLE_FUNCTIONS	()
{
	//MainCon->OUT_STRING(CONCOL_HELP,"Консоль заинитилась :)");

	MainCon->RegFunc("exit",					&Exit);
	MainCon->RegFunc("quit",					&Exit);
	MainCon->RegFunc("quti",					&Exit);
	MainCon->RegFunc("fps",						&FPS);
	MainCon->RegFunc("cls",						&CLS);
	MainCon->RegFunc("echo",					&Echo);
	MainCon->RegFunc("Lighting",				&Lighting);
	MainCon->RegFunc("Lighting2Side",			&Lighting2Side);
	MainCon->RegFunc("DropCommandHistory",		&DropHistory);
}

void	DropHistory	(string s)
{
	if (upcase_string(s) == "HELP")
	{
		MainCon->OUT_STRING(CONCOL_HELP,"Очистка истории команд");
		return;
	}
	MainCon->DropHistory();
}

void	Exit	(string	s)
{
	if (upcase_string(s) == "HELP")
	{
		MainCon->OUT_STRING(CONCOL_HELP,"Выход");
		return;
	}
	Globals.Exiting	=	true;
}

void	FPS		(string	s)
{
	if (upcase_string(s) == "HELP")
	{
		MainCon->OUT_STRING(CONCOL_HELP,"Вывод текущего количества кадров в секунду");
		return;
	}
	MainCon->OUT_STRING(CONCOL_HELP,"FPS: %5.1lf",GT.GetFPS());
}

void	CLS		(string	s)
{
	if (upcase_string(s) == "HELP")
	{
		MainCon->OUT_STRING(CONCOL_HELP,"Очистка экрана");
		return;
	}
	for ( DWORD i = 0 ; i < 80 ; i++ )
		MainCon->OUT_STRING(CONCOL_HELP,"");
}

void	Echo	(string	s)
{
	if (upcase_string(s) == "HELP")
	{
		MainCon->OUT_STRING(CONCOL_HELP,"Дублирование вывода в консоль");
		return;
	}
	if (s.empty())	return;
	MainCon->OUT_STRING(CONCOL_HELP,"%s",s.c_str());
}

void	Lighting	(string	s)
{
	s = upcase_string(s);
	if ( s == string ("TRUE") || s == string ("ON") || s == string ("1") )
	{
		glEnable(GL_LIGHTING);
		MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение включено");
	}
	else if ( s == string ("FALSE") || s == string ("OFF") || s == string ("0") )
	{
		glDisable(GL_LIGHTING);
		MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение выключено");
	}
	else
	{
		MainCon->OUT_STRING(CONCOL_HELP,"Вывод или изменение состояния освещения");
		MainCon->OUT_STRING(CONCOL_HELP,"Использование - lighting [true|ON|1|false|OFF|0]");
		if (glIsEnabled(GL_LIGHTING))
		{
			MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение включено");
		}
		else
		{
			MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение выключено");
		}
	}
}

void	Lighting2Side	(string	s)
{
	s = upcase_string(s);
	if ( s == string ("TRUE") || s == string ("ON") || s == string ("1") )
	{
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,1.0);
		MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение c 2х сторон включено");
	}
	else if ( s == string ("FALSE") || s == string ("OFF") || s == string ("0") )
	{
		glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,0.0);
		MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение c 2х сторон выключено");
	}
	else
	{
		MainCon->OUT_STRING(CONCOL_HELP,"Вывод или изменение состояния освещения (1 или 2 стороны полигонов)");
		MainCon->OUT_STRING(CONCOL_HELP,"Использование - Lighting2Side [true|ON|1|false|OFF|0]");
		GLboolean boolean;
		glGetBooleanv(GL_LIGHT_MODEL_TWO_SIDE,&boolean);
		if (boolean)
		{
			MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение c 2х сторон включено");
		}
		else
		{
			MainCon->OUT_STRING(CONCOL_MESSAGE,"Освещение c 2х сторон выключено");
		}
	}
}