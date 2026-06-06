// Регрессионный smoke-тест GL-бэкенда (батчер gl::Immediate / gl::GpuMesh).
// Создаёт offscreen-окно OpenGL 3.3 Core, рисует каждым путём по примитиву и
// читает пиксели: линии/стрипы, статический VBO, текстура (модуляция), точки с
// размером (gl_PointSize) и толстые линии (разворот в экранные квады).
// Выход 0 = PASS. Не зависит от движка — только от gl/*.cpp.
//
// Сборка из CMake:  cmake -S . -B build -DOGL2_BUILD_TESTS=ON && cmake --build build
// Вручную (macOS):
//   clang++ -std=c++17 -DGL_SILENCE_DEPRECATION OGL2/tests/gl_smoke.cpp \
//     OGL2/src/classes/gl/shader.cpp OGL2/src/classes/gl/immediate.cpp \
//     OGL2/src/classes/gl/glmat.cpp -framework OpenGL \
//     $(pkg-config --cflags --libs glfw3) -o /tmp/gl_smoke && /tmp/gl_smoke
#define GLFW_INCLUDE_NONE
#include "../src/classes/gl/immediate.h"  // тянет ../oGL.h -> <OpenGL/gl3.h>
#include "../src/classes/gl/mat4.h"
#include "../src/classes/gl/glmat.h"
#include <GLFW/glfw3.h>
#include <cstdio>

int main() {
    if (!glfwInit()) { printf("glfwInit failed\n"); return 2; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* w = glfwCreateWindow(64, 64, "smoke", nullptr, nullptr);
    if (!w) { printf("window failed\n"); glfwTerminate(); return 2; }
    glfwMakeContextCurrent(w);

    printf("GL_VERSION  = %s\n", (const char*)glGetString(GL_VERSION));
    printf("GL_SHADING  = %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    glViewport(0, 0, 64, 64);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    // Ортопроекция 0..1 через CPU матричный стек — батчер берёт её из gl::projection/modelview
    gl::matrixMode(gl::PROJECTION); gl::loadIdentity(); gl::ortho(0, 1, 0, 1, -1, 1);
    gl::matrixMode(gl::MODELVIEW);  gl::loadIdentity();

    // (1) Immediate: красный слева через GL_TRIANGLE_STRIP (как фон/строка консоли)
    gl::imColor3f(1.0f, 0.0f, 0.0f);
    gl::imBegin(GL_TRIANGLE_STRIP);
    gl::imVertex2f(0.10f, 0.25f);
    gl::imVertex2f(0.10f, 0.75f);
    gl::imVertex2f(0.40f, 0.25f);
    gl::imVertex2f(0.40f, 0.75f);
    gl::imEnd();

    // (2) GpuMesh: зелёный треугольник справа (статический VBO)
    const float tri[] = {
        // x     y     z    u    v    r    g    b    a    nx   ny   nz
        0.55f, 0.25f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
        0.95f, 0.25f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
        0.75f, 0.75f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 1.f, 0.f, 0.f, 1.f,
    };
    gl::GpuMesh mesh;
    mesh.upload(tri, 3);
    mesh.draw();

    // (3) Текстурированный квад (путь текста): magenta-текстура * белый цвет = magenta
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    const unsigned char mag[4] = {255, 0, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, mag);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    gl::imColor4f(1.f, 1.f, 1.f, 1.f);
    gl::imTexture(tex);
    gl::imBegin(GL_QUADS);
    gl::imTexCoord2f(0, 0); gl::imVertex2f(0.40f, 0.80f);
    gl::imTexCoord2f(1, 0); gl::imVertex2f(0.60f, 0.80f);
    gl::imTexCoord2f(1, 1); gl::imVertex2f(0.60f, 0.95f);
    gl::imTexCoord2f(0, 1); gl::imVertex2f(0.40f, 0.95f);
    gl::imEnd();

    // (4) GL_POINTS с размером через шейдер (частицы): жёлтая точка size=7 у (0.5,0.1)
    gl::imColor4f(1.f, 1.f, 0.f, 1.f);
    gl::imPointSize(7.0f);
    gl::imBegin(GL_POINTS);
    gl::imVertex2f(0.5f, 0.1f);
    gl::imEnd();

    // (5) Толстая линия (в Core glLineWidth>1 нет -> экранные квады): белая, ширина 6
    gl::imColor4f(1.f, 1.f, 1.f, 1.f);
    gl::imLineWidth(6.0f);
    gl::imBegin(GL_LINES);
    gl::imVertex2f(0.05f, 0.85f);
    gl::imVertex2f(0.35f, 0.85f);
    gl::imEnd();

    // (6) Освещённый меш: белый треугольник, нормаль (0,0,1), свет (1,0,0) ->
    // dot=0 -> остаётся только ambient(0.35); белый должен стать серым ~89
    gl::setLight(1.f, 0.f, 0.f, 0.35f);
    const float litTri[] = {
        0.62f, 0.02f, 0.f, 0, 0, 1, 1, 1, 1, 0, 0, 1,
        0.95f, 0.02f, 0.f, 0, 0, 1, 1, 1, 1, 0, 0, 1,
        0.78f, 0.18f, 0.f, 0, 0, 1, 1, 1, 1, 0, 0, 1,
    };
    gl::GpuMesh litMesh;
    litMesh.upload(litTri, 3);
    litMesh.setLit(true);
    litMesh.draw();
    glFinish();

    unsigned char L[3] = {}, R[3] = {}, T[3] = {}, P[3] = {}, Pofs[3] = {}, W[3] = {}, Wof[3] = {}, edge[3] = {9, 9, 9};
    glReadPixels(12, 54, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, W);    // на толстой линии — белый
    glReadPixels(12, 56, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, Wof);  // +2px — белый, если линия толстая
    glReadPixels(16, 32, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, L);    // слева — красный (immediate)
    glReadPixels(48, 32, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, R);    // справа — зелёный (GpuMesh)
    glReadPixels(32, 56, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, T);    // верх — magenta (текстура)
    glReadPixels(32, 6, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, P);     // центр точки — жёлтый
    glReadPixels(34, 6, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, Pofs);  // +2px — жёлтый, если size>1
    unsigned char Lit[3] = {};
    glReadPixels(50, 6, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, Lit);   // освещённый меш — серый ~89
    glReadPixels(2, 2, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, edge);   // угол — чёрный
    printf("red=%d,%d,%d green=%d,%d,%d magenta=%d,%d,%d point=%d,%d,%d point+2=%d,%d,%d thick=%d,%d,%d thick+2=%d,%d,%d corner=%d,%d,%d\n",
           L[0], L[1], L[2], R[0], R[1], R[2], T[0], T[1], T[2], P[0], P[1], P[2], Pofs[0], Pofs[1], Pofs[2],
           W[0], W[1], W[2], Wof[0], Wof[1], Wof[2], edge[0], edge[1], edge[2]);

    bool okIm = L[0] > 200 && L[1] < 40 && L[2] < 40;
    bool okMesh = R[1] > 200 && R[0] < 40 && R[2] < 40;
    bool okTex = T[0] > 200 && T[1] < 40 && T[2] > 200;
    bool okPoint = P[0] > 200 && P[1] > 200 && P[2] < 40;   // жёлтый в центре
    bool okPointSize = Pofs[0] > 200 && Pofs[1] > 200;      // точка крупнее 1px
    bool okThick = W[0] > 200 && W[1] > 200 && W[2] > 200;  // линия видна
    bool okThickW = Wof[0] > 200 && Wof[1] > 200;           // и она толстая (+2px)
    bool okLit = Lit[0] > 60 && Lit[0] < 120 && Lit[1] > 60 && Lit[1] < 120;  // серый ~89 (ambient)
    printf("litmesh=%d,%d,%d (ожид. ~89 = ambient*255)\n", Lit[0], Lit[1], Lit[2]);
    bool ok = okIm && okMesh && okTex && okPoint && okPointSize && okThick && okThickW && okLit &&
              edge[0] < 40 && edge[1] < 40;

    printf("%s\n", ok ? "PASS: батчер (VAO+GLSL330) рисует всё в Core 3.3" : "FAIL");
    glfwTerminate();
    return ok ? 0 : 1;
}
