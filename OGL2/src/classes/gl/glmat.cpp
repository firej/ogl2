#include "./glmat.h"

#include <vector>

namespace gl {
namespace {
std::vector<Mat4> projStack = {Mat4::identity()};
std::vector<Mat4> mvStack = {Mat4::identity()};
MatMode curMode = MODELVIEW;
const double DEG2RAD = 0.0174532925199432957692;

std::vector<Mat4>& stk() { return curMode == PROJECTION ? projStack : mvStack; }
Mat4& top() { return stk().back(); }
}  // namespace

void setMirrorToGL(bool) {}  // больше не используется (батчер читает CPU-стек)

void matrixMode(MatMode m) { curMode = m; }
void loadIdentity() { top() = Mat4::identity(); }
void pushMatrix() { stk().push_back(top()); }
void popMatrix() {
    if (stk().size() > 1) stk().pop_back();
}
void loadMatrix(const Mat4& m) { top() = m; }
void multMatrix(const Mat4& m) { top() = top() * m; }
void translate(double x, double y, double z) {
    top() = top() * Mat4::translate((float)x, (float)y, (float)z);
}
void rotate(double deg, double x, double y, double z) {
    top() = top() * Mat4::rotate((float)(deg * DEG2RAD), (float)x, (float)y, (float)z);
}
void scale(double x, double y, double z) {
    top() = top() * Mat4::scale((float)x, (float)y, (float)z);
}
void ortho(double l, double r, double b, double t, double n, double f) {
    top() = top() * Mat4::ortho((float)l, (float)r, (float)b, (float)t, (float)n, (float)f);
}
void perspective(double fovyDeg, double aspect, double zn, double zf) {
    top() = top() * Mat4::perspective((float)(fovyDeg * DEG2RAD), (float)aspect, (float)zn, (float)zf);
}

const Mat4& projection() { return projStack.back(); }
const Mat4& modelview() { return mvStack.back(); }
const Mat4& current() { return top(); }

}  // namespace gl
