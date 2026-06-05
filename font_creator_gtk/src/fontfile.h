#ifndef FONTFILE_H
#define FONTFILE_H
// Запись шрифта в формат .LFont (IFF-контейнер движка OGL2).
// Формат полностью совместим с тем, что читает OGL2/src/classes/Text.cpp:
//   FORM <size> LFNT
//     FNTN <len> <name>            имя шрифта (для информации)
//     FNTS 4     <fontSize>        размер (= TexWidth/16 - 2)
//     FNTT <len> <png>             текстура-атлас 16x16, RGBA (RGB=255, A=coverage)
//     FNTG 2048  <256 * {float A,B}>  метрики глифов (native little-endian float)
#include <cstdint>
#include <string>

// Метрики одного глифа. A — смещение до символа (left bearing),
// B — смещение после символа (advance - left bearing). Должно быть ровно 8 байт.
struct LFontGlyph_t {
    float A;
    float B;
};

// Сохраняет .LFont. glyphs — массив из 256 элементов. Возвращает false и
// сообщение об ошибке в errMsg при неудаче.
bool WriteFontToFile(const char* fileName, const char* fontName, int fontSize,
                     const void* texturePng, uint32_t pngLen,
                     const LFontGlyph_t* glyphs, std::string* errMsg);

#endif  // FONTFILE_H
