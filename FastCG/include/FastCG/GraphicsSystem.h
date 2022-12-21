#ifndef FASTCG_GRAPHICS_SYSTEM_H
#define FASTCG_GRAPHICS_SYSTEM_H

#ifdef FASTCG_OPENGL
#include <FastCG/OpenGLGraphicsSystem.h>
namespace FastCG
{
    using GraphicsSystem = OpenGLGraphicsSystem;
}
#else
#error "FASTCG: Undefined graphics system"
#endif

namespace FastCG
{
    using Buffer = GraphicsSystem::Buffer;
    using Material = GraphicsSystem::Material;
    using MaterialDefinition = GraphicsSystem::MaterialDefinition;
    using MaterialArgs = GraphicsSystem::Material::MaterialArgs;
    using Mesh = GraphicsSystem::Mesh;
    using RenderingContext = GraphicsSystem::RenderingContext;
    using Shader = GraphicsSystem::Shader;
    using Texture = GraphicsSystem::Texture;
}

#endif