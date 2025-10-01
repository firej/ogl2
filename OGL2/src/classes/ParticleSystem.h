#pragma once
//====== Общие файлы
#include "./globals.h"

class ParticleSystem {
    Point3f *Poss;             // Позиции
    Vector3f *Vels;            // Скорости
    unsigned short Particles;  // Количество частиц
   public:
    Point3f wind;  // Ветер
    Point3f windSource;
    Point3f grav;
    bool windActive;  // Активность ветра
   public:
    ParticleSystem();
    ~ParticleSystem();
    void INIT();                // Инициализация
    void DRAW();                // Рисование
    void ANIM();                // Анимация
    void SetGravity(Point3f g)  // Гравитация
    {
        grav = g;
    }
    void SetWind(Point3f w, Point3f source)  // Ветер
    {
        wind = w;
        windSource = source;
    }
    void SetWindStatus(bool wA) { windActive = wA; }
};
