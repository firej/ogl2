#ifndef SIMPLE_MESH_H
#define SIMPLE_MESH_H

#include "./globals.h"
#include "./oGL.h"
#include "./texture.h"

int glLoadModel(char *filename,GLuint *ListNum);

namespace Mesh
{
	enum	RESULT
	{
		OK,
		FILE_NOT_FOUND,
		NO_MEMORY,
		INVALID_FORMAT,
		NO_LISTS
	};

	struct	material_t
	{
		TexturePointer	tex;						// Указатель на текстуру
		char			TextureName[128];			// Имя текстуры (для RM)
		Vector3f		ambient;					// Составляющие цвета объекта
		Vector3f		diffuse;
		Vector3f		specular;
		GLfloat			shine;
		material_t		()
		{ memset (this,0,sizeof(material_t)); };
	};

	class simpleMesh
	{
	private:
		GLuint			ListID;
		GLuint			mat_q;						// Количество задейтвованных текстур
		Point3f			Translate;					// Смещение, поворот и масштаб
		Point3f			Rotate;
		Point3f			Scale;
		material_t*		material;					// Массив материалов
	public:
		simpleMesh			();
		Mesh::RESULT	Load	(char	*FileName);
		GLvoid			ULoad	(GLvoid);
		GLvoid			Render	(GLvoid);
	};
};


#endif //#ifndef SIMPLE_MESH_H