#ifndef FASTCG_VULKAN_GRAPHICS_CONTEXT_H
#define FASTCG_VULKAN_GRAPHICS_CONTEXT_H

#ifdef FASTCG_VULKAN

#include <FastCG/Graphics/Vulkan/Vulkan.h>
#include <FastCG/Graphics/Vulkan/VulkanTexture.h>
#include <FastCG/Graphics/Vulkan/VulkanShader.h>
#include <FastCG/Graphics/Vulkan/VulkanRenderPass.h>
#include <FastCG/Graphics/Vulkan/VulkanPipeline.h>
#include <FastCG/Graphics/Vulkan/VulkanDescriptorSet.h>
#include <FastCG/Graphics/Vulkan/VulkanBuffer.h>
#include <FastCG/Graphics/BaseGraphicsContext.h>

#include <vector>
#include <cstdint>

namespace FastCG
{
    class VulkanGraphicsSystem;

    class VulkanGraphicsContext : public BaseGraphicsContext<VulkanBuffer, VulkanShader, VulkanTexture>
    {
    public:
        VulkanGraphicsContext(const Args &rArgs);
        virtual ~VulkanGraphicsContext();

        void Begin();
        void PushDebugMarker(const char *pName);
        void PopDebugMarker();
        void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void SetBlend(bool blend);
        void SetBlendFunc(BlendFunc color, BlendFunc alpha);
        void SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha);
        void SetStencilTest(bool stencilTest);
        void SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask);
        void SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass);
        void SetStencilWriteMask(Face face, uint32_t mask);
        void SetDepthTest(bool depthTest);
        void SetDepthWrite(bool depthWrite);
        void SetDepthFunc(CompareOp depthFunc);
        void SetScissorTest(bool scissorTest);
        void SetCullMode(Face face);
        void Copy(const VulkanBuffer *pDst, const void *pSrc, size_t size);
        void Copy(const VulkanBuffer *pDst, const void *pSrc, uint32_t frameIndex, size_t size);
        void Copy(const VulkanTexture *pDst, const void *pSrc, size_t size);
        void Copy(void *pDst, const VulkanBuffer *pSrc, size_t offset, size_t size);
        void Copy(void *pDst, const VulkanBuffer *pSrc, uint32_t frameIndex, size_t offset, size_t size);
        void AddMemoryBarrier();
        void BindShader(const VulkanShader *pShader);
        void BindResource(const VulkanBuffer *pBuffer, const char *pName);
        void BindResource(const VulkanTexture *pTexture, const char *pName);
        void Blit(const VulkanTexture *pSrc, const VulkanTexture *pDst);
        void SetRenderTargets(const VulkanTexture *const *ppRenderTargets, uint32_t renderTargetCount, const VulkanTexture *pDepthStencilBuffer);
        void ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor);
        void ClearDepthStencilBuffer(float depth, int32_t stencil);
        void ClearDepthBuffer(float depth);
        void ClearStencilBuffer(int32_t stencil);
        void SetVertexBuffers(const Buffer *const *ppVertexBuffers, uint32_t vertexBufferCount);
        void SetIndexBuffer(const VulkanBuffer *pBuffer);
        void DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
        void DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset);
        void Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);
        void End();
        double GetElapsedTime() const;

    private:
        enum class DrawCommandType : uint8_t
        {
            INSTANCED_INDEXED
        };

        enum class CopyCommandType : uint8_t
        {
            BUFFER_TO_BUFFER,
            BUFFER_TO_IMAGE,
            IMAGE_TO_IMAGE,
            BLIT
        };

        struct CopyCommandArgs
        {
            struct BufferData
            {
                const VulkanBuffer *pBuffer{nullptr};
                uint32_t frameIndex{~0u};
            };

            struct TextureData
            {
                const VulkanTexture *pTexture;
            };

            union
            {
                BufferData srcBufferData;
                TextureData srcTextureData;
            };
            union
            {
                BufferData dstBufferData;
                TextureData dstTextureData;
            };

            CopyCommandArgs() {}
            CopyCommandArgs(const BufferData &rSrcBufferData, const BufferData &rDstBufferData) : srcBufferData(rSrcBufferData),
                                                                                                  dstBufferData(rDstBufferData)
            {
            }

            CopyCommandArgs(const BufferData &rSrcBufferData, const TextureData &rDstTextureData) : srcBufferData(rSrcBufferData),
                                                                                                    dstTextureData(rDstTextureData)
            {
            }
            CopyCommandArgs(const TextureData &rSrcTextureData, const TextureData &rDstTextureData) : srcTextureData(rSrcTextureData),
                                                                                                      dstTextureData(rDstTextureData)
            {
            }
        };

        struct CopyCommand
        {
#if _DEBUG
            size_t lastMarkerCommandIdx;
#endif
            CopyCommandType type;
            CopyCommandArgs args;
        };

#if _DEBUG
        enum class MarkerCommandType : uint8_t
        {
            PUSH,
            POP
        };

        struct MarkerCommand
        {
            MarkerCommandType type;
            std::string name;
        };
#endif

        static constexpr uint32_t MAX_VERTEX_BUFFER_COUNT = 16;

        struct InvokeCommand
        {
#if _DEBUG
            size_t lastMarkerCommandIdx;
#endif
            VulkanPipelineLayout pipelineLayout;
            uint32_t setCount = 0;
            VkDescriptorSet pSets[VulkanPipelineLayout::MAX_SET_COUNT];
            union
            {
                struct
                {
                    DrawCommandType type;
                    PrimitiveType primitiveType;
                    uint32_t firstInstance;
                    uint32_t instanceCount;
                    uint32_t firstIndex;
                    uint32_t indexCount;
                    int32_t vertexOffset;
                    VkViewport viewport;
                    VkRect2D scissor;
                    uint32_t vertexBufferCount;
                    VkBuffer pVertexBuffers[MAX_VERTEX_BUFFER_COUNT];
                    VkBuffer indexBuffer;
                } drawInfo;
                struct
                {
                    uint32_t groupCountX;
                    uint32_t groupCountY;
                    uint32_t groupCountZ;
                } dispatchInfo;
            };
        };

        struct PipelineBatch
        {
            size_t lastInvokeCommandIdx;
            VulkanPipeline pipeline;
            VulkanPipelineLayoutDescription layoutDescription;
        };

        enum class PassType : uint8_t
        {
            RENDER,
            COMPUTE
        };

        struct PassBatch
        {
            size_t lastCopyCommandIdx;
            size_t lastPipelineBatchIdx;
            PassType type;
            union
            {
                struct
                {
                    VkRenderPass renderPass;
                    VkFramebuffer frameBuffer;
                    uint32_t width;
                    uint32_t height;
                    uint32_t renderTargetCount;
                    const VulkanTexture *ppRenderTargets[VulkanRenderPassDescription::MAX_RENDER_TARGET_COUNT];
                    const VulkanTexture *pDepthStencilBuffer;
                    uint32_t clearValueCount;
                    VkClearValue pClearValues[VulkanRenderPassDescription::MAX_RENDER_TARGET_COUNT + 1];
                    bool depthStencilWrite;
                } renderInfo;
                struct
                {
                } computePassInfo;
            };
        };

        VulkanRenderPassDescription mRenderPassDescription;
        VulkanPipelineDescription mPipelineDescription;
        VulkanPipelineLayout mPipelineLayout;
        std::vector<PassBatch> mPassBatches;
        std::vector<PipelineBatch> mPipelineBatches;
        std::vector<CopyCommand> mCopyCommands;
        std::vector<InvokeCommand> mInvokeCommands;
        VkRenderPass mPrevRenderPass{VK_NULL_HANDLE};
        std::vector<VulkanClearRequest> mClearRequests;
        bool mAddMemoryBarrier{false};
        bool mEnded{true};
#if _DEBUG
        std::vector<MarkerCommand> mMarkerCommands;
#endif
#if !defined FASTCG_DISABLE_GPU_TIMING
        struct TimeElapsedQuery
        {
            uint32_t start;
            uint32_t end;
        };
        std::vector<TimeElapsedQuery> mTimeElapsedQueries;
        std::vector<double> mElapsedTimes;
#endif

        void AddBufferMemoryBarrier(VkBuffer buffer,
                                    VkAccessFlags srcAccessMask,
                                    VkAccessFlags dstAccessMask,
                                    VkPipelineStageFlags srcStageMask,
                                    VkPipelineStageFlags dstStageMask);
        void AddTextureMemoryBarrier(const VulkanTexture *pTexture,
                                     VkImageLayout newLayout,
                                     VkAccessFlags srcAccessMask,
                                     VkAccessFlags dstAccessMask,
                                     VkPipelineStageFlags srcStageMask,
                                     VkPipelineStageFlags dstStageMask);
        void EnqueueCopyCommand(CopyCommandType type,
                                const CopyCommandArgs &rArgs);
        void EnqueueDrawCommand(DrawCommandType type,
                                PrimitiveType primitiveType,
                                uint32_t firstInstance,
                                uint32_t instanceCount,
                                uint32_t firstIndex,
                                uint32_t indexCount,
                                int32_t vertexOffset);
        void EnqueueDispatchCommand(uint32_t groupCountX,
                                    uint32_t groupCountY,
                                    uint32_t groupCountZ);
        void EnqueueTimestampQuery(uint32_t query);
#if !defined FASTCG_DISABLE_GPU_TIMING
        void InitializeTimeElapsedData();
        void RetrieveElapsedTime();
#endif

        friend class VulkanGraphicsSystem;
        friend class VulkanTexture;
    };
}

#endif

#endif