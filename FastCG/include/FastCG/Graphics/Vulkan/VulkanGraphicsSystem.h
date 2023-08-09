#ifndef FASTCG_VULKAN_GRAPHICS_SYSTEM_H
#define FASTCG_VULKAN_GRAPHICS_SYSTEM_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanRenderPass.h>
#include <FastCG/Graphics/Vulkan/VulkanPipeline.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsContext.h>
#include <FastCG/Graphics/Vulkan/VulkanDescriptorSet.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseGraphicsSystem.h>
#include <FastCG/Core/System.h>
#include <FastCG/Core/Hash.h>

#include <vector>
#include <unordered_map>
#include <tuple>
#include <memory>
#include <deque>
#include <cstdint>
#include <cassert>

namespace FastCG
{
    struct VulkanImageMemoryTransition
    {
        VkImageLayout layout;
        VkAccessFlags accessMask;
        VkPipelineStageFlags stageMask;
    };

    class VulkanGraphicsSystem : public BaseGraphicsSystem<VulkanBuffer, VulkanGraphicsContext, VulkanShader, VulkanTexture>
    {
        FASTCG_DECLARE_SYSTEM(VulkanGraphicsSystem, GraphicsSystemArgs);

    public:
        inline uint32_t GetMaxSimultaneousFrames() const
        {
            return mMaxSimultaneousFrames;
        }
        inline uint32_t GetCurrentFrame() const
        {
            return mCurrentFrame;
        }
        inline void DestroyBuffer(const VulkanBuffer *pBuffer);
        inline void DestroyShader(const VulkanShader *pShader);
        inline void DestroyTexture(const VulkanTexture *pTexture);

    protected:
        using Super = BaseGraphicsSystem<VulkanBuffer, VulkanGraphicsContext, VulkanShader, VulkanTexture>;

        VulkanGraphicsSystem(const GraphicsSystemArgs &rArgs);
        virtual ~VulkanGraphicsSystem();

        void OnInitialize() override;
        void OnPreFinalize() override;
        void OnPostFinalize() override;

    private:
        static constexpr VkFormat LAST_FORMAT = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;

        struct DeferredDestroyRequest
        {
            enum class Type : uint8_t
            {
                BUFFER,
                SHADER,
                TEXTURE,
                FRAME_BUFFER
            };

            uint32_t frame;
            union
            {
                const VulkanBuffer *pBuffer;
                const VulkanShader *pShader;
                const VulkanTexture *pTexture;
                VkFramebuffer frameBuffer;
            };
            Type type;

            DeferredDestroyRequest(uint32_t frame, const VulkanBuffer *pBuffer) : frame(frame), type(Type::BUFFER), pBuffer(pBuffer) {}
            DeferredDestroyRequest(uint32_t frame, const VulkanShader *pShader) : frame(frame), type(Type::SHADER), pShader(pShader) {}
            DeferredDestroyRequest(uint32_t frame, const VulkanTexture *pTexture) : frame(frame), type(Type::TEXTURE), pTexture(pTexture) {}
            DeferredDestroyRequest(uint32_t frame, VkFramebuffer frameBuffer) : frame(frame), type(Type::FRAME_BUFFER), frameBuffer(frameBuffer) {}
        };

        struct DescriptorSetLocalPool
        {
            static constexpr size_t MAX_SETS = 64;

            VkDescriptorSet descriptorSets[MAX_SETS]{};
            size_t lastDescriptorSetIdx{0};
        };

        VkInstance mInstance{VK_NULL_HANDLE};
        VkSurfaceKHR mSurface{VK_NULL_HANDLE};
        VkPhysicalDevice mPhysicalDevice{VK_NULL_HANDLE};
        VkPhysicalDeviceProperties mPhysicalDeviceProperties{};
        VkPhysicalDeviceMemoryProperties mPhysicalDeviceMemoryProperties{};
        VkFormatProperties mFormatProperties[((size_t)LAST_FORMAT) + 1]{};
        std::unique_ptr<VkAllocationCallbacks> mAllocationCallbacks{nullptr};
        VkDevice mDevice{VK_NULL_HANDLE};
        uint32_t mGraphicsAndPresentQueueFamilyIndex{~0u};
        VkQueue mGraphicsAndPresentQueue{VK_NULL_HANDLE};
        VkSurfaceTransformFlagBitsKHR mPreTransform;
        VkPresentModeKHR mPresentMode;
        uint32_t mMaxSimultaneousFrames{0};
        uint32_t mCurrentFrame{0};
        uint32_t mSwapChainIndex{0};
        VkSwapchainKHR mSwapChain{VK_NULL_HANDLE};
        VkSurfaceFormatKHR mSwapChainSurfaceFormat;
        std::vector<const VulkanTexture *> mSwapChainTextures;
        std::vector<VkSemaphore> mAcquireSwapChainImageSemaphores;
        std::vector<VkSemaphore> mSubmitFinishedSemaphores;
        std::vector<VkFence> mFrameFences;
        VkCommandPool mCommandPool{VK_NULL_HANDLE};
        std::vector<VkCommandBuffer> mCommandBuffers;
        VkDescriptorPool mDescriptorPool{VK_NULL_HANDLE};
#if _DEBUG
        VkDebugUtilsMessengerEXT mDebugMessenger;
#endif
        VmaAllocator mAllocator;
        std::unordered_map<size_t, VkRenderPass, IdentityHasher<size_t>> mRenderPasses;
        std::unordered_map<size_t, VkFramebuffer, IdentityHasher<size_t>> mFrameBuffers;
        std::unordered_map<size_t, VkPipeline, IdentityHasher<size_t>> mPipelines;
        std::unordered_map<size_t, VkPipelineLayout, IdentityHasher<size_t>> mPipelineLayouts;
        std::unordered_map<size_t, VkDescriptorSetLayout, IdentityHasher<size_t>> mDescriptorSetLayouts;
        std::vector<std::unordered_map<size_t, DescriptorSetLocalPool, IdentityHasher<size_t>>> mDescriptorSetLocalPools;
        std::unordered_map<VkImage, VulkanImageMemoryTransition, IdentityHasher<VkImage>> mLastImageMemoryTransitions;
        VulkanGraphicsContext *mpImmediateGraphicsContext;
        std::deque<DeferredDestroyRequest> mDeferredDestroyRequests;
        std::unordered_map<VkImage, size_t, IdentityHasher<VkImage>> mRenderTargetToFrameBufferHash;
        std::unordered_map<size_t, std::vector<VkImage>, IdentityHasher<size_t>> mFrameBufferHashToRenderTargets;

        inline VkInstance GetVulkanInstance() const;
        inline VkDevice GetDevice() const;
        inline VmaAllocator GetAllocator() const;
        inline const VulkanTexture *GetCurrentSwapChainTexture() const;
        inline VulkanGraphicsContext *GetImmediateGraphicsContext() const;
        inline VkCommandBuffer GetCurrentCommandBuffer() const;
        inline VkAllocationCallbacks *GetAllocationCallbacks() const;
        inline const VkFormatProperties *GetFormatProperties(VkFormat format) const;
        void CreateInstance();
        void CreateSurface();
        void SelectPhysicalDevice();
        void CreateAllocator();
        void GetPhysicalDeviceProperties();
        void CreateDeviceAndGetQueues();
        void RecreateSwapChainAndGetImages();
        void AcquireNextSwapChainImage();
        void CreateSynchronizationObjects();
        void CreateCommandPoolAndCommandBuffers();
        void CreateDescriptorPool();
        void BeginCurrentCommandBuffer();
        void CreateImmediateGraphicsContext();
        void EndCurrentCommandBuffer();
        void DestroyDescriptorSetLayouts();
        void DestroyPipelineLayouts();
        void DestroyPipelines();
        void DestroyFrameBuffers();
        void DestroyRenderPasses();
        void DestroyDescriptorPool();
        void DestroyCommandPoolAndCommandBuffers();
        void DestroySynchronizationObjects();
        void DestroySwapChainAndClearImages();
        void DestroyDeviceAndClearQueues();
        void DestroyAllocator();
        void DestroySurface();
        void DestroyInstance();
        void Resize() {}
        void Present();
        double GetPresentElapsedTime() const;
        double GetGpuElapsedTime() const;
#if _DEBUG
        void PushDebugMarker(VkCommandBuffer commandBuffer, const char *pName);
        void PopDebugMarker(VkCommandBuffer commandBuffer);
        void SetObjectName(const char *pObjectName, VkObjectType objectType, uint64_t objectHandle);
#endif
        std::pair<size_t, VkRenderPass> GetOrCreateRenderPass(const VulkanRenderPassDescription &rRenderPassDescription,
                                                              const std::vector<VulkanClearRequest> &rClearRequests,
                                                              bool depthStencilWrite);
        std::pair<size_t, VkFramebuffer> GetOrCreateFrameBuffer(const VulkanRenderPassDescription &rRenderPassDescription,
                                                                const std::vector<VulkanClearRequest> &rClearRequests,
                                                                bool depthStencilWrite);
        std::pair<size_t, VulkanPipeline> GetOrCreatePipeline(const VulkanPipelineDescription &rPipelineDescription,
                                                              VkRenderPass renderPass,
                                                              uint32_t renderTargetCount,
                                                              const std::vector<const VulkanBuffer *> &rVertexBuffers);
        std::pair<size_t, VkPipelineLayout> GetOrCreatePipelineLayout(const VulkanPipelineLayoutDescription &rPipelineLayoutDescription);
        std::pair<size_t, VkDescriptorSetLayout> GetOrCreateDescriptorSetLayout(const VulkanDescriptorSetLayout &rDescriptorSetLayout);
        std::pair<size_t, VkDescriptorSet> GetOrCreateDescriptorSet(const VulkanDescriptorSetLayout &rDescriptorSetLayout);
        void PerformDeferredDestroys();
        void FinalizeDeferredDestroys();
        inline VulkanImageMemoryTransition GetLastImageMemoryTransition(const VulkanTexture *pTexture) const;
        inline void NotifyImageMemoryTransition(const VulkanTexture *pTexture, const VulkanImageMemoryTransition &rImageMemoryTransition);
        inline void DestroyFrameBuffer(size_t frameBufferHash);

        friend class VulkanBuffer;
        friend class VulkanGraphicsContext;
        friend class VulkanShader;
        friend class VulkanTexture;
    };

}

#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.inc>

#endif

#endif