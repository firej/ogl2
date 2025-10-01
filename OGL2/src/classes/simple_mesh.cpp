#define	_CRT_SECURE_NO_DEPRECATE
#ifdef WIN32
#include "LocusAFX.h"
//#include "../OGL.h"
//#include <windows.h>
//#include <windowsx.h>
//#include <stdlib.h>
//#include <conio.h>
//#include <string.h>
//#include <io.h>
#else
// macOS/Linux includes
#include <cstdio>
#include <cstring>
#include <OpenGL/gl.h>
#define MAX_PATH 260
#endif
#include <stdio.h>

#include "./simple_mesh.h"
#include "./ResMan.h"

char szString[128];
// Macro for finding string in file.
/*	bool FindStr(const char *str,FILE* fHandle)
	{
		do fgets(szString, 128, fHandle);
		while((strncmp(str, szString, sizeof(str)-1))&&(!feof(fHandle)));
		//if (feof(fHandle)) return false;
		return true;
	}	*/
#define FindStr(str, fHandle)\
{ \
	do fgets(szString, 128, fHandle);\
	while((strncmp(str, szString, sizeof(str)-1))&&(!feof(fHandle)));\
}

Vector3f	GetNormal(Point3f	a,Point3f	b,Point3f	c);

//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
//===[ Simple mesh class ]===================================================================================
//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm

			Mesh::simpleMesh::simpleMesh()
{};


// This func creates OpenGL display list from .ASE file.
// It handles vertex coords, face normals, texture coords for all vertices.
// In ListNum var it returns display list index for loaded mesh.
// Now func applies material properties to mesh.
//
// You must set material and texture mapping for mesh in 3DS or this func
// will hang up.
// Спёрто у BH :)

Mesh::RESULT	Mesh::simpleMesh::Load(char *FileName)
{
	FILE *fModel;
	//struct	

	unsigned long Vertices=0,Faces=0,i,TVert=0,TFaces=0;

	GLfloat *vx,*vy,*vz;							// Coords of vertices
	GLfloat	*u,*v;									// Texture coords for vertices
	GLfloat	*nx,*ny,*nz;							// Face normals coords
	GLfloat	shine_str;
	unsigned long *fa,*fb,*fc;						// Faces
	unsigned long *tfa,*tfb,*tfc;					// TFaces

	fModel=fopen(FileName,"rb");
	if(!fModel)	return FILE_NOT_FOUND;
	
	FindStr("\t*MATERIAL_COUNT",fModel);
	sscanf(szString,"\t*MATERIAL_COUNT %i",&mat_q);

	material = new Mesh::material_t		[mat_q];		// Создаём нужное количество указателей

	for (unsigned int i=0;i<mat_q;i++)						// Генерируем имена ресурсов
	{	sprintf(szString,"Mesh_tex_#%d_for_%s",i,FileName);
		memcpy(material[i].TextureName,szString,128);		};

	for (unsigned int loop = 0; loop < mat_q; loop++)
	{
		FindStr("\t\t*MATERIAL_AMBIENT",fModel); // Заполняем структуры материалов
		sscanf(szString,"\t\t*MATERIAL_AMBIENT %f %f %f",
						&material[loop].ambient.d.c.x,
						&material[loop].ambient.d.c.y,
						&material[loop].ambient.d.c.z);

		FindStr("\t\t*MATERIAL_DIFFUSE",fModel);
		sscanf(szString,"\t\t*MATERIAL_DIFFUSE %f %f %f",
						&material[loop].diffuse.d.c.x,
						&material[loop].diffuse.d.c.y,
						&material[loop].diffuse.d.c.z);

		FindStr("\t\t*MATERIAL_SPECULAR",fModel);
		sscanf(szString,"\t\t*MATERIAL_SPECULAR %f %f %f",
						&material[loop].specular.d.c.x,
						&material[loop].specular.d.c.y,
						&material[loop].specular.d.c.z);

		FindStr("\t\t*MATERIAL_SHINE",fModel);
		sscanf(szString,"\t\t*MATERIAL_SHINE %f",&material[loop].shine);

		FindStr("\t\t*MATERIAL_SHINESTRENGTH",fModel);
		sscanf(szString,"\t\t*MATERIAL_SHINESTRENGTH %f",&shine_str);

		material[loop].shine *= shine_str * 128;
		
		int pos = ftell(fModel);
		FindStr("\t\t\t*BITMAP",fModel);
		if (feof(fModel))
		{
			fseek(fModel,pos,0);
			continue;
		}

		char path[MAX_PATH];
		sscanf(szString,"\t\t\t*BITMAP \"%s\"",path);
		if (path[strlen(path)-1] == '\"') path[strlen(path)-1]=0;
		
		if (path[0] != '\n' && path[0] != '\0')//избежим пустых строчек
		{
			rm.LOAD_Texture(material[loop].TextureName,path);
			material[loop].tex = rm.SELECT_Texture(material[loop].TextureName);
			//if (!texture[loop].Load(path))MessageBox(0,path,"Cannot Load!",16);
		}
	}
	//Подготовка списка отображения
	ListID=glGenLists(1);
	if(!ListID) return Mesh::NO_LISTS;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glNewList(ListID,GL_COMPILE);
	glRotated(-90,1,0,0);
	glDisable(GL_TEXTURE_2D);
	//--------------------------

	do
	{
		FindStr("\t\t*MESH_NUMVERTEX",fModel);
		//Если в процессе поиска уткнулись в конец, то нам хватит
		if (feof(fModel)) break;
		//Определим число вершин
		sscanf(szString,"\t\t*MESH_NUMVERTEX %d",&Vertices);

		//Определим число полигонов
		FindStr("\t\t*MESH_NUMFACES",fModel);
		sscanf(szString,"\t\t*MESH_NUMFACES %d",&Faces);

		//Выделим память
		vx=new GLfloat[Vertices];
		vy=new GLfloat[Vertices];
		vz=new GLfloat[Vertices];

		if(!vx || !vy || !vz) {fclose(fModel);return Mesh::NO_MEMORY;}

		fa=new unsigned long[Faces];
		fb=new unsigned long[Faces];
		fc=new unsigned long[Faces];

		if(!fa || !fb || !fc) {fclose(fModel);return Mesh::NO_MEMORY;}

		nx=new GLfloat[Faces];
		ny=new GLfloat[Faces];
		nz=new GLfloat[Faces];

		if(!nx || !ny || !nz) {fclose(fModel);return Mesh::NO_MEMORY;}

		for(i=0;i<Vertices;i++)
		{
			long tp;
			FindStr("\t\t\t*MESH_VERTEX",fModel);
			sscanf(szString,"\t\t\t*MESH_VERTEX %d %f %f %f",&tp,&(vx[i]),&(vy[i]),&(vz[i]));
		}

		for(i=0;i<Faces;i++)
		{
			long tp;
			FindStr("\t\t\t*MESH_FACE",fModel);
			sscanf(szString,"\t\t\t*MESH_FACE %d:    A: %d B: %d C: %d",&tp,&(fa[i]),&(fb[i]),&(fc[i]));
		}

		FindStr("\t\t*MESH_NUMTVERTEX",fModel);
		sscanf(szString,"\t\t*MESH_NUMTVERTEX %d",&TVert);

		u=new GLfloat[TVert];
		v=new GLfloat[TVert];
		
		if(!u || !v) {fclose(fModel);return Mesh::NO_MEMORY;}


		for(i=0;i<TVert;i++)
		{
			long tp1,tp2;
			FindStr("\t\t\t*MESH_TVERT",fModel);
			sscanf(szString,"\t\t\t*MESH_TVERT %d %f %f %f",&tp1,&(u[i]),&(v[i]),&tp2);
		}

		FindStr("\t\t*MESH_NUMTVFACES",fModel);
		sscanf(szString,"\t\t*MESH_NUMTVFACES %d",&TFaces);

		if(Faces!=TFaces)  // Cannot be, but .......
		{
			delete(vx);delete(vy);delete(vz);
			delete(nx);delete(ny);delete(nz);
			delete(fa);delete(fb);delete(fc);
            delete(u);delete(v);
			fclose(fModel);
			return Mesh::INVALID_FORMAT;
		}


		tfa=new unsigned long[TFaces];
		tfb=new unsigned long[TFaces];
		tfc=new unsigned long[TFaces];
		//.............................

		for(i=0;i<TFaces;i++)
		{
			long tp;
			FindStr("\t\t\t*MESH_TFACE",fModel);
			sscanf(szString,"\t\t\t*MESH_TFACE %d %d %d %d",&tp,&(tfa[i]),&(tfb[i]),&(tfc[i]));
		}

		for(i=0;i<Faces;i++)
		{
			long tp;
			FindStr("\t\t\t*MESH_FACENORMAL",fModel);
			sscanf(szString,"\t\t\t*MESH_FACENORMAL %d %f %f %f",&tp,&(nx[i]),&(ny[i]),&(nz[i]));
		}
		
		//Обработаем материал / цвет
		GLfloat Colors[3];
		GLuint material_number = 0;

		glColor4f(1,1,1,1);
		for (unsigned short int in = 0; in < 9; in++)
		{
			fgets(szString,128,fModel);
		}
		if (strstr(szString,"WIREFRAME_COLOR"))
		{
			sscanf(szString,"\t*WIREFRAME_COLOR %f %f %f",&Colors[0],&Colors[1],&Colors[2]);
			glColor3f(Colors[0],Colors[1],Colors[2]);
			glDisable(GL_TEXTURE_2D);
		}
		else
			if (strstr(szString,"*MATERIAL_REF"))
            {
				sscanf(szString,"\t*MATERIAL_REF %i",&material_number);
				glEnable(GL_TEXTURE_2D);
				material[material_number].tex->bind();
			}
			else
			{
				glDisable(GL_TEXTURE_2D);
				glColor3fv(material[material_number].diffuse.d.v);
			}

		glMaterialfv(GL_FRONT,GL_AMBIENT,	material[material_number].ambient.d.v);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,	material[material_number].diffuse.d.v);
		glMaterialfv(GL_FRONT,GL_SPECULAR,	material[material_number].specular.d.v);
		glMaterialfv(GL_FRONT,GL_SHININESS,	&material[material_number].shine);

		glBegin(GL_TRIANGLES);
		for(i=0;i<Faces;i++)
		{
			glNormal3f(nx[i],ny[i],nz[i]);glTexCoord2f(u[tfa[i]],v[tfa[i]]);glVertex3f(vx[fa[i]],vy[fa[i]],vz[fa[i]]);
			glNormal3f(nx[i],ny[i],nz[i]);glTexCoord2f(u[tfb[i]],v[tfb[i]]);glVertex3f(vx[fb[i]],vy[fb[i]],vz[fb[i]]);
			glNormal3f(nx[i],ny[i],nz[i]);glTexCoord2f(u[tfc[i]],v[tfc[i]]);glVertex3f(vx[fc[i]],vy[fc[i]],vz[fc[i]]);
        }
		glEnd();
	

		delete(vx);delete(vy);delete(vz);
		delete(nx);delete(ny);delete(nz);
		delete(fa);delete(fb);delete(fc);
		delete(tfa);delete(tfb);delete(tfc);
		delete(u);delete(v);
	}
	while (!feof(fModel));


	glEndList();
	glPopMatrix();

	fclose(fModel);

	return Mesh::OK;
};
GLvoid			Mesh::simpleMesh::Render(GLvoid)
{
	glPushMatrix();
	glCallList(Mesh::simpleMesh::ListID);
	glPopMatrix();
};

GLvoid			Mesh::simpleMesh::ULoad(GLvoid)
{
	for (GLuint i=0;i<mat_q;i++)
	{
		rm.ULOAD_Texture(material[i].TextureName);
	};
	delete material;
	glDeleteLists(ListID,1);
}

Vector3f	GetNormal(Point3f	a,Point3f	b,Point3f	c)
{
	Vector3f	o,t,r;
	o = b - a;
	t = c - b;
	r.d.c.x = o.d.c.y*t.d.c.z - t.d.c.y*o.d.c.z;
	r.d.c.y = o.d.c.z*t.d.c.x - t.d.c.z*o.d.c.x;
	r.d.c.z = o.d.c.x*t.d.c.y - t.d.c.x*o.d.c.y;
	return	r;
}