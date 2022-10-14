#ifndef FASTCG_TEXTURE_H
#define FASTCG_TEXTURE_H

#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLTexture.h>
namespace FastCG
{
    using Texture = OpenGLTexture;
}
#else
#error "FASTCG: Undefined rendering system"
#endif

#endif