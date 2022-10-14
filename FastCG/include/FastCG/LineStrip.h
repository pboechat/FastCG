#ifndef FASTCG_LINE_STRIP_H
#define FASTCG_LINE_STRIP_H

#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLLineStrip.h>
namespace FastCG
{
    using LineStrip = OpenGLLineStrip;
}
#else
#error "FASTCG: Undefined rendering system"
#endif

#endif