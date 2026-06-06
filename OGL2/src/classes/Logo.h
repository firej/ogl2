#pragma once
#include "./text.h"
#include "./texture.h"
#include "./gl/immediate.h"
#include "./gl/glmat.h"

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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Настроим глубину
        glDepthFunc(GL_NOTEQUAL);
        glDepthMask(false);
        glDisable(GL_DEPTH_TEST);

        Logo.bind();

        glGetIntegerv(GL_VIEWPORT, vp);
        gl::matrixMode(gl::PROJECTION);
        gl::pushMatrix();
        gl::loadIdentity();
        gl::ortho(vp[0], vp[2], vp[1], vp[3], -1, 1);
        gl::matrixMode(gl::MODELVIEW);
        gl::pushMatrix();
        gl::loadIdentity();

        float x, y;
        x = left ? 0 : vp[2] - m_w * scale;
        y = top ? vp[3] - m_h * scale : 0;
        gl::imColor4f(1.0f, 1.0f, 1.0f, alpha);
        gl::imTexture(Logo.Number);
        gl::imBegin(GL_QUADS);
        gl::imTexCoord2f(0, 1); gl::imVertex3f(x, y, 0);
        gl::imTexCoord2f(1, 1); gl::imVertex3f(x + m_w * scale, y, 0);
        gl::imTexCoord2f(1, 0); gl::imVertex3f(x + m_w * scale, y + m_h * scale, 0);
        gl::imTexCoord2f(0, 0); gl::imVertex3f(x + 0, y + m_h * scale, 0);
        gl::imEnd();

        glDepthMask(true);


        gl::matrixMode(gl::PROJECTION);
        gl::popMatrix();
        gl::matrixMode(gl::MODELVIEW);
        gl::popMatrix();
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

        glEnable(GL_BLEND);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  // Чёрный фон
        glClear(GL_COLOR_BUFFER_BIT);          // Очистка буфера цвета
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Настроим глубину
        glDepthFunc(GL_NOTEQUAL);
        glDepthMask(false);
        glDisable(GL_DEPTH_TEST);

        Logo.bind();

        glGetIntegerv(GL_VIEWPORT, vp);
        gl::matrixMode(gl::PROJECTION);
        gl::pushMatrix();
        gl::loadIdentity();
        gl::ortho(vp[0], vp[2], vp[1], vp[3], -1, 1);
        gl::matrixMode(gl::MODELVIEW);
        gl::pushMatrix();
        gl::loadIdentity();

        // Полноэкранный квад логотипа через батчер (вместо vertex-array + glDrawArrays).
        // Матрицы (ortho по viewport) батчер снимет из GL.
        auto drawLogoQuad = [&](float alpha) {
            gl::imColor4f(1.0f, 1.0f, 1.0f, alpha);
            gl::imTexture(Logo.Number);
            gl::imBegin(GL_QUADS);
            gl::imTexCoord2f(0, 1); gl::imVertex2f((float)vp[0], (float)vp[1]);
            gl::imTexCoord2f(1, 1); gl::imVertex2f((float)vp[2], (float)vp[1]);
            gl::imTexCoord2f(1, 0); gl::imVertex2f((float)vp[2], (float)vp[3]);
            gl::imTexCoord2f(0, 0); gl::imVertex2f((float)vp[0], (float)vp[3]);
            gl::imEnd();
        };

        switch (mode) {
            case FJC_STARTUP_LOGO_MODE_BEGIN:
                for (int i = 0; i < FJC_STARTUP_LOGO_STEPS_TO_BLEND; i++) {
                    glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                    drawLogoQuad(float(i) / (float)FJC_STARTUP_LOGO_STEPS_TO_BLEND);
                    SBFunc();
                };
                break;
            case FJC_STARTUP_LOGO_MODE_END:
                for (int i = FJC_STARTUP_LOGO_STEPS_TO_BLEND; i > 0; i--) {
                    glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                    drawLogoQuad(float(i) / (float)FJC_STARTUP_LOGO_STEPS_TO_BLEND);
                    SBFunc();
                };
                break;
            case FJC_STARTUP_LOGO_MODE_PROCEED:
                glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                drawLogoQuad(1.0f);
                SBFunc();
                break;
            case FJC_STARTUP_LOGO_MODE_PROCEED_WITH_TEXT:
                glClear(GL_COLOR_BUFFER_BIT);  // Очистка буфера цвета
                drawLogoQuad(1.0f);
                for (int i = 0, p_shift = 0; i < text_blocks; i++, p_shift = i * sizeof(TextBlock))
                    (&blocks + p_shift)
                        ->font->Print((&blocks + p_shift)->x, (&blocks + p_shift)->y, (&blocks + p_shift)->text);
                SBFunc();
                break;
        }

        gl::matrixMode(gl::PROJECTION);
        gl::popMatrix();
        gl::matrixMode(gl::MODELVIEW);
        gl::popMatrix();
        glBindTexture(GL_TEXTURE_2D, 0);
        // Восстанавливаем depth-состояние, которое поменяли выше, чтобы не ломать
        // последующую отрисовку сцены (иначе запись глубины остаётся выключенной).
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }
};
extern StartUPLogo SimpleLogo;
