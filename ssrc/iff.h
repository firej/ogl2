#ifndef _IFF_IO_HEADER
#define	_IFF_IO_HEADER
//	Interchange Format Files
#include <cstdio>
#include <cstddef>

namespace iff	{
    typedef unsigned char	UBYTE;		/*  8 bits unsigned		*/
	typedef	char			BYTE;		/*  8 bits signed		*/
    typedef short			WORD;		/* 16 bits signed		*/
    typedef unsigned short	UWORD;		/* 16 bits unsigned		*/
    typedef long			LONG;		/* 32 bits signed		*/
#ifdef WIN32
 typedef	unsigned long	DWORD;		/* 32 bits unsigned		*/
#else
 typedef	unsigned int	DWORD;		/* 32 bits unsigned		*/
#endif
 typedef	float			FLOAT4;		/* 32 bits IEEE float	*/

/*
TODO:
 operator type(const operator sourcetype &source)
 operator =(const operator sourcetype &source)
*/

	union	tag_t
	{
		tag_t()
		{}
		tag_t(char	a, char b, char c, char d)
		{
			name[0] = a;
			name[1] = b;
			name[2] = c;
			name[3] = d;
		}
		char	name[4];
		DWORD	i;
	};

#ifdef WIN32
inline	DWORD	SwapBytesDW(const	DWORD	s)	// Обмен порядка байтов
	{
		__asm{
			XOR	EAX,EAX					;
			XOR	EBX,EBX					;
			mov	bl,byte ptr s[0]		;
			mov	EAX,EBX					;
			shl	EAX,8					;
			mov	bl,byte ptr s[1]		;
			or	EAX,EBX					;
			shl	EAX,8					;
			mov	bl,byte ptr s[2]		;
			or	EAX,EBX					;
			shl	EAX,8					;
			mov	bl,byte ptr s[3]		;
			or	EAX,EBX					;
		}
	}
inline	WORD	SwapBytesW(const	WORD	s)	// Изменение порядка байтов
	{
		__asm{
			XOR	EAX,EAX					;
			XOR	EBX,EBX					;
			mov	bl,byte ptr s[0]		;
			mov	AX,BX					;
			shl	AX,8					;
			mov	bl,byte ptr s[1]		;
			or	AX,BX					;
		}
	}
#else
// macOS/Linux byte swapping functions
inline	DWORD	SwapBytesDW(const	DWORD	s)	// Обмен порядка байтов
	{
		return ((s & 0xFF000000) >> 24) |
		       ((s & 0x00FF0000) >> 8)  |
		       ((s & 0x0000FF00) << 8)  |
		       ((s & 0x000000FF) << 24);
	}
inline	WORD	SwapBytesW(const	WORD	s)	// Изменение порядка байтов
	{
		return ((s & 0xFF00) >> 8) | ((s & 0x00FF) << 8);
	}
#endif
inline	DWORD	GetDWORD(FILE	*	f)
	{
		static	DWORD	s;
		fread(&s,4,1,f);
		return SwapBytesDW(s);
	}
inline	void	PutDWORD(FILE	*	f,	DWORD	dw)
	{
		dw = SwapBytesDW(dw);
		fwrite(&dw,4,1,f);
	}
inline	WORD	GetWORD(FILE	*	f)
	{
		static	WORD	s;
		fread(&s,2,1,f);
		return SwapBytesW(s);
	}
inline	void	PutWORD(FILE	*	f,	WORD	w)
	{
		w = SwapBytesW(w);
		fwrite(&w,2,1,f);
	}
inline	FLOAT4	GetFloat(FILE	*	f)
	{
		static	DWORD	d;
		fread(&d,4,1,f);
		d	=	iff::SwapBytesDW(d);
		return *(FLOAT4*)&d;
	}
inline	void	PutFloat(FILE	*	f, iff::FLOAT4	fl)
	{
		static	DWORD	d;
		d	=	*(DWORD*)&fl;
		d	=	iff::SwapBytesDW(d);
		fwrite(&d,4,1,f);
	}
inline char*	GetString(	FILE	*	f,DWORD	*	size)
	{
		(*size) = 0;
		long	pos = ftell(f);
		char	c;
		do
		{			
			fread(&c,1,1,f);
			(*size)++;
		} while	(c != 0);

		fseek(f,pos,SEEK_SET);

		if ((*size) & (1))	(*size)++;

		char	*res = new char[*size];
		fread(res,*size,1,f);

		return res;
	}
inline void	PutString		(	FILE	*	f, char*	s, DWORD	size)
	{
		fwrite(s,1,size,f);
		char	c = 0;
		if (size & 1)	fwrite(&c,1,1,f);
	}
inline BYTE	TestString		(char*, DWORD	size)
	{
		if (size & 1)	return	1;
		else			return	0;
	}
inline void	PutBuffer		(	FILE	*	f,	void*	buf,DWORD	length)
	{
		fwrite(buf,1,length,f);
	}
inline void	GetBuffer		(	FILE	*	f,	void*	buf,DWORD	length)
	{
		fread(buf,1,length,f);
	}
inline	void	PutTAG		(	FILE	*	f,	DWORD	Tag	)
	{
		DWORD	dw = Tag;
		fwrite(&dw,4,1,f);
	}
inline	DWORD	GetTAG		(	FILE	*	f)
	{
		DWORD	dw;
		fread(&dw,4,1,f);
		return	dw;
	}
inline	DWORD	GetVX		(	FILE	*	f	,DWORD	*	size)
	{
		WORD	value = iff::GetWORD(f);
		if (size != NULL )(*size)	+=	2;
		if ( (value & 0xFF00) == 0xFF00 )
		{
			value = ((value & 0xFF) << 16) | iff::GetWORD(f);
			if (size != NULL )(*size)	+=	2;
		}
		return	value;
	}

inline	DWORD	read_unknow_chunk	(	FILE	*	f	)
{
	DWORD	s;
	s = iff::GetDWORD(f);
	fseek(f,s,SEEK_CUR);
	return	s;
}

inline	WORD	read_unknow_subchunk	(	FILE	*	f	)
{
	WORD	s;
	s = iff::GetWORD(f);
	fseek(f,s,SEEK_CUR);
	return	s;
}

const	tag_t	FONTFORMATNAME	('L','F','N','T');
const	tag_t	FONTNAME		('F','N','T','N');
const	tag_t	FONTSIZE		('F','N','T','S');
const	tag_t	FONTTEXTURE		('F','N','T','T');
const	tag_t	FONTGLYPHBLOCK	('F','N','T','G');

#define	ID_LFNT  LWID_('L','F','N','T')
#define	ID_FNTN  LWID_('F','N','T','N')
#define	ID_FNTS  LWID_('F','N','T','S')
#define	ID_FNTT  LWID_('F','N','T','T')
#define	ID_FNTG  LWID_('F','N','T','G')

#define ID_FORM  LWID_('F','O','R','M')
#define ID_LWO2  LWID_('L','W','O','2')
#define ID_LWOB  LWID_('L','W','O','B')

/* top-level chunks */
#define ID_LAYR  LWID_('L','A','Y','R')
#define ID_TAGS  LWID_('T','A','G','S')
#define ID_PNTS  LWID_('P','N','T','S')
#define ID_BBOX  LWID_('B','B','O','X')
#define ID_VMAP  LWID_('V','M','A','P')
#define ID_VMAD  LWID_('V','M','A','D')
#define ID_POLS  LWID_('P','O','L','S')
#define ID_PTAG  LWID_('P','T','A','G')
#define ID_ENVL  LWID_('E','N','V','L')
#define ID_CLIP  LWID_('C','L','I','P')
#define ID_SURF  LWID_('S','U','R','F')
#define ID_DESC  LWID_('D','E','S','C')
#define ID_TEXT  LWID_('T','E','X','T')
#define ID_ICON  LWID_('I','C','O','N')

/* polygon types */
#define ID_FACE  LWID_('F','A','C','E')
#define ID_CURV  LWID_('C','U','R','V')
#define ID_PTCH  LWID_('P','T','C','H')
#define ID_MBAL  LWID_('M','B','A','L')
#define ID_BONE  LWID_('B','O','N','E')

/* polygon tags */
#define ID_SURF  LWID_('S','U','R','F')
#define ID_PART  LWID_('P','A','R','T')
#define ID_SMGP  LWID_('S','M','G','P')

/* envelopes */
#define ID_PRE   LWID_('P','R','E',' ')
#define ID_POST  LWID_('P','O','S','T')
#define ID_KEY   LWID_('K','E','Y',' ')
#define ID_SPAN  LWID_('S','P','A','N')
#define ID_TCB   LWID_('T','C','B',' ')
#define ID_HERM  LWID_('H','E','R','M')
#define ID_BEZI  LWID_('B','E','Z','I')
#define ID_BEZ2  LWID_('B','E','Z','2')
#define ID_LINE  LWID_('L','I','N','E')
#define ID_STEP  LWID_('S','T','E','P')

/* clips */
#define ID_STIL  LWID_('S','T','I','L')
#define ID_ISEQ  LWID_('I','S','E','Q')
#define ID_ANIM  LWID_('A','N','I','M')
#define ID_XREF  LWID_('X','R','E','F')
#define ID_STCC  LWID_('S','T','C','C')
#define ID_TIME  LWID_('T','I','M','E')
#define ID_CONT  LWID_('C','O','N','T')
#define ID_BRIT  LWID_('B','R','I','T')
#define ID_SATR  LWID_('S','A','T','R')
#define ID_HUE   LWID_('H','U','E',' ')
#define ID_GAMM  LWID_('G','A','M','M')
#define ID_NEGA  LWID_('N','E','G','A')
#define ID_IFLT  LWID_('I','F','L','T')
#define ID_PFLT  LWID_('P','F','L','T')

/* surfaces */
#define ID_COLR  LWID_('C','O','L','R')
#define ID_LUMI  LWID_('L','U','M','I')
#define ID_DIFF  LWID_('D','I','F','F')
#define ID_SPEC  LWID_('S','P','E','C')
#define ID_GLOS  LWID_('G','L','O','S')
#define ID_REFL  LWID_('R','E','F','L')
#define ID_RFOP  LWID_('R','F','O','P')
#define ID_RIMG  LWID_('R','I','M','G')
#define ID_RSAN  LWID_('R','S','A','N')
#define ID_TRAN  LWID_('T','R','A','N')
#define ID_TROP  LWID_('T','R','O','P')
#define ID_TIMG  LWID_('T','I','M','G')
#define ID_RIND  LWID_('R','I','N','D')
#define ID_TRNL  LWID_('T','R','N','L')
#define ID_BUMP  LWID_('B','U','M','P')
#define ID_SMAN  LWID_('S','M','A','N')
#define ID_SIDE  LWID_('S','I','D','E')
#define ID_CLRH  LWID_('C','L','R','H')
#define ID_CLRF  LWID_('C','L','R','F')
#define ID_ADTR  LWID_('A','D','T','R')
#define ID_SHRP  LWID_('S','H','R','P')
#define ID_LINE  LWID_('L','I','N','E')
#define ID_LSIZ  LWID_('L','S','I','Z')
#define ID_ALPH  LWID_('A','L','P','H')
#define ID_AVAL  LWID_('A','V','A','L')
#define ID_GVAL  LWID_('G','V','A','L')
#define ID_BLOK  LWID_('B','L','O','K')

/* texture layer */
#define ID_TYPE  LWID_('T','Y','P','E')
#define ID_CHAN  LWID_('C','H','A','N')
#define ID_NAME  LWID_('N','A','M','E')
#define ID_ENAB  LWID_('E','N','A','B')
#define ID_OPAC  LWID_('O','P','A','C')
#define ID_FLAG  LWID_('F','L','A','G')
#define ID_PROJ  LWID_('P','R','O','J')
#define ID_STCK  LWID_('S','T','C','K')
#define ID_TAMP  LWID_('T','A','M','P')

/* texture coordinates */
#define ID_TMAP  LWID_('T','M','A','P')
#define ID_AXIS  LWID_('A','X','I','S')
#define ID_CNTR  LWID_('C','N','T','R')
#define ID_SIZE  LWID_('S','I','Z','E')
#define ID_ROTA  LWID_('R','O','T','A')
#define ID_OREF  LWID_('O','R','E','F')
#define ID_FALL  LWID_('F','A','L','L')
#define ID_CSYS  LWID_('C','S','Y','S')

/* image map */
#define ID_IMAP  LWID_('I','M','A','P')
#define ID_IMAG  LWID_('I','M','A','G')
#define ID_WRAP  LWID_('W','R','A','P')
#define ID_WRPW  LWID_('W','R','P','W')
#define ID_WRPH  LWID_('W','R','P','H')
#define ID_VMAP  LWID_('V','M','A','P')
#define ID_AAST  LWID_('A','A','S','T')
#define ID_PIXB  LWID_('P','I','X','B')

/* procedural */
#define ID_PROC  LWID_('P','R','O','C')
#define ID_COLR  LWID_('C','O','L','R')
#define ID_VALU  LWID_('V','A','L','U')
#define ID_FUNC  LWID_('F','U','N','C')
#define ID_FTPS  LWID_('F','T','P','S')
#define ID_ITPS  LWID_('I','T','P','S')
#define ID_ETPS  LWID_('E','T','P','S')

/* gradient */
#define ID_GRAD  LWID_('G','R','A','D')
#define ID_GRST  LWID_('G','R','S','T')
#define ID_GREN  LWID_('G','R','E','N')
#define ID_PNAM  LWID_('P','N','A','M')
#define ID_INAM  LWID_('I','N','A','M')
#define ID_GRPT  LWID_('G','R','P','T')
#define ID_FKEY  LWID_('F','K','E','Y')
#define ID_IKEY  LWID_('I','K','E','Y')

/* shader */
#define ID_SHDR  LWID_('S','H','D','R')
#define ID_DATA  LWID_('D','A','T','A')
}

#define LWID_(a,b,c,d) (((d)<<24)|((c)<<16)|((b)<<8)|(a))

#define	ID_XFNT  LWID_('X','F','N','T')
#define	ID_FNTN  LWID_('F','N','T','N')
#define	ID_FNTS  LWID_('F','N','T','S')
#define	ID_FNTT  LWID_('F','N','T','T')
#define	ID_FNTG  LWID_('F','N','T','G')

#define ID_FORM  LWID_('F','O','R','M')
#define ID_LWO2  LWID_('L','W','O','2')
#define ID_LWOB  LWID_('L','W','O','B')

/* top-level chunks */
#define ID_LAYR  LWID_('L','A','Y','R')
#define ID_TAGS  LWID_('T','A','G','S')
#define ID_PNTS  LWID_('P','N','T','S')
#define ID_BBOX  LWID_('B','B','O','X')
#define ID_VMAP  LWID_('V','M','A','P')
#define ID_VMAD  LWID_('V','M','A','D')
#define ID_POLS  LWID_('P','O','L','S')
#define ID_PTAG  LWID_('P','T','A','G')
#define ID_ENVL  LWID_('E','N','V','L')
#define ID_CLIP  LWID_('C','L','I','P')
#define ID_SURF  LWID_('S','U','R','F')
#define ID_DESC  LWID_('D','E','S','C')
#define ID_TEXT  LWID_('T','E','X','T')
#define ID_ICON  LWID_('I','C','O','N')

/* polygon types */
#define ID_FACE  LWID_('F','A','C','E')
#define ID_CURV  LWID_('C','U','R','V')
#define ID_PTCH  LWID_('P','T','C','H')
#define ID_MBAL  LWID_('M','B','A','L')
#define ID_BONE  LWID_('B','O','N','E')

/* polygon tags */
#define ID_SURF  LWID_('S','U','R','F')
#define ID_PART  LWID_('P','A','R','T')
#define ID_SMGP  LWID_('S','M','G','P')

/* envelopes */
#define ID_PRE   LWID_('P','R','E',' ')
#define ID_POST  LWID_('P','O','S','T')
#define ID_KEY   LWID_('K','E','Y',' ')
#define ID_SPAN  LWID_('S','P','A','N')
#define ID_TCB   LWID_('T','C','B',' ')
#define ID_HERM  LWID_('H','E','R','M')
#define ID_BEZI  LWID_('B','E','Z','I')
#define ID_BEZ2  LWID_('B','E','Z','2')
#define ID_LINE  LWID_('L','I','N','E')
#define ID_STEP  LWID_('S','T','E','P')

/* clips */
#define ID_STIL  LWID_('S','T','I','L')
#define ID_ISEQ  LWID_('I','S','E','Q')
#define ID_ANIM  LWID_('A','N','I','M')
#define ID_XREF  LWID_('X','R','E','F')
#define ID_STCC  LWID_('S','T','C','C')
#define ID_TIME  LWID_('T','I','M','E')
#define ID_CONT  LWID_('C','O','N','T')
#define ID_BRIT  LWID_('B','R','I','T')
#define ID_SATR  LWID_('S','A','T','R')
#define ID_HUE   LWID_('H','U','E',' ')
#define ID_GAMM  LWID_('G','A','M','M')
#define ID_NEGA  LWID_('N','E','G','A')
#define ID_IFLT  LWID_('I','F','L','T')
#define ID_PFLT  LWID_('P','F','L','T')

/* surfaces */
#define ID_COLR  LWID_('C','O','L','R')
#define ID_LUMI  LWID_('L','U','M','I')
#define ID_DIFF  LWID_('D','I','F','F')
#define ID_SPEC  LWID_('S','P','E','C')
#define ID_GLOS  LWID_('G','L','O','S')
#define ID_REFL  LWID_('R','E','F','L')
#define ID_RFOP  LWID_('R','F','O','P')
#define ID_RIMG  LWID_('R','I','M','G')
#define ID_RSAN  LWID_('R','S','A','N')
#define ID_TRAN  LWID_('T','R','A','N')
#define ID_TROP  LWID_('T','R','O','P')
#define ID_TIMG  LWID_('T','I','M','G')
#define ID_RIND  LWID_('R','I','N','D')
#define ID_TRNL  LWID_('T','R','N','L')
#define ID_BUMP  LWID_('B','U','M','P')
#define ID_SMAN  LWID_('S','M','A','N')
#define ID_SIDE  LWID_('S','I','D','E')
#define ID_CLRH  LWID_('C','L','R','H')
#define ID_CLRF  LWID_('C','L','R','F')
#define ID_ADTR  LWID_('A','D','T','R')
#define ID_SHRP  LWID_('S','H','R','P')
#define ID_LINE  LWID_('L','I','N','E')
#define ID_LSIZ  LWID_('L','S','I','Z')
#define ID_ALPH  LWID_('A','L','P','H')
#define ID_AVAL  LWID_('A','V','A','L')
#define ID_GVAL  LWID_('G','V','A','L')
#define ID_BLOK  LWID_('B','L','O','K')

/* texture layer */
#define ID_TYPE  LWID_('T','Y','P','E')
#define ID_CHAN  LWID_('C','H','A','N')
#define ID_NAME  LWID_('N','A','M','E')
#define ID_ENAB  LWID_('E','N','A','B')
#define ID_OPAC  LWID_('O','P','A','C')
#define ID_FLAG  LWID_('F','L','A','G')
#define ID_PROJ  LWID_('P','R','O','J')
#define ID_STCK  LWID_('S','T','C','K')
#define ID_TAMP  LWID_('T','A','M','P')

/* texture coordinates */
#define ID_TMAP  LWID_('T','M','A','P')
#define ID_AXIS  LWID_('A','X','I','S')
#define ID_CNTR  LWID_('C','N','T','R')
#define ID_SIZE  LWID_('S','I','Z','E')
#define ID_ROTA  LWID_('R','O','T','A')
#define ID_OREF  LWID_('O','R','E','F')
#define ID_FALL  LWID_('F','A','L','L')
#define ID_CSYS  LWID_('C','S','Y','S')

/* image map */
#define ID_IMAP  LWID_('I','M','A','P')
#define ID_IMAG  LWID_('I','M','A','G')
#define ID_WRAP  LWID_('W','R','A','P')
#define ID_WRPW  LWID_('W','R','P','W')
#define ID_WRPH  LWID_('W','R','P','H')
#define ID_VMAP  LWID_('V','M','A','P')
#define ID_AAST  LWID_('A','A','S','T')
#define ID_PIXB  LWID_('P','I','X','B')

/* procedural */
#define ID_PROC  LWID_('P','R','O','C')
#define ID_COLR  LWID_('C','O','L','R')
#define ID_VALU  LWID_('V','A','L','U')
#define ID_FUNC  LWID_('F','U','N','C')
#define ID_FTPS  LWID_('F','T','P','S')
#define ID_ITPS  LWID_('I','T','P','S')
#define ID_ETPS  LWID_('E','T','P','S')

/* gradient */
#define ID_GRAD  LWID_('G','R','A','D')
#define ID_GRST  LWID_('G','R','S','T')
#define ID_GREN  LWID_('G','R','E','N')
#define ID_PNAM  LWID_('P','N','A','M')
#define ID_INAM  LWID_('I','N','A','M')
#define ID_GRPT  LWID_('G','R','P','T')
#define ID_FKEY  LWID_('F','K','E','Y')
#define ID_IKEY  LWID_('I','K','E','Y')

/* shader */
#define ID_SHDR  LWID_('S','H','D','R')
#define ID_DATA  LWID_('D','A','T','A')

#endif	//	_IFF_IO_HEADER