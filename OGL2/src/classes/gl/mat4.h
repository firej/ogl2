#ifndef GL_MAT4_H
#define GL_MAT4_H
// Минимальная матрица 4x4 (column-major, как в OpenGL).
// Используется для современного рендера: MVP в uniform вместо матричного стека.
#include <cmath>

namespace gl {

struct Mat4 {
    float m[16];  // column-major: m[col*4 + row]

    static Mat4 identity() {
        Mat4 r = {};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    // Загрузка из текущего состояния OpenGL (glGetFloatv возвращает column-major)
    const float* data() const { return m; }
    float* data() { return m; }
};

// C = A * B (математическое умножение, column-major хранение)
inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 c = {};
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row) {
            float s = 0.0f;
            for (int k = 0; k < 4; ++k) s += a.m[k * 4 + row] * b.m[col * 4 + k];
            c.m[col * 4 + row] = s;
        }
    return c;
}

inline Mat4 translate(float x, float y, float z) {
    Mat4 r = Mat4::identity();
    r.m[12] = x;
    r.m[13] = y;
    r.m[14] = z;
    return r;
}

inline Mat4 scale(float x, float y, float z) {
    Mat4 r = Mat4::identity();
    r.m[0] = x;
    r.m[5] = y;
    r.m[10] = z;
    return r;
}

// Поворот вокруг произвольной оси (угол в радианах)
inline Mat4 rotate(float angleRad, float x, float y, float z) {
    float len = std::sqrt(x * x + y * y + z * z);
    if (len > 0.0f) {
        x /= len;
        y /= len;
        z /= len;
    }
    float c = std::cos(angleRad), s = std::sin(angleRad), t = 1.0f - c;
    Mat4 r = Mat4::identity();
    r.m[0] = t * x * x + c;
    r.m[1] = t * x * y + s * z;
    r.m[2] = t * x * z - s * y;
    r.m[4] = t * x * y - s * z;
    r.m[5] = t * y * y + c;
    r.m[6] = t * y * z + s * x;
    r.m[8] = t * x * z + s * y;
    r.m[9] = t * y * z - s * x;
    r.m[10] = t * z * z + c;
    return r;
}

// Перспектива (fovy в радианах). Аналог gluPerspective.
inline Mat4 perspective(float fovyRad, float aspect, float znear, float zfar) {
    float f = 1.0f / std::tan(fovyRad * 0.5f);
    Mat4 r = {};
    r.m[0] = f / aspect;
    r.m[5] = f;
    r.m[10] = (zfar + znear) / (znear - zfar);
    r.m[11] = -1.0f;
    r.m[14] = (2.0f * zfar * znear) / (znear - zfar);
    return r;
}

// Ортография. Аналог glOrtho.
inline Mat4 ortho(float l, float r, float b, float t, float n, float f) {
    Mat4 m = Mat4::identity();
    m.m[0] = 2.0f / (r - l);
    m.m[5] = 2.0f / (t - b);
    m.m[10] = -2.0f / (f - n);
    m.m[12] = -(r + l) / (r - l);
    m.m[13] = -(t + b) / (t - b);
    m.m[14] = -(f + n) / (f - n);
    return m;
}

// Видовая матрица камеры. Аналог gluLookAt.
inline Mat4 lookAt(float ex, float ey, float ez, float cx, float cy, float cz,
                   float ux, float uy, float uz) {
    float fx = cx - ex, fy = cy - ey, fz = cz - ez;
    float fl = std::sqrt(fx * fx + fy * fy + fz * fz);
    if (fl > 0.0f) { fx /= fl; fy /= fl; fz /= fl; }
    // s = f x up
    float sx = fy * uz - fz * uy, sy = fz * ux - fx * uz, sz = fx * uy - fy * ux;
    float sl = std::sqrt(sx * sx + sy * sy + sz * sz);
    if (sl > 0.0f) { sx /= sl; sy /= sl; sz /= sl; }
    // u = s x f
    float upx = sy * fz - sz * fy, upy = sz * fx - sx * fz, upz = sx * fy - sy * fx;
    Mat4 r = Mat4::identity();
    r.m[0] = sx;  r.m[4] = sy;  r.m[8] = sz;
    r.m[1] = upx; r.m[5] = upy; r.m[9] = upz;
    r.m[2] = -fx; r.m[6] = -fy; r.m[10] = -fz;
    r.m[12] = -(sx * ex + sy * ey + sz * ez);
    r.m[13] = -(upx * ex + upy * ey + upz * ez);
    r.m[14] = (fx * ex + fy * ey + fz * ez);
    return r;
}

}  // namespace gl

#endif  // GL_MAT4_H
