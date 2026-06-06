#include "./immediate.h"

#include <vector>

#include "./glmat.h"
#include "./mat4.h"
#include "./shader.h"

namespace gl {

// --- GLSL 120 (этап 2.1). При флипе в Core 3.3 заменить на #version 330 ---
static const char* VS_SRC =
    "#version 120\n"
    "attribute vec3 aPos;\n"
    "attribute vec2 aUV;\n"
    "attribute vec4 aColor;\n"
    "uniform mat4 uMVP;\n"
    "uniform float uPointSize;\n"
    "varying vec2 vUV;\n"
    "varying vec4 vColor;\n"
    "void main() {\n"
    "    gl_Position = uMVP * vec4(aPos, 1.0);\n"
    "    gl_PointSize = uPointSize;\n"
    "    vUV = aUV;\n"
    "    vColor = aColor;\n"
    "}\n";

static const char* FS_SRC =
    "#version 120\n"
    "uniform sampler2D uTex;\n"
    "uniform int uUseTexture;\n"
    "varying vec2 vUV;\n"
    "varying vec4 vColor;\n"
    "void main() {\n"
    "    vec4 c = vColor;\n"
    "    if (uUseTexture == 1) c *= texture2D(uTex, vUV);\n"
    "    gl_FragColor = c;\n"
    "}\n";

namespace {
struct State {
    GLenum mode = GL_TRIANGLES;
    float u = 0, v = 0;
    float r = 1, g = 1, b = 1, a = 1;
    float pointSize = 1.0f;
    GLuint tex = 0;
    std::vector<float> buf;  // 9 float на вершину
    GLuint vbo = 0;
    GLuint whiteTex = 0;  // 1x1 белая — подставляется, когда текстуры нет
    Shader shader;
    bool shaderReady = false;
};
State S;
const int FLOATS = 9;
const int STRIDE = FLOATS * sizeof(float);
}  // namespace

static void ensureShader() {
    if (S.shaderReady) return;
    S.shaderReady = true;  // даже при ошибке не пытаемся повторно каждый кадр
    S.shader.build(VS_SRC, FS_SRC);
    glGenBuffers(1, &S.vbo);
    // Дефолтная белая 1x1 текстура: всегда держим валидную текстуру на юните 0,
    // чтобы сэмплер не был "unloadable" на untextured-пути.
    glGenTextures(1, &S.whiteTex);
    glBindTexture(GL_TEXTURE_2D, S.whiteTex);
    const unsigned char white[4] = {255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, white);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Общая draw-логика для imEnd и GpuMesh.
static void setupShaderAndMVP(GLuint tex) {
    Mat4 mvp = projection() * modelview();  // из CPU матричного стека (gl::glmat)
    S.shader.use();
    S.shader.setMat4("uMVP", mvp.m);
    S.shader.setFloat("uPointSize", S.pointSize);
    S.shader.setInt("uUseTexture", tex ? 1 : 0);
    S.shader.setInt("uTex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex ? tex : S.whiteTex);  // всегда валидная текстура
}

static void drawVbo(GLuint vbo, GLenum mode, int vertCount) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(ATTR_POS);
    glEnableVertexAttribArray(ATTR_UV);
    glEnableVertexAttribArray(ATTR_COLOR);
    glVertexAttribPointer(ATTR_POS, 3, GL_FLOAT, GL_FALSE, STRIDE, (void*)0);
    glVertexAttribPointer(ATTR_UV, 2, GL_FLOAT, GL_FALSE, STRIDE, (void*)(3 * sizeof(float)));
    glVertexAttribPointer(ATTR_COLOR, 4, GL_FLOAT, GL_FALSE, STRIDE, (void*)(5 * sizeof(float)));
    glDrawArrays(mode, 0, vertCount);
    // Возврат состояния, чтобы не мешать оставшемуся fixed-function коду
    glDisableVertexAttribArray(ATTR_POS);
    glDisableVertexAttribArray(ATTR_UV);
    glDisableVertexAttribArray(ATTR_COLOR);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void finishDraw() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void imBegin(GLenum mode) {
    S.mode = mode;
    S.buf.clear();
}

void imColor3f(float r, float g, float b) { S.r = r; S.g = g; S.b = b; S.a = 1.0f; }
void imColor4f(float r, float g, float b, float a) { S.r = r; S.g = g; S.b = b; S.a = a; }
void imTexCoord2f(float u, float v) { S.u = u; S.v = v; }
void imNormal3f(float, float, float) {}  // зарезервировано
void imTexture(GLuint tex) { S.tex = tex; }

void imVertex3f(float x, float y, float z) {
    float* p = nullptr;
    S.buf.insert(S.buf.end(), {x, y, z, S.u, S.v, S.r, S.g, S.b, S.a});
    (void)p;
}
void imVertex2f(float x, float y) { imVertex3f(x, y, 0.0f); }

void imEnd() {
    ensureShader();
    if (!S.shader.valid() || S.buf.empty()) {
        S.buf.clear();
        S.tex = 0;
        return;
    }

    // Триангуляция GL_QUADS -> GL_TRIANGLES (квадов нет в Core)
    const std::vector<float>* draw = &S.buf;
    std::vector<float> tris;
    GLenum mode = S.mode;
    if (mode == GL_QUADS) {
        int verts = (int)S.buf.size() / FLOATS;
        tris.reserve((verts / 4) * 6 * FLOATS);
        auto push = [&](int i) {
            const float* v = &S.buf[i * FLOATS];
            tris.insert(tris.end(), v, v + FLOATS);
        };
        for (int q = 0; q + 3 < verts; q += 4) {
            push(q + 0); push(q + 1); push(q + 2);
            push(q + 0); push(q + 2); push(q + 3);
        }
        draw = &tris;
        mode = GL_TRIANGLES;
    }
    int vertCount = (int)draw->size() / FLOATS;

    setupShaderAndMVP(S.tex);
    glBindBuffer(GL_ARRAY_BUFFER, S.vbo);
    glBufferData(GL_ARRAY_BUFFER, draw->size() * sizeof(float), draw->data(), GL_STREAM_DRAW);
    // Размер точки берётся из gl_PointSize шейдера (нужно включить program point size)
    if (mode == GL_POINTS) glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    drawVbo(S.vbo, mode, vertCount);
    if (mode == GL_POINTS) glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    finishDraw();

    S.buf.clear();
    S.tex = 0;
    S.pointSize = 1.0f;
}

void imPointSize(float size) { S.pointSize = size; }

// --- GpuMesh -------------------------------------------------------------
GpuMesh::~GpuMesh() { destroy(); }

void GpuMesh::destroy() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    count_ = 0;
}

void GpuMesh::upload(const float* interleaved, int vertexCount) {
    ensureShader();  // гарантируем валидный контекст/буферы
    if (!vbo_) glGenBuffers(1, &vbo_);
    count_ = vertexCount;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)vertexCount * FLOATS * sizeof(float),
                 interleaved, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GpuMesh::draw(GLuint tex) const {
    if (!vbo_ || count_ == 0 || !S.shader.valid()) return;
    setupShaderAndMVP(tex);
    drawVbo(vbo_, GL_TRIANGLES, count_);
    finishDraw();
}

}  // namespace gl
