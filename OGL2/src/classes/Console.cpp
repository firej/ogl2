#include "LocusAFX.h"
#include "./Console.h"
#include "./ResMan.h"
#include "./Time.h"
#include "./ConsoleFunctions.h"
//#include "./Application.h"

ConVarsMap			CVmap;		// Карта изменяемых переменных
CConsole		*	MainCon	=	NULL;

char	upcase	(	char	a	)
{
	if (a == 'a') return 'A';
	if (a == 'b') return 'B';
	if (a == 'c') return 'C';
	if (a == 'd') return 'D';
	if (a == 'e') return 'E';
	if (a == 'f') return 'F';
	if (a == 'g') return 'G';
	if (a == 'h') return 'H';
	if (a == 'i') return 'I';
	if (a == 'j') return 'J';
	if (a == 'k') return 'K';
	if (a == 'l') return 'L';
	if (a == 'm') return 'M';
	if (a == 'n') return 'N';
	if (a == 'o') return 'O';
	if (a == 'p') return 'P';
	if (a == 'q') return 'Q';
	if (a == 'r') return 'R';
	if (a == 's') return 'S';
	if (a == 't') return 'T';
	if (a == 'u') return 'U';
	if (a == 'v') return 'V';
	if (a == 'w') return 'W';
	if (a == 'x') return 'X';
	if (a == 'y') return 'Y';
	if (a == 'z') return 'Z';

	if (a == 'а') return 'А';
	if (a == 'б') return 'Б';
	if (a == 'в') return 'В';
	if (a == 'г') return 'Г';
	if (a == 'д') return 'Д';
	if (a == 'е') return 'Е';
	if (a == 'ё') return 'Ё';
	if (a == 'ж') return 'Ж';
	if (a == 'з') return 'З';
	if (a == 'и') return 'И';
	if (a == 'й') return 'Й';
	if (a == 'к') return 'К';
	if (a == 'л') return 'Л';
	if (a == 'м') return 'М';
	if (a == 'н') return 'Н';
	if (a == 'о') return 'О';
	if (a == 'п') return 'П';
	if (a == 'р') return 'Р';
	if (a == 'с') return 'С';
	if (a == 'т') return 'Т';
	if (a == 'у') return 'У';
	if (a == 'ф') return 'Ф';
	if (a == 'х') return 'Х';
	if (a == 'ц') return 'Ц';
	if (a == 'ч') return 'Ч';
	if (a == 'ы') return 'Ы';
	if (a == 'э') return 'Э';
	if (a == 'ю') return 'Ю';
	if (a == 'я') return 'Я';
		return a;
}

string	upcase_string	(string	s)
{
	for (size_t	i = 0 ; i < s.size() ; i++ )
	{
		s[i] = upcase(s[i]);
	}
	return s;
}

void	CConsole::INIT()
{
	if (MainCon)
	{
		MainCon->OUT_STRING(CONCOL_ERROR,
			"Почемуто появляется второй объект консоли !!! или второй раз запускается инициализация.");
	}
	else
	{
		MainCon = this;
	}
	State			=	Console::ST_INVISIBLE;
	pers_step		=	0.01;
	transparency	=	0.8;
	height			=	0.8;
	lastTick		=	0.0;
	Period			=	0.65;							// Пол секунды времени
	iAComp			=	8;
	CurVisible		=	true;
	Colors[CONCOL_DEFAULT]		=	Point3f(1.0f,1.0f,1.0f);
	Colors[CONCOL_ERROR]		=	Point3f(1.0f,0.3f,0.3f);
	Colors[CONCOL_HELP]			=	Point3f(0.7f,0.7f,1.0f);
	Colors[CONCOL_WARNING]		=	Point3f(0.8f,0.8f,0.0f);
	Colors[CONCOL_CURSOR]		=	Point3f(1.0f,1.0f,1.0f);
	Colors[CONCOL_BACKGROUND]	=	Point3f(0.1f,0.1f,0.1f);
	Colors[CONCOL_INPUTLINE]	=	Point3f(0.15f,0.15f,0.15f);
	Colors[CONCOL_OK]			=	Point3f(0.3f,1.0f,0.3f);

	CFmap.clear();
	CVmap.clear();

	if (list)	DEINIT();
	ListElements				=	Globals.VP.ScreenStrings;
	Console::ListElement	*	next	=	NULL;
	Console::ListElement	*	flist	=	NULL;
	for ( UINT i = 0 ; i < ListElements ; i++ )
	{
		next = new Console::ListElement;
		next->data.c	=	CONCOL_DEFAULT;
		next->data.s	=	string("");
		if ( i != 0 )
			list->next	=	next;
		else
			flist		=	next;
		list = next;
	}
	list->next				=	flist;				// Закругляем список
	//RegFunc					("get",&Console_Get);
	//RegFunc					("set",&Console_Set);
	RegFunc					("Help",&Console_Help);
	INIT_CONSOLE_FUNCTIONS	();
}
void	CConsole::DEINIT()
{
	if (!list)	return;
	Console::ListElement	*	next;
	for ( UINT i = 0 ; i < ListElements ; i++ )
	{
		next = list->next;
		delete	list;
		list = next;
	}
	ListElements			=	NULL;
	list					=	NULL;
}

void	CConsole::OUT_STRING	(BYTE	c, const char *fmt, ...)
{
	char	text[512];
	va_list    ap;				// Указатель на список аргументов
	if (fmt == NULL || IsBadStringPtr(fmt,200))		// Если нет текста
		return;					// Ничего не делать
    va_start(ap, fmt);			// Разбор строки переменных
    vsprintf(text, fmt, ap);	// И конвертирование символов в реальные коды
    va_end(ap);					// Результат помещается в строку
	list->data.c	=	c;
	list->data.s	=	string(text);
	list = list->next;			// Переключаемся на следующию строчку
	list->data.s.clear();		// Очистка текущей строчки
	list->data.c = CONCOL_DEFAULT;
	chars	=	0;
	cursor	=	0;
}
void	CConsole::Draw()
{
	switch	(State)
	{
	case Console::ST_INVISIBLE:	return;			// Как можно рисовать консоль в закрытом состоянии?
	case Console::ST_FADEIN:	persent += pers_step * GT.GetFrameTime() * 60;
		if (persent >= transparency)
			State = Console::ST_VISIBLE;	break;
	case Console::ST_FADEOUT:	persent -= pers_step * GT.GetFrameTime() * 60;
		if (persent <= 0)
			State = Console::ST_INVISIBLE;	break;
	default:break;
	}

	lastTick	+=	GT.GetFrameTime();
	if (lastTick > Period)
	{
		lastTick	-=	Period;
		CurVisible	=	!CurVisible;
	}
	glPushAttrib(GL_LIST_BIT|GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_CURRENT_BIT);		// Сохранение настроек
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	//T->bind();
	glDisable(GL_DEPTH_TEST);							// Disables Depth Testing
	//glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();										// Store The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	glOrtho(0,1,1.0/height,0,-1,1);							// Set Up An Ortho Screen
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPushMatrix();										// Store The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm   РИСОВАНИЕ КОНСОЛИ
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm ФОН
	glColor4d(	Colors[CONCOL_BACKGROUND].d.c.x,
				Colors[CONCOL_BACKGROUND].d.c.y,
				Colors[CONCOL_BACKGROUND].d.c.z,
				persent);
	glBegin(GL_TRIANGLE_STRIP);
		glVertex2d( 0.0 , 0.0 );
		glVertex2d( 0.0 , 1.0 - 1.0/ListElements );
		glVertex2d( 1.0 , 0.0 );
		glVertex2d( 1.0 , 1.0 - 1.0/ListElements );
	glEnd();
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm Строка ввода
	glColor4d(	Colors[CONCOL_INPUTLINE].d.c.x,
				Colors[CONCOL_INPUTLINE].d.c.y,
				Colors[CONCOL_INPUTLINE].d.c.z,
				persent);
	glBegin(GL_TRIANGLE_STRIP);
		glVertex2d( 0.0, 1.0 - 1.0/ListElements );
		glVertex2d( 0.0, 1.0 );
		glVertex2d( 1.0, 1.0 - 1.0/ListElements );
		glVertex2d( 1.0, 1.0 );
	glEnd();
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm Текст вводимый
	ConsoleFont->SetStyle	(Text::left,Text::top,height);
	ConsoleFont->SetColor	(Colors[CONCOL_DEFAULT],(float)persent/(float)transparency);
	ConsoleFont->Print		(0.0, 0.975*height, "%s",list->data.s.c_str());
	Console::ListElement	*	next = list->next;
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm Текст основной
	for (DWORD i = 0 ; i < ListElements - 1 ; i++)
	{
		ConsoleFont->SetColor(Colors[next->data.c]);
		ConsoleFont->Print		(0.0, 0.025*height*i, "%s",next->data.s.c_str());
		next = next->next;
	}
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm Текст автодополнения
	if ( ! list->data.s.empty() )
	{
		string	s1,s2;
		s2 = list->data.s;
		s2 = upcase_string(s2);
		s1 = FindNextSym(s2);
		for (WORD i = 0 ; i < iAComp - 1 ; i++)
		{
			s2 = s1;
			s1 = FindNextSym(s2);
			ConsoleFont->SetColor(Colors[CONCOL_DEFAULT],((float)iAComp * 2.0f - i)/((float)iAComp*2.0f) );
			ConsoleFont->Print		(0.0, 0.025*height*(ListElements + i), "%s",s2.c_str());
			if (s2 == s1) break;
		}
	}
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm КурсорчеГ
	if (CurVisible)
	{
		string	tmp;
		tmp = list->data.s.substr(0,cursor);
		float	offset = ConsoleFont->GetStrWidth(tmp.c_str());
		glColor4f	(	Colors[CONCOL_CURSOR].d.c.x,
						Colors[CONCOL_CURSOR].d.c.y,
						Colors[CONCOL_CURSOR].d.c.z,
						(float)persent/(float)transparency);
		glLineWidth (1.0f);
		glBegin	(GL_LINES);
			glVertex2f	( offset, 1.0f - 1.0f/ListElements );
			glVertex2f	( offset, 1.0f );
		glEnd	();
	}
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm   КОНЕЦ
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glPopMatrix();										// Restore The Old Projection Matrix
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glPopAttrib();										// Возврат
}

void	CConsole::ProcessChar	(char c)
{
	if (!Visible())	return;
	if ( c < 32 && c > 0		)				return;
	if ( c == '`' /*|| c == '~'	*/)				return;
	if ( c == 127				)				return;
	if (cursor == chars)
	{
		list->data.s.append(1,c);		// Прибавляем в конец символ
		cursor++;
		chars++;
	}
	else
	{
		list->data.s.insert(cursor,1,c);			// Вставляем в позицию курсора символ
		cursor++;
		chars++;
	}
}
void	CConsole::ProcessKey	(DWORD	key)
{
	chars = (DWORD)list->data.s.size();
	if (cursor > chars) cursor = chars;
	if (!Visible())
		return;
	if (key == VK_LEFT)
	{
		if ( cursor > 0 ) cursor--;
	}
	if (key == VK_END)
	{
		cursor = chars;
	}
	if (key == VK_HOME)
	{
		cursor = 0;
	}
	if (key == VK_RIGHT)
	{
		if ( cursor < chars ) cursor++;
	}
	if (key == VK_DELETE)
	{
		if ( cursor != chars && chars != 0 )
		{
			list->data.s.erase(cursor,1);
			chars--;
		}
	}
	if ( key == VK_RETURN )
	{
		if (!list->data.s.empty())
		{
			string com = list->data.s;
			list = list->next;			// Переключаемся на следующию строчку
			list->data.s.clear();		// Очистка текущей строчки
			list->data.c = CONCOL_DEFAULT;
			chars	=	0;
			cursor	=	0;
			DoString(com);
		}
	}
	if ( key == VK_BACK )
	{
		if ( cursor != 0 && chars != 0 )
		{
			list->data.s.erase(cursor-1,1);
			cursor--;
			chars--;
		}
	}
	if (key == VK_TAB)
	{
		list->data.s	=	FindNextSym(upcase_string(list->data.s));
		list->data.s	+=	" ";
		chars			=	(DWORD)list->data.s.size();
		cursor			=	chars;
	}
	if (key == VK_UP)
	{
		if (!CommandsHistory.empty())
		{
			if (iCndHistory == CommandsHistory.end())
				iCndHistory = CommandsHistory.begin();
			else
				iCndHistory++;
			if (iCndHistory == CommandsHistory.end())
				iCndHistory = CommandsHistory.begin();
			list->data.s = (*iCndHistory);
			cursor	=	(DWORD) list->data.s.size();
		}
	}
	if (key == VK_DOWN)
	{
		if (!CommandsHistory.empty())
		{
			if (iCndHistory == CommandsHistory.begin())
				iCndHistory = CommandsHistory.end();
			iCndHistory--;
			list->data.s = (*iCndHistory);
			cursor	=	(DWORD)list->data.s.size();
		}
	}

}
void	CConsole::ChangeState()
{
	if ((State == Console::ST_FADEIN) || (State == Console::ST_VISIBLE))
			State = Console::ST_FADEOUT;
		else
			State	=	Console::ST_FADEIN;
}
void	CConsole::Show()
{
	if ((State != Console::ST_FADEIN) && (State != Console::ST_VISIBLE))
		State = Console::ST_FADEIN;
}
void	CConsole::Hide()
{
	if ((State != Console::ST_FADEOUT) && (State != Console::ST_INVISIBLE))
		State = Console::ST_FADEOUT;
}
bool	CConsole::Visible()
{
	if ((State == Console::ST_FADEIN) || (State == Console::ST_VISIBLE))
			return true;
		else
			return false;
}
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//mmmmmmmmmmmmmmmm Выполнение консольных команд mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
void	CConsole::DoString(string s)
{
	bool	verdict = !CommandsHistory.empty();
	if (verdict)		// Если история не пуста делаем ещё одну проверку
	{
		verdict = *(CommandsHistory.begin()) != s;	// Повторяющиеся команды не имеет смысла логировать
	}
	// После всех проверок добавляем команду в историю
	if (CommandsHistory.empty() || verdict)
	{
		CommandsHistory.insert(CommandsHistory.begin(),s);
	}
	iCndHistory = CommandsHistory.end();			// Переводим указатель истории на начало

	DWORD	comLength	=	(DWORD)	s.find(' ');
	string	command, cParam;
	command	=	s.substr		(0,comLength);
	command	=	upcase_string	(command);
	if ( comLength < s.size() - 1 )
	{
		cParam	=	s.substr		(comLength+1,s.size()-comLength-1);
	}

	ConFuncsMap::iterator	i	=	CFmap.find(command);
	if ( i == CFmap.end())
	{
		//ConVarsMap::iterator j	=	CVmap.find(command);					// Поищем переменную
		//if ( j == CVmap.end())
		//{
			OUT_STRING(CONCOL_ERROR,"Неизвестная комманда");
		//}
		//else
		//{
		//	Console_Get(command);		// Найдена переменная - выведем на экран её значение
		//}
		return;
	}
	else
	{
		(*i->second)(cParam);
	}
}

string	CConsole::FindNextSym	(string	s)
{
	iCFuncsMap i = CFmap.begin();
	iCFuncsMap j = i;
	while(i->first <= s)
	{
		i++;
		if ( i == CFmap.end() ) return	j->first;
		j = i;
	}
	return i->first;
}

void	CConsole::RegFunc		(std::string s, ConFunc cf)
{
	if ( cf == NULL )	return;
	if ( s.empty()	)	return;
	s = upcase_string(s);
	ConFuncsMap::iterator	i = CFmap.find(s);
	if ( i == CFmap.end() )
	{
		CFmap[s] = cf;
	}
}
void	CConsole::UnRegFunc(std::string s)
{
	s = upcase_string(s);
	if (s.empty())		return;
	ConFuncsMap::iterator	i = CFmap.find(s);
	if ( i != CFmap.end() )
	{
		CFmap.erase(i);
	}
}

/*void	CConsole::RegVar		(string s,Console::VarType type,void* var,BYTE maxsize)
{
	if ( var == NULL	)	return;
	if ( s.empty()		)	return;
	s = upcase_string(s);
	ConVarsMap::iterator	i = CVmap.find(s);
	if ( i == CVmap.end() )
	{
		Var_t	vr;
		vr.type		=	type;
		vr.var		=	var;
		vr.maxsize	=	maxsize;
		CVmap[s]	=	vr;
	}
}

void	CConsole::UnRegVar		(string	s)				// Убирать регистрацию переменной
{
	if (s.empty())		return;
	s = upcase_string(s);
	ConVarsMap::iterator	i = CVmap.find(s);
	if ( i != CVmap.end() )
	{
		CVmap.erase(i);
	}
}*/

void CConsole::DropHistory()
{
	CommandsHistory.clear();
}

#if 0
void	Console_Set		(std::string	s)
{
	DWORD	varLength	=	(DWORD)	s.find(' ');
	if (varLength == s.size())
	{
		MainCon->OUT_STRING(CONCOL_ERROR,"Использование: set <переменная> значение1 [ значение2 ] [ значение3 ]");
		return;
	}
	string	var, vParam;
	var	=	s.substr		(0,varLength);
	var	=	upcase_string	(var);
	vParam	=	s.substr	(varLength+1,s.size()-varLength-1);

	ConVarsMap::iterator i	=	CVmap.find(var);
	if ( i == CVmap.end())
	{
		MainCon->OUT_STRING(CONCOL_ERROR,"Неизвестная переменная");
		return;
	}
	else
	{
		switch(i->second.type)
		{
		case	Console::VT_BOOLEAN:
			varLength	=	(DWORD)	vParam.find	(' ');
			vParam		=	vParam.substr		(0,varLength);
			vParam		=	upcase_string		(vParam);
			if ( vParam == string("TRUE") || vParam == string("ON") )
				*( (bool *) i->second.var) = true;
			if ( vParam == string("FALSE") || vParam == string("OFF") )
				*( (bool *) i->second.var) = false;
			break;
		case	Console::VT_COLOR3f:
			sscanf(vParam.c_str(),"%f %f %f",&(((Point3f*)(i->second.var))->d.c.x),&(((Point3f*)(i->second.var))->d.c.y),&(((Point3f*)(i->second.var))->d.c.z));
			break;
		case	Console::VT_COLOR4f:
			MainCon->OUT_STRING(CONCOL_ERROR,"Установку 4х мерных цветовых переменных сделать лень");
			break;
		case	Console::VT_QWORD:
			sscanf(vParam.c_str(),"%llu",(unsigned long long * )i->second.var);
			break;
		case	Console::VT_DWORD:
			sscanf(vParam.c_str(),"%u",(unsigned int *)i->second.var);
			break;
		case	Console::VT_WORD:
			sscanf(vParam.c_str(),"%hu",(unsigned short int *)i->second.var);
			break;
		case	Console::VT_BYTE:
			{
				WORD	tmp;
				sscanf(vParam.c_str(),"%hu",&tmp);
				*((unsigned char*)i->second.var) = (BYTE) tmp;
			}
			break;
		case	Console::VT_CHAR:
			{
				int		tmp;
				sscanf(vParam.c_str(),"%i",&tmp);
				*((char*)i->second.var) = (char) tmp;
			}
			break;
		case	Console::VT_INT16:
			sscanf(vParam.c_str(),"%hi",(short int *)i->second.var);
			break;
		case	Console::VT_INT32:
			sscanf(vParam.c_str(),"%i",(int *)i->second.var);
			break;
		case	Console::VT_INT64:
			sscanf(vParam.c_str(),"%ll",(long long *)i->second.var);
			break;
		case	Console::VT_FLOAT32:
			sscanf(vParam.c_str(),"%f",(float *)i->second.var);
			break;
		case	Console::VT_FLOAT64:
			sscanf(vParam.c_str(),"%lf",(double*)i->second.var);
			break;
		case	Console::VT_POINT2f:
			sscanf(vParam.c_str(),"%f %f",&(((Point2f*)(i->second.var))->d.c.x),&(((Point2f*)(i->second.var))->d.c.y));
			break;
		case	Console::VT_STRING:
			strncpy((char*)i->second.var,vParam.c_str(),i->second.maxsize);
			break;
		}
	}
}
#endif

void	Console_Help	(std::string	s)
{
	MainCon->OUT_STRING(CONCOL_HELP,"Список допустимых команд консоли LocusEngine (более подробно \"команда help\")");
	unsigned int	strings = Globals.VP.ScreenStrings - 2;
	unsigned int	maxLenght = 0;
	char			buf[512];
	char			forsprintf[32];			// Строка спецификаций для sprintf
	ConFuncsMap	*	map;
	map = MainCon->GetMap();
	// Находим максимальную длину команды в словаре
	for (ConFuncsMap::iterator	i = map->begin() ; i != map->end(); i++ )
	{
		if (i->first.length() > maxLenght) maxLenght = (unsigned int)i->first.length();
	}
	maxLenght += 2;
	sprintf(forsprintf,"%%s %%-%ds",maxLenght);
	for ( unsigned int i = 0 ; i < strings && i < map->size(); i++ )
	{
		memset(buf,0,sizeof(buf));
		for ( unsigned int j = 0 ; j*strings + i < map->size() ; j++ )
		{
			ConFuncsMap::iterator it = map->begin();
			for (unsigned int k = 0 ; k < j*strings+i ; k++) it++;
			sprintf(buf,forsprintf,buf,it->first.c_str());
		}
		MainCon->OUT_STRING(CONCOL_MESSAGE,buf);
	}
}
