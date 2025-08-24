#ifndef	RESMAN_HEADER
#define	RESMAN_HEADER

#include "./globals.h"
#include "./Text.h"
#include "./Texture.h"
#include "./lwo_mesh.h"
#ifdef WIN32
#include <hash_map>
using namespace stdext;
#else
#include <unordered_map>
#define hash_map unordered_map
#endif
#include <string>
using namespace std;

/*
		Resource manadger for OGL2 project LOCUS Engine
			by FJ													*/

enum	ResourceType		// Тип ресурса
{
	Res_FONT,				// Объект - шрифта
	Res_TEXTURE,			// Объект - текстура
	Res_MESH				// Объект - простая 3D модель
};

struct	ResourceObject
{
	ResourceObject()
	{memset(this,0,sizeof(ResourceObject));};
	void*			Res;		// Указатель на абстрактный ресурс
	int				used;		// Количество использованных экземпляров
	ResourceType	RType;		// Тип ресурса
};
typedef hash_map <string,ResourceObject>	ResMapL_t;			// Словарь для сопоставления имён и функций
typedef	ResMapL_t :: iterator				ResMapL_Iter_t;		// Итератор для словаря
typedef	ResMapL_t :: const_iterator			ResMapL_cIter_t;	// Константный итератор

struct	ResPointer
{
	void*				Res;
};
class	FindRes_ret_t
{
public:
	FindRes_ret_t(bool	Need_Pass,ResMapL_Iter_t	Data)
	{	need_pass	=	Need_Pass;	data	=	Data;	}
	bool			need_pass;
	ResMapL_Iter_t	data;
};

typedef hash_map <string,ResPointer>		ResMapH_t;			// Словарь для сопоставления имён и функций
typedef	ResMapH_t :: iterator				ResMapH_Iter_t;		// Итератор для словаря
typedef	ResMapH_t :: const_iterator			ResMapH_cIter_t;	// Константный итератор


class	ResMan
{
protected:
	ResMapL_t	mapL;			// массив физического (низкого) уровня
ResMapL_Iter_t	mapLi;
	ResMapH_t	mapH;			// массив идентификаторов высшего (логического) уровня
ResMapH_Iter_t	mapHi;
	PFont		defFont;		// Набор стандартных 
TexturePointer	defTexture;
	meshPointer	defMesh;
//	int			Resources;		// Количество загруженных ресурсов
FindRes_ret_t	FindRes(ResMapH_Iter_t	mapHi);	/*	Поиск элемента в mapL, соотв-го данному ресурсу	*/
public:
	ResMan();
	~ResMan();

	void INIT				(	char*	DefFont,		// Инициализация менеджера ресурсов
								char*	DefTexture,
								char*	DefMesh);
	void DEINIT				(void);			// Конец работы менеджера ресурсов
// LOAD functions
	void			LOAD_Font		(char* ResName,char* FontName);	// Загрузка шрифта
	PFont			SELECT_Font		(char* ResName);				// Выбор шрифта
	void			ULOAD_Font		(char* ResName);
	void			LOAD_Texture	(char *ResName,char *FileName);	// Загрузка текстур
TexturePointer		SELECT_Texture	(char* ResName);
	void			ULOAD_Texture	(char* ResName);
	void			LOAD_Mesh		(char *ResName,char *FileName);	// Загрузка 3Д объектов
	meshPointer		SELECT_Mesh		(char* ResName);
	void			ULOAD_Mesh		(char* ResName);

};
extern ResMan		rm;
bool InitFonts();
bool DeInitFonts();

#endif