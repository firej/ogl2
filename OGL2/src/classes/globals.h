#pragma once

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE
#define _WIN32_WINNT				0x0500
#endif
//#define ENABLE_TEXTURE_RENDERING

#include "./shared/mymath.h"
#include "./shared/ASSERT.h"
#include "./files.h"

extern	LogFile	LF;								// Лог-файл
extern	CFGFile	CF;								// Файл с настройками

#define	FJC_ERROR		true
#define	FJC_NO_ERROR	false


// Мои Define'ы
	// Различные дополнительные возможности
#define HALF_SECOND_FPS_REFRESH					// Привязка времени обновления FPS к 0,5 секунды
#define TIME_TO_CALCULATE_CPU_SPEED 200
// #define DOUBLE_TIME_PRESISION
// Просто символьные константы
#define INPUT_MODE_FPS_GAME				0
#define INPUT_MODE_AUTOSYM_GAME			1
#define INPUT_MODE_MENU					2
#define INPUT_MODE_CONSOLE				3

#define	FJC_TEX_NO_FILTERING			0
#define FJC_TEX_ANISOTROPIC_FILTERING	1
#define FJC_TEX_BILINEAR_FILTERING		2
#define FJC_TEX_TRILINEAR_FILTERING		3

#define CAM_LOOKTO_MODE_VECTOR			0					// Задание направления через вектор направления
#define CAM_LOOKTO_MODE_POINT			1					// --//-- точку на которую толжная указывать камера
#define CAM_LOOKTO_MODE_ANGLES			2					// Задание углов поворота камеры (наибыстрейший метод)
#ifndef FJ_INPUT_ENABLED
#define CAM_MOVE_FORWARD				0					// Камера двигается вперёд
#define CAM_MOVE_BACKWARD				1					// Камера двигается назад
#define CAM_MOVE_LEFT					2					// Камера двигается влево
#define CAM_MOVE_RIGHT					3					// Камера двигается вправо
#endif

// Мои типы
typedef void (*SwapBuffersFunction)(void);
typedef void (*KeyFunc)(void);								// Тип указателя на косольную функцию

#ifndef WIN32
typedef unsigned int				UINT;
typedef unsigned char				BYTE;
typedef unsigned char				bool;
typedef unsigned long long			QWORD
#if		sizeof(UINT) == 4
typedef unsigned int				DWORD
#elif	sizeof(unsigned long int) == 4
typedef unsigned long int			DWORD
#else
#error Could not find bilt-in type for DWORD
#endif
#endif

struct CameraSet								// Настройки камеры
{
	double	ForwardSpeed,						// Скорость перемещения вперёд
			BackwardSpeed,						// Скорость перемещения назад
			StrafeSpeed;						// Скорость стрейфа

	double	Sensitivity;						// Чувствительность мыши
	Point3d	CamStartPosition;					// Стартовая позиция камеры
};
extern CameraSet	CamS;						// Реальность

namespace EStates
{
	enum	type
	{
		RENDER_3D,
		RENDER_2D,
		PLAY_CLIP
	};
}
namespace ERS
{
	namespace Draw
	{
		enum type
		{
			SCENE		=	0,
			TEXTURE		=	1,
			CONSOLE		=	2
		};
	}
	namespace Mesh
	{
		enum type
		{
			NORMAL		=	0,
			WIRE		=	1,
			HALFWIRE	=	2
		};
	}
}
namespace	AR							// Aspect Ratio  - соотношение сторон экрана
{
	enum	type
	{
		NORMAL		=	0,				// 4:3
		AR4_3		=	0,
		WIDE_HDTV	=	1,				// Широкий экран стандарта HTDV или с такимиже пропорциями
		AR16_9		=	1,
		NORMAL_LCD	=	2,				// Стандарт для LCD панелей с азрешением 1280x1024
		AR1280_1024	=	2,
		WIDE_LDC	=	3,				// Широкий экран LCD (ноутбуки и некоторые мониторы)
		AR16_10		=	3,				// с соотношением сторон 16:10 или разрешением 1280x800
		AR1280_800	=	3
	};
}
struct GlobalsStruct								// Глобальные настройки и переменные
{
	struct
	{
		bool		IsFullScreen;					// Индикатор полноэкранного режима
		int			Width,Height;					// Ширина и высота окна
		int			Bits,Hzs;						// Глубина цвета и развёртка экрана
		int			Vsync;							// Состояние вертикальной синхронизации (1 - включено)
		UINT		ScreenStrings;					// Количество строк на экране
		AR::type	AR;								// соотношение сторон области вывода
	}	VP;
	unsigned char	TextureFiltering;				// Тип фильтрации текстур
//	unsigned int	lists;							// Количество задействованых списков (попутно и номер последнего)
	struct	tagRESstruct{
		char	TEXTURE_DIR	[128];					// Имена директорий, где расположены ресурсы
		char	FONT_DIR	[128];
		char	MESH_DIR	[128];
	}	RES;
	EStates::type				es;
	struct	tagERSs
	{
		ERS::Draw::type	d;
		ERS::Mesh::type	m;
	}	ERS;
	struct	tagEFlags
	{
		bool	show_stat;
		bool	show_gui;
		bool	LWOCullFace;					// Удаление оборотных граней LWO моделей
	}			EFl;
	bool	Exiting;
};
extern GlobalsStruct			Globals;		// Глобальные настройки и переменные

struct KeySettings								// Структура содержащая информацию о функциональных клавишах
{
	// Клавиши управления камерой
	WPARAM CamMoveForward;						// Перемещение вперёд
	WPARAM CamMoveBackward;						// Перемещение назад
	WPARAM CamMoveLeft;							// Перемещение влево
	WPARAM CamMoveRight;						// Перемещение вправо
	WPARAM CamBackToCenter;						// Перемещение в начальную точку
	WPARAM MoreFastMove;						// Ускорение

};
extern KeySettings				KeySet;			// Структура содержащая информацию о функциональных клавишах

// Структура с информацией о видеокарте
struct VCInfo									// Информация о видео подсистеме
{
	int		MaxAnisotropy;						// Максимально поддерживаемя анизотропная фильтрация
	char	*SupportedGlExtentions;				// Поддерживаемые OpenGL разширения видеокарты
	char	*RendererName;
	char	*VendorName;
	char	*OpenGL_Version;
};
struct CPUInfo
{
	CPUInfo()
	{
		memset(this,0,sizeof(CPUInfo));
	}
	int	speed;								// Скорость процессора в МГц
	char	vendor_id	[13];
	char	id			[64];
	char	name		[64];
	char	SSE;
	char	SSE2;
	char	AMD_3DNow;
	char	AMD_3DNow_Ex;
	char	RDTSC;
	char	MMX;

};
// Структура содержащая информацию о системе в которой запускается приложение
struct SysInformationData
{
	VCInfo	video;								// Информация о видео подсистеме
	CPUInfo	cpu;								// Инфа о процессоре
};

extern SysInformationData		SysInfo;		// Инцормация о системе
