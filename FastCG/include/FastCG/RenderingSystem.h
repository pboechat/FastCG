#ifndef FASTCG_RENDERING_SYSTEM_H
#define FASTCG_RENDERING_SYSTEM_H

#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLRenderingSystem.h>
namespace FastCG
{
    using RenderingSystem = OpenGLRenderingSystem;
}
#else
#error "FASTCG: Undefined rendering system"
#endif

#endif