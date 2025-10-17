#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux includes
#include <OpenGL/gl.h>
#endif

#include <iostream>
#include <string>
#include <vector>

#include "../Ex/stb/stb_image.h"
#include "./objects.h"

using namespace std;
#include "./objects.h"
SkyBox::SkyBox() {}
SkyBox::~SkyBox() {}

void SkyBox::LoadTextures(ILenum Type, char *NameOfTex)  // Загрузка текстур неба
{
    // t.Load(FJC_TEX_ANISOTROPIC_FILTERING,Type,NameOfTex);
    t.Load(FJC_TEX_NO_FILTERING, Type, NameOfTex);
}

float SkyA[] = {0.50f, 0.00f, -1.0f, 1.0f,  -1.0f, 0.75f, 0.00f, -1.0f, 1.0f,  1.0f,
                0.75f, 0.25f, 1.0f,  1.0f,  1.0f,  0.50f, 0.25f, 1.0f,  1.0f,  -1.0f,

                0.50f, 0.50f, -1.0f, -1.0f, -1.0f, 0.75f, 0.50f, 1.0f,  -1.0f, -1.0f,
                0.75f, 0.75f, 1.0f,  -1.0f, 1.0f,  0.50f, 0.75f, -1.0f, -1.0f, 1.0f,

                0.00f, 0.50f, -1.0f, -1.0f, -1.0f, 0.25f, 0.50f, -1.0f, -1.0f, 1.0f,
                0.25f, 0.25f, -1.0f, 1.0f,  1.0f,  0.00f, 0.25f, -1.0f, 1.0f,  -1.0f,

                0.75f, 0.50f, 1.0f,  -1.0f, -1.0f, 0.75f, 0.25f, 1.0f,  1.0f,  -1.0f,
                0.50f, 0.25f, 1.0f,  1.0f,  1.0f,  0.50f, 0.50f, 1.0f,  -1.0f, 1.0f,

                0.25f, 0.50f, -1.0f, -1.0f, 1.0f,  0.50f, 0.50f, 1.0f,  -1.0f, 1.0f,
                0.50f, 0.25f, 1.0f,  1.0f,  1.0f,  0.25f, 0.25f, -1.0f, 1.0f,  1.0f,

                1.00f, 0.50f, -1.0f, -1.0f, -1.0f, 1.00f, 0.25f, -1.0f, 1.0f,  -1.0f,
                0.75f, 0.25f, 1.0f,  1.0f,  -1.0f, 0.75f, 0.50f, 1.0f,  -1.0f, -1.0f};

float skyboxVertices[] = {
    // positions
    -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
    -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

    1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
    1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

unsigned int loadCubemap(vector<string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        } else {
            cout << "Cubemap failed to load at path: " << faces[i] << endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void SkyBox::operator()() {

    glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);  // Выключаем тест глубины для того чтобы нарисовать небо
    glDisable(GL_LIGHTING);
    glCullFace(GL_FRONT);
    glEnable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    t.bind();
    glColor3d(1, 1, 1);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glVertexPointer(3, GL_FLOAT, 5 * sizeof(float), SkyA + 2);
    glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(float), SkyA);

    glDrawArrays(GL_QUADS, 0, 24);

    // Рисуем жирные границы skybox'а - используем отдельную геометрию
    glDisable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(40.0f);
    glColor3d(0.0, 0.0, 0.0);  // Черные границы

    // Возвращаем режим заливки
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glPopAttrib();
    glClear(GL_DEPTH_BUFFER_BIT);  // Очистка буфера глубины
}
