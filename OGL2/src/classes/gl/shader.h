#ifndef GL_SHADER_H
#define GL_SHADER_H
// Простой GLSL-шейдер для современного рендера.
// На этапе 2.1-контекста используется GLSL #version 120; при флипе в Core 3.3
// меняется только текст шейдеров.
#include "../oGL.h"

namespace gl {

// Стандартные локации атрибутов (биндятся до линковки, т.к. GLSL 120 не умеет
// layout(location=...)). Совпадают с раскладкой вершины в Immediate-батчере.
enum Attrib { ATTR_POS = 0, ATTR_UV = 1, ATTR_COLOR = 2, ATTR_NORMAL = 3 };

class Shader {
   public:
    Shader() = default;
    ~Shader();

    // Компилирует и линкует программу. Возвращает false при ошибке (лог в stderr).
    bool build(const char* vertexSrc, const char* fragmentSrc);
    void use() const;
    bool valid() const { return prog != 0; }
    GLuint id() const { return prog; }

    GLint uniform(const char* name);  // с кэшем
    void setMat4(const char* name, const float* m16);
    void setInt(const char* name, int v);
    void setFloat(const char* name, float v);
    void setVec4(const char* name, float x, float y, float z, float w);

   private:
    GLuint prog = 0;
    // маленький кэш локаций юниформов
    static const int CACHE = 16;
    const char* uNames[CACHE] = {};
    GLint uLocs[CACHE] = {};
    int uCount = 0;
};

}  // namespace gl

#endif  // GL_SHADER_H
