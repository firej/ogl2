#include "./glmat.h"

#include <vector>

#include "../oGL.h"

namespace gl {
namespace {
std::vector<Mat4> projStack = {Mat4::identity()};
std::vector<Mat4> mvStack = {Mat4::identity()};
MatMode curMode = MODELVIEW;
bool mirror = false;  // батчер на CPU-стеке; фикс-функция GL больше не нужна
const double DEG2RAD = 0.0174532925199432957692;

std::vector<Mat4>& stk() { return curMode == PROJECTION ? projStack : mvStack; }
Mat4& top() { return stk().back(); }

// Зеркалим текущую матрицу в фикс-функцию GL (GL current == CPU top)
void mirrorLoad() {
    if (!mirror) return;
    glMatrixMode(curMode == PROJECTION ? GL_PROJECTION : GL_MODELVIEW);
    glLoadMatrixf(top().m);
}
}  // namespace

void setMirrorToGL(bool on) { mirror = on; }

void matrixMode(MatMode m) {
    curMode = m;
    if (mirror) glMatrixMode(m == PROJECTION ? GL_PROJECTION : GL_MODELVIEW);
}

void loadIdentity() {
    top() = Mat4::identity();
    mirrorLoad();
}

void pushMatrix() {
    stk().push_back(top());
    if (mirror) {
        glMatrixMode(curMode == PROJECTION ? GL_PROJECTION : GL_MODELVIEW);
        glPushMatrix();  // GL дублирует текущую (== CPU top) — глубина стека синхронна
    }
}

void popMatrix() {
    if (stk().size() > 1) stk().pop_back();
    if (mirror) {
        glMatrixMode(curMode == PROJECTION ? GL_PROJECTION : GL_MODELVIEW);
        glPopMatrix();
    }
}

void loadMatrix(const Mat4& m) {
    top() = m;
    mirrorLoad();
}
void multMatrix(const Mat4& m) {
    top() = top() * m;
    mirrorLoad();
}
void translate(double x, double y, double z) {
    top() = top() * Mat4::translate((float)x, (float)y, (float)z);
    mirrorLoad();
}
void rotate(double deg, double x, double y, double z) {
    top() = top() * Mat4::rotate((float)(deg * DEG2RAD), (float)x, (float)y, (float)z);
    mirrorLoad();
}
void scale(double x, double y, double z) {
    top() = top() * Mat4::scale((float)x, (float)y, (float)z);
    mirrorLoad();
}
void ortho(double l, double r, double b, double t, double n, double f) {
    top() = top() * Mat4::ortho((float)l, (float)r, (float)b, (float)t, (float)n, (float)f);
    mirrorLoad();
}
void perspective(double fovyDeg, double aspect, double zn, double zf) {
    top() = top() * Mat4::perspective((float)(fovyDeg * DEG2RAD), (float)aspect, (float)zn, (float)zf);
    mirrorLoad();
}

const Mat4& projection() { return projStack.back(); }
const Mat4& modelview() { return mvStack.back(); }
const Mat4& current() { return top(); }

}  // namespace gl
