#include "LocusAFX.h"
#include "./ResMan.h"

ResMan	rm;							// Объект менеджера ресурсов (настоящий)

ResMan::ResMan()
{
}
ResMan::~ResMan()
{
}

void		ResMan::INIT(	char*	DefFont,
							char*	DefTexture,
							char*	DefMesh)
{
	this->LOAD_Texture	("defaultTXTR",DefTexture);
	this->defTexture	=	this->SELECT_Texture("defaultTXTR");
	this->LOAD_Font		("defaultFONT",DefFont);
	this->defFont		=	this->SELECT_Font	("defaultFONT");
	this->LOAD_Mesh		("defaultMESH",DefMesh);
	this->defMesh		=	this->SELECT_Mesh	("defaultMESH");
}
void		ResMan::DEINIT(void)
{
	mapH.clear();		//	Очистка массива высокого уровня
	while	(!mapL.empty())
	{
		mapLi = mapL.begin();	// Получаем адрес начала словаря
		ResourceObject RO = mapLi->second;
		switch	(RO.RType)
		{
		case	Res_FONT:
			if (RO.Res!=NULL)
			{
				PFont(mapLi->second.Res)->ULOAD();
				delete (PFont)RO.Res;
			}
			else FASSERT(RO.Res);
			break;
		case	Res_TEXTURE:
			if (RO.Res!=NULL) delete (TexturePointer)RO.Res;
			else FASSERT(RO.Res);
			break;
		case	Res_MESH:
			if (RO.Res!=NULL)
			{
				meshPointer(mapLi->second.Res)->ULoad();
				delete (meshPointer)RO.Res;
			}
			else FASSERT(RO.Res);
			break;
		}
		mapL.erase(mapLi);
	}
	mapL.clear();		//	Очистка массива низкого уровня, хотя он и должен быть пустым
}

void	ResMan::LOAD_Font(char* ResName,char* FontName)
{
	PFont	F;
	Text::RESULT res;
	mapLi = mapL.find(string(FontName));			// Поиск елемента в словаре (Есть ли он там?)
	if (mapLi != mapL.end())
	{
		mapLi->second.used++;
		mapHi = mapH.find(string(ResName));
		if (mapHi != mapH.end())
		{
			LF.Logf("ResourceManager","Шрифт с именем %s уже загружен",ResName);
			return;
		}
		mapH[string(ResName)].Res = mapLi->second.Res;
		return;
	}
	else
	{
		char	buf[512];
		strcpy(buf,Globals.RES.FONT_DIR);
		strcat(buf,FontName);
		ResourceObject	RO;
		F			= new Font;
		res = F->LOAD(buf);
		if (res == Text::OK)
		{
			RO.RType	=	Res_FONT;
			RO.used		=	1;
			RO.Res		=	(void*)F;
			mapL[string(FontName)] = RO;
			LF.Logf("ResourceManager","Loaded font \"%s\"",ResName);
		}
	}
	mapHi = mapH.find(string(ResName));
	if (mapHi != mapH.end())
	{
		LF.Logf("ResourceManager","Шрифт с именем %s уже загружен",ResName);
		return;
	}
	if (res == Text::OK)
	{
		mapH[string(ResName)].Res				=	F;
	}
	else
	{	//	Загрузка не удалась, потому пишем указатель на стандартный объект
		mapH[string(ResName)].Res = defFont;
		LF.Logf("ResourceManager","Не найден шрифт %s",FontName);
	}
}

PFont	ResMan::SELECT_Font(char* ResName)
{
	if (IsBadReadPtr(ResName,1))	return defFont;
	mapHi = mapH.find(string(ResName));
	if (mapHi == mapH.end())
		return defFont;
	return	(PFont)mapHi->second.Res;
}
void	ResMan::ULOAD_Font(char* ResName)
{
	mapHi	=	mapH.find(string(ResName));
	if (mapHi	==	mapH.end())
	{
		return;
	}
	FindRes_ret_t	ret	=	FindRes(mapHi);
	mapLi	=	ret.data;
	mapH.erase(mapHi);
	if (ret.need_pass	==	true)
	{
		return;	//	Не удалять стандартный ресурс
	}
	if (mapLi->second.used > 1)
	{
		mapLi->second.used --;
	}
	else
	{
		if (mapLi->second.Res != defFont)
		{
			PFont(mapLi->second.Res)->ULOAD();
			delete	(PFont)(mapLi->second.Res);
		}
		mapL.erase(mapLi);
	}
}

void	ResMan::LOAD_Texture(char *ResName, char *FileName)
{
	TexturePointer	T;
	bool res;
	mapLi = mapL.find(string(FileName));			// Поиск елемента в словаре (Есть ли он там?)
	if (mapLi != mapL.end())
	{
		mapLi->second.used++;
		mapHi = mapH.find(string(ResName));
		if (mapHi != mapH.end())
		{
			LF.Logf("ResourceManager","Текстура с именем %s уже загружена",ResName);
			return;
		}
		mapH[string(ResName)].Res = mapLi->second.Res;
		return;
	}
	else
	{
		char	buf[512];
		strcpy(buf,Globals.RES.TEXTURE_DIR);
		strcat(buf,FileName);
		ResourceObject	RO;
		T			= new TextureClass;
		res = T->Load(buf);
		if (res == false)
		{
			RO.RType	=	Res_TEXTURE;
			RO.used		=	1;
			RO.Res		=	(void*)T;
			mapL[string(FileName)] = RO;
			LF.Logf("ResourceManager","Загружена текстура \"%s\"",ResName);
		}
	}
	mapHi = mapH.find(string(ResName));
	if (mapHi != mapH.end())
	{
		LF.Logf("ResourceManager","Текстура с именем %s уже загружена",ResName);
		return;
	}
	if (res == false)
	{
		mapH[string(ResName)].Res = T;
	}
	else
	{	//	Загрузка не удалась, потому пишем указатель на стандартный объект
		LF.Logf("ResourceManager","Не найдена текстура %s",FileName);
		mapH[string(ResName)].Res				=	defTexture;
	}
}
TexturePointer	ResMan::SELECT_Texture(char* ResName)
{
	if (IsBadReadPtr(ResName,1))	return defTexture;
	mapHi = mapH.find(string(ResName));
	if (mapHi == mapH.end())
		return defTexture;
	return	(TexturePointer)mapHi->second.Res;
}
void	ResMan::ULOAD_Texture(char *ResName)
{
	mapHi	=	mapH.find(string(ResName));
	if (mapHi	==	mapH.end())
	{
		return;
	}
	FindRes_ret_t	ret	=	FindRes(mapHi);
	mapLi	=	ret.data;
	mapH.erase(mapHi);
	if (ret.need_pass	==	true)
	{
		return;
	}
	if (mapLi->second.used > 1)
	{
		mapLi->second.used --;
	}
	else
	{
		if (mapLi->second.Res != defTexture)
		{
			delete	(TexturePointer)(mapLi->second.Res);
		}
		mapL.erase(mapLi);
	}
}
//===[ MESHES ]==============================================================================================
void	ResMan::LOAD_Mesh(char *ResName, char *FileName)
{
	Mesh::RESULT res;
	meshPointer	M = 0;
	mapLi = mapL.find(string(FileName));			// Поиск елемента в словаре (Есть ли он там?)
	if (mapLi != mapL.end())
	{
		mapLi->second.used++;
		mapHi = mapH.find(string(ResName));
		if (mapHi != mapH.end())
		{
			LF.Logf("ResourceManager","Меш с именем %s уже загружен",ResName);
			return;
		}
		mapH[string(ResName)].Res = mapLi->second.Res;
		return;
	}
	else
	{
		char	buf[512];
		strcpy(buf,Globals.RES.MESH_DIR);
		strcat(buf,FileName);
		ResourceObject	RO;
		M			= new Mesh::mesh;
		res = M->Load(buf);
		if (res == Mesh::OK)
		{
			RO.RType	=	Res_MESH;
			RO.used		=	1;
			RO.Res		=	(void*)M;
			mapL[string(FileName)] = RO;
			LF.Logf("ResourceManager","Loaded mesh \"%s\" из %s",ResName,FileName);
		}
	}
	mapHi = mapH.find(string(ResName));
	if (mapHi != mapH.end())
	{
		LF.Logf("ResourceManager","Меш с именем %s уже загружен",ResName);
		return;
	}
	if (res == Mesh::OK)
	{
		mapH[string(ResName)].Res = M;
	}
	else
	{	//	Загрузка не удалась, потому пишем указатель на стандартный объект
		LF.Logf("ResourceManager","Файл %s не найден!!! Используем стандартный ресурс",FileName);
		mapH[string(ResName)].Res				=	defMesh;
	}
}

meshPointer	ResMan::SELECT_Mesh(char* ResName)
{
	if (IsBadReadPtr(ResName,1))	return defMesh;
	mapHi = mapH.find(string(ResName));
	if (mapHi == mapH.end())
		return defMesh;
	return	(meshPointer)mapHi->second.Res;
}
void	ResMan::ULOAD_Mesh(char *ResName)
{
	mapHi	=	mapH.find(string(ResName));
	if (mapHi	==	mapH.end())
	{
		return;
	}
	FindRes_ret_t	ret	=	FindRes(mapHi);
	mapLi	=	ret.data;
	mapH.erase(mapHi);
	if (ret.need_pass	==	true)
	{
		return;	//	Не удалять стандартный ресурс
	}
	if (mapLi->second.used > 1)
	{
		mapLi->second.used --;
	}
	else
	{
		if (mapLi->second.Res != defMesh)
		{
			meshPointer(mapLi->second.Res)->ULoad();
			delete	(TexturePointer)(mapLi->second.Res);
		}
		mapL.erase(mapLi);
	}
}

FindRes_ret_t	ResMan::FindRes(ResMapH_Iter_t	mapHi)
{
	for(ResMapL_Iter_t	i=mapL.begin();i != mapL.end();	i++)
	{
		if ((i->second.Res	==	this->defFont)||
			(i->second.Res	==	this->defMesh)||
			(i->second.Res	==	this->defTexture))
			return FindRes_ret_t(true,i);
		if (mapHi->second.Res == i->second.Res)
			return FindRes_ret_t(false,i);
	}
	__asm	INT	3;
}
