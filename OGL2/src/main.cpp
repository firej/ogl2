/*==============================================================================================\\
|| Main program file for FJ Code 3D Demo                                                        ||
|| File version 0.1.5                                                                           ||
||                                                                                              ||
||    ||        //*****\  //******#  ||      |  //*******                                       ||
||    ||        ||     |  ||         ||      |  ||                                              ||
||    ||        ||     |  ||         ||      |  \\######\      /**** |\  | /***  | |\  | /****  ||
||    ||        ||     |  ||         ||      |          |      |---  | \ | |  __ | | \ | |---   ||
||    \\______  \\_____/  \\_______  \\______/  \\______/      \____ |  \| \___/ | |  \| \____  ||
||                                                                                              ||
|| copyright FJ ART / FJC, 2005 - 2007                                                          ||
\\==============================================================================================*/
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include "LocusAFX.h"
//====== Подключение Header'ов
#include "./MAIN.h"

Application MyApplication;
// Главная функция приложения!
int APIENTRY WinMain( HINSTANCE,
					  HINSTANCE,
					  LPSTR,
					  int)
{
	//  Слежение за утечками памяти
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF );
	MyApplication.Birth();								// Инициализация приложения
	MyApplication.Work();								// Цикл обработки сообщений
	MyApplication.Suicide();							// Убивание приложения
	return 0;
}
#else
// macOS/Linux версия
#include "./classes/ApplicationClass.h"

Application MyApplication;

int main(int argc, char* argv[])
{
	MyApplication.Birth();								// Инициализация приложения
	MyApplication.Work();								// Цикл обработки сообщений
	MyApplication.Suicide();							// Убивание приложения
	return 0;
}
#endif
