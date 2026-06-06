#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux includes
#include <OpenGL/gl.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>

// Windows API stubs for macOS/Linux
inline bool IsBadStringPtr(const char *ptr, size_t len) { return ptr == nullptr; }
#endif
#include "../../../ssrc/iff.h"
#include "../Ex/IL/il.h"
#include "./ResMan.h"
#include "./gl/immediate.h"
#include "./text.h"

Font::Font(void) {
    // Инициализируем поля по отдельности вместо memset
    Base = 0;
    T = nullptr;
    va = Text::top;
    ha = Text::left;
    memset(lfg, 0, sizeof(lfg));
    Color = Vector3f(1.0f, 1.0f, 1.0f);
    alfa = 1.0f;
    Size = 1.0;
}
Font::~Font(void) {}  // display lists больше не используются
void Font::SetStyle(Text::Align halign, Text::Align valign, double size) {
    this->va = valign;
    this->ha = halign;
    this->Size = size;
}
void Font::Print(GLdouble X, GLdouble Y, const char *fmt, ...) {
    static char text[512];  // Место для нашей строки
    // memset(text,0,256);
    va_list ap;                                   // Указатель на список аргументов
    if (fmt == NULL || IsBadStringPtr(fmt, 200))  // Если нет текста
        return;                                   // Ничего не делать
    va_start(ap, fmt);                            // Разбор строки переменных
    vsnprintf(text, sizeof(text), fmt, ap);       // И конвертирование символов в реальные коды
    va_end(ap);                                   // Результат помещается в строку

    glPushAttrib(GL_LIST_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);  // Сохранение настроек
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    T->bind();
    glDisable(GL_DEPTH_TEST);  // Disables Depth Testing
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);  // Select The Projection Matrix
    glPushMatrix();               // Store The Projection Matrix
    glLoadIdentity();             // Reset The Projection Matrix

    glOrtho(0, Font::scr_width, 480, 0, -1, 1);  // Set Up An Ortho Screen
    glMatrixMode(GL_MODELVIEW);                  // Select The Modelview Matrix
    glPushMatrix();                              // Store The Modelview Matrix
    glLoadIdentity();                            // Reset The Modelview Matrix

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // цвет шрифта задаётся в батчере (imColor4f ниже)
    glTranslated(X * Font::scr_width, Y * 480.0,
                 0);  // Position The Text (0,0 - Bottom Left)
    switch (this->ha) {
        case Text::center: {
            size_t strl = strlen(text);
            GLdouble shift = 0.0;
            for (size_t i = 0; i < strl; i++) {
                shift += lfg[(unsigned char)text[i]].A + lfg[(unsigned char)text[i]].B;
            }
            glTranslated(-shift / 2 * (float)Size * 30 / Globals.VP.ScreenStrings, 0.0, 0.0);
        } break;
        case Text::right: {
            size_t strl = strlen(text);
            GLdouble shift = 0.0;
            for (size_t i = 0; i < strl; i++) {
                shift += lfg[(unsigned char)text[i]].A + lfg[(unsigned char)text[i]].B;
            }
            glTranslated(-shift * (float)Size * 30 / Globals.VP.ScreenStrings, 0.0, 0.0);
        } break;
    }
    glScaled(Size * 30 / Globals.VP.ScreenStrings, Size * 30 / Globals.VP.ScreenStrings, 1.0);

    // Рисуем строку текстурированными квадами глифов через батчер (вместо display
    // lists). Матрицы (ortho + позиция/выравнивание/масштаб) батчер снимет из GL,
    // продвижение по строке (A/B) запекаем в координату x.
    gl::imColor4f((float)Color.d.c.x, (float)Color.d.c.y, (float)Color.d.c.z, alfa);
    gl::imTexture(T->Number);
    gl::imBegin(GL_QUADS);
    float x = 0.0f;
    size_t len = strlen(text);
    for (size_t n = 0; n < len; n++) {
        unsigned char b = (unsigned char)text[n];
        if (b == 0) continue;
        int cell = b - 1;  // ячейка атласа 16x16, как при создании списков
        int i = cell / 16, j = cell % 16;
        float u0 = (float)(j * 0.0625), u1 = (float)((j + 15.0 / 16.0) * 0.0625);
        float v0 = (float)(i * 0.0625), v1 = (float)((i + 15.0 / 16.0) * 0.0625);
        x += lfg[b].A;
        gl::imTexCoord2f(u0, v1); gl::imVertex2f(x + 0.0f, 15.0f);   // левая нижняя
        gl::imTexCoord2f(u1, v1); gl::imVertex2f(x + 15.0f, 15.0f);  // правая нижняя
        gl::imTexCoord2f(u1, v0); gl::imVertex2f(x + 15.0f, 0.0f);   // правая верхняя
        gl::imTexCoord2f(u0, v0); gl::imVertex2f(x + 0.0f, 0.0f);    // левая верхняя
        x += lfg[b].B;
    }
    gl::imEnd();

    glMatrixMode(GL_PROJECTION);  // Select The Projection Matrix
    glPopMatrix();                // Restore The Old Projection Matrix
    glMatrixMode(GL_MODELVIEW);   // Select The Modelview Matrix
    glPopMatrix();                // Restore The Old Projection Matrix
    glEnable(GL_DEPTH_TEST);      // Enables Depth Testing
    glPopAttrib();                // Возврат
}
Text::RESULT Font::LOAD(const char *FileName) {
    iff::tag_t tagb;       // Буферная переменная
    DWORD sizeb;           // Буфер для чтения размера
    char *texture = NULL;  // Буфер для хранения текстуры
    DWORD tsize = 0;

    FILE *f = fopen(FileName, "rb");
    if (!f) return Text::FILE_NOT_EXIST;
    tagb.i = iff::GetTAG(f);
    if (tagb.i != ID_FORM) return Text::BAD_FORMAT;
    sizeb = iff::GetDWORD(f);  // Читаем размер

    tagb.i = iff::GetTAG(f);  // Читаем формат файла
    if (tagb.i != iff::FONTFORMATNAME.i) return Text::BAD_FORMAT;
    do {
        tagb.i = iff::GetTAG(f);
        if (feof(f)) break;
        if (tagb.i == iff::FONTTEXTURE.i) {
            sizeb = iff::GetDWORD(f);
            texture = new char[sizeb];
            tsize = sizeb;
            iff::GetBuffer(f, texture, sizeb);
        } else if (tagb.i == iff::FONTGLYPHBLOCK.i) {
            sizeb = iff::GetDWORD(f);
            iff::GetBuffer(f, this->lfg, sizeb);
        } else
            iff::read_unknow_chunk(f);
    } while (!feof(f));
    fclose(f);  // Файл отработан!
                // Создание самого шрифта
    // Загрузка текстуры
    // Создание пути к текстуре из пути к шрифту (они должны быть в одной папке)
    if (!texture) return Text::BAD_FORMAT;
    T = new TextureClass;
    if (T->LoadL(IL_PNG, texture, tsize)) {
        delete T;
        delete[] texture;
        return Text::BAD_FORMAT;
    }
    delete[] texture;
    // Display lists больше не нужны — строки рисуются батчером в Print()
    // (метрики глифов уже загружены в lfg).
    Base = 0;
    return Text::OK;
};

float Font::GetStrWidth(const char *str) {
    size_t strl = strlen(str);
    float shift = 0.0;
    for (size_t i = 0; i < strl; i++) {
        shift += lfg[(unsigned char)str[i]].A + lfg[(unsigned char)str[i]].B;
    }
    return shift / (float)Font::scr_width * (float)Size * 30 / Globals.VP.ScreenStrings;
}

void Font::ULOAD(void) { delete T; }

void Font::SetColor(Vector3f C) { Color = C; }
void Font::SetColor(float r, float g, float b) {
    Vector3f temp_color(r, g, b);
    Color = temp_color;
}
void Font::SetColor(Vector3f C, float alfap) {
    Color = C;
    alfa = alfap;
}
void Font::SetColor(float r, float g, float b, float alfap) {
    Color = Vector3f(r, g, b);
    alfa = alfap;
}

GLdouble Font::scr_width;  // Статическая переменная

void Font::SetAspectRatio() {
    switch (Globals.VP.AR) {
        case AR::NORMAL:
            Font::scr_width = 640;  // Самое нормальное такое разрешеньице
            break;
        case AR::NORMAL_LCD:
            Font::scr_width = 1280.0 * 480.0 / 1024.0;  // Ширина в пикселах для
            break;                                      // разрешения 1280x1024 и пропорциональных ему
        case AR::WIDE_HDTV:
            Font::scr_width = 16.0 * 480.0 / 9.0;  // HDTV разрешение 1920x1080
            break;
        case AR::WIDE_LDC:
            Font::scr_width = 16.0 * 480.0 / 10.0;  // WIDE LCD - 1280x800
            break;
    }
}
