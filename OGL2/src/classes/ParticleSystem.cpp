#ifdef WIN32
#include "LocusAFX.h"
#else
// macOS/Linux includes
#include <OpenGL/gl3.h>

#include <cstdlib>
#endif

#include "./ParticleSystem.h"
#include "./gl/immediate.h"
#include "./time.h"

ParticleSystem::ParticleSystem() {
    Particles = 10000;
    this->Poss = new Point3f[Particles];
    this->Vels = new Vector3f[Particles];
}
ParticleSystem::~ParticleSystem() {
    delete[] this->Poss;
    delete[] this->Vels;
}

void ParticleSystem::INIT() {
    for (unsigned short i = 0; i < Particles; i++) {
        Poss[i].d.c.x = rand() / (float)RAND_MAX * 1000.0f - 500.0f;
        Poss[i].d.c.y = rand() / (float)RAND_MAX * 1000.0f;
        Poss[i].d.c.z = rand() / (float)RAND_MAX * 1000.0f - 500.0f;
        Vels[i].d.c.x = 0;
        Vels[i].d.c.y = 0;
        Vels[i].d.c.z = 0;
    }
    grav = Vector3f(0.0, -0.5f, 0.0);
    wind = Vector3f(100.0, 0.0, 0.0);
    windSource = Vector3f(1.0, 0.0, 1.0);
    windActive = false;
}
void ParticleSystem::ANIM() {
    Point3f windA;
    Point3f tmp;
    for (unsigned short i = 0; i < Particles; i++) {
        Vels[i] +=
            grav * (float)GT.GetFrameTime() + grav * (rand() / (float)RAND_MAX * 2.0f - 1.0f) / 250.0f;  // Ускорение
        if (windActive) {
            tmp = Poss[i];
            tmp -= windSource;
            windA = wind;
            windA = windA / (float)tmp.abs();
            Vels[i] += windA * (float)GT.GetFrameTime();  // Ускорение от ветра
        } else {
            Vels[i].d.c.x /= 1.001f;
            Vels[i].d.c.z /= 1.001f;
        }
        Poss[i] += Vels[i] * (float)GT.GetFrameTime();  // Перемещение
        if (Poss[i].d.c.y < 0) {
            Poss[i].d.c.x = rand() / (float)RAND_MAX * 1000.0f - 500.0f;
            Poss[i].d.c.y = 1000;
            Poss[i].d.c.z = rand() / (float)RAND_MAX * 1000.0f - 500.0f;
            Vels[i].d.c.x = 0;
            Vels[i].d.c.y = 0;
            Vels[i].d.c.z = 0;
        }
    }
}
void ParticleSystem::DRAW() {
    ANIM();
    glEnable(GL_BLEND);
    gl::imPointSize(5.0f);
    gl::imColor4f(1.0f, 1.0f, 1.0f, 0.95f);
    gl::imBegin(GL_POINTS);
    for (unsigned short i = 0; i < Particles; i++) {
        gl::imVertex3f(Poss[i].d.v[0], Poss[i].d.v[1], Poss[i].d.v[2]);
    }
    gl::imEnd();
}
