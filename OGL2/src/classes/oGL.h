#pragma once

// OpenGL headers for cross-platform compatibility
#ifdef WIN32
#include <gl/gl.h>
#include <gl/glu.h>

#include "../Ex/MesaGL/glew.h"
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
