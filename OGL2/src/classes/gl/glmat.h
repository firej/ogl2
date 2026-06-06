#ifndef GL_GLMAT_H
#define GL_GLMAT_H
// CPU-сторонний матричный стек — замена фикс-функции glMatrixMode/glPushMatrix/
// glTranslate/gluPerspective/... Нужен для перехода в Core 3.3, где их нет.
//
// На время перехода (контекст всё ещё 2.1) операции ЗЕРКАЛЯТСЯ в GL, чтобы
// батчер мог по-прежнему читать матрицы через glGetFloatv и подсистемы можно
// было конвертировать по одной без регрессий. Перед флипом в Core зеркало
// выключается (setMirrorToGL(false)), а батчер переключается на projection()/
// modelview().
#include "./mat4.h"

namespace gl {

enum MatMode { PROJECTION = 0, MODELVIEW = 1 };

void matrixMode(MatMode m);
void loadIdentity();
void pushMatrix();
void popMatrix();
void loadMatrix(const Mat4& m);
void multMatrix(const Mat4& m);
void translate(double x, double y, double z);
void rotate(double deg, double x, double y, double z);  // угол в градусах (как glRotated)
void scale(double x, double y, double z);
void ortho(double l, double r, double b, double t, double n, double f);
void perspective(double fovyDeg, double aspect, double zn, double zf);  // как gluPerspective

const Mat4& projection();
const Mat4& modelview();
const Mat4& current();

void setMirrorToGL(bool on);  // дублировать ли операции в фикс-функцию GL

}  // namespace gl

#endif  // GL_GLMAT_H
