#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux includes
#include <OpenGL/gl.h>
#endif

#include "./objects.h"
		SkyBox::SkyBox		()
{
}
		SkyBox::~SkyBox		()
{
}

void	SkyBox::LoadTextures(ILenum	Type,char 	*NameOfTex)		// Загрузка текстур неба
{
	//t.Load(FJC_TEX_ANISOTROPIC_FILTERING,Type,NameOfTex);
	t.Load(FJC_TEX_NO_FILTERING,Type,NameOfTex);
}

float SkyA[] = {
0.50f, 0.00f,-1.0f,  1.0f, -1.0f,
0.75f, 0.00f,-1.0f,  1.0f,  1.0f,
0.75f, 0.25f, 1.0f,  1.0f,  1.0f,
0.50f, 0.25f, 1.0f,  1.0f, -1.0f,
							   
0.50f, 0.50f,-1.0f, -1.0f, -1.0f,
0.75f, 0.50f, 1.0f, -1.0f, -1.0f,
0.75f, 0.75f, 1.0f, -1.0f,  1.0f,
0.50f, 0.75f,-1.0f, -1.0f,  1.0f,
							   
0.00f, 0.50f,-1.0f, -1.0f, -1.0f,
0.25f, 0.50f,-1.0f, -1.0f,  1.0f,
0.25f, 0.25f,-1.0f,  1.0f,  1.0f,
0.00f, 0.25f,-1.0f,  1.0f, -1.0f,
							   
0.75f, 0.50f, 1.0f, -1.0f, -1.0f,
0.75f, 0.25f, 1.0f,  1.0f, -1.0f,
0.50f, 0.25f, 1.0f,  1.0f,  1.0f,
0.50f, 0.50f, 1.0f, -1.0f,  1.0f,
							   
0.25f, 0.50f,-1.0f, -1.0f,  1.0f,
0.50f, 0.50f, 1.0f, -1.0f,  1.0f,
0.50f, 0.25f, 1.0f,  1.0f,  1.0f,
0.25f, 0.25f,-1.0f,  1.0f,  1.0f,
							   
1.00f, 0.50f,-1.0f, -1.0f, -1.0f,
1.00f, 0.25f,-1.0f,  1.0f, -1.0f,
0.75f, 0.25f, 1.0f,  1.0f, -1.0f,
0.75f, 0.50f, 1.0f, -1.0f, -1.0f 
};

void SkyBox::operator ()()
{
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);		// Выключаем тест глубины для того чтобы нарисовать небо
	glDisable(GL_LIGHTING);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	t.bind();			
	glColor3d(1,1,1);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer		(3,GL_FLOAT,5*sizeof(float),SkyA+2);
	glTexCoordPointer	(2,GL_FLOAT,5*sizeof(float),SkyA);

	glDrawArrays(GL_QUADS,0,24);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glPopAttrib();
	glClear(GL_DEPTH_BUFFER_BIT);   // Очистка буфера глубины
}
