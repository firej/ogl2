#pragma once

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
#else
    // Заглушки для macOS/Linux
    void* psi;
    void* pavi;
    void* pgf;
    void* bmih;
    void* hdd;
    void* hBitmap;
    void* hdc;
#endif
    long lastframe;       // Last Frame Of The Stream
    int width;            // Video Width
    int height;           // Video Height
    char* pdata;          // Pointer To Texture Data
    int mpf;              // Will Hold Rough Milliseconds Per Frame
    unsigned char* data;  // Pointer To Our Resized Image
    int next;             // Used For Animation
    int frame;            // Frame Counter
   public:
    AVI_player(void);
    ~AVI_player(void);
    bool Open(TCHAR* FileName, int Width = 512, int Height = 512);
    bool OpenAVI(char* szFile);  // Opens An AVI File (szFile)
    void Caching() {
        for (int i = 0; (i < 15) && (i < lastframe); i++) GrabAVIFrame(i);
        GrabAVIFrame(lastframe - 1);
    }
    void GrabAVIFrame();           // Grabs A Frame From The Stream
    void GrabAVIFrame(int frame);  // Grabs A Frame From The Stream
    void CloseAVI();               // Properly Closes The Avi File
    bool End();
};
