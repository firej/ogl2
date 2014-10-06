#include "LocusAFX.h"
#include "./Files.h"
#include "./shared/ASSERT.h"

// Реализация базового класса File
File::File()
{
};

File::File(	char* Fname,
			char  Omode,
			char  Ftype,
			bool  isExtended)
{
	Open(Fname,Omode,Ftype,isExtended);
};

bool File::Open(	char* Fname,
					char  Omode,
					char  Ftype,
					bool  isExtended)
{
	memset (modestr,0,4);
	modestr [0] = Omode;
	if (isExtended)	{						// Проверка нужен ли + в fopen
						modestr [1] = '+';
						modestr [2] = Ftype;
					}
			else	{
						modestr	[1] = Ftype;
					}
	isOpen = true;
	f = fopen(Fname,modestr);
	FASSERT(f);
//	return (bool)f;
	return true;   // не имеет смысла из-за FASSERT'а выше
};

void File::Close()
{
//	FASSERT(f);
	if (f) fclose(f);
};
File::~File()
{
	if (f) fclose(f);
}
/*=[ Файлы: Реализация: ]=======================================================================\\
||	1) Лог файл
||	2) Конфигурационный файл
||			  
\\==============================================================================================*/

void FJCGetTD (char mode,char* str)						// Находим время для вывода в файл
{	// mode = 0 - нужна дата без времени
	SYSTEMTIME LocalTime;
	GetLocalTime(&LocalTime);
    if (mode & FJC_NEED_TIME)	// Нужно время
			if (mode & FJC_NEED_DATE)	// Нужна дата
					sprintf(	str,
								"(%02d:%02d:%02d) %02d.%02d.%04d",
								LocalTime.wHour,		// Часы
								LocalTime.wMinute,		// Минуты
								LocalTime.wSecond,		// Секунды
								LocalTime.wDay,			// День(число)
								LocalTime.wMonth,		// Месяц
								LocalTime.wYear			// Год
							);
			else	// if (mode && FJC_NEED_DATE)
					sprintf(	str,
								"(%02d:%02d:%02d)",
								LocalTime.wHour,		// Часы
								LocalTime.wMinute,		// Минуты
								LocalTime.wSecond		// Секунды
							);
			else	// if (mode && FJC_NEED_TIME)
					sprintf(	str,
								"%02d.%02d.%04d",
								LocalTime.wDay,			// День(число)
								LocalTime.wMonth,		// Месяц
								LocalTime.wYear			// Год
							);
}

bool LogFile::Start(char *message)
{
	if (!f) return NULL;
	msgLength = (unsigned int)strlen(message);				  
	memset(msg,0,256);
	strcpy(msg,message);
	FJCGetTD(FJC_NEED_TIME|FJC_NEED_DATE,buf2);
	sprintf(	buf,
				"%s: %31.31s %s",
				msg,
				"Session sucessfully started at",
				buf2);
	Msg(buf);
	return true;
}
bool LogFile::Msg(char* msg)
{
	if (!f) return NULL;
	fprintf (f,"%s\n",msg);							// Простое запихивание строки в файл
	return true;
}
bool LogFile::Logf(char *block, char* str, ...)
{
	if (!f) return NULL;
	FJCGetTD(FJC_NEED_TIME,buf);
	sprintf(	buf2,
				"\t%s : <%15.15s> :: %s\n",
				buf,
				block,
				str
			);
	va_list    ap;									// Указатель на список аргументов
    va_start(ap, str);								// Разбор строки переменных
	static char formatbuf [512];
	vsprintf(formatbuf,buf2,ap);
	va_end(ap);										// Результат помещается в строку
	fprintf(f,"%s", formatbuf);						// И конвертирование символов в реальные коды
 	return true;
}

bool LogFile::Log(char *block, char* msg, bool ok)
{
	if (!f) return NULL;
	FJCGetTD(FJC_NEED_TIME,buf);
	sprintf(	buf2,
				"\t%s : <%15.15s> :: %50.50s\t\t\t ",
				buf,
				block,
				msg);
	if (ok)	strcat(buf2, "[   OK   ]");
		else strcat(buf2,"[ FAILED ]");
	Msg(buf2);
 	return true;
}
bool LogFile::End(bool is_normal)
{
	if (!f) return NULL;
	FJCGetTD(FJC_NEED_TIME|FJC_NEED_DATE,buf2);
	if (is_normal)
		sprintf(	buf,
					"%s: %31.31s %s",
					msg,
					"Session sucessfully ended at",
					buf2);
	else	// Не нормально :=)
		sprintf(	buf,
					"%s: Fatal error at %s",
					msg,
					buf2);
	Msg(buf);																					
	return true;
}
//==================================================================================================================
char* CFGFile::Gets(char* string)
{
	if (!f) return NULL;
	do
	{
		fgets(string,256,f);
		CleanString(string);
		if (feof(f))
		{
			if ( (string[0]!=0) &&(string[0]!=10) ) return string;
			return NULL;
		};
	}
	while	((string[0]==10)||(string [0] == ';')||(( string [0] == '/' )&&( string [1] == '/' ) ) );
	return string;
}

void CleanString(char *s)
{
	static char s2[512];							// Очишеная строка
	static bool	word;								// Показывает что сейчас копируются символы слова(не пробелы)
	memset(s2,0,sizeof(s2));
	int i,j;										// Итераторы по строкам
	for (i=0,j=0;(s[i]==' ')||(s[i]==9);i++);		// Пропуск пробелов и табо в начале
	for (;(s[i]!=0)&&(s[i]!=10);)
	{
		if ((s[i]==' ')||(s[i]==9))
		{
			s2[j] = ' '; j++;i++;
			for (;(s[i]==' ')||(s[i]==9);i++);
		}
		else
		{
			s2[j]=s[i];i++;j++;
		}
	}
	memset (s,0,strlen(s)+1);
	memcpy ( s,s2,strlen(s2) );
}