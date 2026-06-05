#include "fontfile.h"

#include <cstdio>
#include <cstring>

// IFF-ридер/райтер движка. Содержит SwapBytes* (big-endian DWORD/WORD),
// PutTAG/PutDWORD/PutString/PutBuffer и теги FONT*. Уже имеет ветку для
// не-Windows платформ, поэтому переиспользуется как есть.
#include "../../ssrc/iff.h"

// На не-Windows платформах DWORD объявлен внутри namespace iff.
using iff::DWORD;

// Серилизация повторяет WriteFontToFile из оригинальной Font_creator/src/main.cpp,
// но метрики и PNG приходят готовыми (их считает Pango/Cairo, а не GDI).
bool WriteFontToFile(const char* fileName, const char* fontName, int fontSize,
                     const void* texturePng, uint32_t pngLen,
                     const LFontGlyph_t* glyphs, std::string* errMsg) {
    FILE* f = fopen(fileName, "wb");
    if (!f) {
        if (errMsg) *errMsg = std::string("Не удалось открыть файл для записи: ") + fileName;
        return false;
    }

    const DWORD nameLen = (DWORD)strlen(fontName);
    const DWORD namePad = iff::TestString((char*)fontName, nameLen);  // 1, если длина нечётная

    iff::PutTAG(f, ID_FORM);  // Идентификатор FORM

    // Считаем размер содержимого FORM (всё, что после FORM <size>)
    size_t fsize = 0;
    fsize += sizeof(iff::FONTFORMATNAME);                  // ID формата (LFNT)
    fsize += sizeof(iff::FONTNAME) + 4 + nameLen + namePad;  // имя шрифта
    fsize += sizeof(iff::FONTSIZE) + 4 + 4;                // размер шрифта
    fsize += sizeof(iff::FONTTEXTURE) + 4 + pngLen;        // текстура
    fsize += sizeof(iff::FONTGLYPHBLOCK) + 4 + sizeof(LFontGlyph_t) * 256;  // глифы
    iff::PutDWORD(f, (DWORD)fsize);

    iff::PutTAG(f, iff::FONTFORMATNAME.i);

    // Блок имени шрифта
    iff::PutTAG(f, iff::FONTNAME.i);
    iff::PutDWORD(f, nameLen + namePad);
    iff::PutString(f, (char*)fontName, nameLen);

    // Блок размера шрифта
    iff::PutTAG(f, iff::FONTSIZE.i);
    iff::PutDWORD(f, 4);
    iff::PutDWORD(f, (DWORD)fontSize);

    // Блок текстуры (PNG)
    iff::PutTAG(f, iff::FONTTEXTURE.i);
    iff::PutDWORD(f, pngLen);
    iff::PutBuffer(f, (void*)texturePng, pngLen);

    // Блок метрик глифов (массив из 256 пар float, native little-endian)
    iff::PutTAG(f, iff::FONTGLYPHBLOCK.i);
    iff::PutDWORD(f, 256 * sizeof(LFontGlyph_t));
    iff::PutBuffer(f, (void*)glyphs, 256 * sizeof(LFontGlyph_t));

    fclose(f);
    return true;
}
