# Font_creator (GTK4)

Кроссплатформенный порт утилиты `Font_creator` — генератора шрифтовых атласов
`.LFont` для движка OGL2. Оригинал был завязан на Win32 (GDI + ресурсный диалог +
DevIL); эта версия использует **GTK4 + Pango + Cairo + GdkPixbuf** и собирается на
Linux и macOS (а при наличии GTK4 — и на Windows).

Выходной формат `.LFont` **полностью совместим** с тем, что читает движок
(`OGL2/src/classes/Text.cpp`), — существующие шрифты и новые взаимозаменяемы.

## Что делает

Шрифт выбирается нативным пикером (`GtkFontDialogButton`), который показывает
реально установленные в системе шрифты — список берётся из Pango-fontmap, а тот
использует backend конкретной ОС: fontconfig (Linux), CoreText (macOS),
DirectWrite (Windows). По выбранному семейству (+ размер атласа, толщина, курсив,
подчёркивание, зачёркивание) утилита рендерит сетку 16×16 символов
(коды cp1251 1..255) в текстуру, считает метрики каждого глифа и упаковывает
PNG + метрики в IFF-контейнер `.LFont`.

| Оригинал (Win32) | Здесь (GTK4) |
|---|---|
| Диалог из `.rc` + `DialogProc` | GTK4 (виджеты в коде) |
| Поле ввода имени шрифта | `GtkFontDialogButton` — нативный пикер установленных шрифтов |
| `CreateFont` / `ExtTextOut` (GDI) | Pango + Cairo |
| `GetCharABCWidthsFloat` | `pango_layout_get_pixel_extents` |
| DevIL (`ilutWinSaveImage`) | GdkPixbuf (`save_to_buffer "png"`) |
| `SHBrowseForFolder` | `GtkFileDialog` |
| `MessageBox` | `GtkAlertDialog` |

## Зависимости

- CMake ≥ 3.16, компилятор C++17
- GTK4, Pango, Cairo, GdkPixbuf (тянутся вместе с `gtk4`)

```bash
# macOS
brew install gtk4
# Debian/Ubuntu
sudo apt install libgtk-4-dev cmake g++
# Fedora
sudo dnf install gtk4-devel cmake gcc-c++
```

## Сборка

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

На macOS, если pkg-config не видит gtk4, добавьте пути Homebrew:

```bash
export PKG_CONFIG_PATH="$(brew --prefix)/lib/pkgconfig:$(brew --prefix)/share/pkgconfig:$PKG_CONFIG_PATH"
```

## Запуск

GUI:

```bash
./build/font_creator_gtk
```

Безоконный режим (для скриптов/CI):

```bash
./build/font_creator_gtk --render <Имя шрифта> <размер> <выход.LFont> [--bold N] [--italic] [--underline] [--strike]
# пример:
./build/font_creator_gtk --render "Menlo" 1024 Fonts/Menlo.LFont --bold 700
```

`<размер>` — сторона квадратной текстуры (128/256/512/1024/2048). Размер шрифта
в файле = `размер/16 − 2` (как в оригинале).

## Формат .LFont (для справки)

IFF-контейнер, DWORD-ы big-endian, метрики — native little-endian float:

```
FORM <size> LFNT
  FNTN <len> <name>                 имя шрифта
  FNTS 4     <fontSize>             размер (= texSize/16 - 2)
  FNTT <len> <png>                  атлас 16×16, RGBA (RGB=255, alpha=покрытие)
  FNTG 2048  <256 × {float A, B}>   метрики: A = left bearing, B = advance − A
```

## Заметки о совместимости

- Атлас индексируется по **Windows-1251** (как при `RUSSIAN_CHARSET` в оригинале):
  байт `v` рисуется в ячейке `(col=(v−1)%16, row=(v−1)/16)`. Кириллица попадает в
  те же позиции, что и у старых шрифтов.
- Cairo рисует сверху-вниз, что совпадает с тем, как движок читает текстуру, —
  вертикальный переворот, в отличие от GDI/DIB, не нужен.
- Абсолютный масштаб метрик может на доли отличаться от Windows-версии (GDI
  меряет по высоте ячейки, Pango — по кеглю; мы калибруем размер по высоте
  `ascent+descent`, чтобы быть близко). Движок всё равно масштабирует текст через
  `Size`, поэтому на отображение это не влияет.
