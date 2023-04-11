#ifdef FASTCG_VULKAN

#if defined FASTCG_WINDOWS
#include <FastCG/WindowsApplication.h>
#elif defined FASTCG_LINUX
#include <FastCG/X11Application.h>
#endif
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Exception.h>
#include <FastCG/FastCG.h>
#include <FastCG/AssetSystem.h>

#include <cassert>

namespace FastCG
{
    VulkanGraphicsSystem::VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs) : BaseGraphicsSystem(rArgs)
    {
    }

    VulkanGraphicsSystem::~VulkanGraphicsSystem() = default;

    void VulkanGraphicsSystem::OnInitialize()
    {
        BaseGraphicsSystem::OnInitialize();

        // TODO

#ifdef FASTCG_LINUX
        X11Application::GetInstance()->CreateSimpleWindow();
#endif
    }

    void VulkanGraphicsSystem::Present()
    {
    }

    double VulkanGraphicsSystem::GetPresentElapsedTime() const
    {
        return 0;
    }

    double VulkanGraphicsSystem::GetGpuElapsedTime() const
    {
        return 0;
    }
}

#endif