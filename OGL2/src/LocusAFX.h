#pragma once
//#pragma warning(disable:4996)
#define _CRT_SECURE_NO_DEPRECATE
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#endif
#define	_WIN32_WINNT 0x0500

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <windows.h>
#include <string.h>
#include <stdio.h>

//======[ OpenGL ]===============================================================================================
#include "./Ex/MesaGL/glew.h"
#include "./Ex/MesaGL/gl.h"
#include "./Ex/MesaGL/glu.h"

#pragma comment (lib, "src/ex/MesaGL/glew32.lib")

//#define WGL_WGLEXT_PROTOTYPES		// Включаем дополнительные возможности из WGL
//#include "../Ex/MesaGL/WGLEXT.H"
#ifdef AUX_ENABLED
#include "../Ex/MesaGL/Glaux.h"
#endif

//====== Подключение графических библиотек
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#ifdef AUX_ENABLED
#pragma comment (lib, "glaux.lib")
#endif

#define ILU_ENABLED

//======[ OpenIL ( DevIL)]=======================================================================================
#include "./Ex/IL/il.h"
#ifdef ILU_ENABLED
#include "./Ex/IL/ilu.h"
#endif
//#include "../Ex/IL/ilut.h"
//====== Подключение библиотек OIL
#pragma comment (lib, "src/ex/il/DevIL.lib")
#ifdef ILU_ENABLED
#pragma comment (lib, "src/ex/il/ilu.lib")
#endif

//======[ Звук - OpenAL ]========================================================================================
#include "./EX/AL/AL.h"
#include "./EX/AL/alc.h"
#include "./EX/AL/ALut.h"
#pragma comment (lib, "src/ex/al/OpenAL32.lib")
#pragma comment (lib, "src/ex/al/ALut.lib")

#include "./classes/shared/ASSERT.h"
#include "./classes/shared/MyMath.h"
#include "./classes/files.h"
