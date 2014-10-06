#pragma once

// Здесь описаны функции реагирования на ошибки

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "..\Globals.h"
#define MY_APP_ERROR_HDR ("Found a Error!")
#define NEED_IGNORE (1)
#define NEED_DEBUG  (2)
char __stdcall _FASSERT(void* expr, void *filename, unsigned int linenum,const char* FunctionName);

//#define FASSERT(expression) _FASSERT((void*)(expression),__FILE__,__LINE__)
//#pragma warning(disable:4312)
#define FASSERT(expr) {if (NEED_DEBUG==_FASSERT(reinterpret_cast<void*>(expr || expr),__FILE__,__LINE__,TEXT( #expr))){__asm INT 3};};
//#pragma warning(default:4312)

//void CheckGLErrors();
