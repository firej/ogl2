#include "LocusAFX.h"
#include "./lwo_mesh.h"
#include <stdio.h>
#include "../../../ssrc/iff.h"

#define TEN_BITS_MASK	0x03FF
#define	BAD_LAYER		0xFFFFFFFF
static iff::DWORD	filesize;
static iff::DWORD	sizeb;							// Буффер для считывания 4-х байтового размера
static iff::tag_t	tagb;
static iff::DWORD	iLayerCur;						// Текущий слой
static iff::DWORD	iSurfCur;						// Текущий сурфейс
// Функции для парсинга файла
void			read_unknow_chunk(FILE * f);

Mesh::LWOMesh::LWOMesh()
{
	this->iLayers		=	0;
	this->iSurfaces		=	0;
	compiled = false;
}

Mesh::RESULT	Mesh::LWOMesh::Load(char *FileName)
{
	iLayerCur			=		BAD_LAYER;			// Устанавливаем начальный номер слоя!
	FILE *fModel;
	fModel=fopen(FileName,"rb");					// Открываем файл на чтение в бинарном виде
	if(!fModel) return Mesh::FILE_NOT_FOUND;

// file ::= FORM { 'LWO2'[ID4], data[CHUNK] * }
	tagb.i = iff::GetTAG(fModel);
	if (tagb.i != ID_FORM)							// Первые 4 символа должны быть FORM
		return Mesh::INVALID_FORMAT;
	filesize = iff::GetDWORD(fModel);				// Здесь находим размер всего файла с модельками + 8
	tagb.i = iff::GetTAG(fModel);					// Читаем буковки LWO2
	if (tagb.i != ID_LWO2)
		return Mesh::INVALID_FORMAT;
	do
	{
		tagb.i = iff::GetTAG(fModel);
		if (feof(fModel))	break;
		switch(tagb.i)
		{
		case ID_PNTS:	FASSERT (ReadPNTSChunk(fModel) == FJC_NO_ERROR);	break;
		case ID_POLS:	FASSERT (ReadPOLSChunk(fModel) == FJC_NO_ERROR);	break;
		case ID_LAYR:	FASSERT (ReadLAYRChunk(fModel) == FJC_NO_ERROR);	break;
		case ID_TAGS:	FASSERT (ReadTAGSChunk(fModel) == FJC_NO_ERROR);	break;
		case ID_SURF:	FASSERT (ReadSURFChunk(fModel) == FJC_NO_ERROR);	break;
		case ID_PTAG:	FASSERT (ReadPTAGChunk(fModel) == FJC_NO_ERROR);	break;
		default: iff::read_unknow_chunk(fModel);
		}
	}
	while(!feof(fModel));
	PostLoadProcessing();
	fclose(fModel);
	return Mesh::OK;
}
bool	Mesh::LWOMesh::ReadPNTSChunk(	FILE	*	f	)
{
	sizeb = iff::GetDWORD(f);						// Прочитали размер блока PNTS
	if (((sizeb/3)*3)!=(sizeb))	return	FJC_ERROR;	// Маненькая проверка
	sizeb /= 12;									// Получаем количество точек
	Layers[iLayerCur].iVertexes	=	sizeb;
	Layers[iLayerCur].Vertexes	=	new lwVertex_t[sizeb];		// Выделяем память под массив вершин
	for (DWORD i = 0 ; i < sizeb ; i++ )
	{
		Layers[iLayerCur].Vertexes[i].v.d.c.x = iff::GetFloat(f);
		Layers[iLayerCur].Vertexes[i].v.d.c.y = iff::GetFloat(f);
		Layers[iLayerCur].Vertexes[i].v.d.c.z = iff::GetFloat(f);
	}
	return FJC_NO_ERROR;
}
bool	Mesh::LWOMesh::ReadPOLSChunk(	FILE	*	f	)
{
	long	posPOLS;								// Запоминаем позицию в файле на начале секции полигонов
	DWORD	pols = 0;								// Количество полигонов для подсчёта
	WORD	lvertexes;								// Вершин на полигон
	sizeb	= iff::GetDWORD(f);
	tagb.i	= iff::GetTAG(f);								// считываем FACE
	if (tagb.i	!=	ID_FACE)
		return FJC_ERROR;
	sizeb -= 4;										// Получили размер без букв FACE
	__asm shr sizeb,1;								// Делим на 2 и получаем количество 2-хбайтных элементов
	posPOLS = ftell(f);
	for (UINT i = 0; i < sizeb ;)
	{
		lvertexes = iff::GetWORD(f);				// Читаем количество вершин
		lvertexes &= TEN_BITS_MASK;
		i+=1+lvertexes;								// Учитываем размер вершин
		__asm shl lvertexes,1;
		fseek(f,lvertexes,SEEK_CUR);				// Сдвигаем указатель в файле на количество вершин
		pols++;										// Считаем полигоны
	}
	fseek(f,posPOLS,SEEK_SET);						// Откат указателя назад (в начало POLS)

	Layers[iLayerCur].iPols	=	pols;
	Layers[iLayerCur].Pols	=	new Mesh::Poly_t[pols];			// Выделяем память под полигоны

	for (UINT i = 0; i < pols ;i++)
	{
		lvertexes = iff::GetWORD(f);				// Читаем количество вершин
		lvertexes &= TEN_BITS_MASK;
		Layers[iLayerCur].Pols[i].v	=	lvertexes;
		Layers[iLayerCur].Pols[i].vi	=	new WORD	[lvertexes];
		Layers[iLayerCur].Pols[i].ns	=	new bool	[lvertexes];	// Выделяем память под флаги
		memset(Layers[iLayerCur].Pols[i].ns,0,sizeof(bool)*lvertexes);
		for (int j = 0; j<lvertexes ; j++ )
		{
			WORD vi = iff::GetWORD(f);
			Layers[iLayerCur].Pols[i].vi[j]	= vi;		// Читаем полигоны в память
		}
	}
	return FJC_NO_ERROR;
}
bool	Mesh::LWOMesh::ReadLAYRChunk(FILE *f)
{
	iLayers++;
	iLayerCur	=	iLayers	-	1;
	lwLayer_t	tempL;
	sizeb = iff::GetDWORD(f);						// Читаем размер блока
	tempL.number			=	iff::GetWORD(f);
	sizeb -= 2;
	tempL.flags				=	iff::GetWORD(f);
	sizeb -= 2;
	tempL.PivotPoint.d.c.x	=	iff::GetFloat(f);
	tempL.PivotPoint.d.c.y	=	iff::GetFloat(f);
	tempL.PivotPoint.d.c.z	=	iff::GetFloat(f);
	sizeb -= 12;
	DWORD	strsize = 0;
	char	*tmp;
	tmp	= iff::GetString(f,&strsize);
	tempL.name = tmp;
	delete	[]	tmp;
	sizeb -= strsize;								// Пропускаем всё что осталось :)
	fseek(f,sizeb,SEEK_CUR);
	Layers.push_back(tempL);
	return FJC_NO_ERROR;
}

bool	Mesh::LWOMesh::ReadTAGSChunk	(	FILE	*	f	)
{
	sizeb	=	iff::GetDWORD(f);
	char	*	strb	=	new	char	[sizeb];
	iff::GetBuffer(f,strb,sizeb);					// Записали в буффер все строки (теги)
	tag_t		tmps;								// Временная переменная строки
	tmps.tn	=	strb;
	TagList.tag.push_back(tmps);
	for ( WORD i = 0 ; i < sizeb - 1 ; i++ )
	{
		if ( strb[i] == 0 && strb[i+1] != 0 && i < sizeb-1 )
		{
			tmps.tn = strb+i+1;
			TagList.tag.push_back(tmps);
		}
	}
	delete	[]	strb;
	return FJC_NO_ERROR;
}

bool	Mesh::LWOMesh::ReadSURFChunk	(	FILE	*	f	)
{
	iSurfaces++;
	iSurfCur = iSurfaces - 1;
	lwSurface_t	tmpSurf;
	SurfList.push_back(tmpSurf);
	DWORD	SurfChunkSize = iff::GetDWORD(f);
	DWORD	ReadedSize	=	0;
	DWORD	StringSize	=	0;
	DWORD	&ChunkSize = StringSize;			// Старая переменная с новым именем
	char	*tmp;
	tmp = iff::GetString(f,&StringSize);
	SurfList[iSurfCur].name			=	tmp;
	delete	[]	tmp;
	ReadedSize += StringSize;
	tmp = iff::GetString(f,&StringSize);
	SurfList[iSurfCur].AncestorName	=	tmp;
	delete	[]	tmp;
	ReadedSize += StringSize;
	for (  ; ReadedSize < SurfChunkSize ; ReadedSize += ChunkSize )
	{
		tagb.i = iff::GetTAG(f);			// Получаем тег субчанка
		ReadedSize += 6;					// Прибавляем размер ID чанка и его размер
		switch(tagb.i)
		{
		case ID_COLR:	ChunkSize	=	ReadSURFCOLRSubChunk(f);	break;
		case ID_DIFF:	ChunkSize	=	ReadSURFDIFFSubChunk(f);	break;
		case ID_SPEC:	ChunkSize	=	ReadSURFSPECSubChunk(f);	break;
		case ID_TRAN:	sizeb = iff::GetWORD(f);
			float	alfa;
			alfa	=	1.0f - iff::GetFloat(f);
			if (alfa > 1) alfa = 1.0f;				// Маленькие ограничения на прозрачность
			if (alfa < 0) alfa = 0.0f;
			SurfList[iSurfCur].color.d.c.a	=	alfa;
			iff::GetVX(f,NULL);
			break;
		case ID_SMAN:	sizeb = iff::GetWORD(f);
			SurfList[iSurfCur].nSmoothAngle	=	iff::GetFloat(f);
			if ( SurfList[iSurfCur].nSmoothAngle > 0 )
				SurfList[iSurfCur].Smoothing = true;
			break;
		/*case ID_LAYR:	ReadLAYRChunk(fModel);	break;
		case ID_TAGS:	ReadTAGSChunk(fModel);	break;
		case ID_SURF:	ReadSURFChunk(fModel);	break;*/
		default:		ChunkSize	=	iff::read_unknow_subchunk(f);
		}
	}
	return FJC_NO_ERROR;
}

WORD	Mesh::LWOMesh::ReadSURFCOLRSubChunk	(	FILE	*	f	)
{
	WORD	s	=	iff::GetWORD(f);
	this->SurfList[iSurfCur].color.d.c.r	=	iff::GetFloat(f);
	this->SurfList[iSurfCur].color.d.c.g	=	iff::GetFloat(f);
	this->SurfList[iSurfCur].color.d.c.b	=	iff::GetFloat(f);
	iff::GetVX(f,NULL);				//	envelope смещение
	return	s;
}
WORD	Mesh::LWOMesh::ReadSURFDIFFSubChunk	(	FILE	*	f	)
{
	WORD	s		=	iff::GetWORD(f);
	Color4f	diff	=	SurfList[iSurfCur].color;
	diff			=	diff * iff::GetFloat(f);
	SurfList[iSurfCur].Diffuse	=	diff;
	iff::GetVX(f,NULL);				//	envelope смещение
	return	s;
}

WORD	Mesh::LWOMesh::ReadSURFSPECSubChunk	(	FILE	*	f	)
{
	WORD	s		=	iff::GetWORD(f);
	Color4f	spec	=	SurfList[iSurfCur].color;
	spec			=	spec * iff::GetFloat(f);
	SurfList[iSurfCur].Specular	=	spec;
	iff::GetVX(f,NULL);				//	envelope смещение
	return	s;
}

bool	Mesh::LWOMesh::ReadCLIPChunk	(	FILE	*	f	)
{
	UNREFERENCED_PARAMETER(f);
	return FJC_NO_ERROR;
}

bool	Mesh::LWOMesh::ReadPTAGChunk	(	FILE	*	f	)
{
	sizeb	=	iff::GetDWORD(f);
	tagb.i	=	iff::GetTAG(f);
	sizeb	-=	4;
	if (tagb.i != ID_SURF)			// Нам не нужны всякие странные птэги
	{
		fseek(f,sizeb,SEEK_CUR);
		return	FJC_NO_ERROR;
	}
	DWORD	polyindex	=	NULL;
	DWORD	indexSize	=	NULL;
	for (; sizeb ;)
	{
		indexSize = 0;
		polyindex	=	iff::GetVX(f,&indexSize);
		this->Layers[iLayerCur].Pols[polyindex].iSurf	=	iff::GetWORD(f);
		sizeb	-=	indexSize + 2;	// Вычитаем размер 2х индексов
	}
	return FJC_NO_ERROR;
}

void	Mesh::LWOMesh::PostLoadProcessing	(	void	)
{
	for ( WORD i = 0 ; i < SurfList.size() ; i++ )
	{
		SurfList[i].Diffuse.d.c.a = SurfList[i].color.d.c.a;
	}
// Смотрим соответствие имени в теге и имени сурфейса и соответственно пишем тегу номер сурфейса
	for ( WORD i = 0 ; i < TagList.tag.size() ; i++ )	// Цикл по тегам
	{
		for ( WORD j = 0 ; j < SurfList.size() ; j++ )	// Внутри цикл по сурфейсам
		{	// Если название сурфейса и тега совпадают
			if ( (SurfList[j].name) == (TagList.tag[i].tn) )
			{
				TagList.tag[i].iSurf	=	j;			// Записываем номер сурфеса тегу
				break;
			}//if ( SurfList[j] == TagList.tag[i].tn )
		}//for ( WORD j++ ; j < SurfList.size() ; j++ )
	}//for ( WORD i = 0 ; i < TagList.tag.size() ; i++ )


	// Прописываем каждому полигону вместо номера тега номер сурфейса
	for (DWORD ind = 0 ; ind < iLayers ; ind++ )
	{
		lwLayer_t *l = &Layers[ind];
		for (DWORD i = 0 ; i< l->iPols; i++)
		{
			l->Pols[i].iSurf = TagList.tag[l->Pols[i].iSurf].iSurf;
		}
	}
	// Просчитываем для каждого полигона нормаль
	for (DWORD ind = 0 ; ind < iLayers ; ind++ )
	{
		lwLayer_t *l = &Layers[ind];
		for (DWORD i = 0 ; i< l->iPols; i++)
		{
			if (l->Pols[i].v < 3) continue;
			Vector3f	v1,v2;
			v1 = l->Vertexes[l->Pols[i].vi[1]].v - l->Vertexes[l->Pols[i].vi[0]].v;
			v2 = l->Vertexes[l->Pols[i].vi[2]].v - l->Vertexes[l->Pols[i].vi[0]].v;
			l->Pols[i].normal = v1.vector_product(v2);
			l->Pols[i].normal.normalize();
		}
	}
	//mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
	// Сглаживание нормалей / Normal smoothing
	// Первым делом для каждой вершины слоя создаём список полигонов, которым она принадлежит
	for (DWORD ind = 0 ; ind < iLayers ; ind++ )
	{
		lwLayer_t *l = &Layers[ind];
		// Для текущего слоя создаём массив вершин типа NormalSmoothVertex_t
		NormalSmoothVertex_t* vert = new NormalSmoothVertex_t[l->iVertexes];
		// Указываем каждой вершинке её номер
		for	(DWORD j = 0 ; j < l->iVertexes ; j++ ) vert[j].n = j;
		// Записываем для каждой вершины её полигоны
		for (DWORD i = 0 ; i < l->iPols ; i++)
		{
			if (l->Pols[i].v < 3) continue; // полигоны меньше 3х точек нам не нужны
			for (WORD j = 0 ; j < l->Pols[i].v ; j++ )
			{
				NormalSmoothPolygonPointer_t	somepol;
				somepol.p	=	&(l->Pols[i]);
				WORD	index = l->Pols[i].vi[j];
				somepol.np	=	j;
				vert[index].polys.push_back(somepol);
			}
		}
		// Сглаживание нормалей
		for (DWORD i = 0 ; i < l->iVertexes ; i++)	// Цикл по всем вершинам
		{
			// Попарное сравнение всех полигонов в которые входит данная вершина
			for ( DWORD k = 0 ; k < vert[i].polys.size() ; k++ )
			{
				for ( DWORD li = k ; li < vert[i].polys.size() ; li++ )
				{
					if ( li == k ) continue;
	// Если полигоны принадлежат разным материалам или материал без сглаживания мы их не обрабатываем
					if (vert[i].polys[k].p->iSurf != vert[i].polys[li].p->iSurf ||
						SurfList[vert[i].polys[li].p->iSurf].Smoothing == false)
						continue;
	// Теперь находим угол между нормалями полигонов (этот угол используется для решения - сглажывать или нет)
					float	surfAngle = SurfList[vert[i].polys[li].p->iSurf].nSmoothAngle;
					float	polyAngle = (float)vert[i].polys[k].p->normal.angle(vert[i].polys[li].p->normal);
					if ( polyAngle > 0 && polyAngle < surfAngle )
					{// Угол между нормалями меньше максимального для текущего материала
						// Значит этот полигон ещё не внёс свой вклад в результирующую нормаль
						if( vert[i].polys[k].s == false )
						{
							vert[i].normal									+=	vert[i].polys[k].p->normal;
							vert[i].polys[k].p->ns[vert[i].polys[k].np]		=	true;
							vert[i].polys[k].s								=	true;
						}
						if( vert[i].polys[li].s == false )
						{
							vert[i].normal									+=	vert[i].polys[li].p->normal;
							vert[i].polys[li].p->ns[vert[i].polys[li].np]	=	true;
							vert[i].polys[li].s								=	true;
						}
					}// if ( polyAngle > 0 && polyAngle < surfAngle )
				}// for ( DWORD li = k ; li < vert[i].polys.size() ; li++ )
			}// for ( DWORD k = 0 ; k < vert[i].polys.size() ; k++ )
			vert[i].normal.normalize();
			l->Vertexes[i].n = vert [i].normal;
		}// for (DWORD i = 0 ; i < l->iVertexes ; i++)
		// Очищаем массивы полигонов у каждой вершины
		for	(DWORD j = 0 ; j < l->iVertexes ; j++ ) vert[j].polys.clear();
		// Удаляем массив вершин
		delete [] vert;
	}

	CompileList();
}

void	Mesh::LWOMesh::CompileList		(	)
{
	if (compiled) return;
	compiled = true;
	list = glGenLists(1);
	glNewList(list,GL_COMPILE);
	DrawFromSource();
	glEndList();
}

void	Mesh::LWOMesh::DrawFromSource	(void)
{	// Выправление системы координат
	DWORD	CurrentSurface;
	CurrentSurface = 0xFFFFFFFF;
	for (DWORD ind = 0 ; ind < this->iLayers ; ind++ )
	{
		lwLayer_t *l = &Layers[ind];
		for (DWORD i = 0 ; i< l->iPols; i++)
		{
			if (CurrentSurface != l->Pols[i].iSurf)
			{
				CurrentSurface = l->Pols[i].iSurf;
				glColor4fv(	SurfList[CurrentSurface].Diffuse.d.v);
				glMaterialfv(GL_FRONT,GL_AMBIENT,Color4f().d.v);
				glMaterialfv(GL_FRONT,GL_DIFFUSE,SurfList[CurrentSurface].Diffuse.d.v);
				glMaterialfv(GL_FRONT,GL_SPECULAR,SurfList[CurrentSurface].Specular.d.v);
			}
			glBegin(GL_POLYGON);
			for (int j = 0; j < l->Pols[i].v; j++)
			{
				if (l->Pols[i].ns[j])
					glNormal3fv(l->Vertexes[l->Pols[i].vi[j]].n.d.v);
				else
					glNormal3fv(l->Pols[i].normal.d.v);
				glVertex3fv(l->Vertexes[l->Pols[i].vi[j]].v.d.v);
			}
			glEnd();
		}
	}
}

void	Mesh::LWOMesh::Render	(void)
{
	glPushAttrib(GL_DEPTH_BUFFER_BIT|GL_ENABLE_BIT|GL_CURRENT_BIT);		// Сохранение настроек
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	if (Globals.ERS.m	== ERS::Mesh::WIRE)
	{
		glDisable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glLineWidth(1.0);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_CULL_FACE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glDisable(GL_TEXTURE_2D);
		if (Globals.EFl.LWOCullFace)
		{
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
		}
		else	glDisable(GL_CULL_FACE);
	}
	glCallList(list);
	glPopAttrib();											// Возврат
}
void	Mesh::LWOMesh::ULoad	(void)
{
	if (compiled)
		glDeleteLists(list,1);
	compiled = false;
	for (DWORD j = 0 ; j < this->iLayers ; j++ )
	{
		lwLayer_t *l = &Layers[j];
		for (DWORD i = 0 ; i < l->iPols   ; i++ )
		{
			delete [] l->Pols[i].vi;
			delete [] l->Pols[i].ns;
		}
		delete [] l->Pols;
		delete [] l->Vertexes;
	}
	Layers.clear();
	TagList.tag.clear();
	SurfList.clear();
}
