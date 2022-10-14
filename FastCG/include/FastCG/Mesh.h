#ifndef FASTCG_MESH_H
#define FASTCG_MESH_H

#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLMesh.h>
namespace FastCG
{
    using Mesh = OpenGLMesh;
}
#else
#error "FASTCG: Undefined rendering system"
#endif

#endif