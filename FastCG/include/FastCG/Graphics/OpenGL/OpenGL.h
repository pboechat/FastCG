#ifndef FASTCG_OPENGL_H
#define FASTCG_OPENGL_H

#ifdef FASTCG_OPENGL

#include <GL/glew.h>
#include <GL/gl.h>

#if defined FASTCG_WINDOWS
#include <GL/wglew.h>
#elif defined FASTCG_LINUX
#include <GL/glxew.h>
#endif

#endif

#endif