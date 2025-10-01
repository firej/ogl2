#pragma once
//====== Общие файлы
#include "./globals.h"

#ifdef WIN32
#include <windows.h>
#else
#include <limits.h>
#define _MAX_PATH PATH_MAX
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
// DevIL типы для macOS/Linux
typedef unsigned int ILenum;
typedef unsigned int ILuint;
#endif
#include <stdio.h>
#include <stdlib.h>

class TextureClass {
   protected:
    char TextureFileName[_MAX_PATH];  // Имя файла с рисунком
    int iHeight, iWidth, Bpp;         // Ширина, высота и !БАЙТОВОСТЬ! рисунка
    ILenum FType;                     // Тип текстуры (JPEG,BMP,PNG...)
    int size;                         // Размер текстуры
    unsigned int type;                // Тип хранения данных
    unsigned char* data;              // Данные картинки
    int FilterMode;                   // Режим отображения текстуры
   public:
    GLuint Number;  // Индекс текстуры

    TextureClass(void);
    ~TextureClass(void);

    void SetFileName(const char* Name);                // Устанавливает имя файла для текстуры
    void SetFileType(ILenum Type);                     // Установка типа файла
    void SetFM(int FM);                                // Установить режим фильтрации
    char* GetFileName(void);                           // Возврат имени файла с текстурой
    int GetNumber(void) { return Number; };            // Должен возвращать номер текстуры
    bool Load();                                       // Грузит текстуру в память
    bool Load(int FM, ILenum Type, const char* Name);  // Таж фигня но с параметрами
    bool Load(const char* Name);                       // Здесь OIL сам определяет тип и фильтрация по умолчанию
    bool LoadL(ILenum Type, void* Lump, ILuint size);  // Загрузка текстуры из памяти

    const int GetWidth() { return iWidth; };  // No comments :=)
    const int GetHeight() { return iHeight; };

    void bind()  // Задействовать текстуру
    {
        glBindTexture(GL_TEXTURE_2D, Number);
    }
};

typedef TextureClass* TexturePointer;

#ifdef ENABLE_TEXTURE_RENDERING

#define FJC_RTEX_NO_FILTERING 0  // Режимы фильтрации для рендер-текстур
#define FJC_RTEX_BILINEAR_FILTERING 1

class FJCRendererTexture {
   protected:
    GLuint Number;             // Индекс текстуры
    int iHeight, iWidth, Bpp;  // Ширина, высота и !БАЙТОВОСТЬ! рисунка
    int size;                  // Размер текстуры	(и байтах)
    unsigned char* data;       // Данные картинки
    int FilterMode;            // Режим отображения текстуры
    char ScreenShot[64];       // Имя для скрина
    unsigned int ScreenIndex;  // Индекс имени скрина
   public:
    FJCRendererTexture(void);
    ~FJCRendererTexture(void);

    void SetSize(int x, int y)  // установить размер
    {
        iHeight = y;
        iWidth = x;
        size = x * y * 4;
    };
    void SetFM(int FM)  // Установить режим фильтрации
    {
        FilterMode = FM;
    };
    int GetNumber(void)  // Должен возвращать номер текстуры
    {
        return Number;
    };

    void Init(int Width, int Height, int FilterMode = FJC_RTEX_BILINEAR_FILTERING);
    void Init();  // Инициализация тектуры (выделение памяти и вся фигня)

    void CopyBackbufferToTexture();  // Копировать backbuffer в текстуру

    void bind();  // Задействовать текстуру

    void Save();  // Сохранение картинки на винт
};
extern FJCRendererTexture RendTex;  // Одна текстурка для работы
#endif
