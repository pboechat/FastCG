#ifndef FASTCG_OPENGL_H
#define FASTCG_OPENGL_H

#ifdef FASTCG_OPENGL

#if !defined FASTCG_ANDROID
#include <GL/glew.h>
#endif

#if defined FASTCG_ANDROID
#include <GLES3/gl32.h>
#else
#include <GL/gl.h>
#endif

#if defined FASTCG_WINDOWS
#include <GL/wglew.h>
#elif defined FASTCG_LINUX
#include <GL/glxew.h>
#elif defined FASTCG_ANDROID
#include <EGL/egl.h>
#endif

#endif

#endif