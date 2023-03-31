#ifndef FASTCG_GRAPHICS_SYSTEM_H
#define FASTCG_GRAPHICS_SYSTEM_H

#ifdef FASTCG_OPENGL
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
namespace FastCG
{
    using GraphicsSystem = OpenGLGraphicsSystem;
}
#else
#error "FASTCG: Undefined graphics system"
#endif

namespace FastCG
{
    using Shader = GraphicsSystem::Shader;
    using Texture = GraphicsSystem::Texture;
    using Mesh = GraphicsSystem::Mesh;
    using MaterialArgs = GraphicsSystem::Material::MaterialArgs;
    using MaterialDefinition = GraphicsSystem::MaterialDefinition;
    using Material = GraphicsSystem::Material;
    using GraphicsContext = GraphicsSystem::GraphicsContext;
    using Buffer = GraphicsSystem::Buffer;
}

#endif