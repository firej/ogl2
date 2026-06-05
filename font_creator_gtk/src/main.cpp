// Кроссплатформенный Locus Engine FontBuilder на GTK4 + Pango/Cairo.
// Порт оригинальной Windows-утилиты Font_creator (Win32 GDI + DevIL).
// Генерирует шрифтовые атласы .LFont, совместимые с движком OGL2.
#include <gtk/gtk.h>
#include <pango/pangocairo.h>

#include <cstring>
#include <string>
#include <vector>

#include "cp1251.h"
#include "fontfile.h"

static_assert(sizeof(LFontGlyph_t) == 8, "LFontGlyph_t must be 8 bytes");

// Тестовая строка превью (UTF-8). В оригинале — cp1251.
static const char* TEST_STRING = "ABCDabcd АБВГабвг 123 [!,.;:]";

// Список толщин — как в оригинале (значение FW_* + подпись).
static const char* BOLD_LABELS[] = {
    "  0 - FW_DONTCARE",  "100 - FW_THIN",    "200 - FW_EXTRALIGHT",
    "300 - FW_LIGHT",     "400 - FW_NORMAL",  "500 - FW_MEDIUM",
    "600 - FW_SEMIBOLD",  "700 - FW_BOLD",    "800 - FW_EXTRABOLD",
    "900 - FW_HEAVY",     nullptr};
static const int BOLD_VALUES[10] = {0, 100, 200, 300, 400, 500, 600, 700, 800, 900};

static const char* TEXSIZE_LABELS[] = {"128x128", "256x256", "512x512",
                                       "1024x1024", "2048x2048", nullptr};
static const int TEXSIZE_VALUES[5] = {128, 256, 512, 1024, 2048};

struct AppState {
    GtkWidget* window = nullptr;
    GtkFontDialogButton* font_btn = nullptr;  // нативный пикер шрифта
    GtkWidget* drawing = nullptr;
    GtkDropDown* dd_texsize = nullptr;
    GtkDropDown* dd_bold = nullptr;
    GtkCheckButton* chk_italic = nullptr;
    GtkCheckButton* chk_underline = nullptr;
    GtkCheckButton* chk_strike = nullptr;
    GtkCheckButton* chk_mono = nullptr;
    GtkWidget* entry_folder = nullptr;
};
static AppState S;

struct Params {
    std::string family;
    int weight = 400;  // PangoWeight (100..900)
    bool italic = false;
    bool underline = false;
    bool strike = false;
};

static Params current_params() {
    Params p;
    const char* name = nullptr;
    PangoFontDescription* fd = gtk_font_dialog_button_get_font_desc(S.font_btn);
    if (fd) name = pango_font_description_get_family(fd);  // владелец — кнопка, не free
    p.family = (name && *name) ? name : "Sans";
    guint bi = gtk_drop_down_get_selected(S.dd_bold);
    p.weight = BOLD_VALUES[bi < 10 ? bi : 5];
    if (p.weight == 0) p.weight = 400;  // FW_DONTCARE -> normal
    p.italic = gtk_check_button_get_active(S.chk_italic);
    p.underline = gtk_check_button_get_active(S.chk_underline);
    p.strike = gtk_check_button_get_active(S.chk_strike);
    return p;
}

static int texsize_value() {
    guint i = gtk_drop_down_get_selected(S.dd_texsize);
    return TEXSIZE_VALUES[i < 5 ? i : 3];
}

// Создаёт описание шрифта по параметрам. Размер задаётся отдельно.
static PangoFontDescription* make_desc(const Params& p, double size_px) {
    PangoFontDescription* d = pango_font_description_new();
    pango_font_description_set_family(d, p.family.c_str());
    pango_font_description_set_weight(d, (PangoWeight)p.weight);
    pango_font_description_set_style(d, p.italic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);
    pango_font_description_set_absolute_size(d, size_px * PANGO_SCALE);
    return d;
}

static PangoAttrList* make_attrs(const Params& p) {
    PangoAttrList* al = pango_attr_list_new();
    if (p.underline)
        pango_attr_list_insert(al, pango_attr_underline_new(PANGO_UNDERLINE_SINGLE));
    if (p.strike) pango_attr_list_insert(al, pango_attr_strikethrough_new(TRUE));
    return al;
}

static void show_msg(const std::string& m) {
    GtkAlertDialog* d = gtk_alert_dialog_new("%s", m.c_str());
    gtk_alert_dialog_show(d, GTK_WINDOW(S.window));
    g_object_unref(d);
}

// ---------------------------------------------------------------------------
// Превью
// ---------------------------------------------------------------------------
static void preview_draw(GtkDrawingArea*, cairo_t* cr, int w, int h, gpointer) {
    cairo_set_source_rgb(cr, 1, 1, 1);  // белый фон, как в оригинале
    cairo_paint(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);  // чёрный текст

    Params p = current_params();
    PangoLayout* layout = pango_cairo_create_layout(cr);
    PangoFontDescription* d = make_desc(p, 22.0);
    pango_layout_set_font_description(layout, d);
    PangoAttrList* al = make_attrs(p);
    pango_layout_set_attributes(layout, al);
    pango_attr_list_unref(al);
    pango_layout_set_text(layout, TEST_STRING, -1);

    int tw = 0, th = 0;
    pango_layout_get_pixel_size(layout, &tw, &th);
    cairo_move_to(cr, (w - tw) / 2.0, (h - th) / 2.0);
    pango_cairo_show_layout(cr, layout);

    pango_font_description_free(d);
    g_object_unref(layout);
}

static void on_input_changed(GtkWidget*, gpointer) {
    if (S.drawing) gtk_widget_queue_draw(S.drawing);
}

// ---------------------------------------------------------------------------
// Рендер атласа
// ---------------------------------------------------------------------------
// Рисует сетку 16x16 глифов (коды cp1251 1..255), заполняет метрики и
// кодирует результат в PNG (RGB=белый, alpha=покрытие глифа).
static bool render_atlas(const Params& p, int W, int H, std::vector<guint8>& pngOut,
                         LFontGlyph_t glyphs[256], std::string& err) {
    const int PPS = W / 16;                       // пикселей на символ (ячейка)
    double fontSize = (double)(PPS - 2);          // как в оригинале: Width/16 - 2
    if (fontSize < 1) fontSize = 1;
    memset(glyphs, 0, sizeof(LFontGlyph_t) * 256);

    // A8-поверхность = канал покрытия (coverage), ровно то, что нужно в alpha.
    cairo_surface_t* surf = cairo_image_surface_create(CAIRO_FORMAT_A8, W, H);
    if (cairo_surface_status(surf) != CAIRO_STATUS_SUCCESS) {
        err = "Не удалось создать поверхность Cairo (слишком большой размер?)";
        cairo_surface_destroy(surf);
        return false;
    }
    cairo_t* cr = cairo_create(surf);
    cairo_set_source_rgba(cr, 1, 1, 1, 1);  // непрозрачный источник -> coverage в A8

    PangoLayout* layout = pango_cairo_create_layout(cr);
    PangoContext* ctx = pango_layout_get_context(layout);

    // Калибруем размер так, чтобы высота ячейки (ascent+descent) ≈ PPS — это
    // повторяет семантику CreateFont(height) в оригинале на GDI.
    PangoFontDescription* desc = make_desc(p, PPS);
    PangoFontMetrics* fm = pango_context_get_metrics(ctx, desc, nullptr);
    double asc = pango_font_metrics_get_ascent(fm) / (double)PANGO_SCALE;
    double dsc = pango_font_metrics_get_descent(fm) / (double)PANGO_SCALE;
    pango_font_metrics_unref(fm);
    double cell = asc + dsc;
    double scale = (cell > 1.0) ? (double)PPS / cell : 1.0;
    pango_font_description_set_absolute_size(desc, PPS * scale * PANGO_SCALE);
    pango_layout_set_font_description(layout, desc);

    // Нормировка метрик: 12 единиц на "кегль", перевод points->logical (96/72),
    // деление на fontSize — как в оригинальном WriteFontToFile/PointSizetoLogicalX.
    const double K = 12.0 * (96.0 / 72.0) / fontSize;

    for (int v = 1; v <= 255; ++v) {
        char utf8[5];
        int n = cp1251_to_utf8((unsigned char)v, utf8);
        if (n == 0) continue;  // неопределённый символ cp1251

        const int idx = v - 1;
        const int row = idx / 16;
        const int col = idx % 16;

        pango_layout_set_text(layout, utf8, n);
        PangoAttrList* al = make_attrs(p);
        pango_layout_set_attributes(layout, al);
        pango_attr_list_unref(al);

        cairo_move_to(cr, col * PPS, row * PPS);  // верх-лево ячейки (как TA_TOP)
        pango_cairo_show_layout(cr, layout);

        PangoRectangle ink, logical;
        pango_layout_get_pixel_extents(layout, &ink, &logical);
        double lsb = ink.x;            // левый отступ (left side bearing)
        double adv = logical.width;    // ширина-продвижение (advance)
        glyphs[v].A = (float)(K * lsb);
        glyphs[v].B = (float)(K * (adv - lsb));
    }

    pango_font_description_free(desc);
    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_flush(surf);

    // Собираем RGBA: RGB=белый, alpha=покрытие из A8.
    int stride = cairo_image_surface_get_stride(surf);
    const unsigned char* sdata = cairo_image_surface_get_data(surf);
    std::vector<guint8> rgba((size_t)W * H * 4);
    for (int y = 0; y < H; ++y) {
        const unsigned char* srow = sdata + (size_t)y * stride;
        for (int x = 0; x < W; ++x) {
            size_t o = ((size_t)y * W + x) * 4;
            rgba[o + 0] = 255;
            rgba[o + 1] = 255;
            rgba[o + 2] = 255;
            rgba[o + 3] = srow[x];
        }
    }
    cairo_surface_destroy(surf);

    // Кодируем PNG (GdkPixbuf хранит RGBA без премультипликации).
    GdkPixbuf* pb = gdk_pixbuf_new_from_data(rgba.data(), GDK_COLORSPACE_RGB, TRUE, 8,
                                             W, H, W * 4, nullptr, nullptr);
    gchar* buf = nullptr;
    gsize buflen = 0;
    GError* gerr = nullptr;
    gboolean ok = gdk_pixbuf_save_to_buffer(pb, &buf, &buflen, "png", &gerr, nullptr);
    g_object_unref(pb);
    if (!ok) {
        err = gerr ? gerr->message : "Не удалось закодировать PNG";
        if (gerr) g_error_free(gerr);
        return false;
    }
    pngOut.assign(buf, buf + buflen);
    g_free(buf);
    return true;
}

static void on_render(GtkButton*, gpointer) {
    Params p = current_params();
    int W = texsize_value();

    std::vector<guint8> png;
    LFontGlyph_t glyphs[256];
    std::string err;
    if (!render_atlas(p, W, W, png, glyphs, err)) {
        show_msg("Ошибка рендеринга: " + err);
        return;
    }

    const char* folderC = gtk_editable_get_text(GTK_EDITABLE(S.entry_folder));
    std::string folder = (folderC && *folderC) ? folderC : ".";
    int fontSize = W / 16 - 2;
    if (fontSize < 1) fontSize = 1;
    std::string path = folder + "/" + p.family + ".LFont";

    std::string werr;
    if (!WriteFontToFile(path.c_str(), p.family.c_str(), fontSize, png.data(),
                         (uint32_t)png.size(), glyphs, &werr)) {
        show_msg("Ошибка сохранения: " + werr);
        return;
    }
    show_msg("Шрифт сохранён:\n" + path);
}

// ---------------------------------------------------------------------------
// Выбор папки
// ---------------------------------------------------------------------------
static void folder_chosen(GObject* src, GAsyncResult* res, gpointer) {
    GError* e = nullptr;
    GFile* f = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(src), res, &e);
    if (f) {
        char* path = g_file_get_path(f);
        if (path) {
            gtk_editable_set_text(GTK_EDITABLE(S.entry_folder), path);
            g_free(path);
        }
        g_object_unref(f);
    }
    if (e) g_error_free(e);  // отмена пользователем — тоже сюда, молча игнорируем
}

static void on_browse(GtkButton*, gpointer) {
    GtkFileDialog* d = gtk_file_dialog_new();
    gtk_file_dialog_set_title(d, "Выберите папку для сохранения файлов шрифта…");
    gtk_file_dialog_select_folder(d, GTK_WINDOW(S.window), nullptr, folder_chosen, nullptr);
    g_object_unref(d);
}

static void on_exit(GtkButton*, gpointer) { gtk_window_close(GTK_WINDOW(S.window)); }

// ---------------------------------------------------------------------------
// UI
// ---------------------------------------------------------------------------
static GtkWidget* labeled(const char* text) {
    GtkWidget* l = gtk_label_new(text);
    gtk_widget_set_halign(l, GTK_ALIGN_START);
    return l;
}

static void on_activate(GtkApplication* app, gpointer) {
    S.window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(S.window), "Locus Engine FontBuilder");
    gtk_window_set_default_size(GTK_WINDOW(S.window), 480, 0);

    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(box, 12);
    gtk_widget_set_margin_bottom(box, 12);
    gtk_widget_set_margin_start(box, 12);
    gtk_widget_set_margin_end(box, 12);
    gtk_window_set_child(GTK_WINDOW(S.window), box);

    gtk_box_append(GTK_BOX(box), labeled("Шрифт (из установленных в системе):"));
    GtkFontDialog* fdlg = gtk_font_dialog_new();
    gtk_font_dialog_set_title(fdlg, "Выберите шрифт");
    // gtk_font_dialog_button_new забирает владение диалогом (transfer full).
    S.font_btn = GTK_FONT_DIALOG_BUTTON(gtk_font_dialog_button_new(fdlg));
    gtk_font_dialog_button_set_level(S.font_btn, GTK_FONT_LEVEL_FAMILY);  // только семейство
    PangoFontDescription* initdesc = pango_font_description_from_string("Consolas");
    gtk_font_dialog_button_set_font_desc(S.font_btn, initdesc);
    pango_font_description_free(initdesc);
    g_signal_connect(S.font_btn, "notify::font-desc", G_CALLBACK(on_input_changed), nullptr);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(S.font_btn));

    // Превью
    S.drawing = gtk_drawing_area_new();
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(S.drawing), 456);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(S.drawing), 64);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(S.drawing), preview_draw, nullptr,
                                   nullptr);
    GtkWidget* frame = gtk_frame_new(nullptr);
    gtk_frame_set_child(GTK_FRAME(frame), S.drawing);
    gtk_box_append(GTK_BOX(box), frame);

    // Размер текстуры
    gtk_box_append(GTK_BOX(box), labeled("Размер текстуры атласа:"));
    S.dd_texsize = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(TEXSIZE_LABELS));
    gtk_drop_down_set_selected(S.dd_texsize, 3);  // 1024x1024
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(S.dd_texsize));

    // Толщина
    gtk_box_append(GTK_BOX(box), labeled("Толщина (Weight):"));
    S.dd_bold = GTK_DROP_DOWN(gtk_drop_down_new_from_strings(BOLD_LABELS));
    gtk_drop_down_set_selected(S.dd_bold, 5);  // FW_MEDIUM
    g_signal_connect(S.dd_bold, "notify::selected", G_CALLBACK(on_input_changed), nullptr);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(S.dd_bold));

    // Стили
    GtkWidget* styles = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    S.chk_italic = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Курсив"));
    S.chk_underline = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Подчёркнутый"));
    S.chk_strike = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Зачёркнутый"));
    S.chk_mono = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Моноширинный"));
    gtk_widget_set_sensitive(GTK_WIDGET(S.chk_mono), FALSE);  // как в оригинале (WS_DISABLED)
    for (GtkCheckButton* c : {S.chk_italic, S.chk_underline, S.chk_strike})
        g_signal_connect(c, "toggled", G_CALLBACK(on_input_changed), nullptr);
    gtk_box_append(GTK_BOX(styles), GTK_WIDGET(S.chk_italic));
    gtk_box_append(GTK_BOX(styles), GTK_WIDGET(S.chk_underline));
    gtk_box_append(GTK_BOX(styles), GTK_WIDGET(S.chk_strike));
    gtk_box_append(GTK_BOX(styles), GTK_WIDGET(S.chk_mono));
    gtk_box_append(GTK_BOX(box), styles);

    // Папка сохранения
    gtk_box_append(GTK_BOX(box), labeled("Папка для сохранения:"));
    GtkWidget* folderRow = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    S.entry_folder = gtk_entry_new();
    gtk_editable_set_text(GTK_EDITABLE(S.entry_folder), "Fonts");
    gtk_widget_set_hexpand(S.entry_folder, TRUE);
    GtkWidget* browse = gtk_button_new_with_label("…");
    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse), nullptr);
    gtk_box_append(GTK_BOX(folderRow), S.entry_folder);
    gtk_box_append(GTK_BOX(folderRow), browse);
    gtk_box_append(GTK_BOX(box), folderRow);

    // Кнопки
    GtkWidget* buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_top(buttons, 6);
    GtkWidget* btnExit = gtk_button_new_with_label("Выход");
    GtkWidget* btnRender = gtk_button_new_with_label("Создать шрифт");
    gtk_widget_set_hexpand(btnExit, TRUE);
    gtk_widget_set_hexpand(btnRender, TRUE);
    gtk_widget_add_css_class(btnRender, "suggested-action");
    g_signal_connect(btnExit, "clicked", G_CALLBACK(on_exit), nullptr);
    g_signal_connect(btnRender, "clicked", G_CALLBACK(on_render), nullptr);
    gtk_box_append(GTK_BOX(buttons), btnExit);
    gtk_box_append(GTK_BOX(buttons), btnRender);
    gtk_box_append(GTK_BOX(box), buttons);

    gtk_window_present(GTK_WINDOW(S.window));
}

// Безоконный режим для скриптов/CI:
//   font_creator_gtk --render <family> <texsize> <out.LFont> [--bold N] [--italic]
static int run_batch(int argc, char** argv) {
    Params p;
    p.family = argv[2];
    int W = atoi(argv[3]);
    const char* out = argv[4];
    for (int i = 5; i < argc; ++i) {
        if (!strcmp(argv[i], "--bold") && i + 1 < argc) p.weight = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--italic")) p.italic = true;
        else if (!strcmp(argv[i], "--underline")) p.underline = true;
        else if (!strcmp(argv[i], "--strike")) p.strike = true;
    }
    if (p.weight == 0) p.weight = 400;

    std::vector<guint8> png;
    LFontGlyph_t glyphs[256];
    std::string err;
    if (!render_atlas(p, W, W, png, glyphs, err)) {
        g_printerr("render error: %s\n", err.c_str());
        return 1;
    }
    int fontSize = W / 16 - 2;
    if (fontSize < 1) fontSize = 1;
    std::string werr;
    if (!WriteFontToFile(out, p.family.c_str(), fontSize, png.data(),
                         (uint32_t)png.size(), glyphs, &werr)) {
        g_printerr("write error: %s\n", werr.c_str());
        return 1;
    }
    g_print("Saved %s (%zu bytes texture, %dx%d)\n", out, png.size(), W, W);
    return 0;
}

int main(int argc, char** argv) {
    if (argc >= 5 && !strcmp(argv[1], "--render")) return run_batch(argc, argv);

    GtkApplication* app =
        gtk_application_new("org.locus.fontbuilder", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), nullptr);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
