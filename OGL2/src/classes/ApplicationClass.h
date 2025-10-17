#pragma once
#ifndef APPLICATIONCLASS_HEADER
#define APPLICATIONCLASS_HEADER

// #define AUX_ENABLED
// // Включение библиотеки AUX
#define _CRT_SECURE_NO_DEPRECATE
//====== Загрузка моих собственных классов и функций
#include "./Input.h"
#include "./Text.h"
#include "./Texture.h"
#include "./camera.h"
#include "./globals.h"
#include "./objects.h"
#include "./sound/Sound.h"

// #include "./PhysiX.h"
#include "./Console.h"
#include "./Logo.h"
#include "./ParticleSystem.h"
#include "./ResMan.h"
#include "./Video/AVI_player.h"
#include "./lwo_mesh.h"

#ifdef WIN32
#include "./../../resource.h"
#define MY_WINDOW_CLASS_NAME ("FJC_OGL_Window_CLASS")
#define MY_WINDOW_NAME ("FJC_OGL_Window")
typedef BOOL(APIENTRY *wglSwapIntervalEXT_Func)(int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // Функция окна
#else
// macOS/Linux заглушки для типов данных
#ifdef __APPLE__
// macOS использует GLFW для кроссплатформенности
#define USE_GLFW
#include <GLFW/glfw3.h>
// Заглушки для X11 типов (для совместимости со старым кодом)
typedef void *Display;
typedef void *XVisualInfo;
typedef unsigned long Colormap;
typedef struct {
    int dummy;
} XSetWindowAttributes;
typedef unsigned long Window;
typedef void *GLXContext;
typedef struct {
    int type;
} XEvent;
typedef int Bool;
#define MapNotify 19
#else
// Linux версия - включаем X11/GLX заголовки
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
int attributeList[] = {GLX_USE_GL, GLX_RGBA, GL_TRUE, GLX_DOUBLEBUFFER};

static Bool WaitForNotify(Display *d, XEvent *e, char *arg) {
    return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}
#endif
#endif
// Глобальные переменные
extern GLfloat LightAmbient[4];   // Значения фонового света
extern GLfloat LightDiffuse[4];   // Значения диффузного света
extern GLfloat LightPosition[4];  // Позиция света
extern TextureLogo NVLogo;
extern bool TextBenchmark;

void SwapBuffersEXT();  // Смена кадров - одна для быстрой замены

// Forward declarations
struct GLFWwindow;
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void char_callback(GLFWwindow *window, unsigned int codepoint);

// Класс описывающий работу всего графического приложения
class Application {
    friend void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

   protected:
    bool AppLoopFunc();  // Функция приёма сообщений
    Camera Cam;          // Камера для First-Person-Mode
    AVI_player AVIp;     // Плеер заставок )
    ParticleSystem PS;   // Система частиц
   public:
    CConsole CCons;  // Экземпляр консоли
                     // protected:

   public:
#ifdef WIN32
    struct Vars_t {
        struct App_t {
            HINSTANCE HInstance;  // Хендл экземпляра приложения
        } App;
        struct WindowVars_t {
            HDC DC;                  // Хендл контекста устройства
            HGLRC RC;                // Хендл контекста рендера
            HWND MHWnd;              // Хендл главного окна
            DEVMODE PrevDS, CurrDS;  // Настройки экрана до и после запуска
        } win;

    } Vars;
#else
    Display *dpy;              // Линк на дисплей
    XVisualInfo *vi;           // Инфа о визуале
    Colormap cmap;             // Карта цветов?
    XSetWindowAttributes swa;  // Атрибуты окна
    Window win;                // Само окно
    GLXContext cx;             // Контекст
    XEvent event;              // Событие

#endif
    bool IsFullScreen;  // Флаг полноэкранного режима

    Application();                  // Конструктор класса
    ~Application();                 // Деструктор класса
    bool ReadConfig();              // Загрузка настроек из файла
    bool SaveConfig();              // Сохранение настроек в файл
    bool InitOpenGL();              // Инициалазация настроек OpenGL
    bool InitOpenIL();              // Инициализация графической библиотеки
    bool GetSysInfo();              // Узнаём конфигурацию системы
    bool Birth();                   // Функция инициализации приложения
    bool Suicide();                 // Деинициализация приложения
    bool Work();                    // Нормальная работа приложения
    bool ReshapeWindow(int, int);   // Изменение размеров рисовательной области
    bool RenderScene();             // Отрисовка картинки
    void RenderTexture();           // Отрисовка текущей забинденой текстуры
    void ShowMovie(bool CanBreak);  // Показывает видик
    bool LoadResources();           // Загрузка ресурсов
    void ProcessKBInput();          // Реакция на нажатия клавиш
};

extern Application *MainApplication;  // Указатель на объект приложения
#endif                                // #ifndef APPLICATIONCLASS_HEADER
