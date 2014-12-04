// Файл реализации класса Application
#include "LocusAFX.h"
#include "./ApplicationClass.h"
#include "./time.h"
//#include <ntddk.h>

Application *	MainApplication;					// Указатель на объект приложения
GlobalsStruct		Globals;							// Глобальные данные о приложении
KeySettings			KeySet;								// Настройки клавиатуры
SysInformationData	SysInfo;							// Информация о системе
	LogFile	LF;											// Лог-файл
	CFGFile	CF;											// Файл с настройками
Timer				GT;									// Глобальный таймер
Input::Input_t*		Input::I;							// Объект инпута

char	ModelName[1024]	=	"test_ball_t.lwo";

TextureLogo NVLogo;
StartUPLogo SimpleLogo;

GLfloat LightAmbient	[]=	{ 1.0f, 1.0f, 1.0f, 1.0f };	// Значения фонового света
GLfloat LightDiffuse	[]=	{ 1.0f, 1.0f, 1.0f, 1.0f };	// Значения диффузного света
GLfloat LightPosition	[]=	{ 0.0f, 0.0f, 0.0f, 1.0f };	// Позиция света

Application::Application()
{
	MainApplication	= this;

	LF.Open("data/log_file.log",'w');
	CF.Open("data/config.cfg",'r','t',false);
	LF.Start("FJC 3D Engine");
}

Application::~Application()
{
	LF.Close();
	CF.Close();
	ShowCursor(true);
}

bool Application::InitOpenGL()
{
#ifdef WIN32
	wglSwapIntervalEXT_Func wglSwapIntervalEXT = (wglSwapIntervalEXT_Func)wglGetProcAddress("wglSwapIntervalEXT");
	if(wglSwapIntervalEXT) wglSwapIntervalEXT(Globals.VP.Vsync);//1 - чтобы включить
	else FASSERT (wglSwapIntervalEXT);
#else
#error Linux is not supported yet
#endif
	LF.Logf("InitOpenGL","Vsync status : %d",Globals.VP.Vsync);
	//===[ Инициализация всякой фигни ]===================================================||
		Cam.Init();
	//===[ Наинициализировались ]=========================================================||
	glClearColor(0.0f, 0.0f, 0.25f, 0.0f);
							// Очистка экрана в черный цвет
	glClearDepth(1.0);		// Разрешить очистку буфера глубины
	glDepthFunc(GL_LESS);	// Тип теста глубины (рисуется всё что ближе)
	glEnable(GL_DEPTH_TEST);// разрешить тест глубины
	glShadeModel(GL_SMOOTH);// разрешить плавное цветовое сглаживание
	ReshapeWindow(Globals.VP.Width,Globals.VP.Height);
	glEnable(GL_TEXTURE_2D);	// Разрешение наложения текстур
	glClearDepth(1.0f);           // Установка буфера глубины
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Улучшенные вычисления перспективы
	glDisable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
	glEnable(GL_DITHER);
	//glEnable(GL_CULL_FACE);
	//==== Свет ==========================================================================//
	glEnable(GL_LIGHT1); // Разрешение источника света номер один
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Установка Фонового Света
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Установка Диффузного Света
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);	// Позиция света
	glEnable(GL_LIGHTING);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE,0.0);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	//=== /Свет ==========================================================================//

	return	NO_ERROR;
}

bool Application::InitOpenIL()
{
	ilInit();
#ifdef ILU_ENABLED
	iluInit();
#endif
	return NO_ERROR;
}

bool Application::GetSysInfo()
{
// ===[ Видео акселератор ]===================================================================================
	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,&SysInfo.video.MaxAnisotropy);
	SysInfo.video.RendererName	= (char *)glGetString(GL_RENDERER);					// Выводим имя карты
	LF.Logf("GetSysInfo","Render            %s",SysInfo.video.RendererName);
	SysInfo.video.VendorName	= (char *)glGetString(GL_VENDOR);					// Выводим имя поставщика
	LF.Logf("GetSysInfo","Vendor            %s",SysInfo.video.VendorName);
	SysInfo.video.OpenGL_Version= (char *)glGetString(GL_VERSION);					// Выводим версию
	LF.Logf("GetSysInfo","OpenGL version is %s",SysInfo.video.OpenGL_Version);
	SysInfo.video.SupportedGlExtentions = (char *)glGetString(GL_EXTENSIONS);
	//LF.Msg("Найденые расширения видеокарты"); LF.Msg(SysInfo.video.SupportedGlExtentions);
// ===[ Процессор::реестр ]=======================================================================================
#ifdef WIN32
	HKEY CPinfo;
	FASSERT(!RegOpenKeyEx(HKEY_LOCAL_MACHINE,"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",NULL,KEY_QUERY_VALUE,&CPinfo));
	DWORD bufsize = sizeof(SysInfo.cpu.speed);
	RegQueryValueEx(CPinfo,"~MHz",NULL,NULL,(LPBYTE)&SysInfo.cpu.speed,&bufsize);
	LF.Logf("GetSysInfo","CPU speed is %d (by M$ Windows)",SysInfo.cpu.speed);
	bufsize = sizeof(SysInfo.cpu.id);
	RegQueryValueEx(CPinfo,"Identifier",NULL,NULL,(LPBYTE)&SysInfo.cpu.id,&bufsize);
	LF.Logf("GetSysInfo","CPU identifier \"%s\"",SysInfo.cpu.id);
	bufsize = sizeof(SysInfo.cpu.name);
	RegQueryValueEx(CPinfo,"ProcessorNameString",NULL,NULL,(LPBYTE)&SysInfo.cpu.name,&bufsize);
	LF.Logf("GetSysInfo","CPU name  \"%s\" ",SysInfo.cpu.name);
	bufsize = sizeof(SysInfo.cpu.vendor_id);
	RegQueryValueEx(CPinfo,"VendorIdentifier",NULL,NULL,(LPBYTE)&SysInfo.cpu.vendor_id,&bufsize);
	LF.Logf("GetSysInfo","Vendor ID \"%s\"",SysInfo.cpu.vendor_id);
	RegCloseKey(CPinfo);
		// Расширения процессора
	char fichi[128];
	sprintf(fichi,"Supported extentions: ");
	SysInfo.cpu.MMX			= (char)IsProcessorFeaturePresent (PF_MMX_INSTRUCTIONS_AVAILABLE)	;
	if (SysInfo.cpu.MMX) strcat(fichi," MMX");
	SysInfo.cpu.AMD_3DNow	= (char)IsProcessorFeaturePresent (PF_3DNOW_INSTRUCTIONS_AVAILABLE)	;
	if (SysInfo.cpu.AMD_3DNow) strcat(fichi," AMD_3DNow");
	SysInfo.cpu.RDTSC		= (char)IsProcessorFeaturePresent (PF_RDTSC_INSTRUCTION_AVAILABLE)	;
	if (SysInfo.cpu.RDTSC) strcat(fichi," RDTSC");
	SysInfo.cpu.SSE			= (char)IsProcessorFeaturePresent (PF_XMMI_INSTRUCTIONS_AVAILABLE)	;
	if (SysInfo.cpu.SSE) strcat(fichi," SSE");
	SysInfo.cpu.SSE2		= (char)IsProcessorFeaturePresent (PF_XMMI64_INSTRUCTIONS_AVAILABLE);
	if (SysInfo.cpu.SSE2) strcat(fichi," SSE2");
	LF.Logf("GetSysInfo","%s",fichi);
#else
// ===[ Процессор::ассемблер ]====================================================================================
	unsigned __int64 ticks;
	ticks = GetCycleCount();
	Sleep (TIME_TO_CALCULATE_CPU_SPEED);
	ticks = GetCycleCount() - ticks;
	SysInfo.cpu.speed = int ((float)ticks /(TIME_TO_CALCULATE_CPU_SPEED * 1000.0f));
	LF.Logf("GetSysInfo","CPU speed is %d (by ASM)",SysInfo.cpu.speed);
	union CPUID_string_union_type{
		char name [16];
		struct{
			DWORD a,b,c,zero;
		};
	}			CPU_name;
	DWORD a,b,c;								// Временные переменные для доставания имени процессора
	memset (&CPU_name,0,sizeof(CPU_name));
	DWORD	CPU_features,						// Возможности процессора 32 бита
			CPU_data_1,							// Данные о прецессоре первые 32 бита
			CPU_data_2;							// Данные о прецессоре вторые 32 бита
	__asm {
		mov EAX, 00000000h						// Имя вендора процессора
		CPUID
		mov a,EBX
		mov b,EDX
		mov c,ECX

		mov EAX, 00000001h
		CPUID
		mov CPU_features,	EDX
		mov CPU_data_1,		EBX
		mov CPU_data_2,		ECX
	};	CPU_name.a = a;	CPU_name.b = b;	CPU_name.c = c;
	LF.Logf("GetSysInfo","CPU_name            %s",CPU_name.name);

#define SOPROCESSOR_ENABLED		0x1			//	00000000000000000000000000000001
#define RDTSC_ENABLED			0x8			//	00000000000000000000000000001000
#define MMX_ENABLED				0x400000	//	00000000010000000000000000000000
#define SSE_ENABLED				0x800000	//	00000000100000000000000000000000
#define SSE2_ENABLED			0x1000000	//	00000001000000000000000000000000
#define AMD_3D_NOW_ENABLED		0x40000000	//	01000000000000000000000000000000
#define AMD_3D_NOW_Ex_ENABLED	0x80000000	//	10000000000000000000000000000000
#pragma warning (disable:4127)
	if (CPU_features & RDTSC_ENABLED)			SysInfo.cpu.RDTSC		= true;
	if (CPU_features & MMX_ENABLED)				SysInfo.cpu.MMX			= true;
	if (CPU_features & SSE_ENABLED)				SysInfo.cpu.SSE			= true;
	if (CPU_features & SSE2_ENABLED)			SysInfo.cpu.SSE2		= true;
	if (CPU_features & AMD_3D_NOW_ENABLED)		SysInfo.cpu.AMD_3DNow	= true;
	if (CPU_features & AMD_3D_NOW_Ex_ENABLED)	SysInfo.cpu.AMD_3DNow_Ex= true;
#pragma warning (default:4127)
	char fichi[128];
	sprintf(fichi,"Supported extentions: ");
	if (SysInfo.cpu.SSE)				strcat(fichi," SSE");
	if (SysInfo.cpu.SSE2)				strcat(fichi," SSE2");
	if (SysInfo.cpu.MMX)				strcat(fichi," MMX");
	if (SysInfo.cpu.AMD_3DNow)			strcat(fichi," AMD_3DNow");
	if (SysInfo.cpu.AMD_3DNow_Ex)		strcat(fichi," AMD_3DNow_Ex");
	if (SysInfo.cpu.RDTSC)				strcat(fichi," RDTSC");
	LF.Logf("GetSysInfo","%s",fichi);
#endif
	//if (!SysInfo.cpu.RDTSC) {MessageBox(NULL,"Your CPU not supported RDTSC instruction\nYou are crazy???","You need new CPU",MB_OK|MB_ICONERROR); ExitProcess(1);}
	return NO_ERROR;
}


bool Application::ReadConfig()
{
	Globals.ERS.m				=	ERS::Mesh::NORMAL;
	Globals.ERS.d				=	ERS::Draw::SCENE;
	Globals.EFl.show_stat		=	true;
	Globals.EFl.LWOCullFace		=	true;

	strcpy(Globals.RES.TEXTURE_DIR,"data/textures/");
	strcpy(Globals.RES.MESH_DIR,"data/obj/");
	strcpy(Globals.RES.FONT_DIR,"data/fonts/");
	// Настройки по умолчанию
	Globals.VP.IsFullScreen		=	false;
	Globals.VP.Width			=	1024;		// Видео настройки
	Globals.VP.Height			=	768;		// Видео настройки
	Globals.VP.AR				=	AR::NORMAL;	// Установка AR
	Font::SetAspectRatio();
	Globals.VP.ScreenStrings	=	40;			// Установка количества строк на экране (для консоли)
	Globals.VP.Bits				=	32;			// Видео настройки
	Globals.VP.Hzs				=	85;			// Видео настройки

	KeySet.CamMoveForward		=	'W'		;	// W key
	KeySet.CamMoveBackward		=	'S'		;	// S
	KeySet.CamMoveLeft			=	'A'		;	// A
	KeySet.CamMoveRight			=	'D'		;	// D
	KeySet.CamBackToCenter		=	'F'		;	// F
	KeySet.MoreFastMove			=	VK_SHIFT;	// SHIFT

	CamS.BackwardSpeed			=	7.0f	;
	CamS.ForwardSpeed			=	7.0f	;
	CamS.StrafeSpeed			=	7.0f	;
	CamS.Sensitivity			=	2.0f	;
	CamS.CamStartPosition		=	Point3d(0.0f,-2.0f,-10.0f);
	//CamS.CamStartPosition		=	Point3d(0.0f,-2.0f,0.0f);
	
	Globals.TextureFiltering	=	FJC_TEX_ANISOTROPIC_FILTERING;
//	Globals.lists				=	0;
	// =========================================== Конец настроек по умолчанию

	char	buf2 [512];							// Буфер для чтения конфига
	char	name [128];							// Имя опции
	char	param[384];							// Параметр
	char	*buf;								// Указатель на буфер (или 0)
	int i = 0;
	bool	False = false;

	do
	{
		i=0;
		memset(name,0,128);
		memset(param,0,384);
		memset(buf2,0,512);
		buf = CF.Gets(buf2);
		if (buf == NULL) break;
			for (;(buf[i]!=' ')&&(buf[i]!=0)&&(i<127);i++){
				name[i]=buf[i];
			};
			i++;								// Пропуск пробела!(он один потому что CleanString)
			for (int j=0;(i < 512)&&(buf2[i]!=0);i++,j++){
				param[j] = buf2[i];
			};
			_strlwr(name);
		do{
			if (strcmp( name , "width" ) == 0){							// Ширина экрана ( окошка )
				sscanf(param,"%d",&Globals.VP.Width);
				break;
			}
			if (strcmp( name , "height" ) ==0){							// Высота экрана ( окошка )
				sscanf(param,"%d",&Globals.VP.Height);
				break;
			}
			if (strcmp( name , "fullscreen" ) ==0){						// запускать в полноэкранном режиме?
				if (strcmp(param,"true")==0)	{IsFullScreen = true;};
				if (strcmp(param,"false")==0)	{IsFullScreen = false;};
				break;
			}
			if (strcmp( name , "bits" ) ==0){							// битность экрана
				sscanf(param,"%d",&Globals.VP.Bits);
				break;
			}
			if (strcmp( name , "frequency" ) ==0){						// Частота развёртки
				sscanf(param,"%d",&Globals.VP.Hzs);
				break;
			}
			if (strcmp( name , "vsync" ) ==0){							// Вертикальная синхронизация
				if (strcmp(param,"ON")==0)	{Globals.VP.Vsync = 1;};
				if (strcmp(param,"OFF")==0)	{Globals.VP.Vsync = 0;};
				break;
			}
			if (strcmp( name , "model" ) ==0){							// Вертикальная синхронизация
				strcpy(ModelName,param);
				break;
			}
		}while (False);
	}
	while (buf!=NULL);
	return NO_ERROR;
};
bool Application::SaveConfig()
{
	return NO_ERROR;
};

bool Application::LoadResources()
{
	MSB.LoadTextures(IL_PNG,"data/textures/sky/Sky.png");

	NVLogo.init(IL_PNG,"data/textures/Logos/nv/nvlogo_ue.png");

	//rm.LOAD_Font("Courier","Courier New.OGLFont");
	rm.LOAD_Font("Console","Consolas.LFont");
	rm.LOAD_Font("ZLion","zlionheart.LFont");
//	rm.LOAD_Font("Acme","ACME.OGLFont");
//	rm.LOAD_Font("Morpheus","Morpheus.OGLFont");
	rm.SELECT_Font("Courier")->SetColor(1,1,1);

	rm.LOAD_Mesh("someMesh",ModelName);


#ifdef ENABLE_TEXTURE_RENDERING
RendTex.Init(512,512);
#endif
	return NO_ERROR;
};

bool Application::AppLoopFunc()
{
	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));
	do	{// Обработка всех сообщений
			while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (GetMessage(&msg, NULL, 0, 0))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);	
				}	
			}
		ProcessKBInput();
		switch (Globals.ERS.d)
		{
		case ERS::Draw::SCENE:
			RenderScene();
			CCons.Draw();
			break;
		case ERS::Draw::TEXTURE:
			RenderTexture();
		};
		SwapBuffersEXT();
		}
	while (	!Globals.Exiting );
	return true;// Выход
}

bool Application::Birth()
{
	CoInitializeEx(NULL,NULL);
	Globals.Exiting	=	false;
	srand(GetTickCount());
	Input::I = new Input::Input_t;
	//===[ КОНФИГУРАЦИЯ ]===========================================================================
		ReadConfig();
	//===[ КОНФИГУРАЦИЯ ]===========================================================================	
	CCons.INIT();
#ifdef WIN32
	WNDCLASSEX wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT WindowRect;

	WindowRect.left = 0L;
	WindowRect.top	= 0L;
	WindowRect.right = Globals.VP.Width;
	WindowRect.bottom = Globals.VP.Height;

	Application::Vars.App.HInstance = GetModuleHandle(NULL);
	memset(&wc,0,sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC) WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = Application::Vars.App.HInstance;
	wc.hIcon = LoadIcon(Application::Vars.App.HInstance,MAKEINTRESOURCE(IDI_ICON1));
	wc.hIconSm = LoadIcon(Application::Vars.App.HInstance,MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+2);//NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = MY_WINDOW_CLASS_NAME;

	FASSERT(RegisterClassEx(&wc));
	if(IsFullScreen)
	{
		EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&Application::Vars.win.PrevDS);
		memset(&Application::Vars.win.CurrDS, 0, sizeof(Application::Vars.win.CurrDS));
		Application::Vars.win.CurrDS.dmSize = sizeof(Application::Vars.win.CurrDS);
		Application::Vars.win.CurrDS.dmPelsWidth = Globals.VP.Width;
		Application::Vars.win.CurrDS.dmPelsHeight = Globals.VP.Height;
		Application::Vars.win.CurrDS.dmBitsPerPel = Globals.VP.Bits;
		Application::Vars.win.CurrDS.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;
		Application::Vars.win.CurrDS.dmDisplayFrequency=Globals.VP.Hzs;

		if(ChangeDisplaySettings(&Application::Vars.win.CurrDS,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if(MessageBox(0, "Cannot run in fullscreen mode!\nRun in window?",
				"Hardware trouble!", MB_YESNO|MB_ICONEXCLAMATION) == IDYES)
			{
				IsFullScreen = false;
			}
			else 
			{
				MessageBox(0, "OK!!! I'm quit...", "!!!", MB_OK | MB_ICONQUESTION);
				PostQuitMessage(1);
				return NO_ERROR;
			};
		};
	};

	if(IsFullScreen)
	{
			dwExStyle	= WS_EX_APPWINDOW/* | WS_EX_TOPMOST*/; // WS_EX_TOPMOST - Поверх всех окон
			dwStyle		= WS_POPUP;
	}
	else
	{
// ВНИМАНИЕ Здесь обнаружены глюки при использовании nView от nVidia!
			dwExStyle	= WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle		= WS_OVERLAPPEDWINDOW;
	};
	ShowCursor(false);
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);
	if (WindowRect.left < 0)
		WindowRect.right -= WindowRect.left,WindowRect.left = 0;
	if (WindowRect.top < 0)
		WindowRect.bottom -= WindowRect.top,WindowRect.top = 0;
	Application::Vars.win.MHWnd =
			CreateWindowEx(		dwExStyle, 
								MY_WINDOW_CLASS_NAME,
								MY_WINDOW_NAME,
								dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								WindowRect.left,
								WindowRect.top,
								WindowRect.right - WindowRect.left,
								WindowRect.bottom - WindowRect.top,
								NULL,
								NULL,
								GetModuleHandle(0),
								NULL
								);
	FASSERT(Application::Vars.win.MHWnd);
	ShowWindow(Application::Vars.win.MHWnd, SW_SHOW);
	SetForegroundWindow(Application::Vars.win.MHWnd);
	SetFocus(Application::Vars.win.MHWnd);

	// Получаем контекст устройства
	Application::Vars.win.DC = GetDC(Application::Vars.win.MHWnd);
		FASSERT(Application::Vars.win.DC);
	// Установка формата пикселей
		PIXELFORMATDESCRIPTOR mainPFD;
		memset(&mainPFD,0,sizeof(PIXELFORMATDESCRIPTOR));
		mainPFD.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
		mainPFD.nVersion		= 1;
		mainPFD.iPixelType		= PFD_TYPE_RGBA;
		mainPFD.dwFlags			= PFD_DRAW_TO_WINDOW |PFD_DOUBLEBUFFER |PFD_SUPPORT_OPENGL;
		mainPFD.iLayerType		= PFD_MAIN_PLANE;
		mainPFD.cDepthBits		=	16;
		int PF;
		PF = ChoosePixelFormat(Application::Vars.win.DC,&mainPFD);
		FASSERT(PF);	// Выбрали!
		FASSERT(SetPixelFormat(Application::Vars.win.DC,PF,&mainPFD));			// Поставили!
	// Создание контекста рендера
		Application::Vars.win.RC = wglCreateContext (Application::Vars.win.DC);
		FASSERT(Application::Vars.win.RC);
	// Установка полученого контекста главным
		FASSERT(wglMakeCurrent (Application::Vars.win.DC, Application::Vars.win.RC));
#else
		dpy = XOpenDisplay(0);						// Открываем дисплей по-умолчанию (может быть сетевой)
		vi = glXChooseVisual(	dpy,				// Получаем подходящий визуал
								DefaultScreen(dpy),
								attributeList);
		cx = glXCreateContext(dpy, vi, 0, GL_TRUE);	// Создание контекста
		// create	a color	map
		cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
				 vi->visual, AllocNone);
		// Создаём окно
		swa.colormap = cmap;
		swa.border_pixel = 0;
		swa.event_mask = StructureNotifyMask;
		win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, 100, 100,
			  0, vi->depth,	InputOutput, vi->visual,
			  CWBorderPixel|CWColormap|CWEventMask,	&swa);
		XMapWindow(dpy, win);
		XIfEvent(dpy, &event, WaitForNotify, (char*)win);

		
		glXMakeCurrent(dpy, win, cx);				// Привязка контекста к окну
#endif
		GetSysInfo();
#ifndef	_DEBUG
		if (AVIp.Open("data/VIDEO/logo.avi") == FJC_NO_ERROR)
		{
			AVIp.Caching();
			GT.Start();
			ShowMovie(true);
			AVIp.CloseAVI();
		}
		else
		{
			LF.Logf("Birth","Невозможно загрузить и проиграть заставку");
		}
#endif
	// Инициализация библиотек и глобальных объектов
		InitOpenGL();						// Графика - OpenGL
		LF.Log("BIRTH","Init OpenGL complete");
		InitOpenIL();						// Поддержка изображений - OpenIL(DevIL)
		LF.Log("BIRTH","Init OpenIL complete");
// инициализация менеджера ресурсов		
		rm.INIT("Consolas.LFont","default.png","cube.lwo");
		CCons.SetFont(rm.SELECT_Font("Console"));

		SimpleLogo.init(&SwapBuffersEXT,IL_PNG,"data/textures/Logos/NewLOGO.PNG");
		SimpleLogo.render_logo(FJC_STARTUP_LOGO_MODE_BEGIN);
		rm.SELECT_Font("Courier")->SetColor(1,0,0);
		SimpleLogo.render_logo(	FJC_STARTUP_LOGO_MODE_PROCEED_WITH_TEXT,1,
								TextBlock(rm.SELECT_Font("Consolas"),"Now loading, please wait",250.0f,300.0f));
		Sound.Init();
		TestBuf.Init();
		TestBuf.LoadWav("data/sounds/KDE_Startup_new2.wav");
		TestSource.Init();
		TestSource.LinkWithBufer(&TestBuf);
		LF.Log("BIRTH","Init OpenAL complete");
		InitMATH();							// Математическая библиотека
		LF.Log("BIRTH","InitMATH");
		LF.Msg("Init complete");
		LoadResources();
		PS.INIT();
	return NO_ERROR;
};

bool Application::Suicide()
{
	Sound.DeInit();
	TestBuf.DeInit();
	TestSource.DeInit();
	rm.DEINIT();
#ifdef WIN32
	if (Application::Vars.win.RC)											
	{	wglMakeCurrent(NULL, NULL);			// This frees our rendering memory and sets everything back to normal
		wglDeleteContext(Application::Vars.win.RC);	// Delete our OpenGL Rendering Context	
	}
	if (Application::Vars.win.DC)		// Release our HDC from memory
		ReleaseDC(Application::Vars.win.MHWnd, Application::Vars.win.DC);
	if (IsFullScreen)						// If we were in full screen
	{	ChangeDisplaySettings(&Application::Vars.win.PrevDS,0);	// If So Switch Back To The Desktop
		ShowCursor(TRUE);					// Show Mouse Pointer
	}
	DestroyWindow(MainApplication->Vars.win.MHWnd);
	UnregisterClass(MY_WINDOW_CLASS_NAME, Application::Vars.App.HInstance);// Free the window class
#else
	
	glXMakeCurrent		( dpy,win,NULL );
	glXDestroyContext	( dpy,	cx	);
#endif
	CCons.DEINIT();
	delete Input::I;
LF.End(true);
	CoUninitialize();
	return NO_ERROR;
};

bool Application::Work()
{
	GT.Start();
	AppLoopFunc();			// Вызов цикла обработки сообщений
	return NO_ERROR;
};

bool Application::ReshapeWindow(int width, int height)
{
	if (height==0)	height=1;		// Чтобы избежать деления на ноль
	glViewport(0,0,width,height);	// Создание "вюпорта"
	Globals.VP.Width=width;	Globals.VP.Height=height;

	glMatrixMode(GL_PROJECTION);	// Матрица проэкций
	glLoadIdentity();				// Загрузка единичной матрицы
	// Умное создание Frustum'a (или глупое - кому как)
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.5f,10000.0f);
	//glFrustum(,,,,,);

	glMatrixMode(GL_MODELVIEW);		// Видовая матрица
	glLoadIdentity();				// Загрузка единичной матрицы
	return NO_ERROR;
};
void Application::ProcessKBInput()
{
	if (Input::I->process(VK_OEM_3))
		CCons.ChangeState();

	if (CCons.Visible())	return;		// Если консоль видима, оставляем все кнопочки ей

	if (Input::I->get(VK_ESCAPE))
	{
		Globals.Exiting = true;
	}

	Cam.KeybMove();
	static bool soundkey;
	//	soundkey = Input::I->get();
	soundkey = Input::I->get('R');
	if ((soundkey)&&(!TestSource.NowPlaying()))
	{
		TestSource.Play();
	}
	else if (!soundkey)
	{
		TestSource.Stop();
	}

	if (Input::I->process('M')) 
	{
		TextBenchmark = !TextBenchmark;
	}

	if (Input::I->process(VK_F2))
	{
		if (Globals.ERS.m ==	ERS::Mesh::NORMAL)
			Globals.ERS.m =		ERS::Mesh::WIRE;
		else
			Globals.ERS.m =		ERS::Mesh::NORMAL;
	}
	if (Input::I->process(VK_F1))
	{
		Globals.EFl.show_stat	=	!Globals.EFl.show_stat;
		//Globals.EFl.show_gui	=	!Globals.EFl.show_gui;
	}
	if (Input::I->process(VK_F3))
	{
		Globals.EFl.LWOCullFace	=	!Globals.EFl.LWOCullFace;
	}
	if (Input::I->get('Q')) 
	{
		PS.SetWindStatus(true);
	}
	else
	{
		PS.SetWindStatus(false);
	}
	if (Input::I->process(VK_UP))
	{
		PS.grav = PS.grav / 1.05f;
	}
	if (Input::I->process(VK_DOWN))
	{
		PS.grav = PS.grav * 1.05f;
	}
	if (Input::I->process(VK_LEFT))
	{
		PS.wind.d.c.x = PS.wind.d.c.x + 50.0f;
	}
	if (Input::I->process(VK_RIGHT))
	{
		PS.wind.d.c.x = PS.wind.d.c.x - 50.0f;
	}

};


LRESULT CALLBACK WndProc(	HWND hWnd,
							UINT uMsg,
							WPARAM wParam,
							LPARAM lParam)
{
	static LRESULT    lRet = 0; 
	
	switch (uMsg)
	{
	case WM_SIZE:		// Событие: Изменение размера
		if(!MainApplication->IsFullScreen)			// Если приложение работает в окне
		{MainApplication->ReshapeWindow(LOWORD(lParam), HIWORD(lParam));}
		break; 
//Управляющие события
	case WM_LBUTTONDOWN:
		//if ()
		Input::I->KeyDown	(VK_LBUTTON);
		break;
	case WM_LBUTTONUP:
		Input::I->KeyUp		(VK_LBUTTON);
		break;
	case WM_RBUTTONDOWN:
		Input::I->KeyDown	(VK_RBUTTON);
		break;
	case WM_RBUTTONUP:
		Input::I->KeyUp		(VK_RBUTTON);
		break;
	case WM_MBUTTONDOWN:
		Input::I->KeyDown	(VK_MBUTTON);
		break;
	case WM_MBUTTONUP:
		Input::I->KeyUp		(VK_MBUTTON);
		break;
	case WM_XBUTTONUP:
		switch (HIWORD(wParam)){
		case XBUTTON1:	Input::I->KeyDown	(VK_XBUTTON1);	break;
		case XBUTTON2:	Input::I->KeyDown	(VK_XBUTTON2);	break;
		}; break;
	case WM_XBUTTONDOWN:
		switch (HIWORD(wParam)){
		case XBUTTON1:	Input::I->KeyUp		(VK_XBUTTON1);	break;
		case XBUTTON2:	Input::I->KeyUp		(VK_XBUTTON2);	break;
		}; break;
	case WM_MOUSEWHEEL:
		static short offset;
		offset = HIWORD(wParam);
		Input::I->wheel += offset/WHEEL_DELTA;
		break;

	case WM_KEYDOWN:
		Input::I->KeyDown	(wParam);
		MainApplication->CCons.ProcessKey((DWORD)wParam);
		break;
	case WM_KEYUP:
		Input::I->KeyUp		(wParam);
		break;

	case WM_CHAR:
		MainApplication->CCons.ProcessChar((char)(wParam & 0x000000FF));
		break;

	case WM_CLOSE:										// Закрываемся уже
		Globals.Exiting = true;
		break;
	default:											// Return by default
		lRet	=	DefWindowProc (hWnd, uMsg, wParam, lParam);
		break;
	}

	return lRet;										// Return by default
};

void SwapBuffersEXT()
{
#ifdef WIN32
	SwapBuffers(MainApplication->Vars.win.DC);
#else
	glXSwapBuffers(MainApplication->dpy,MainApplication->win);
#endif
}

void Application::RenderTexture()
{
	glPushAttrib(GL_TEXTURE_BIT|GL_ENABLE_BIT|GL_LIGHTING_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);							// Чёрный фон
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Очистка буфера цвета
	glDepthFunc(GL_NOTEQUAL);										// Настроим глубину
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);

	glLoadIdentity();
	glOrtho(0,1,0,1,-1,1);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glBegin(GL_QUADS);		// Отрисовываем текущую текстурку
		glTexCoord2d(0,1);		glVertex2d(0,1);
		glTexCoord2d(1,1);		glVertex2d(1,1);
		glTexCoord2d(1,0);		glVertex2d(1,0);
		glTexCoord2d(0,0);		glVertex2d(0,0);
	glEnd();

	glPopAttrib();

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void	Application::ShowMovie(bool	CanBreak)
{
	MSG msg;
	ZeroMemory(&msg,sizeof(MSG));
	do
	{// Обработка всех сообщений
			while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (GetMessage(&msg, NULL, 0, 0))
				{
					//TranslateMessage(&msg);
					DispatchMessage(&msg);	
				}	
			}
		GT.NewFrame();
		AVIp.GrabAVIFrame();
		RenderTexture();
		SwapBuffersEXT();
		if (CanBreak)
		{
			if (Input::I->get(' ')		||
				Input::I->get(VK_RETURN)||
				Input::I->get(VK_ESCAPE))
			{
				break;
			}
		}
	}
	while (	!AVIp.End());
}
