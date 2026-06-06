#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux includes
#include <OpenGL/gl3.h>
#endif

#include <iostream>
#include <string>
#include <vector>

#include "../Ex/stb/stb_image.h"
#include "./gl/immediate.h"
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

    glDisable(GL_DEPTH_TEST);  // Выключаем тест глубины для того чтобы нарисовать небо
    glCullFace(GL_FRONT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // Небо: текстурированные квады через батчер (вместо vertex-array + glDrawArrays).
    // SkyA — интерливед, по 5 float на вершину: u, v, x, y, z. Матрицы (камера)
    // батчер снимет из текущего состояния GL.
    gl::imColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    gl::imTexture(t.Number);
    gl::imBegin(GL_QUADS);
    for (int v = 0; v < 24; v++) {
        const float *p = SkyA + v * 5;
        gl::imTexCoord2f(p[0], p[1]);
        gl::imVertex3f(p[2], p[3], p[4]);
    }
    gl::imEnd();

    glClear(GL_DEPTH_BUFFER_BIT);  // Очистка буфера глубины
}
