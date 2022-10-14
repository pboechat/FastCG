#ifndef FASTCG_SHADER_H
#define FASTCG_SHADER_H

#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLShader.h>
namespace FastCG
{
    using Shader = OpenGLShader;
}
#else
#error "FASTCG: Undefined rendering system"
#endif

#endif