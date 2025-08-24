#ifndef OGL_H
#define OGL_H

// OpenGL headers for cross-platform compatibility
#ifdef WIN32
#include "../Ex/MesaGL/glew.h"
#include <gl/gl.h>
#include <gl/glu.h>
#else
// macOS/Linux OpenGL headers
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#endif

#endif // OGL_H