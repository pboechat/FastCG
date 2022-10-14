#ifndef FASTCG_POINTS_H
#define FASTCG_POINTS_H

#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLPoints.h>
namespace FastCG
{
    using Points = OpenGLPoints;
}
#else
#error "FASTCG: Undefined rendering system"
#endif

#endif