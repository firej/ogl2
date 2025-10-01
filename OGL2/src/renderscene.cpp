#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux stubs for Windows-specific includes
#endif

#include "./classes/ApplicationClass.h"
#include "./classes/Time.h"

bool TextBenchmark = false;

bool Application::RenderScene() {
    GT.NewFrame();
    glClear(GL_DEPTH_BUFFER_BIT);  // Очистка буфера глубины
    Cam.Look();
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //	РИСОВАНИЕ
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //   rm.SELECT_Mesh("someMesh")->Render();

    //   PS.DRAW();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //	Рисование осей координат :)
    //	Ось X
    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);  // Сохранение настроек
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    glScaled(1.0, 1.0, 1.0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glLineWidth(8.0);
    glColor3d(0.0, 1.0, 0.0);
    glBegin(GL_LINES);
    {
        glVertex3d(0.00, 0.00, 0.00);
        glVertex3d(4.00, 0.00, 0.00);
        glVertex3d(4.00, 0.00, 0.00);
        glVertex3d(3.50, 0.00, 0.25);
        glVertex3d(3.50, 0.00, -0.25);
        glVertex3d(4.00, 0.00, 0.00);
        glVertex3d(4.00, 0.00, 0.00);
        glVertex3d(3.50, 0.25, 0.00);
        glVertex3d(3.50, -0.25, 0.00);
        glVertex3d(4.00, 0.00, 0.00);
    }
    glEnd();
    // Ось Y
    glColor3d(0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    {
        glVertex3d(0.00, 0.00, 0.00);
        glVertex3d(0.00, 4.00, 0.00);
        glVertex3d(0.00, 4.00, 0.00);
        glVertex3d(0.00, 3.50, 0.25);
        glVertex3d(0.00, 3.50, -0.25);
        glVertex3d(0.00, 4.00, 0.00);
        glVertex3d(0.00, 4.00, 0.00);
        glVertex3d(0.25, 3.50, 0.00);
        glVertex3d(-0.25, 3.50, 0.00);
        glVertex3d(0.00, 4.00, 0.00);
    }
    glEnd();
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_LINES);
    {
        glVertex3d(0.00, 0.00, 0.00);
        glVertex3d(0.00, 0.00, 4.00);
        glVertex3d(0.00, 0.00, 4.00);
        glVertex3d(0.00, 0.25, 3.50);
        glVertex3d(0.00, -0.25, 3.50);
        glVertex3d(0.00, 0.00, 4.00);
        glVertex3d(0.00, 0.00, 4.00);
        glVertex3d(0.25, 0.00, 3.50);
        glVertex3d(-0.25, 0.00, 3.50);
        glVertex3d(0.00, 0.00, 4.00);
    }
    glEnd();
    glPopMatrix();
    glPopAttrib();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //	ТЕКСТ
    // TODO: удалить енто после полной отладки текста

    if (TextBenchmark) {
        Font *Cour = rm.SELECT_Font("Courier");
        rm.SELECT_Font("Console")->SetStyle(Text::left, Text::top, 1.0);
        rm.SELECT_Font("Console")->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
        for (int i = 0; i < 40; i++) {
            for (int j = 0; j < 8; j++) {
                Cour->Print((j) / 8.0, i / 40.0, "FPS:%4.1f", GT.GetFPS());
            }
        }
    } else {
        if (Globals.EFl.show_stat) {
            PFont uif = rm.SELECT_Font("Console");
            uif->SetStyle(Text::left, Text::top, 1.0);
            uif->SetColor(0.0f, 0.0f, 0.0f, 1.0f);
            uif->Print(0.0, 0.0, "Current position is (%5.1f,%5.1f,%5.1f)", Cam.Position.d.c.x, Cam.Position.d.c.y,
                       Cam.Position.d.c.z);
            uif->Print(0.0f, 0.025, "FPS:    %5.1f", GT.GetFPS());
            uif->Print(0.0f, 0.05, "CPU speed is %d", SysInfo.cpu.speed);
            //			rm.SELECT_Font("Console")->Print(0.0f,0.075,"Cursor
            // at %d",CCons.cursor);
            // rm.SELECT_Font("Console")->Print(0.0f,0.1,"Chars in string
            // %d",CCons.chars);

            char str[512] = "Buttons pressed : ";
            for (WPARAM i = 0; i < 256; i++) {
                if (Input::I->get(i)) sprintf(str, "%s %s", str, Input::I->getSN(i));
            }
            uif->Print(0.0f, 0.975, "%s ", str);
            memset(str, 0, sizeof(str));
            strcpy(str, "Buttons pressed : ");
            for (WPARAM i = 0; i < 256; i++) {
                if (Input::I->get(i)) sprintf(str, "%s %d", str, i);
            }
            uif->Print(0.0f, 0.95, "%s ", str);
            uif->Print(0.0f, 0.925, "Wheel shift: %d ", Input::I->wheel);

            /*uif->SetStyle(Text::left,Text::top,1.0);
            uif->SetColor(0.0f,0.0f,0.0f,1.0f);
            uif->Print(0.0,0.075,"Состояние ветра - %d",PS.windActive);
            uif->Print(0.0,0.1,
                    "Скорость ветра -
            %5.1f,%5.1f,%5.1f",PS.wind.d.c.x,PS.wind.d.c.y,PS.wind.d.c.z);
            uif->Print(0.0,0.125,
                    "Гравитация     -
            %5.1f,%5.1f,%5.1f",PS.grav.d.c.x,PS.grav.d.c.y,PS.grav.d.c.z);*/

            uif->SetStyle(Text::right, Text::top, 1.0);
            uif->Print(0.95, 0.0, "Количество материалов - %d", rm.SELECT_Mesh("someMesh")->iSurfaces);
            meshPointer sm = rm.SELECT_Mesh("someMesh");
            for (WORD i = 0; i < rm.SELECT_Mesh("someMesh")->iSurfaces; i++)
                uif->Print(1.0, 0.025 * (i + 1), "#%02d \"% 20s\" - (%3.2f,%3.2f,%3.2f) %3.2f", i,
                           sm->SurfList[i].name.c_str(), sm->SurfList[i].Diffuse.d.c.r, sm->SurfList[i].Diffuse.d.c.g,
                           sm->SurfList[i].Diffuse.d.c.b, sm->SurfList[i].color.d.c.a);
            // uif->Print(0.95,0.5,"Информация о модели:");
            // uif->Print(0.95,0.5,"",sm->);
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //	Интерфейс пользователя
    //   NVLogo.render_logo();

#ifdef ENABLE_TEXTURE_RENDERING
    if (Input::I->process('T')) {
        RendTex.CopyBackbufferToTexture();
        RendTex.Save();
    }
#endif
    return FJC_NO_ERROR;
};
