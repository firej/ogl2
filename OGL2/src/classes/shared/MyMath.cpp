#include "MyMath.h"

#ifdef WIN32
#include "../../LocusAFX.h"
#else
#include <cmath>
typedef unsigned int DWORD;
#endif

float _sint[361];
float _cost[361];
float _tant[361];

void InitMATH(void) {
    for (int i = 0; i <= 360; i++) _sint[i] = sin(((float)i) * PI2 / 360);
    for (int i = 0; i <= 360; i++) _cost[i] = cos(((float)i) * PI2 / 360);
    for (int i = 0; i <= 360; i++) _tant[i] = tan(((float)i) * PI2 / 360);
}

float sint(float angle) {
    while (angle > 360) angle -= 360;
    while (angle < 0) angle += 360;
    return _sint[(int)angle];
}

double sint(double angle) {
    while (angle > 360) angle -= 360;
    while (angle < 0) angle += 360;
    return _sint[(int)angle];
}

float cost(float angle) {
    while (angle > 360) angle -= 360;
    while (angle < 0) angle += 360;
    return _cost[(int)angle];
}

double cost(double angle) {
    while (angle > 360) angle -= 360;
    while (angle < 0) angle += 360;
    return _cost[(int)angle];
}

float tant(float angle) {
    while (angle > 360) angle -= 360;
    while (angle < 0) angle += 360;
    return _tant[(int)angle];
}

double tant(double angle) {
    while (angle > 360) angle -= 360;
    while (angle < 0) angle += 360;
    return _tant[(int)angle];
}

#ifdef WIN32
// Быстрый корень от nVidia
static float _0_47 = 0.47f;
static float _1_47 = 1.47f;

float __fastcall ulrsqrt(float x)  // Обратный корень
{
    DWORD y;
    float r;
    _asm
        {
    mov     eax, 07F000000h+03F800000h  // (ONE_AS_INTEGER<<1) + ONE_AS_INTEGER
    sub     eax, x
    sar     eax, 1

    mov     y, eax  // y
    fld     _0_47  // 0.47
    fmul    DWORD PTR x  // x*0.47

    fld     DWORD PTR y
    fld     st(0)  // y y x*0.47
    fmul    st(0), st(1)  // y*y y x*0.47

    fld     _1_47  // 1.47 y*y y x*0.47
    fxch    st(3)  // x*0.47 y*y y 1.47
    fmulp   st(1), st(0)  // x*0.47*y*y y 1.47
    fsubp   st(2), st(0)  // y 1.47-x*0.47*y*y
    fmulp   st(1), st(0)  // result
    fstp    y
    and     y, 07FFFFFFFh

        }
    r = *(float *)&y;
    // optional
    r = (3.0f - x * (r * r)) * r * 0.5f;  // remove for low accuracy
    return r;
}

float __fastcall ulsqrt(float x)  // Настоящий корень
{
    return x * ulrsqrt(x);
}

#else

// Для macOS/Linux используем стандартные функции
float ulrsqrt(float x) { return 1.0f / sqrt(x); }

float ulsqrt(float x) { return sqrt(x); }

#endif
