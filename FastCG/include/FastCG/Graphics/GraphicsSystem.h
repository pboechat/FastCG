#ifndef FASTCG_GRAPHICS_SYSTEM_H
#define FASTCG_GRAPHICS_SYSTEM_H

#if defined FASTCG_OPENGL
#include <FastCG/Graphics/OpenGL/OpenGLGraphicsSystem.h>
namespace FastCG
{
    using GraphicsSystem = OpenGLGraphicsSystem;
}
#elif defined FASTCG_VULKAN
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
namespace FastCG
{
    using GraphicsSystem = VulkanGraphicsSystem;
}
#else
#error "FASTCG: Undefined graphics system"
#endif

namespace FastCG
{
    using Shader = GraphicsSystem::Shader;
    using Texture = GraphicsSystem::Texture;
    using GraphicsContext = GraphicsSystem::GraphicsContext;
    using Buffer = GraphicsSystem::Buffer;
}

#endif