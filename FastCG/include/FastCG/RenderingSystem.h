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

namespace FastCG
{
    using Buffer = RenderingSystem::Buffer;
    using Material = RenderingSystem::Material;
    using MaterialDefinition = RenderingSystem::MaterialDefinition;
    using MaterialArgs = RenderingSystem::Material::MaterialArgs;
    using Mesh = RenderingSystem::Mesh;
    using RenderingContext = RenderingSystem::RenderingContext;
    using Shader = RenderingSystem::Shader;
    using Texture = RenderingSystem::Texture;
}

#endif