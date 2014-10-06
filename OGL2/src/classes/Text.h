#ifndef TEXT_H
#define TEXT_H

//OpenGL Header Files
#include "./globals.h"
#include "./texture.h"

#define	DISPLAY_LINES			40.0
#define	STRING(str_number)		(str_number-1)/DISPLAY_LINES

#ifndef _WIN32						// переобъявление вне форточек
typedef struct _ABC { 
  int     abcA; 
  UINT    abcB; 
  int     abcC; 
} ABC, *PABC;
#endif

namespace Text
{
	enum	Align
	{
		left,						// Выравнивание по горизонтали по левому краю
		center,						// по центру (h/v)
		right,						// по правому краю
		top,						// по вертикали по верху
		bottom						// по низу
	};
	enum	RESULT
	{
		OK,
		FILE_NOT_EXIST,
		BAD_FORMAT
	};
};
struct	LFontGlyph_t
{
	float	A;			// Смещение до символа
	float	B;			// Смещение после символа
};
	class Font
	{
	protected:
//		bool			list_builded	;	// Флаг наличия созданных списков
		Text::Align		ha				;	// Горизонтальное выравнвание
		Text::Align		va				;	// Вертикальное
		GLuint			Base			;	// Начало списков с буковками
		Vector3f		Color			;	// Цвет шрифта
		float			alfa			;	// Прозрачность
		LFontGlyph_t	lfg[256]		;	// Массив с информацией о нашем шрифте
		double			Size			;	// Размер шрифта (относительный)
	TexturePointer		T				;	// Текстура шрифта
	static	GLdouble	scr_width		;	// Зависит от соотношений сторон экрана
	public:
		Font(void);
		~Font(void);
static	void	SetAspectRatio();			// Установка соотношений сторон экрана
		void	Print	(GLdouble X, GLdouble Y, const char *fmt, ...);	// Печатать текст
Text::RESULT	LOAD	(const char *name);	// Грузить шрифт
		void	ULOAD	();
		void	SetStyle(Text::Align halign, Text::Align valign, double size);
Text::Align		GetHA	(void)	{return ha;};
Text::Align		GetVA	(void)	{return va;};
		double	GetSize	(void)	{return Size;}
		void	SetColor(Vector3f C);
		void	SetColor(float r,float g,float b);
		void	SetColor(Vector3f C, float alfa);
		void	SetColor(float r,float g,float b, float alfa);
		float	GetStrWidth(const char	*	str);
	};
typedef Font* PFont;

#endif	//#ifndef TEXT_H