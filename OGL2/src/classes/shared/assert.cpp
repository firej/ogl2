#include "./ASSERT.h"

#pragma warning(disable : 4996)

char __stdcall _FASSERT(void* expr, void* filename, unsigned int linenum, const char* FunctionName) {
    if (!expr) {
        //		ShowCursor(true);
        char buf[400] = "";
        sprintf(buf,
                "Не выполняется утверждение в программе!\nУтверждение:%s\nФайл:\"%s\"\nСтрока №%u\n\tПрервать - "
                "выход\n\tПовторить - перейти в Debug\n\tОтменить - игнорировать",
                FunctionName, filename, linenum);
        //		int ret = MessageBox(0, buf, MY_APP_ERROR_HDR, MB_ABORTRETRYIGNORE | MB_ICONWARNING | MB_DEFBUTTON2);
        //		switch(ret)
        //		{
        //		case IDABORT	:ExitProcess(0)		;break;
        //		case IDRETRY	:return NEED_DEBUG	;break;									// DEBUG останов
        //		case IDIGNORE	:ShowCursor(false); return NEED_IGNORE	; break;	// Игнор
        //		}
    }
    return 0;
}
/*
#include "../OGL.h"
void CheckGLErrors()
{
    GLenum err = glGetError();
    if (err!=GL_NO_ERROR)
    {
        char buf[50];
        sprintf (buf,"%d",err);
        MessageBox(NULL,buf,"GL ERROR!",MB_OK|MB_ICONERROR);
    }
}
*/
