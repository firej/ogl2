#pragma once

/**
 * @file AVI_player.h
 * @brief Кроссплатформенный видеоплеер
 *
 * На Windows использует VFW (Video for Windows) для AVI файлов.
 * На macOS/Linux использует FFmpeg для поддержки MP4, AVI, MKV и других форматов.
 */

#ifdef WIN32
#pragma warning(disable : 4201)
#include <vfw.h>  // Header File For Video For Windows
#pragma warning(default : 4201)
#endif

#ifdef UNICODE
typedef wchar_t TCHAR, *PTCHAR;
#else /* UNICODE */
typedef char TCHAR, *PTCHAR;
#endif

// На не-Windows платформах используем FFmpeg напрямую
#ifndef WIN32
// Forward declarations для FFmpeg структур
struct AVFormatContext;
struct AVCodecContext;
struct SwsContext;
struct AVFrame;
struct AVPacket;
#endif

class AVI_player {
   private:
    int twidth;
    int theight;
#ifdef WIN32
    AVISTREAMINFO psi;      // Pointer To A Structure Containing Stream Info
    PAVISTREAM pavi;        // Handle To An Open Stream
    PGETFRAME pgf;          // Pointer To A GetFrame Object
    BITMAPINFOHEADER bmih;  // Header Information For DrawDibDraw Decoding
    HDRAWDIB hdd;           // Handle For Our Dib
    HBITMAP hBitmap;        // Handle To A Device Dependant Bitmap
    HDC hdc;                // Creates A Compatible Device Context
    long lastframe;         // Last Frame Of The Stream
    int width;              // Video Width
    int height;             // Video Height
    char* pdata;            // Pointer To Texture Data
    int mpf;                // Will Hold Rough Milliseconds Per Frame
    unsigned char* data;    // Pointer To Our Resized Image
    int next;               // Used For Animation
    int frame;              // Frame Counter
#else
    // На macOS/Linux используем FFmpeg напрямую
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    SwsContext* swsContext;
    AVFrame* avFrame;
    AVFrame* frameRGB;
    AVPacket* packet;
    unsigned char* rgbBuffer;
    int videoStreamIndex;

    unsigned char* data;    // Указатель на данные кадра
    int frame;              // Текущий кадр
    long lastframe;         // Последний кадр
    int mpf;                // Миллисекунды на кадр
    GLuint textureID;       // ID OpenGL текстуры
    GLuint pboID;           // ID Pixel Buffer Object
    bool usePBO;            // Использовать ли PBO

    // Вспомогательные методы для FFmpeg
    bool decodeNextFrame();
    void convertFrameToRGB();
#endif

   public:
    AVI_player(void);
    ~AVI_player(void);

    /**
     * @brief Открыть видеофайл
     * @param FileName Путь к файлу (AVI на Windows, любой формат на macOS/Linux)
     * @param Width Ширина выходной текстуры (0 = использовать оригинальное разрешение)
     * @param Height Высота выходной текстуры (0 = использовать оригинальное разрешение)
     * @return true при ошибке, false при успехе (FJC_ERROR/FJC_NO_ERROR)
     */
    bool Open(TCHAR* FileName, int Width = 0, int Height = 0);
    bool OpenAVI(char* szFile);  // Opens An AVI File (szFile)

    /**
     * @brief Кэширование первых кадров
     */
    void Caching() {
        for (int i = 0; (i < 15) && (i < lastframe); i++) GrabAVIFrame(i);
        GrabAVIFrame((int)(lastframe - 1));
    }

    /**
     * @brief Получить текущий кадр на основе времени
     */
    void GrabAVIFrame();

    /**
     * @brief Получить конкретный кадр
     * @param frame Номер кадра
     */
    void GrabAVIFrame(int frame);

    /**
     * @brief Закрыть видеофайл
     */
    void CloseAVI();

    /**
     * @brief Проверить достижение конца файла
     * @return true если конец файла
     */
    bool End();

    /**
     * @brief Получить указатель на данные текущего кадра (RGB)
     * @return Указатель на буфер данных
     */
    unsigned char* GetFrameData() { return data; }

    /**
     * @brief Получить ширину выходного буфера
     */
    int GetWidth() const { return twidth; }

    /**
     * @brief Получить высоту выходного буфера
     */
    int GetHeight() const { return theight; }

    /**
     * @brief Получить текущий номер кадра
     */
    int GetCurrentFrame() const { return frame; }

    /**
     * @brief Получить общее количество кадров
     */
    long GetTotalFrames() const { return lastframe; }

    /**
     * @brief Получить ID OpenGL текстуры
     */
    GLuint GetTextureID() const { return textureID; }
};
