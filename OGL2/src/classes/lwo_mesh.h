#ifndef LWO_MESH_H
#define LWO_MESH_H

#include "./globals.h"
#include "./texture.h"
// Вектор контейнер для слоёв
#include <vector>
#include <map>
#include <string>
#include <set>
using namespace std;
namespace Mesh
{
	typedef	enum
	{
		OK,
		FILE_NOT_FOUND,
		NO_MEMORY,
		INVALID_FORMAT,
		NO_LISTS
	} RESULT;

	typedef	struct	lwSurface_tt
	{
		lwSurface_tt()
		{
			DoubleSided		= false;
			texture			= false;
			Smoothing		= false;
		}
		string			name;			// Имя материала
		string			AncestorName;	// Имя родительского материала (не учитываем)
		Color4f			color;			// Общий цвет
		Color4f			Diffuse;		// Диффузный цвет
		Color4f			Specular;		// Спекулярная состовляющая
		bool			DoubleSided;	// Флаг двухсторонности полигона
		bool			texture;		// Определяет валидность следующих далее полей
		TexturePointer	Texure;			// Указатель текстуры
		int				imageIndex;		// Индекс текстуры в массиве clipVector
		bool			Smoothing;		// Флаг сглаживания (сглаживаются ли нормали)
		float			nSmoothAngle;	// Маскимальный угол для сглаживания (больше уже не сглаживается)
	}	lwSurface_t	;

	typedef	struct clipElement_tt
	{
		string			Name;
		TexturePointer	LoadedTexture;
	}	clipElement_t;

	typedef	vector	<clipElement_t>	clipVector_t;
	typedef	vector	<lwSurface_t>	lwSurfList_t;

	typedef	struct tag_tt
	{
		string		tn;
		WORD		iSurf;
	}	tag_t;
	typedef	vector<tag_t>		tag_v_t;

	typedef	struct lwTagList_tt
	{
	//	int				count;
	//	int				offset;					// only used during reading
		tag_v_t			tag;					// array of strings
	}	lwTagList_t	;

	typedef	struct Poly_tt
	{
		Poly_tt()
		{
			v		=	0;
			vi		=	0;
			ns		=	0;
			iSurf	=	0;
		}
		WORD		v;						// Количество вершин на полигоне
		WORD		*vi;					// Массив индексов вершин
		bool		*ns;					// Массив флагов сглаженности нормали
		// если true - нормаль берётся у текущей точки иначе у полигона
		WORD		iSurf;					// Индекс сурфейса
		Vector3f	normal;					// Нормаль полигона
	}	Poly_t	;

	typedef	struct BoundingBox_tt
	{
		Point3f		a;
		Point3f		b;
	}	BoundingBox_t	;

	typedef	struct lwVertex_tt
	{
		Point3f		v;						// Координаты текущей вершины
		Point3f		n;						// Нормаль у текущей вершины
	}	lwVertex_t	;

	typedef	struct lwLayer_tt
	{
		WORD			number;
		WORD			flags;
		WORD			pivot;
		string			name;
		Point3f			PivotPoint;			// Главная точка слоя (от неё отсчёт)
		BoundingBox_t	*bbox;				// Опциональный ббокс
		lwVertex_t	*	Vertexes;			// Массив вершин
		DWORD			iVertexes;			// Количество вершин
		Poly_t		*	Pols;				// Массив полигонов
		DWORD			iPols;				// Количество полигонов
	}	lwLayer_t	;

	typedef vector<lwLayer_t>	lwLayers;
//mmm Сглаживание нормалей mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
	typedef	struct NormalSmoothPolygonPointer_tt
	{
		NormalSmoothPolygonPointer_tt()
		{
			s = false;
		}
		NormalSmoothPolygonPointer_tt(Poly_t * pp)	// Специальный конструтор
		{
			p = pp;
			s = false;
		}
		Poly_t	*	p;		// Указатель на полигон
		bool		s;		// Флаг сглаживания
		WORD		np;		// Номер вершины в полигоне
		// значит что этот полигон должен использовать сглаженную нормаль по этой вершине
	}	NormalSmoothPolygonPointer_t;
	typedef	struct NormalSmoothVertex_tt
	{
		// Массив полигонов, которым принадлежит данная вершина
		vector<NormalSmoothPolygonPointer_t>	polys;
		DWORD									n;		// Номер текущей вершины
		Vector3f								normal;	// результирующая нормаль
	}	NormalSmoothVertex_t;

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//mmm Класс mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
	class LWOMesh
	{
	private:
		// Данные геометрии
		lwLayers		Layers;						// Массив слоёв
		DWORD			iLayers;					// Счётчик слоёв
		GLuint			ListID;
		// Данные материалов и т. д.
		lwTagList_t		TagList;					// Массив тэгов
		// Данные объекта
		Point3f			Translate;					// Смещение, поворот и масштаб
		Point3f			Rotate;
		Point3f			Scale;
		DWORD			Pols;
		DWORD			Points;
		GLuint			list;
		bool			compiled;
		clipVector_t	clipVector;					// Список всех имён текстур
	public:		lwSurfList_t	SurfList;			// Список сурфейсов
		WORD			iSurfaces;					// Количество сурфейсов

		//material_t	*	material;				// Массив материалов
		//int				mi;						// Индекс материала
		//Point3f		*	vertexes;				// Массив вершин
		//Point2f		*	tc;						// Текстурные координаты
		//Mesh::Poly_t	*	Pols;					// Массив полигонов
		//DWORD			iPols;						// Количество полигонов
	// Читалки чанков
	private:
		bool	ReadPNTSChunk		(	FILE	*	f	);
		bool	ReadPOLSChunk		(	FILE	*	f	);
		bool	ReadLAYRChunk		(	FILE	*	f	);
		bool	ReadTAGSChunk		(	FILE	*	f	);
		bool	ReadSURFChunk		(	FILE	*	f	);
		bool	ReadPTAGChunk		(	FILE	*	f	);
		bool	ReadCLIPChunk		(	FILE	*	f	);
	// Читалки субчанков
		WORD	ReadSURFCOLRSubChunk(	FILE	*	f	);
		WORD	ReadSURFDIFFSubChunk(	FILE	*	f	);
		WORD	ReadSURFSPECSubChunk(	FILE	*	f	);
	// Постобработка файла - сортировка сурфейсов, запоминание индексов и т. д. и т. п.
		void	PostLoadProcessing	(	void			);
		void	DrawFromSource		(	void			);
		void	CompileList			(	void			);
	public:
		LWOMesh			();
		Mesh::RESULT	Load		(char	*FileName);
		GLvoid			ULoad		(GLvoid);
		GLvoid			Render		(GLvoid);
		WORD			GetLayers	();
		DWORD			GetPoints	();
		DWORD			GetPolygons	();
	};

	typedef	Mesh::LWOMesh	mesh;
};
typedef Mesh::LWOMesh* meshPointer;
#endif //#ifndef LWO_MESH_H