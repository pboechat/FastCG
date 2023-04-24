#ifndef FASTCG_VULKAN_GRAPHICS_SYSTEM_H
#define FASTCG_VULKAN_GRAPHICS_SYSTEM_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsContext.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseGraphicsSystem.h>
#include <FastCG/Core/System.h>

#include <vector>
#include <memory>
#include <cstdint>

namespace FastCG
{
    class VulkanGraphicsSystem : public BaseGraphicsSystem<VulkanBuffer, VulkanGraphicsContext, VulkanShader, VulkanTexture>
    {
        FASTCG_DECLARE_SYSTEM(VulkanGraphicsSystem, GraphicsSystemArgs);

    protected:
        VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~VulkanGraphicsSystem();

        void OnInitialize() override;
        void OnFinalize() override;

    private:
        VkInstance mInstance{VK_NULL_HANDLE};
        VkSurfaceKHR mSurface{VK_NULL_HANDLE};
        VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
        VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties;
        std::unique_ptr<VkAllocationCallbacks> mAllocationCallbacks{nullptr};
        VkDevice mDevice{VK_NULL_HANDLE};
        uint32_t mGraphicsAndPresentQueueFamilyIndex{~0u};
        VkQueue mGraphicsAndPresentQueue{VK_NULL_HANDLE};
        VkSurfaceTransformFlagBitsKHR mPreTransform;
        VkPresentModeKHR mPresentMode;
        uint32_t mMaxSimultaneousFrames{0};
        uint32_t mCurrentFrame{0};
        VkSwapchainKHR mSwapChain{VK_NULL_HANDLE};
        VkSurfaceFormatKHR mSwapChainSurfaceFormat;
        std::vector<VkImage> mSwapChainImages;
        std::vector<VkSemaphore> mAcquireSwapChainImageSemaphores;
        std::vector<VkSemaphore> mSubmitFinishedSemaphores;
        std::vector<VkFence> mFrameFences;
        VkCommandPool mCommandPool{VK_NULL_HANDLE};
        std::vector<VkCommandBuffer> mCommandBuffers;

        void CreateInstance();
        void CreateSurface();
        void SelectPhysicalDevice();
        void GetPhysicalDeviceMemoryProperties();
        void CreateDeviceAndGetQueues();
        void CreateSwapChainAndGetImages();
        void RecreateSwapChainAndGetImages();
        void CreateSynchronizationObjects();
        void CreateCommandPoolAndCommandBuffers();
        void DestroyCommandPoolAndCommandBuffers();
        void DestroySynchronizationObjects();
        void DestroySwapChainAndClearImages();
        void DestroyDeviceAndClearQueues();
        void DestroySurface();
        void DestroyInstance();
        void Resize() {}
        void Present();
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;
    };

}

#endif

#endif