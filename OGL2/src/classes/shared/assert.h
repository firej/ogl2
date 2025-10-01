#pragma once

// Здесь описаны функции реагирования на ошибки

#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>

#include <cassert>

#define MY_APP_ERROR_HDR ("Found a Error!")
#define NEED_IGNORE (1)
#define NEED_DEBUG (2)

#ifdef WIN32
char __stdcall _FASSERT(void* expr, void* filename, unsigned int linenum, const char* FunctionName);
#define FASSERT(expr)                                                                                         \
    {                                                                                                         \
        if (NEED_DEBUG == _FASSERT(reinterpret_cast<void*>(expr || expr), __FILE__, __LINE__, TEXT(#expr))) { \
            __asm INT 3                                                                                       \
        };                                                                                                    \
    };
#else
// Для macOS/Linux используем стандартный assert
#define FASSERT(expr) assert(expr)
#endif

// void CheckGLErrors();
