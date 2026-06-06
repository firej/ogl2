#include "./shader.h"

#include <cstdio>
#include <cstring>

namespace gl {

static GLuint compileStage(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        fprintf(stderr, "[Shader] %s compile error:\n%s\n",
                type == GL_VERTEX_SHADER ? "vertex" : "fragment", log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

Shader::~Shader() {
    if (prog) glDeleteProgram(prog);
}

bool Shader::build(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vs = compileStage(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!vs || !fs) {
        if (vs) glDeleteShader(vs);
        if (fs) glDeleteShader(fs);
        return false;
    }
    prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    // Привязка стандартных атрибутов до линковки (GLSL 120)
    glBindAttribLocation(prog, ATTR_POS, "aPos");
    glBindAttribLocation(prog, ATTR_UV, "aUV");
    glBindAttribLocation(prog, ATTR_COLOR, "aColor");
    glBindAttribLocation(prog, ATTR_NORMAL, "aNormal");
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024] = {};
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        fprintf(stderr, "[Shader] link error:\n%s\n", log);
        glDeleteProgram(prog);
        prog = 0;
        return false;
    }
    return true;
}

void Shader::use() const { glUseProgram(prog); }

GLint Shader::uniform(const char* name) {
    for (int i = 0; i < uCount; ++i)
        if (uNames[i] == name || strcmp(uNames[i], name) == 0) return uLocs[i];
    GLint loc = glGetUniformLocation(prog, name);
    if (uCount < CACHE) {
        uNames[uCount] = name;
        uLocs[uCount] = loc;
        uCount++;
    }
    return loc;
}

void Shader::setMat4(const char* name, const float* m16) {
    glUniformMatrix4fv(uniform(name), 1, GL_FALSE, m16);
}
void Shader::setInt(const char* name, int v) { glUniform1i(uniform(name), v); }
void Shader::setFloat(const char* name, float v) { glUniform1f(uniform(name), v); }
void Shader::setVec3(const char* name, float x, float y, float z) {
    glUniform3f(uniform(name), x, y, z);
}
void Shader::setVec4(const char* name, float x, float y, float z, float w) {
    glUniform4f(uniform(name), x, y, z, w);
}

}  // namespace gl
