#include "LocusAFX.h"
#include "./Texture.h"

TextureClass::TextureClass(void)
{
	Number	=	0;
	FType	=	0;
};// Constructor

TextureClass::~TextureClass(void)
{
	glDeleteTextures(1,&Number);
};// Destructor

char* TextureClass::GetFileName(void)
{
	return TextureFileName;
};

void TextureClass::SetFileName(const char *Name)
{
	sprintf(TextureFileName,"%s",Name);
};
void TextureClass::SetFileType(ILenum Type)
{
	FType=Type;
};
void TextureClass::SetFM(int FM)
{
	FilterMode = FM;
};
bool TextureClass::Load(int FM,ILenum Type, const char* Name)
{
	FilterMode = FM;
	FType = Type;
	sprintf(TextureFileName,"%s",Name);
	Load();
	return NO_ERROR;
};
bool TextureClass::Load(const char* Name)
{
	FilterMode = (int) Globals.TextureFiltering;
	sprintf(TextureFileName,"%s",Name);
	return Load();
};
bool TextureClass::Load()
{
	//ilLoad(FType,TextureFileName);				// Загрузка файла с явным указанием типа
	if (IL_FALSE == ilLoadImage(TextureFileName))		// Загрузка файла
	{
		return true;
	};
	//{char* strError = iluErrorString(err);MessageBox(NULL, Message, "Ошибка при загрузке!", MB_OK);
	iWidth = ilGetInteger(IL_IMAGE_WIDTH);			// Ширина изображения
	iHeight = ilGetInteger(IL_IMAGE_HEIGHT);		// Высота изображения
	Bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);	// Число байт на пиксель
	size = iWidth * iHeight * Bpp;					// Размер памяти
	data = new unsigned char[size];					// Память под массив
	unsigned char* copyData = ilGetData();			// Получение растровых данных
	memcpy(data, copyData, size);					// Копирование растровых данных

	switch (Bpp) {									// переопределить тип для OpenGL
	case 1:	type = GL_RGB8;		break;				// Картинка с палитрой
	case 3:	type = GL_RGB;		break;				// 24 бита (Red, Green, Blue)
	case 4:	type = GL_RGBA;		break;}				// 32 бита (Red, Green, Blue, Alpha)
	
//============================================================================================================||
//===== Загрузка текстуры в OpenGL ===========================================================================||
	glGenTextures(1, &Number);
	glBindTexture(GL_TEXTURE_2D, Number);
	
	//выбираем фильтрацию
	switch (FilterMode)
	{
    //без фильтрации
	case FJC_TEX_NO_FILTERING:
		{
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, Bpp, iWidth, iHeight, 0, type, GL_UNSIGNED_BYTE, data);
		}break;

		//простая билинейная фильтрация
	case FJC_TEX_BILINEAR_FILTERING:
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, Bpp, iWidth, iHeight, 0, type, GL_UNSIGNED_BYTE, data);

		}break;

		//Трилинейная
	case FJC_TEX_TRILINEAR_FILTERING:
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			//--Делаем мипмапы
			gluBuild2DMipmaps(GL_TEXTURE_2D, Bpp, iWidth, iHeight, type, GL_UNSIGNED_BYTE, data);

		}break;

		//Анизотропная
	case FJC_TEX_ANISOTROPIC_FILTERING:
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,SysInfo.video.MaxAnisotropy);
			//--Делаем мипмапы
			gluBuild2DMipmaps(GL_TEXTURE_2D, Bpp, iWidth, iHeight, type, GL_UNSIGNED_BYTE, data);
		}break;
	}//switch(filtermode)
	// Удаление ненужного массива
	if (data) {	delete [] data;	data = NULL;}
	return false;
};
bool	TextureClass::LoadL(ILenum Type, void *Lump, ILuint size)
{
	FilterMode = (int) Globals.TextureFiltering;
	if (IL_FALSE == ilLoadL(Type,Lump,size))		// Загрузка файла
	{
		return true;
	};
	//{char* strError = iluErrorString(err);MessageBox(NULL, Message, "Ошибка при загрузке!", MB_OK);
	iWidth = ilGetInteger(IL_IMAGE_WIDTH);			// Ширина изображения
	iHeight = ilGetInteger(IL_IMAGE_HEIGHT);		// Высота изображения
	Bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);	// Число байт на пиксель
	size = iWidth * iHeight * Bpp;					// Размер памяти
	data = new unsigned char[size];					// Память под массив
	unsigned char* copyData = ilGetData();			// Получение растровых данных
	memcpy(data, copyData, size);					// Копирование растровых данных

	switch (Bpp) {									// переопределить тип для OpenGL
	case 1:	type = GL_RGB8;		break;				// Картинка с палитрой
	case 3:	type = GL_RGB;		break;				// 24 бита (Red, Green, Blue)
	case 4:	type = GL_RGBA;		break;}				// 32 бита (Red, Green, Blue, Alpha)
	
//============================================================================================================||
//===== Загрузка текстуры в OpenGL ===========================================================================||
	glGenTextures(1, &Number);
	glBindTexture(GL_TEXTURE_2D, Number);
	
	//выбираем фильтрацию
	switch (FilterMode)
	{
    //без фильтрации
	case FJC_TEX_NO_FILTERING:
		{
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, Bpp, iWidth, iHeight, 0, type, GL_UNSIGNED_BYTE, data);
		}break;

		//простая билинейная фильтрация
	case FJC_TEX_BILINEAR_FILTERING:
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, Bpp, iWidth, iHeight, 0, type, GL_UNSIGNED_BYTE, data);

		}break;

		//Трилинейная
	case FJC_TEX_TRILINEAR_FILTERING:
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			//--Делаем мипмапы
			gluBuild2DMipmaps(GL_TEXTURE_2D, Bpp, iWidth, iHeight, type, GL_UNSIGNED_BYTE, data);

		}break;

		//Анизотропная
	case FJC_TEX_ANISOTROPIC_FILTERING:
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,SysInfo.video.MaxAnisotropy);
			//--Делаем мипмапы
			gluBuild2DMipmaps(GL_TEXTURE_2D, Bpp, iWidth, iHeight, type, GL_UNSIGNED_BYTE, data);
		}break;
	default:
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, Bpp, iWidth, iHeight, 0, type, GL_UNSIGNED_BYTE, data);
		break;
	}//switch(filtermode)
	// Удаление ненужного массива
	if (data) {	delete [] data;	data = NULL;}
	return false;
}

#ifdef ENABLE_TEXTURE_RENDERING
FJCRendererTexture::FJCRendererTexture(void)
{
	Number		=	0;
	FilterMode	=	FJC_RTEX_BILINEAR_FILTERING;
}// Constructor

FJCRendererTexture::~FJCRendererTexture	(void)
{
	glDeleteTextures(1,&Number);
}// Destructor

void FJCRendererTexture::Init	(
									int Width,
									int Height,
									int FilterMode
								)
{
	ScreenIndex = 0;
	SetSize( Width, Height);
	SetFM(FilterMode);
	Init();
}

void FJCRendererTexture::Init	()
{
	data = (unsigned char*)new GLuint[(size * sizeof(unsigned char))];
	// Очистка памяти массива
	ZeroMemory(data,(size * sizeof(unsigned char)));
	glGenTextures(1, &Number);            // Создать 1 текстуру
	glBindTexture(GL_TEXTURE_2D, Number); // Связать текстуру
	// Построить текстуру по информации в data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//выбираем фильтрацию
	/*switch (FilterMode)
	{
    //без фильтрации
	case FJC_TEX_NO_FILTERING:
		{
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, 4, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}break;

		//простая билинейная фильтрация
	case FJC_TEX_BILINEAR_FILTERING:
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, 4, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}break;
	}//switch(filtermode)*/
	//delete [] data;   // Удалить data
}

void FJCRendererTexture::bind	()
{
	glBindTexture(GL_TEXTURE_2D, Number);
}

void FJCRendererTexture::CopyBackbufferToTexture()
{
	bind();
	// Копирование области просмотра в текстуру
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, iWidth, iHeight);
	static int ret = glGetError();
	if (ret != GL_NO_ERROR) {__asm INT 3};
}

void FJCRendererTexture::Save()
{
	bind();
	glGetTexImage(GL_TEXTURE_2D,0,GL_RGB,GL_UNSIGNED_INT,data);
	static int ret;
	ret = glGetError();
	if (ret != GL_NO_ERROR) {__asm INT 3};;
	ret = ilTexImage( iWidth, iHeight, 0, 4, IL_RGBA, IL_UNSIGNED_INT, data);
	if (ret == 0) MessageBox(NULL,"Error on loading texture into IL","ERROR!",16);
	ret = ilGetError();
	if (ret != 0) MessageBox(NULL,iluErrorString(ret),"ERROR! in ilTexImage",16);
	do	{
	ScreenIndex++;
	sprintf (ScreenShot,"SS_%05d.png",ScreenIndex);
	ret = ilSaveImage(ScreenShot);
	ret = ilGetError();
	} while (ret == IL_FILE_ALREADY_EXISTS);
	if (ret != 0) MessageBox(NULL,iluErrorString(ret),"ERROR! in ilSaveImage",16);
}

FJCRendererTexture RendTex;						// Одна текстурка для работы
#endif
