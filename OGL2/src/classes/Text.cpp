#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux includes
#include <OpenGL/gl.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>

// Windows API stubs for macOS/Linux
inline bool IsBadStringPtr(const char *ptr, size_t len) {
  return ptr == nullptr;
}
#endif
#include "../../../ssrc/iff.h"
#include "../Ex/IL/il.h"
#include "./ResMan.h"
#include "./Text.h"

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
Font::~Font(void) { glDeleteLists(Base, 256); }
void Font::SetStyle(Text::Align halign, Text::Align valign, double size) {
  this->va = valign;
  this->ha = halign;
  this->Size = size;
}
void Font::Print(GLdouble X, GLdouble Y, const char *fmt, ...) {
  static char text[512]; // Место для нашей строки
  // memset(text,0,256);
  va_list ap; // Указатель на список аргументов
  if (fmt == NULL || IsBadStringPtr(fmt, 200)) // Если нет текста
    return;                                    // Ничего не делать
  va_start(ap, fmt);       // Разбор строки переменных
  vsprintf(text, fmt, ap); // И конвертирование символов в реальные коды
  va_end(ap); // Результат помещается в строку

  glPushAttrib(GL_LIST_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT |
               GL_CURRENT_BIT); // Сохранение настроек
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  T->bind();
  glDisable(GL_DEPTH_TEST); // Disables Depth Testing
  glEnable(GL_TEXTURE_2D);
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPushMatrix();              // Store The Projection Matrix
  glLoadIdentity();            // Reset The Projection Matrix

  glOrtho(0, Font::scr_width, 480, 0, -1, 1); // Set Up An Ortho Screen
  glMatrixMode(GL_MODELVIEW);                 // Select The Modelview Matrix
  glPushMatrix();                             // Store The Modelview Matrix
  glLoadIdentity();                           // Reset The Modelview Matrix

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4f(Color.d.c.x, Color.d.c.y, Color.d.c.z,
            alfa); // Установка цвета шрифта
  glTranslated(X * Font::scr_width, Y * 480.0,
               0); // Position The Text (0,0 - Bottom Left)
  switch (this->ha) {
  case Text::center: {
    size_t strl = strlen(text);
    GLdouble shift = 0.0;
    for (size_t i = 0; i < strl; i++) {
      shift += lfg[(unsigned char)text[i]].A + lfg[(unsigned char)text[i]].B;
    }
    glTranslated(-shift / 2 * (float)Size * 30 / Globals.VP.ScreenStrings, 0.0,
                 0.0);
  } break;
  case Text::right: {
    size_t strl = strlen(text);
    GLdouble shift = 0.0;
    for (size_t i = 0; i < strl; i++) {
      shift += lfg[(unsigned char)text[i]].A + lfg[(unsigned char)text[i]].B;
    }
    glTranslated(-shift * (float)Size * 30 / Globals.VP.ScreenStrings, 0.0,
                 0.0);
  } break;
  }
  glScaled(Size * 30 / Globals.VP.ScreenStrings,
           Size * 30 / Globals.VP.ScreenStrings, 1.0);
  glListBase(this->Base); // Choose The Font Base
  glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE,
              text);           // Write The Text To The Screen
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPopMatrix();               // Restore The Old Projection Matrix
  glMatrixMode(GL_MODELVIEW);  // Select The Modelview Matrix
  glPopMatrix();               // Restore The Old Projection Matrix
  glEnable(GL_DEPTH_TEST);     // Enables Depth Testing
  glPopAttrib();               // Возврат
}
Text::RESULT Font::LOAD(const char *FileName) {
  iff::tag_t tagb;      // Буферная переменная
  DWORD sizeb;          // Буфер для чтения размера
  void *texture = NULL; // Буфер для хранения текстуры
  DWORD tsize = NULL;

  FILE *f = fopen(FileName, "rb");
  if (!f)
    return Text::FILE_NOT_EXIST;
  tagb.i = iff::GetTAG(f);
  if (tagb.i != ID_FORM)
    return Text::BAD_FORMAT;
  sizeb = iff::GetDWORD(f); // Читаем размер

  tagb.i = iff::GetTAG(f); // Читаем формат файла
  if (tagb.i != iff::FONTFORMATNAME.i)
    return Text::BAD_FORMAT;
  do {
    tagb.i = iff::GetTAG(f);
    if (feof(f))
      break;
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
  fclose(f); // Файл отработан!
             // Создание самого шрифта
  // Загрузка текстуры
  // Создание пути к текстуре из пути к шрифту (они должны быть в одной папке)
  if (!texture)
    return Text::BAD_FORMAT;
  T = new TextureClass;
  if (T->LoadL(IL_PNG, texture, tsize)) {
    delete T;
    delete[] texture;
    return Text::BAD_FORMAT;
  }
  delete[] texture;
  // Создание дисплейных списков
  Base = glGenLists(256);
  int sym;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      sym = i * 16 + j + 1;
      glNewList(Base + sym, GL_COMPILE);
      glTranslatef(lfg[sym].A, 0, 0);
      glBegin(GL_QUADS);
      glTexCoord2d((j) * 0.0625,
                   (i + 15.0 / 16.0) *
                       0.0625f); // Точка в текстуре (Левая нижняя)
      glVertex2i(0, 15); // Координаты вершины (Левая нижняя)
      glTexCoord2d((j + 15.0 / 16.0) * 0.0625,
                   (i + 15.0 / 16.0) *
                       0.0625); // Точка на текстуре (Правая нижняя)
      glVertex2i(15, 15); // Координаты вершины (Правая нижняя)
      glTexCoord2d((j + 15.0 / 16.0) * 0.0625,
                   (i) * 0.0625); // Точка текстуры (Верхняя правая)
      glVertex2i(15, 0); // Координаты вершины (Верхняя правая)
      glTexCoord2d((j) * 0.0625,
                   (i) * 0.0625); // Точка текстуры (Верхняя левая)
      glVertex2i(0, 0); // Координаты вершины (Верхняя левая)
      glEnd();
      glTranslatef(lfg[sym].B, 0, 0);
      // glTranslated(10.0,0.0,0.0);
      glEndList();
    };
  };
  return Text::OK;
};

float Font::GetStrWidth(const char *str) {
  size_t strl = strlen(str);
  float shift = 0.0;
  for (size_t i = 0; i < strl; i++) {
    shift += lfg[(unsigned char)str[i]].A + lfg[(unsigned char)str[i]].B;
  }
  return shift / (float)Font::scr_width * (float)Size * 30 /
         Globals.VP.ScreenStrings;
}

void Font::ULOAD(void) {
  glDeleteLists(Base, 256);
  delete T;
}

void Font::SetColor(Vector3f C) { Color = C; }
void Font::SetColor(float r, float g, float b) {
  // Проверяем валидность указателя this
  if (this == nullptr) {
    printf("ERROR: this pointer is null in Font::SetColor!\n");
    return;
  }

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

GLdouble Font::scr_width; // Статическая переменная

void Font::SetAspectRatio() {
  switch (Globals.VP.AR) {
  case AR::NORMAL:
    Font::scr_width = 640; // Самое нормальное такое разрешеньице
    break;
  case AR::NORMAL_LCD:
    Font::scr_width = 1280.0 * 480.0 / 1024.0; // Ширина в пикселах для
    break; // разрешения 1280x1024 и пропорциональных ему
  case AR::WIDE_HDTV:
    Font::scr_width = 16.0 * 480.0 / 9.0; // HDTV разрешение 1920x1080
    break;
  case AR::WIDE_LDC:
    Font::scr_width = 16.0 * 480.0 / 10.0; // WIDE LCD - 1280x800
    break;
  }
}