#pragma once
#include "./text.h"
#include "./texture.h"

#ifndef WIN32
// DevIL константы для macOS/Linux
#ifndef IL_PNG
#define IL_PNG 0x0425
#endif
#endif

/************************************************************************************************************\
\*********************************	Супер прикольная штукенция от nVidia
*********************************/
class TextureLogo {
   protected:
    const char *m_logoname;
    TextureClass Logo;
    int m_w, m_h;

   public:
    TextureLogo() {};
    bool init(unsigned int logo_file_type = IL_PNG, const char *logoname = "data/textures/MySimpleTexture.png") {
        m_logoname = logoname;
        Logo.Load(FJC_TEX_ANISOTROPIC_FILTERING, logo_file_type, m_logoname);

        m_w = Logo.GetWidth();
        m_h = Logo.GetHeight();
        return true;
    }
    void render_logo(float scale = 0.5, float alpha = 0.5, bool left = false, bool top = true) {
        GLint vp[4];
        glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Настроим глубину
        glDepthFunc(GL_NOTEQUAL);
        glDepthMask(false);
        glDisable(GL_DEPTH_TEST);

        if (glActiveTextureARB) {
            glActiveTextureARB(GL_TEXTURE1_ARB);
            glDisable(GL_TEXTURE_2D);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
            glActiveTextureARB(GL_TEXTURE0_ARB);
        }
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        Logo.bind();

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glGetIntegerv(GL_VIEWPORT, vp);
        glLoadIdentity();
        gluOrtho2D(vp[0], vp[2], vp[1], vp[3]);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        float x, y;
        x = left ? 0 : vp[2] - m_w * scale;
        y = top ? vp[3] - m_h * scale : 0;
        glColor4f(1.0f, 1.0f, 1.0f, alpha);

        glBegin(GL_QUADS);
        glTexCoord2f(0, 1);
        glVertex3f(x, y, 0);
        glTexCoord2f(1, 1);
        glVertex3f(x + m_w * scale, y, 0);
        glTexCoord2f(1, 0);
        glVertex3f(x + m_w * scale, y + m_h * scale, 0);
        glTexCoord2f(0, 0);
        glVertex3f(x + 0, y + m_h * scale, 0);
        glEnd();

        glDepthMask(true);

        glPopAttrib();

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

#define FJC_STARTUP_LOGO_MODE_BEGIN 0              // "Появить" лого
#define FJC_STARTUP_LOGO_MODE_PROCEED_WITH_TEXT 1  // Показать текст
#define FJC_STARTUP_LOGO_MODE_PROCEED 2            // Очистить
#define FJC_STARTUP_LOGO_MODE_END 3                // Затушить лого
#ifdef _DEBUG
#define FJC_STARTUP_LOGO_STEPS_TO_BLEND 10  // Количетво шагов для блендинга
#else
#define FJC_STARTUP_LOGO_STEPS_TO_BLEND 100
#endif
#define FJC_STARTUP_LOGO_TIME_TO_START 300  // Время появления/затухания в миллисекундах

class TextBlock {
   public:
    Font *font;
    const char *text;
    float x;
    float y;
    TextBlock(Font *fontp, const char *textp, float xp, float yp) : text(textp) {
        font = fontp;
        x = xp;
        y = yp;
    };
};

class StartUPLogo {
   protected:
    const char *m_logoname;
    TextureClass Logo;
    SwapBuffersFunction SBFunc;

   public:
    StartUPLogo() {};
    bool init(void *SwapBuffersFunc, unsigned int logo_file_type = IL_PNG,
              const char *logoname = "data/textures/MySimpleTexture.png") {
        m_logoname = logoname;
        Logo.Load(FJC_TEX_ANISOTROPIC_FILTERING, logo_file_type, m_logoname);
        SBFunc = (SwapBuffersFunction)SwapBuffersFunc;
        return true;
    }
    void render_logo(unsigned char mode, int text_blocks = 0, TextBlock blocks = TextBlock(NULL, NULL, 0, 0), ...) {
        GLint vp[4];

        glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_GEN_R);
        glDisable(GL_TEXTURE_GEN_Q);
        glEnable(GL_BLEND);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Чёрный фон
        glClear(GL_COLOR_BUFFER_BIT);          // Очистка буфера цвета
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Настроим глубину
        glDepthFunc(GL_NOTEQUAL);
        glDepthMask(false);
        glDisable(GL_DEPTH_TEST);

        if (glActiveTextureARB) {
            glActiveTextureARB(GL_TEXTURE1_ARB);
            glDisable(GL_TEXTURE_2D);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_NONE);
            glActiveTextureARB(GL_TEXTURE0_ARB);
        }
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        Logo.bind();

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glGetIntegerv(GL_VIEWPORT, vp);
        gluOrtho2D(vp[0], vp[2], vp[1], vp[3]);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        static int PointsArray[12] = {vp[0], vp[1], 0, vp[2], vp[1], 0, vp[2], vp[3], 0, vp[0], vp[3], 0};
        static int TexCoordArray[8] = {0, 1, 1, 1, 1, 0, 0, 0};
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_INT, 12, PointsArray);
        glTexCoordPointer(2, GL_INT, 8, TexCoordArray);

        switch (mode) {
            case FJC_STARTUP_LOGO_MODE_BEGIN:
                for (int i = 0; i < FJC_STARTUP_LOGO_STEPS_TO_BLEND; i++) {
                    glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                    glColor4f(1.0f, 1.0f, 1.0f, float(i) / (float)FJC_STARTUP_LOGO_STEPS_TO_BLEND);
                    glDrawArrays(GL_QUADS, 0, 4);
                    SBFunc();
                };
                break;
            case FJC_STARTUP_LOGO_MODE_END:
                for (int i = FJC_STARTUP_LOGO_STEPS_TO_BLEND; i > 0; i--) {
                    glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                    glColor4f(1.0f, 1.0f, 1.0f, float(i) / (float)FJC_STARTUP_LOGO_STEPS_TO_BLEND);
                    glDrawArrays(GL_QUADS, 0, 4);
                    SBFunc();
                };
                break;
            case FJC_STARTUP_LOGO_MODE_PROCEED:
                glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glDrawArrays(GL_QUADS, 0, 4);
                SBFunc();
                break;
            case FJC_STARTUP_LOGO_MODE_PROCEED_WITH_TEXT:
                glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                glDrawArrays(GL_QUADS, 0, 4);
                for (int i = 0, p_shift = 0; i < text_blocks; i++, p_shift = i * sizeof(TextBlock))
                    (&blocks + p_shift)
                        ->font->Print((&blocks + p_shift)->x, (&blocks + p_shift)->y, (&blocks + p_shift)->text);
                SBFunc();
                break;
        }
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glPopAttrib();

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
extern StartUPLogo SimpleLogo;
