#include "LocusAFX.h"
#include "../globals.h"
#include "../time.h"
#include "AVI_player.h"
#pragma comment( lib, "vfw32.lib" )								// Search For VFW32.lib While Linking

bool				end_of_file		=	false;

bool				OpenAVI				(char* szFile);// Opens An AVI File (szFile)
void flipIt(void* b,int texS)							// Flips The Red And Blue Bytes (256x256)
{
	__asm												// Assembler Code To Follow
	{
		mov ecx, texS									// Counter Set To Dimensions Of Our Memory Block
		mov ebx, b										// Points ebx To Our Data (b)
		label:											// Label Used For Looping
			mov al,[ebx+0]								// Loads Value At ebx Into al
			mov ah,[ebx+2]								// Loads Value At ebx+2 Into ah
			mov [ebx+2],al								// Stores Value In al At ebx+2
			mov [ebx+0],ah								// Stores Value In ah At ebx
			
			add ebx,3									// Moves Through The Data By 3 Bytes
			dec ecx										// Decreases Our Loop Counter
			jnz label									// If Not Zero Jump Back To Label
	}
}

bool	AVI_player::Open(TCHAR *FileName,int Width, int Height)
{
	this->twidth		=	Width;
	this->theight		=	Height;
	return OpenAVI(FileName);
}

AVI_player::AVI_player(void)
{}

AVI_player::~AVI_player(void)
{}

bool AVI_player::OpenAVI(char* szFile)								// Opens An AVI File (szFile)
{
	hdc = CreateCompatibleDC(0);
	AVIFileInit();													// Opens The AVIFile Library
	if (GetLastError())
	{
		return FJC_ERROR;		// Какая-то ошибка инициализации видео библиотеки
	}
	hdd = DrawDibOpen();											// Grab A Device Context For Our Dib

	// Opens The AVI Stream
	if (AVIStreamOpenFromFile(&pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0)
	{
		// An Error Occurred Opening The Stream
		return FJC_ERROR;
	}

	AVIStreamInfo(pavi, &psi, sizeof(psi));							// Reads Information About The Stream Into psi
	width	=	psi.rcFrame.right	-	psi.rcFrame.left;			// Width Is Right Side Of Frame Minus Left
	height	=	psi.rcFrame.bottom	-	psi.rcFrame.top;			// Height Is Bottom Of Frame Minus Top

	lastframe	=	AVIStreamLength(pavi);							// The Last Frame Of The Stream

	mpf		=	AVIStreamSampleToTime(pavi,lastframe)/lastframe;	// Calculate Rough Milliseconds Per Frame

	bmih.biSize = sizeof (BITMAPINFOHEADER);						// Size Of The BitmapInfoHeader
	bmih.biPlanes = 1;												// Bitplanes	
	bmih.biBitCount = 24;											// Bits Format We Want (24 Bit, 3 Bytes)
	bmih.biWidth = twidth;											// Width We Want (256 Pixels)
	bmih.biHeight = theight;										// Height We Want (256 Pixels)
	bmih.biCompression = BI_RGB;									// Requested Mode = RGB

	hBitmap = CreateDIBSection (hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&data), NULL, NULL);
		DWORD err = GetLastError();
		if ( err != 0 )
			LF.Logf("AVI Player","Error #%d on CreateDIBSection",err);
	SelectObject (hdc, hBitmap);									// Select hBitmap Into Our Device Context (hdc)

	pgf=AVIStreamGetFrameOpen(pavi, NULL);							// Create The PGETFRAME	Using Our Request Mode
	if (pgf==NULL)
	{
		// An Error Occurred Opening The Frame
		return FJC_ERROR;
	}
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Set Texture Min Filter
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return FJC_NO_ERROR;
}

void AVI_player::GrabAVIFrame()						// Grabs A Frame From The Stream
{
	frame = (int)((1000.0*GT.GetWorldTime())/(double)mpf);
	if (frame >= lastframe) return;
	LPBITMAPINFOHEADER lpbi;						// Holds The Bitmap Header Information
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);		// Grab Data From The AVI Stream
	if (!lpbi) return;
	pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
	DrawDibDraw (hdd, hdc, 0, 0, twidth, theight, lpbi, pdata, 0, 0, width, height, 0);

	//flipIt(data,twidth*theight);					// Swap The Red And Blue Bytes (GL Compatibility)

	// Update The Texture
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, twidth, theight, GL_BGR, GL_UNSIGNED_BYTE, data);
}
// Специально для возможности кеширования
void AVI_player::GrabAVIFrame(int frame)				// Grabs A Frame From The Stream
{
	this->frame = frame;
	LPBITMAPINFOHEADER lpbi;										// Holds The Bitmap Header Information
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);		// Grab Data From The AVI Stream
	pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
	DrawDibDraw (hdd, hdc, 0, 0, twidth, theight, lpbi, pdata, 0, 0, width, height, 0);
	//flipIt(data,twidth*theight);					// Swap The Red And Blue Bytes (GL Compatibility)

	// Update The Texture
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, twidth, theight, GL_BGR, GL_UNSIGNED_BYTE, data);
}
void AVI_player::CloseAVI()								// Properly Closes The Avi File
{
	DeleteObject(hBitmap);							// Delete The Device Dependant Bitmap Object
	DrawDibClose(hdd);								// Closes The DrawDib Device Context
	AVIStreamGetFrameClose(pgf);						// Deallocates The GetFrame Resources
	AVIStreamRelease(pavi);							// Release The Stream
	AVIFileExit();										// Release The File
}

bool AVI_player::End()
{
	if (( lastframe - frame ) <= 0 )
			return true;
	else	return false;
}