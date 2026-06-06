#ifndef GL_IMMEDIATE_H
#define GL_IMMEDIATE_H
// Immediate-батчер: замена glBegin/glVertex/... на VBO+шейдер.
//
// Ключевая идея переходного периода: на end() батчер СНИМАЕТ текущие матрицы
// GL (GL_PROJECTION/GL_MODELVIEW) через glGetFloatv и отдаёт MVP юниформом.
// Поэтому новый код рисует в том же пространстве, что и старый fixed-function
// (камера, glPushMatrix и т.п. продолжают работать). На финальном флипе в
// Core 3.3 снятие матриц заменим на собственный матричный стек.
//
// Вершина: pos(3) + uv(2) + color(4). GL_QUADS триангулируется автоматически.
#include "../oGL.h"

// GL_QUADS удалён в Core 3.3, но батчер принимает его и сам триангулирует.
#ifndef GL_QUADS
#define GL_QUADS 0x0007
#endif

namespace gl {

void imBegin(GLenum mode);                       // GL_LINES, GL_TRIANGLES, GL_QUADS, ...
void imColor3f(float r, float g, float b);
void imColor4f(float r, float g, float b, float a);
void imTexCoord2f(float u, float v);
void imNormal3f(float x, float y, float z);      // нормаль вершины (для освещения мешей)
void imVertex3f(float x, float y, float z);
void imVertex2f(float x, float y);
void imTexture(GLuint tex);                       // 0 = без текстуры (только цвет вершин)
void imPointSize(float size);                     // размер точек для GL_POINTS
void imLineWidth(float px);                        // толщина GL_LINES в пикселях (через квады)
void imEnd();                                     // снимает матрицы, заливает VBO, рисует

// Глобальный направленный свет (мир): направление НА источник + доля ambient.
void setLight(float dx, float dy, float dz, float ambient);

// Статический меш в VBO (заливается один раз). Формат вершины такой же, как у
// батчера: pos(3) + uv(2) + color(4) + normal(3) = 12 float. Рисует
// треугольниками, MVP снимает из текущего состояния GL (как imEnd).
class GpuMesh {
   public:
    GpuMesh() = default;
    ~GpuMesh();
    GpuMesh(const GpuMesh&) = delete;  // владеет GL-буфером
    GpuMesh& operator=(const GpuMesh&) = delete;
    void upload(const float* interleaved, int vertexCount);  // 12 float на вершину
    void draw(GLuint tex = 0) const;                          // 0 = без текстуры
    void setLit(bool l) { lit_ = l; }                         // включить освещение меша
    void destroy();
    bool valid() const { return vbo_ != 0; }
    int count() const { return count_; }

   private:
    GLuint vbo_ = 0;
    int count_ = 0;
    bool lit_ = false;
};

}  // namespace gl

#endif  // GL_IMMEDIATE_H
