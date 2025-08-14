#ifdef FASTCG_VULKAN

#include <FastCG/Core/Log.h>
#include <FastCG/Graphics/Vulkan/VulkanErrorHandling.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsContext.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Platform/Application.h>

#include <algorithm>
#include <cstring>
#include <limits>

namespace FastCG
{
    VkBuffer GetCurrentVkBuffer(const VulkanBuffer *pBuffer)
    {
        uint32_t frameIndex;
        if (pBuffer->IsMultiFrame())
        {
            frameIndex = VulkanGraphicsSystem::GetInstance()->GetCurrentFrame();
        }
        else
        {
            frameIndex = 0;
        }
        return pBuffer->GetFrameData(frameIndex).buffer;
    }

    VulkanGraphicsContext::VulkanGraphicsContext(const Args &rArgs)
        : BaseGraphicsContext<VulkanBuffer, VulkanShader, VulkanTexture>(rArgs)
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        InitializeTimeElapsedData();
#endif
    }

    VulkanGraphicsContext::~VulkanGraphicsContext() = default;

    void VulkanGraphicsContext::Begin()
    {
        assert(mEnded);
        mRenderPassDescription.renderTargetCount = 0;
        mRenderPassDescription.pDepthStencilBuffer = nullptr;
        memset(&mPipelineDescription, 0, sizeof(mPipelineDescription));
        mPipelineLayout.setCount = 0;
#if !defined FASTCG_DISABLE_GPU_TIMING
        if (mTimeElapsedQueries.size() != VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames())
        {
            InitializeTimeElapsedData();
        }
        mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()] = {
            VulkanGraphicsSystem::GetInstance()->NextQuery(), VulkanGraphicsSystem::GetInstance()->NextQuery()};
        EnqueueTimestampQuery(mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()].start,
                              VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);
        mElapsedTimes[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()] = 0;
#endif
        mEnded = false;
        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Context began (context: %s)", GetName().c_str());
    }

    void VulkanGraphicsContext::PushDebugMarker(const char *pName)
    {
        assert(pName != nullptr);
#if _DEBUG
        mMarkerCommands.emplace_back(MarkerCommand{MarkerCommandType::PUSH, pName});
#endif
    }

    void VulkanGraphicsContext::PopDebugMarker()
    {
#if _DEBUG
        mMarkerCommands.emplace_back(MarkerCommand{MarkerCommandType::POP});
#endif
    }

    void VulkanGraphicsContext::SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        mPipelineDescription.graphicsInfo.viewport = VkViewport{(float)x, (float)y, (float)width, (float)height, 0, 1};
    }

    void VulkanGraphicsContext::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
    {
        mPipelineDescription.graphicsInfo.scissor = VkRect2D{x, y, width, height};
    }

    void VulkanGraphicsContext::SetBlend(bool blend)
    {
        mPipelineDescription.graphicsInfo.blend = blend;
    }

    void VulkanGraphicsContext::SetBlendFunc(BlendFunc color, BlendFunc alpha)
    {
        mPipelineDescription.graphicsInfo.blendState.colorOp = color;
        mPipelineDescription.graphicsInfo.blendState.alphaOp = alpha;
    }

    void VulkanGraphicsContext::SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha,
                                                BlendFactor dstAlpha)
    {
        mPipelineDescription.graphicsInfo.blendState.srcColorFactor = srcColor;
        mPipelineDescription.graphicsInfo.blendState.dstColorFactor = dstColor;
        mPipelineDescription.graphicsInfo.blendState.srcAlphaFactor = srcAlpha;
        mPipelineDescription.graphicsInfo.blendState.dstAlphaFactor = dstAlpha;
    }

    void VulkanGraphicsContext::SetStencilTest(bool stencilTest)
    {
        mPipelineDescription.graphicsInfo.stencilTest = stencilTest;
    }

    void VulkanGraphicsContext::SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask)
    {
        assert(face == Face::FRONT || face == Face::BACK || face == Face::FRONT_AND_BACK);
        if (face == Face::FRONT || face == Face::FRONT_AND_BACK)
        {
            mPipelineDescription.graphicsInfo.stencilFrontState.compareOp = stencilFunc;
            mPipelineDescription.graphicsInfo.stencilFrontState.reference = ref;
            mPipelineDescription.graphicsInfo.stencilFrontState.compareMask = mask;
        }
        if (face == Face::BACK || face == Face::FRONT_AND_BACK)
        {
            mPipelineDescription.graphicsInfo.stencilBackState.compareOp = stencilFunc;
            mPipelineDescription.graphicsInfo.stencilBackState.reference = ref;
            mPipelineDescription.graphicsInfo.stencilBackState.compareMask = mask;
        }
    }

    void VulkanGraphicsContext::SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass)
    {
        assert(face == Face::FRONT || face == Face::BACK || face == Face::FRONT_AND_BACK);
        if (face == Face::FRONT || face == Face::FRONT_AND_BACK)
        {
            mPipelineDescription.graphicsInfo.stencilFrontState.stencilFailOp = stencilFail;
            mPipelineDescription.graphicsInfo.stencilFrontState.depthFailOp = depthFail;
            mPipelineDescription.graphicsInfo.stencilFrontState.passOp = depthPass;
        }
        if (face == Face::BACK || face == Face::FRONT_AND_BACK)
        {
            mPipelineDescription.graphicsInfo.stencilBackState.stencilFailOp = stencilFail;
            mPipelineDescription.graphicsInfo.stencilBackState.depthFailOp = depthFail;
            mPipelineDescription.graphicsInfo.stencilBackState.passOp = depthPass;
        }
    }

    void VulkanGraphicsContext::SetStencilWriteMask(Face face, uint32_t mask)
    {
        if (face == Face::FRONT || face == Face::FRONT_AND_BACK)
        {
            mPipelineDescription.graphicsInfo.stencilFrontState.writeMask = mask;
        }
        if (face == Face::BACK || face == Face::FRONT_AND_BACK)
        {
            mPipelineDescription.graphicsInfo.stencilBackState.writeMask = mask;
        }
    }

    void VulkanGraphicsContext::SetDepthTest(bool depthTest)
    {
        mPipelineDescription.graphicsInfo.depthTest = depthTest;
    }

    void VulkanGraphicsContext::SetDepthWrite(bool depthWrite)
    {
        mPipelineDescription.graphicsInfo.depthWrite = depthWrite;
    }

    void VulkanGraphicsContext::SetDepthFunc(CompareOp depthFunc)
    {
        mPipelineDescription.graphicsInfo.depthFunc = depthFunc;
    }

    void VulkanGraphicsContext::SetScissorTest(bool scissorTest)
    {
        mPipelineDescription.graphicsInfo.scissorTest = scissorTest;
    }

    void VulkanGraphicsContext::SetCullMode(Face face)
    {
        mPipelineDescription.graphicsInfo.cullMode = face;
    }

    void VulkanGraphicsContext::Copy(const VulkanBuffer *pDst, const void *pSrc, size_t size)
    {
        assert(pDst != nullptr);
        uint32_t frameIndex;
        if (pDst->IsMultiFrame())
        {
            frameIndex = VulkanGraphicsSystem::GetInstance()->GetCurrentFrame();
        }
        else
        {
            frameIndex = 0;
        }
        Copy(pDst, pSrc, frameIndex, size);
    }

    void VulkanGraphicsContext::Copy(const VulkanBuffer *pDst, const void *pSrc, uint32_t frameIndex, size_t size)
    {
        assert(pDst != nullptr);
        assert(pSrc != nullptr);
        assert(size > 0);

        auto &rBufferFrameData = pDst->GetFrameData(frameIndex);
        assert(rBufferFrameData.allocation != VK_NULL_HANDLE);

        if (pDst->UsesMappableMemory())
        {
            void *pMappedDst;
            FASTCG_CHECK_VK_RESULT(vmaMapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                                rBufferFrameData.allocation, &pMappedDst));
            std::memcpy(pMappedDst, pSrc, size);
            vmaUnmapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(), rBufferFrameData.allocation);

            VkMemoryPropertyFlags memPropFlags;
            vmaGetAllocationMemoryProperties(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                             rBufferFrameData.allocation, &memPropFlags);
            if ((memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
            {
                FASTCG_CHECK_VK_RESULT(vmaFlushAllocation(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                                          rBufferFrameData.allocation, 0, size));
            }
        }
        else
        {
            auto *pStagingBuffer = VulkanGraphicsSystem::GetInstance()->CreateBuffer(
                {"Staging Buffer for " + pDst->GetName() +
                     (frameIndex > 0 ? " (" + std::to_string(frameIndex) + ")" : ""),
                 BufferUsageFlagBit::DYNAMIC,
                 size,
                 pSrc,
                 {},
                 true});

            EnqueueCopyCommand(CopyCommandType::BUFFER_TO_BUFFER,
                               CopyCommandArgs{{pStagingBuffer, 0}, {pDst, frameIndex}});

            VulkanGraphicsSystem::GetInstance()->DestroyBuffer(pStagingBuffer);
        }
    }

    void VulkanGraphicsContext::Copy(const VulkanTexture *pDst, const void *pSrc, size_t size)
    {
        assert(pDst != nullptr);
        assert(pDst->GetAllocation() != VK_NULL_HANDLE);
        assert(pSrc != nullptr);
        assert(size > 0);

        if (pDst->UsesMappableMemory())
        {
            void *pMappedDst;
            FASTCG_CHECK_VK_RESULT(
                vmaMapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(), pDst->GetAllocation(), &pMappedDst));
            std::memcpy(pMappedDst, pSrc, size);
            vmaUnmapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(), pDst->GetAllocation());

            VkMemoryPropertyFlags memPropFlags;
            vmaGetAllocationMemoryProperties(VulkanGraphicsSystem::GetInstance()->GetAllocator(), pDst->GetAllocation(),
                                             &memPropFlags);
            if ((memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
            {
                FASTCG_CHECK_VK_RESULT(vmaFlushAllocation(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                                          pDst->GetAllocation(), 0, VK_WHOLE_SIZE));
            }
        }
        else
        {
            auto *pStagingBuffer = VulkanGraphicsSystem::GetInstance()->CreateBuffer(
                {"Staging Buffer for " + pDst->GetName(), BufferUsageFlagBit::DYNAMIC, size, pSrc, {}, true});

            EnqueueCopyCommand(CopyCommandType::BUFFER_TO_IMAGE, CopyCommandArgs{{pStagingBuffer, 0}, {pDst}});

            VulkanGraphicsSystem::GetInstance()->DestroyBuffer(pStagingBuffer);
        }
    }

    void VulkanGraphicsContext::Copy(void *pDst, const VulkanBuffer *pSrc, size_t offset, size_t size)
    {
        assert(pSrc != nullptr);
        uint32_t frameIndex;
        if (pSrc->IsMultiFrame())
        {
            frameIndex = VulkanGraphicsSystem::GetInstance()->GetCurrentFrame();
        }
        else
        {
            frameIndex = 0;
        }
        Copy(pDst, pSrc, frameIndex, offset, size);
    }

    void VulkanGraphicsContext::Copy(void *pDst, const VulkanBuffer *pSrc, uint32_t frameIndex, size_t offset,
                                     size_t size)
    {
        assert(pDst != nullptr);
        assert(pSrc != nullptr);
        assert(size > 0);

        auto &rBufferFrameData = pSrc->GetFrameData(frameIndex);
        assert(rBufferFrameData.allocation != VK_NULL_HANDLE);

        if (pSrc->UsesMappableMemory())
        {
            void *pMappedData;
            FASTCG_CHECK_VK_RESULT(vmaMapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                                rBufferFrameData.allocation, &pMappedData));
            pMappedData = (void *)((char *)pMappedData + offset);
            std::memcpy(pDst, pMappedData, size);
            vmaUnmapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(), rBufferFrameData.allocation);

            VkMemoryPropertyFlags memPropFlags;
            vmaGetAllocationMemoryProperties(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                             rBufferFrameData.allocation, &memPropFlags);
            if ((memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
            {
                FASTCG_CHECK_VK_RESULT(vmaInvalidateAllocation(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
                                                               rBufferFrameData.allocation, 0, VK_WHOLE_SIZE));
            }
        }
        else
        {
            // TODO:
        }
    }

    void VulkanGraphicsContext::AddMemoryBarrier()
    {
        mAddMemoryBarrier = true;
    }

    void VulkanGraphicsContext::BindShader(const VulkanShader *pShader)
    {
        assert(pShader != nullptr);
        if (mPipelineDescription.pShader == pShader)
        {
            return;
        }

        mPipelineDescription.pShader = pShader;
        mPipelineLayout.setCount = 0;
    }

    void VulkanGraphicsContext::BindResource(const VulkanBuffer *pBuffer, const char *pName)
    {
        assert(pBuffer != nullptr);
        assert(pName != nullptr);
        assert(mPipelineDescription.pShader != nullptr);
        auto location = mPipelineDescription.pShader->GetResourceLocation(pName);
        if (location.set == ~0u && location.binding == ~0u)
        {
            return;
        }
        assert(location.set < VulkanPipelineLayout::MAX_SET_COUNT);
        assert(location.binding < VulkanDescriptorSet::MAX_BINDING_COUNT);
        if (mPipelineLayout.setCount <= location.set)
        {
            auto oldCount = mPipelineLayout.setCount;
            mPipelineLayout.setCount = location.set + 1;
            std::memset(mPipelineLayout.pSet + oldCount, 0,
                        (mPipelineLayout.setCount - oldCount) * sizeof(mPipelineLayout.pSet[0]));
        }
        auto &rDescriptorSet = mPipelineLayout.pSet[location.set];
        if (rDescriptorSet.bindingCount <= location.binding)
        {
            auto oldCount = rDescriptorSet.bindingCount;
            rDescriptorSet.bindingCount = location.binding + 1;
            std::memset(rDescriptorSet.pBindings + oldCount, 0,
                        (rDescriptorSet.bindingCount - oldCount) * sizeof(rDescriptorSet.pBindings[0]));
        }
        auto &rBinding = rDescriptorSet.pBindings[location.binding];
        rBinding.pBuffer = pBuffer;
    }

    void VulkanGraphicsContext::BindResource(const VulkanTexture *pTexture, const char *pName)
    {
        assert(pName != nullptr);
        assert(mPipelineDescription.pShader != nullptr);
        auto location = mPipelineDescription.pShader->GetResourceLocation(pName);
        if (location.set == ~0u && location.binding == ~0u)
        {
            return;
        }
        assert(location.set < VulkanPipelineLayout::MAX_SET_COUNT);
        assert(location.binding < VulkanDescriptorSet::MAX_BINDING_COUNT);
        if (mPipelineLayout.setCount <= location.set)
        {
            auto oldCount = mPipelineLayout.setCount;
            mPipelineLayout.setCount = location.set + 1;
            std::memset(mPipelineLayout.pSet + oldCount, 0,
                        (mPipelineLayout.setCount - oldCount) * sizeof(mPipelineLayout.pSet[0]));
        }
        auto &rDescriptorSet = mPipelineLayout.pSet[location.set];
        if (rDescriptorSet.bindingCount <= location.binding)
        {
            auto oldCount = rDescriptorSet.bindingCount;
            rDescriptorSet.bindingCount = location.binding + 1;
            std::memset(rDescriptorSet.pBindings + oldCount, 0,
                        (rDescriptorSet.bindingCount - oldCount) * sizeof(rDescriptorSet.pBindings[0]));
        }
        auto &rBinding = rDescriptorSet.pBindings[location.binding];
        rBinding.pTexture = pTexture;
    }

    void VulkanGraphicsContext::Blit(const VulkanTexture *pSrc, const VulkanTexture *pDst)
    {
        assert(pSrc != nullptr);
        assert(pDst != nullptr);
        EnqueueCopyCommand(CopyCommandType::BLIT, CopyCommandArgs{{pSrc}, {pDst}});
    }

    void VulkanGraphicsContext::ClearRenderTarget(uint32_t renderTargetIdx, const glm::vec4 &rClearColor)
    {
        assert(renderTargetIdx < mRenderPassDescription.renderTargetCount);
        auto &rClearRequest = mRenderPassDescription.colorClearRequests[renderTargetIdx];
        rClearRequest.value.color.float32[0] = rClearColor.r;
        rClearRequest.value.color.float32[1] = rClearColor.g;
        rClearRequest.value.color.float32[2] = rClearColor.b;
        rClearRequest.value.color.float32[3] = rClearColor.a;
        rClearRequest.flags = VulkanClearRequestFlagBit::COLOR_OR_DEPTH;
    }

    void VulkanGraphicsContext::ClearDepthStencilBuffer(float depth, int32_t stencil)
    {
        assert(mRenderPassDescription.pDepthStencilBuffer != nullptr);
        auto &rClearRequest = mRenderPassDescription.depthStencilClearRequest;
        rClearRequest.value.depthStencil.depth = depth;
        rClearRequest.value.depthStencil.stencil = (uint32_t)stencil;
        rClearRequest.flags = VulkanClearRequestFlagBit::COLOR_OR_DEPTH | VulkanClearRequestFlagBit::STENCIL;
    }

    void VulkanGraphicsContext::ClearDepthBuffer(float depth)
    {
        assert(mRenderPassDescription.pDepthStencilBuffer != nullptr);
        auto &rClearRequest = mRenderPassDescription.depthStencilClearRequest;
        rClearRequest.value.depthStencil.depth = depth;
        rClearRequest.flags |= VulkanClearRequestFlagBit::COLOR_OR_DEPTH;
    }

    void VulkanGraphicsContext::ClearStencilBuffer(int32_t stencil)
    {
        assert(mRenderPassDescription.pDepthStencilBuffer != nullptr);
        auto &rClearRequest = mRenderPassDescription.depthStencilClearRequest;
        rClearRequest.value.depthStencil.stencil = stencil;
        rClearRequest.flags |= VulkanClearRequestFlagBit::STENCIL;
    }

    void VulkanGraphicsContext::SetRenderTargets(const VulkanTexture *const *ppRenderTargets,
                                                 uint32_t renderTargetCount, const VulkanTexture *pDepthStencilBuffer)
    {
        assert(renderTargetCount <= VulkanRenderPassDescription::MAX_RENDER_TARGET_COUNT);
        // TODO: find a smarter way to check if we need to issue clean commands
        if (mNoDrawSinceLastRenderTargetsSet &&
            (mRenderPassDescription.renderTargetCount > 0 || mRenderPassDescription.pDepthStencilBuffer != nullptr))
        {
            for (uint32_t i = 0; i < mRenderPassDescription.renderTargetCount; ++i)
            {
                const auto &rClearRequest = mRenderPassDescription.colorClearRequests[i];
                if (rClearRequest.flags != VulkanClearRequestFlagBit::NONE)
                {
                    auto *pRenderTarget = mRenderPassDescription.ppRenderTargets[i];
                    if (pRenderTarget == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
                    {
                        pRenderTarget = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
                    }

                    mClearCommands.emplace_back(ClearCommand {
#if _DEBUG
                        mMarkerCommands.size(),
#endif
                            pRenderTarget, rClearRequest
                    });
                }
            }
            if (mRenderPassDescription.pDepthStencilBuffer != nullptr &&
                mRenderPassDescription.depthStencilClearRequest.flags != VulkanClearRequestFlagBit::NONE)
            {
                mClearCommands.emplace_back(ClearCommand {
#if _DEBUG
                    mMarkerCommands.size(),
#endif
                        mRenderPassDescription.pDepthStencilBuffer, mRenderPassDescription.depthStencilClearRequest
                });
            }
        }
        mRenderPassDescription.renderTargetCount = renderTargetCount;
        for (uint32_t i = 0; i < renderTargetCount; ++i)
        {
            const auto *pRenderTarget = ppRenderTargets[i];
            if (pRenderTarget == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
            {
                pRenderTarget = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
            }
            mRenderPassDescription.ppRenderTargets[i] = pRenderTarget;
            mRenderPassDescription.colorClearRequests[i] = {};
        }
        mRenderPassDescription.pDepthStencilBuffer = pDepthStencilBuffer;
        mRenderPassDescription.depthStencilClearRequest = {};
        mNoDrawSinceLastRenderTargetsSet = true;
    }

    void VulkanGraphicsContext::SetVertexBuffers(const Buffer *const *ppVertexBuffers, uint32_t vertexBufferCount)
    {
        assert(ppVertexBuffers != nullptr);
        assert(vertexBufferCount <= MAX_VERTEX_BUFFER_COUNT);
        mPipelineDescription.graphicsInfo.vertexBufferCount = vertexBufferCount;
        std::memcpy(mPipelineDescription.graphicsInfo.ppVertexBuffers, ppVertexBuffers,
                    vertexBufferCount * sizeof(const VulkanBuffer *));
    }

    void VulkanGraphicsContext::SetIndexBuffer(const VulkanBuffer *pBuffer)
    {
        assert(pBuffer != nullptr);
        mPipelineDescription.graphicsInfo.pIndexBuffer = pBuffer;
    }

    void VulkanGraphicsContext::DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount,
                                            int32_t vertexOffset)
    {
        EnqueueDrawCommand(DrawCommandType::INSTANCED_INDEXED, primitiveType, 0, 1, firstIndex, indexCount,
                           vertexOffset);
    }

    void VulkanGraphicsContext::DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance,
                                                     uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount,
                                                     int32_t vertexOffset)
    {
        EnqueueDrawCommand(DrawCommandType::INSTANCED_INDEXED, primitiveType, firstInstance, instanceCount, firstIndex,
                           indexCount, vertexOffset);
    }

    void VulkanGraphicsContext::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        EnqueueDispatchCommand(groupCountX, groupCountY, groupCountZ);
    }

    void VulkanGraphicsContext::EnqueueCopyCommand(CopyCommandType type, const CopyCommandArgs &rArgs)
    {
        mCopyCommands.emplace_back(CopyCommand {
#if _DEBUG
            mMarkerCommands.size(),
#endif
                type, rArgs
        });
    }

    void VulkanGraphicsContext::EnqueueDrawCommand(DrawCommandType type, PrimitiveType primitiveType,
                                                   uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex,
                                                   uint32_t indexCount, int32_t vertexOffset)
    {
        assert(mPipelineDescription.pShader != nullptr);

        auto &rFragmentOutputDescription = mPipelineDescription.pShader->GetOutputDescription();
        auto renderPassDescription = mRenderPassDescription;
        for (size_t i = 0; i < renderPassDescription.renderTargetCount; ++i)
        {
            auto it = rFragmentOutputDescription.find((uint32_t)i);
            if (it == rFragmentOutputDescription.end())
            {
                renderPassDescription.ppRenderTargets[i] = nullptr;
            }
        }

        bool depthWrite = mPipelineDescription.graphicsInfo.depthWrite;
        bool stencilWrite = mPipelineDescription.graphicsInfo.stencilTest &&
                            (mPipelineDescription.graphicsInfo.stencilBackState.writeMask != 0 ||
                             mPipelineDescription.graphicsInfo.stencilFrontState.writeMask != 0);

        auto renderPass = VulkanGraphicsSystem::GetInstance()
                              ->GetOrCreateRenderPass(renderPassDescription, depthWrite, stencilWrite)
                              .second;
        assert(renderPass != VK_NULL_HANDLE);

        auto frameBuffer = VulkanGraphicsSystem::GetInstance()
                               ->GetOrCreateFrameBuffer(renderPassDescription, depthWrite, stencilWrite)
                               .second;
        assert(frameBuffer != VK_NULL_HANDLE);

        auto newPassBatch = false;
        PassBatch *pPassBatch;
        if (mPassBatches.empty() || mPassBatches.back().type != PassType::RENDER ||
            mPassBatches.back().renderInfo.frameBuffer != frameBuffer ||
            mPassBatches.back().renderInfo.renderPass != renderPass)
        {
            mPassBatches.emplace_back();
            pPassBatch = &mPassBatches.back();
            newPassBatch = true;
            pPassBatch->type = PassType::RENDER;
            pPassBatch->renderInfo.renderPass = renderPass;
            pPassBatch->renderInfo.frameBuffer = frameBuffer;
            pPassBatch->renderInfo.width = std::numeric_limits<uint32_t>::max();
            pPassBatch->renderInfo.height = std::numeric_limits<uint32_t>::max();
            pPassBatch->renderInfo.renderTargetCount = 0;
            pPassBatch->renderInfo.clearValueCount = 0;
            pPassBatch->renderInfo.depthWrite = depthWrite;
            pPassBatch->renderInfo.stencilWrite = stencilWrite;

            auto CalculateRenderArea = [pPassBatch](const VulkanTexture *pRenderTarget) {
                pPassBatch->renderInfo.width = std::min(pPassBatch->renderInfo.width, pRenderTarget->GetWidth());
                pPassBatch->renderInfo.height = std::min(pPassBatch->renderInfo.height, pRenderTarget->GetHeight());
            };

            for (uint32_t i = 0; i < renderPassDescription.renderTargetCount; ++i)
            {
                auto *pRenderTarget = renderPassDescription.ppRenderTargets[i];
                if (pRenderTarget == nullptr)
                {
                    continue;
                }

                CalculateRenderArea(pRenderTarget);

                const auto &rClearRequest = renderPassDescription.colorClearRequests[i];
                auto &rClearValue = pPassBatch->renderInfo.pClearValues[pPassBatch->renderInfo.clearValueCount++];
                if (rClearRequest.flags != VulkanClearRequestFlagBit::NONE)
                {
                    rClearValue = rClearRequest.value;
                }
                else
                {
                    rClearValue = {};
                }

                pPassBatch->renderInfo.ppRenderTargets[pPassBatch->renderInfo.renderTargetCount++] = pRenderTarget;
            }

            if (renderPassDescription.pDepthStencilBuffer != nullptr)
            {
                CalculateRenderArea(renderPassDescription.pDepthStencilBuffer);

                const auto &rClearRequest = renderPassDescription.depthStencilClearRequest;
                auto &rClearValue = pPassBatch->renderInfo.pClearValues[pPassBatch->renderInfo.clearValueCount++];
                if (rClearRequest.flags != VulkanClearRequestFlagBit::NONE)
                {
                    rClearValue = rClearRequest.value;
                }
                else
                {
                    rClearValue = {};
                }

                pPassBatch->renderInfo.pDepthStencilBuffer = renderPassDescription.pDepthStencilBuffer;
            }
        }
        else
        {
            pPassBatch = &mPassBatches.back();
        }

        uint32_t renderTargetCount = 0;
        for (uint32_t i = 0; i < renderPassDescription.renderTargetCount; ++i)
        {
            auto *pRenderTarget = renderPassDescription.ppRenderTargets[i];
            if (pRenderTarget == nullptr)
            {
                continue;
            }

            renderTargetCount++;
        }

        auto pipeline = VulkanGraphicsSystem::GetInstance()
                            ->GetOrCreateGraphicsPipeline(mPipelineDescription, renderPass, renderTargetCount)
                            .second;
        assert(pipeline.pipeline != VK_NULL_HANDLE);

        PipelineBatch *pPipelineBatch;
        if (mPipelineBatches.empty() || newPassBatch || mPipelineBatches.back().pipeline.pipeline != pipeline.pipeline)
        {
            mPipelineBatches.emplace_back();
            pPipelineBatch = &mPipelineBatches.back();
            pPipelineBatch->pipeline = pipeline;
            pPipelineBatch->layoutDescription = mPipelineDescription.pShader->GetPipelineLayoutDescription();
        }
        else
        {
            pPipelineBatch = &mPipelineBatches.back();
        }

        pPassBatch->lastClearCommandIdx = mClearCommands.size();
        pPassBatch->lastCopyCommandIdx = mCopyCommands.size();
        pPassBatch->lastPipelineBatchIdx = mPipelineBatches.size();

        mInvokeCommands.emplace_back();
        auto *pInvokeCommand = &mInvokeCommands.back();

#if _DEBUG
        pPipelineBatch->pShader = mPipelineDescription.pShader;
#endif
        pPipelineBatch->lastInvokeCommandIdx = mInvokeCommands.size();

#if _DEBUG
        pInvokeCommand->lastMarkerCommandIdx = mMarkerCommands.size();
#endif

        pInvokeCommand->drawInfo.primitiveType = primitiveType;
        pInvokeCommand->drawInfo.firstInstance = firstInstance;
        pInvokeCommand->drawInfo.instanceCount = instanceCount;
        pInvokeCommand->drawInfo.firstIndex = firstIndex;
        pInvokeCommand->drawInfo.indexCount = indexCount;
        pInvokeCommand->drawInfo.vertexOffset = vertexOffset;
        const auto &rViewport = mPipelineDescription.graphicsInfo.viewport;
        pInvokeCommand->drawInfo.viewport = {rViewport.x,        rViewport.height - rViewport.y,
                                             rViewport.width,    -rViewport.height,
                                             rViewport.minDepth, rViewport.maxDepth}; // flip the viewport y axis
        // pDrawCommand->viewport = {rViewport.x, rViewport.y, rViewport.width, rViewport.height, rViewport.minDepth,
        // rViewport.maxDepth};
        const auto &rScissor = mPipelineDescription.graphicsInfo.scissor;
        if (rScissor.extent.width == 0 || rScissor.extent.height == 0)
        {
            pInvokeCommand->drawInfo.scissor = {(int32_t)rViewport.x, (int32_t)rViewport.y, (uint32_t)rViewport.width,
                                                (uint32_t)rViewport.height};
        }
        else
        {
            pInvokeCommand->drawInfo.scissor = rScissor;
        }
        pInvokeCommand->pipelineLayout = mPipelineLayout;
        pInvokeCommand->drawInfo.vertexBufferCount = mPipelineDescription.graphicsInfo.vertexBufferCount;
        for (uint32_t i = 0; i < pInvokeCommand->drawInfo.vertexBufferCount; ++i)
        {
            pInvokeCommand->drawInfo.pVertexBuffers[i] =
                GetCurrentVkBuffer(mPipelineDescription.graphicsInfo.ppVertexBuffers[i]);
        }
        pInvokeCommand->drawInfo.indexBuffer = GetCurrentVkBuffer(mPipelineDescription.graphicsInfo.pIndexBuffer);

        mNoDrawSinceLastRenderTargetsSet = false;
    }

    void VulkanGraphicsContext::EnqueueDispatchCommand(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
    {
        assert(groupCountX > 0);
        assert(groupCountY > 0);
        assert(groupCountZ > 0);
        assert(mPipelineDescription.pShader != nullptr);

        PassBatch *pPassBatch;
        if (mPassBatches.empty() || mPassBatches.back().type != PassType::COMPUTE)
        {
            mPassBatches.emplace_back();
            pPassBatch = &mPassBatches.back();
            pPassBatch->type = PassType::COMPUTE;
        }
        else
        {
            pPassBatch = &mPassBatches.back();
        }

        auto pipeline = VulkanGraphicsSystem::GetInstance()->GetOrCreateComputePipeline(mPipelineDescription).second;
        assert(pipeline.pipeline != VK_NULL_HANDLE);

        PipelineBatch *pPipelineBatch;
        if (mPipelineBatches.empty() || mPipelineBatches.back().pipeline.pipeline != pipeline.pipeline)
        {
            mPipelineBatches.emplace_back();
            pPipelineBatch = &mPipelineBatches.back();
            pPipelineBatch->pipeline = pipeline;
            pPipelineBatch->layoutDescription = mPipelineDescription.pShader->GetPipelineLayoutDescription();
        }
        else
        {
            pPipelineBatch = &mPipelineBatches.back();
        }

        pPassBatch->lastClearCommandIdx = mClearCommands.size();
        pPassBatch->lastCopyCommandIdx = mCopyCommands.size();
        pPassBatch->lastPipelineBatchIdx = mPipelineBatches.size();

        mInvokeCommands.emplace_back();
        auto *pInvokeCommand = &mInvokeCommands.back();

        pPipelineBatch->lastInvokeCommandIdx = mInvokeCommands.size();

#if _DEBUG
        pInvokeCommand->lastMarkerCommandIdx = mMarkerCommands.size();
#endif

        pInvokeCommand->dispatchInfo.groupCountX = groupCountX;
        pInvokeCommand->dispatchInfo.groupCountY = groupCountY;
        pInvokeCommand->dispatchInfo.groupCountZ = groupCountZ;
        pInvokeCommand->pipelineLayout = mPipelineLayout;
    }

    void VulkanGraphicsContext::End()
    {
        assert(!mEnded);

        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Ending context (context: %s)", GetName().c_str());

        // TODO: make this less brittle and possibly dynamic
        constexpr size_t MAX_WRITES = 128;
        constexpr size_t MAX_RESOURCES = MAX_WRITES;

        VkWriteDescriptorSet pSetWrites[MAX_WRITES];
        VkDescriptorImageInfo pImageInfos[MAX_RESOURCES];
        VkDescriptorBufferInfo pBufferInfos[MAX_RESOURCES];

#if _DEBUG
        size_t lastMarkerCommandIdx = 0;
        auto ProccessMarkers = [&](size_t end) {
            for (; lastMarkerCommandIdx < end; ++lastMarkerCommandIdx)
            {
                auto &rMarkerCommand = mMarkerCommands[lastMarkerCommandIdx];
                switch (rMarkerCommand.type)
                {
                case MarkerCommandType::PUSH:
                    VulkanGraphicsSystem::GetInstance()->PushDebugMarker(
                        VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), rMarkerCommand.name.c_str());
                    break;
                case MarkerCommandType::POP:
                    VulkanGraphicsSystem::GetInstance()->PopDebugMarker(
                        VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer());
                    break;
                default:
                    FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled marker command type %d",
                                           (int)rMarkerCommand.type);
                    break;
                }
            }
        };
#endif

        size_t lastUsedClearCommandIdx = 0;
        auto ProcessClears = [&](size_t end) {
            for (size_t i = lastUsedClearCommandIdx, ic = 0; lastUsedClearCommandIdx < end;
                 ++lastUsedClearCommandIdx, ++ic)
            {
                FASTCG_UNUSED(i);
                FASTCG_UNUSED(ic);

                FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\tClear [%zu/%zu]", ic, end - i - 1);

                const auto &rClearCommand = mClearCommands[lastUsedClearCommandIdx];

#if _DEBUG
                ProccessMarkers(rClearCommand.lastMarkerCommandIdx);
#endif

                const auto *pTexture = rClearCommand.pTexture;
                if (pTexture == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
                {
                    pTexture = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
                }

                // TODO: avoid write barriers in non-overlapping writes
                auto lastImageMemoryBarrier = VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pTexture);
                AddTextureMemoryBarrier(pTexture, lastImageMemoryBarrier.layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                        lastImageMemoryBarrier.accessMask, VK_ACCESS_TRANSFER_WRITE_BIT,
                                        lastImageMemoryBarrier.stageMask, VK_PIPELINE_STAGE_TRANSFER_BIT);

                VkImageSubresourceRange subresourceRange;
                subresourceRange.baseMipLevel = 0;
                subresourceRange.levelCount = pTexture->GetMipCount();
                subresourceRange.baseArrayLayer = 0;
                subresourceRange.layerCount = pTexture->GetSlices();

                const auto &rClearRequest = rClearCommand.clearRequest;

                if (IsDepthFormat(pTexture->GetFormat()))
                {
                    if ((rClearRequest.flags & VulkanClearRequestFlagBit::COLOR_OR_DEPTH) != 0)
                    {
                        subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                    }
                    if ((rClearRequest.flags & VulkanClearRequestFlagBit::STENCIL) != 0)
                    {
                        subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                    }

                    vkCmdClearDepthStencilImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                                pTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                &rClearRequest.value.depthStencil, 1, &subresourceRange);
                }
                else
                {
                    assert((rClearRequest.flags & VulkanClearRequestFlagBit::COLOR_OR_DEPTH) != 0);

                    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    vkCmdClearColorImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                         pTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                         &rClearRequest.value.color, 1, &subresourceRange);
                }
            }
        };

        size_t lastUsedCopyCommandIdx = 0;
        auto ProcessCopies = [&](size_t end) {
            for (size_t i = lastUsedCopyCommandIdx, ic = 0; lastUsedCopyCommandIdx < end;
                 ++lastUsedCopyCommandIdx, ++ic)
            {
                FASTCG_UNUSED(i);
                FASTCG_UNUSED(ic);

                FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\tCopy [%zu/%zu]", ic, end - i - 1);

                const auto &rCopyCommand = mCopyCommands[lastUsedCopyCommandIdx];

#if _DEBUG
                ProccessMarkers(rCopyCommand.lastMarkerCommandIdx);
#endif

                switch (rCopyCommand.type)
                {
                case CopyCommandType::BUFFER_TO_BUFFER: {
                    FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Buffer %s -> Buffer %s",
                                       rCopyCommand.args.srcBufferData.pBuffer->GetName().c_str(),
                                       rCopyCommand.args.dstBufferData.pBuffer->GetName().c_str());
                    VkBufferCopy copyRegion;
                    copyRegion.srcOffset = 0;
                    copyRegion.dstOffset = 0;
                    copyRegion.size = rCopyCommand.args.srcBufferData.pBuffer->GetDataSize();
                    auto &rSrcBufferFrameData = rCopyCommand.args.srcBufferData.pBuffer->GetFrameData(
                        rCopyCommand.args.srcBufferData.frameIndex);
                    auto &rDstBufferFrameData = rCopyCommand.args.dstBufferData.pBuffer->GetFrameData(
                        rCopyCommand.args.dstBufferData.frameIndex);

                    auto lastBufferMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastBufferMemoryBarrier(rSrcBufferFrameData.buffer);
                    if (!IsVkReadOnlyAccessFlags(lastBufferMemoryBarrier.accessMask))
                    {
                        AddBufferMemoryBarrier(rSrcBufferFrameData.buffer, lastBufferMemoryBarrier.stageMask,
                                               VK_ACCESS_TRANSFER_READ_BIT, lastBufferMemoryBarrier.stageMask,
                                               VK_PIPELINE_STAGE_TRANSFER_BIT);
                    }
                    // TODO: avoid write barriers in non-overlapping writes
                    lastBufferMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastBufferMemoryBarrier(rDstBufferFrameData.buffer);
                    AddBufferMemoryBarrier(rDstBufferFrameData.buffer, lastBufferMemoryBarrier.stageMask,
                                           VK_ACCESS_TRANSFER_WRITE_BIT, lastBufferMemoryBarrier.stageMask,
                                           VK_PIPELINE_STAGE_TRANSFER_BIT);
                    vkCmdCopyBuffer(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                    rSrcBufferFrameData.buffer, rDstBufferFrameData.buffer, 1, &copyRegion);
                }
                break;
                case CopyCommandType::BUFFER_TO_IMAGE: {
                    auto *pDstTexture = rCopyCommand.args.dstTextureData.pTexture;
                    if (pDstTexture == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
                    {
                        pDstTexture = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
                    }

                    FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Buffer %s -> Image %s",
                                       rCopyCommand.args.srcBufferData.pBuffer->GetName().c_str(),
                                       pDstTexture->GetName().c_str());

                    auto &rSrcBufferFrameData = rCopyCommand.args.srcBufferData.pBuffer->GetFrameData(
                        rCopyCommand.args.srcBufferData.frameIndex);

                    auto lastBufferMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastBufferMemoryBarrier(rSrcBufferFrameData.buffer);
                    if (!IsVkReadOnlyAccessFlags(lastBufferMemoryBarrier.accessMask))
                    {
                        AddBufferMemoryBarrier(rSrcBufferFrameData.buffer, lastBufferMemoryBarrier.stageMask,
                                               VK_ACCESS_TRANSFER_READ_BIT, lastBufferMemoryBarrier.stageMask,
                                               VK_PIPELINE_STAGE_TRANSFER_BIT);
                    }
                    auto lastImageMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pDstTexture);
                    AddTextureMemoryBarrier(pDstTexture, lastImageMemoryBarrier.layout,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, lastImageMemoryBarrier.accessMask,
                                            VK_ACCESS_TRANSFER_WRITE_BIT, lastImageMemoryBarrier.stageMask,
                                            VK_PIPELINE_STAGE_TRANSFER_BIT);

                    std::vector<VkBufferImageCopy> bufferCopyRegions;
                    if (pDstTexture->GetMipCount() > 1)
                    {
                        // 1D/2D potentially mipped textures
                        size_t dataOffset = 0;
                        bufferCopyRegions.resize(pDstTexture->GetMipCount());
                        for (uint8_t mip = 0; mip < pDstTexture->GetMipCount(); ++mip)
                        {
                            auto &rBufferCopyRegion = bufferCopyRegions[mip];
                            rBufferCopyRegion.bufferOffset = (VkDeviceSize)dataOffset;
                            rBufferCopyRegion.bufferRowLength = 0;
                            rBufferCopyRegion.bufferImageHeight = 0;
                            rBufferCopyRegion.imageSubresource.aspectMask = pDstTexture->GetAspectFlags();
                            rBufferCopyRegion.imageSubresource.mipLevel = (uint32_t)mip;
                            rBufferCopyRegion.imageSubresource.baseArrayLayer = 0;
                            rBufferCopyRegion.imageSubresource.layerCount = 1; // TODO: support array mips
                            rBufferCopyRegion.imageOffset = {0, 0, 0};
                            rBufferCopyRegion.imageExtent = {pDstTexture->GetWidth(mip), pDstTexture->GetHeight(mip),
                                                             1}; // TODO: support 3D mips
                            dataOffset += pDstTexture->GetMipDataSize(mip);
                        }
                    }
                    else if (pDstTexture->GetSlices() > 1)
                    {
                        // 2D array/cubemap textures
                        size_t dataOffset = 0;
                        bufferCopyRegions.resize(pDstTexture->GetSlices());
                        for (uint32_t slice = 0; slice < pDstTexture->GetSlices(); ++slice)
                        {
                            auto &rBufferCopyRegion = bufferCopyRegions[slice];
                            rBufferCopyRegion.bufferOffset = (VkDeviceSize)dataOffset;
                            rBufferCopyRegion.bufferRowLength = 0;
                            rBufferCopyRegion.bufferImageHeight = 0;
                            rBufferCopyRegion.imageSubresource.aspectMask = pDstTexture->GetAspectFlags();
                            rBufferCopyRegion.imageSubresource.mipLevel = 0;
                            rBufferCopyRegion.imageSubresource.baseArrayLayer = slice;
                            rBufferCopyRegion.imageSubresource.layerCount = 1;
                            rBufferCopyRegion.imageOffset = {0, 0, 0};
                            rBufferCopyRegion.imageExtent = {pDstTexture->GetWidth(), pDstTexture->GetHeight(),
                                                             1}; // TODO: support 3D arrays
                            dataOffset += pDstTexture->GetSliceDataSize();
                        }
                    }
                    else
                    {
                        // 3D textures
                        bufferCopyRegions.emplace_back(VkBufferImageCopy{});
                        auto &rBufferCopyRegion = bufferCopyRegions.back();
                        rBufferCopyRegion.bufferOffset = 0;
                        rBufferCopyRegion.bufferRowLength = 0;
                        rBufferCopyRegion.bufferImageHeight = 0;
                        rBufferCopyRegion.imageSubresource.aspectMask = pDstTexture->GetAspectFlags();
                        rBufferCopyRegion.imageSubresource.mipLevel = 0; // TODO: support 3D mips
                        rBufferCopyRegion.imageSubresource.baseArrayLayer = 0;
                        rBufferCopyRegion.imageSubresource.layerCount = 1;
                        rBufferCopyRegion.imageOffset = {0, 0, 0};
                        rBufferCopyRegion.imageExtent = {pDstTexture->GetWidth(), pDstTexture->GetHeight(),
                                                         pDstTexture->GetDepth()};
                    }
                    vkCmdCopyBufferToImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                           rCopyCommand.args.srcBufferData.pBuffer
                                               ->GetFrameData(rCopyCommand.args.srcBufferData.frameIndex)
                                               .buffer,
                                           pDstTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                           (uint32_t)bufferCopyRegions.size(), &bufferCopyRegions[0]);
                }
                break;
                case CopyCommandType::IMAGE_TO_IMAGE: {
                    auto *pSrcTexture = rCopyCommand.args.srcTextureData.pTexture;
                    if (pSrcTexture == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
                    {
                        pSrcTexture = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
                    }
                    auto *pDstTexture = rCopyCommand.args.dstTextureData.pTexture;
                    if (pDstTexture == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
                    {
                        pDstTexture = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
                    }

                    FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Image %s -> Image %s (Copy)",
                                       pSrcTexture->GetName().c_str(), pDstTexture->GetName().c_str());

                    auto lastImageMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pSrcTexture);
                    if (!IsVkReadOnlyAccessFlags(lastImageMemoryBarrier.accessMask))
                    {
                        AddTextureMemoryBarrier(pSrcTexture, lastImageMemoryBarrier.layout,
                                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, lastImageMemoryBarrier.accessMask,
                                                VK_ACCESS_TRANSFER_READ_BIT, lastImageMemoryBarrier.stageMask,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT);
                    }
                    // TODO: avoid write barriers in non-overlapping writes
                    lastImageMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pDstTexture);
                    AddTextureMemoryBarrier(pDstTexture, lastImageMemoryBarrier.layout,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, lastImageMemoryBarrier.accessMask,
                                            VK_ACCESS_TRANSFER_WRITE_BIT, lastImageMemoryBarrier.stageMask,
                                            VK_PIPELINE_STAGE_TRANSFER_BIT);

                    std::vector<VkImageCopy> imageCopyRegions;
                    if (pSrcTexture->GetMipCount() > 1)
                    {
                        imageCopyRegions.resize(pSrcTexture->GetSlices());
                        for (uint8_t mip = 0; mip < pDstTexture->GetSlices(); ++mip)
                        {
                            auto &rImageCopyRegion = imageCopyRegions[mip];
                            rImageCopyRegion.srcSubresource.aspectMask = pSrcTexture->GetAspectFlags();
                            rImageCopyRegion.srcSubresource.baseArrayLayer = (uint32_t)mip;
                            rImageCopyRegion.srcSubresource.layerCount = 1;
                            rImageCopyRegion.srcSubresource.mipLevel = 0;
                            rImageCopyRegion.srcOffset = {0, 0, 0};
                            rImageCopyRegion.dstSubresource.aspectMask = pDstTexture->GetAspectFlags();
                            rImageCopyRegion.dstSubresource.baseArrayLayer = 0;
                            rImageCopyRegion.dstSubresource.layerCount = 1;
                            rImageCopyRegion.dstSubresource.mipLevel = (uint32_t)mip;
                            rImageCopyRegion.dstOffset = {0, 0, 0};
                            rImageCopyRegion.extent.width = pSrcTexture->GetWidth();
                            rImageCopyRegion.extent.height = pSrcTexture->GetHeight();
                            rImageCopyRegion.extent.depth = 1;
                        }
                    }
                    else if (pSrcTexture->GetSlices() > 1)
                    {
                        imageCopyRegions.resize(pSrcTexture->GetSlices());
                        for (uint32_t slice = 0; slice < pDstTexture->GetSlices(); ++slice)
                        {
                            auto &rImageCopyRegion = imageCopyRegions[slice];
                            rImageCopyRegion.srcSubresource.aspectMask = pSrcTexture->GetAspectFlags();
                            rImageCopyRegion.srcSubresource.baseArrayLayer = slice;
                            rImageCopyRegion.srcSubresource.layerCount = 1;
                            rImageCopyRegion.srcSubresource.mipLevel = 0;
                            rImageCopyRegion.srcOffset = {0, 0, 0};
                            rImageCopyRegion.dstSubresource.aspectMask = pDstTexture->GetAspectFlags();
                            rImageCopyRegion.dstSubresource.baseArrayLayer = slice;
                            rImageCopyRegion.dstSubresource.layerCount = 1;
                            rImageCopyRegion.dstSubresource.mipLevel = 0;
                            rImageCopyRegion.dstOffset = {0, 0, 0};
                            rImageCopyRegion.extent.width = pSrcTexture->GetWidth();
                            rImageCopyRegion.extent.height = pSrcTexture->GetHeight();
                            rImageCopyRegion.extent.depth = 1;
                        }
                    }
                    else
                    {
                        imageCopyRegions.emplace_back(VkImageCopy{});
                        auto &rImageCopyRegion = imageCopyRegions.back();
                        rImageCopyRegion.srcSubresource.aspectMask = pSrcTexture->GetAspectFlags();
                        rImageCopyRegion.srcSubresource.baseArrayLayer = 0;
                        rImageCopyRegion.srcSubresource.layerCount = 1;
                        rImageCopyRegion.srcSubresource.mipLevel = 0;
                        rImageCopyRegion.srcOffset = {0, 0, 0};
                        rImageCopyRegion.dstSubresource.aspectMask = pDstTexture->GetAspectFlags();
                        rImageCopyRegion.dstSubresource.baseArrayLayer = 0;
                        rImageCopyRegion.dstSubresource.layerCount = 1;
                        rImageCopyRegion.dstSubresource.mipLevel = 0;
                        rImageCopyRegion.dstOffset = {0, 0, 0};
                        rImageCopyRegion.extent.width = pSrcTexture->GetWidth();
                        rImageCopyRegion.extent.height = pSrcTexture->GetHeight();
                        rImageCopyRegion.extent.depth = pSrcTexture->GetDepth();
                    }

                    vkCmdCopyImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                   pSrcTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   pDstTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                   (uint32_t)imageCopyRegions.size(), &imageCopyRegions[0]);
                }
                break;
                case CopyCommandType::BLIT: {
                    auto *pSrcTexture = rCopyCommand.args.srcTextureData.pTexture;
                    if (pSrcTexture == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
                    {
                        pSrcTexture = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
                    }
                    auto *pDstTexture = rCopyCommand.args.dstTextureData.pTexture;
                    if (pDstTexture == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
                    {
                        pDstTexture = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
                    }

                    FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Image %s -> Image %s (Blit)",
                                       pSrcTexture->GetName().c_str(), pDstTexture->GetName().c_str());

                    auto lastImageMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pSrcTexture);
                    if (!IsVkReadOnlyAccessFlags(lastImageMemoryBarrier.accessMask))
                    {
                        AddTextureMemoryBarrier(pSrcTexture, lastImageMemoryBarrier.layout,
                                                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, lastImageMemoryBarrier.accessMask,
                                                VK_ACCESS_TRANSFER_READ_BIT, lastImageMemoryBarrier.stageMask,
                                                VK_PIPELINE_STAGE_TRANSFER_BIT);
                    }
                    lastImageMemoryBarrier =
                        VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pDstTexture);
                    // TODO: avoid write barriers in non-overlapping writes
                    AddTextureMemoryBarrier(pDstTexture, lastImageMemoryBarrier.layout,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, lastImageMemoryBarrier.accessMask,
                                            VK_ACCESS_TRANSFER_WRITE_BIT, lastImageMemoryBarrier.stageMask,
                                            VK_PIPELINE_STAGE_TRANSFER_BIT);

                    VkImageBlit imageBlit{};
                    imageBlit.srcSubresource.aspectMask = pSrcTexture->GetAspectFlags();
                    imageBlit.srcSubresource.baseArrayLayer = 0;
                    imageBlit.srcSubresource.layerCount = 1; // TODO: support array textures
                    imageBlit.srcSubresource.mipLevel = 0;   // TODO: support mipped textures
                    imageBlit.srcOffsets[0] = {0, 0, 0};
                    imageBlit.srcOffsets[1] = {(int32_t)pSrcTexture->GetWidth(), (int32_t)pSrcTexture->GetHeight(),
                                               1}; // TODO: support 3D textures
                    imageBlit.dstSubresource.aspectMask = pDstTexture->GetAspectFlags();
                    imageBlit.dstSubresource.baseArrayLayer = 0;
                    imageBlit.dstSubresource.layerCount = 1; // TODO: support array textures
                    imageBlit.dstSubresource.mipLevel = 0;   // TODO: support mipped textures
                    imageBlit.dstOffsets[0] = {0, 0, 0};
                    imageBlit.dstOffsets[1] = {(int32_t)pDstTexture->GetWidth(), (int32_t)pDstTexture->GetHeight(),
                                               1}; // TODO: support 3D textures

                    vkCmdBlitImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                   pSrcTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   pDstTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageBlit,
                                   GetVkFilter(pSrcTexture->GetFilter()));
                }
                break;
                default:
                    FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled copy command type %d", (int)rCopyCommand.type);
                    continue;
                }
            }
        };

        size_t lastUsedPipelineBatchIdx = 0;
        size_t lastUsedInvokeCommandIdx = 0;

        auto GetVkPipelineStageFlags = [](VkShaderStageFlags stageFlags) {
            VkPipelineStageFlags flags = 0;
            if ((stageFlags & VK_SHADER_STAGE_VERTEX_BIT) != 0)
            {
                flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
            }
            if ((stageFlags & VK_SHADER_STAGE_FRAGMENT_BIT) != 0)
            {
                flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            }
            if ((stageFlags & VK_SHADER_STAGE_COMPUTE_BIT) != 0)
            {
                flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
            }
            return flags;
        };

        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Iterating over passes (%zu)", mPassBatches.size());

        for (size_t i = 0; i < mPassBatches.size(); ++i)
        {
            FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tPass [%zu/%zu]", i, mPassBatches.size() - 1);

            auto &rPassBatch = mPassBatches[i];

            FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tProcessing pre-pass clears (%zu)",
                               rPassBatch.lastClearCommandIdx - lastUsedClearCommandIdx);

            ProcessClears(rPassBatch.lastClearCommandIdx);

            FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tProcessing pre-pass copies (%zu)",
                               rPassBatch.lastCopyCommandIdx - lastUsedCopyCommandIdx);

            ProcessCopies(rPassBatch.lastCopyCommandIdx);

            FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tSetting up pipeline resources (%zu)",
                               rPassBatch.lastPipelineBatchIdx - lastUsedPipelineBatchIdx);

            size_t savedLastUsedInvokeCommandIdx = lastUsedInvokeCommandIdx;

            for (size_t j = lastUsedPipelineBatchIdx, jc = 0; j < rPassBatch.lastPipelineBatchIdx; ++j, ++jc)
            {
                FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\tPipeline [%zu/%zu]", jc,
                                   rPassBatch.lastPipelineBatchIdx - lastUsedPipelineBatchIdx);

                auto &rPipelineBatch = mPipelineBatches[j];

                for (size_t k = savedLastUsedInvokeCommandIdx, kc = 0;
                     savedLastUsedInvokeCommandIdx < rPipelineBatch.lastInvokeCommandIdx;
                     ++savedLastUsedInvokeCommandIdx, ++kc)
                {
                    FASTCG_UNUSED(k);
                    FASTCG_UNUSED(kc);

                    FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\tInvoke [%zu/%zu]", kc,
                                       rPipelineBatch.lastInvokeCommandIdx - k);

                    auto &rInvokeCommand = mInvokeCommands[savedLastUsedInvokeCommandIdx];

                    rInvokeCommand.setCount = rPipelineBatch.layoutDescription.setLayoutCount;

                    uint32_t setWritesCount = 0;
                    uint32_t lastImageInfoIdx = 0;
                    uint32_t lastBufferInfoIdx = 0;
                    for (uint32_t l = 0; l < rPipelineBatch.layoutDescription.setLayoutCount; ++l)
                    {
                        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t\tPipeline resource [%zu/%zu]", l,
                                           rPipelineBatch.layoutDescription.size() - 1);

                        auto &rSetLayout = rPipelineBatch.layoutDescription.pSetLayouts[l];
                        auto &rSetUsage = rInvokeCommand.pipelineLayout.pSet[l];
                        auto &rSet = rInvokeCommand.pSets[l];
                        if (rSetLayout.bindingLayoutCount == 0)
                        {
                            rSet = VK_NULL_HANDLE;
                            continue;
                        }
                        rSet = VulkanGraphicsSystem::GetInstance()->GetOrCreateDescriptorSet(rSetLayout).second;
                        for (uint32_t m = 0; m < rSetLayout.bindingLayoutCount; ++m)
                        {
                            auto &rBindingLayout = rSetLayout.pBindingLayouts[m];
                            auto *pBinding = rSetUsage.bindingCount > rBindingLayout.binding
                                                 ? &rSetUsage.pBindings[rBindingLayout.binding]
                                                 : nullptr;
                            auto &rSetWrite = pSetWrites[setWritesCount++];
                            rSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                            rSetWrite.pNext = nullptr;
                            rSetWrite.descriptorCount = 1;
                            rSetWrite.descriptorType = rBindingLayout.type;
                            rSetWrite.dstArrayElement = 0;
                            rSetWrite.dstBinding = rBindingLayout.binding;
                            rSetWrite.dstSet = rSet;
                            if (rBindingLayout.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                            {
                                auto &rImageInfo = pImageInfos[lastImageInfoIdx++];
                                assert(lastImageInfoIdx < MAX_RESOURCES);
                                const VulkanTexture *pTexture = nullptr;
                                if (pBinding != nullptr)
                                {
                                    pTexture = pBinding->pTexture;
                                }
                                if (pTexture == nullptr)
                                {
                                    // TODO: support other texture types!
                                    pTexture =
                                        VulkanGraphicsSystem::GetInstance()->GetMissingTexture(TextureType::TEXTURE_2D);
                                }
                                rImageInfo.sampler = pTexture->GetDefaultSampler();
                                rImageInfo.imageView = pTexture->GetDefaultImageView();

                                VkImageLayout newLayout;
                                if (rPassBatch.type == PassType::RENDER &&
                                    pTexture == rPassBatch.renderInfo.pDepthStencilBuffer)
                                {
                                    newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                                }
                                else
                                {
                                    newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                }

                                rImageInfo.imageLayout = newLayout;
                                rSetWrite.pImageInfo = &rImageInfo;

                                auto lastImageMemoryBarrier =
                                    VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pTexture);
                                AddTextureMemoryBarrier(pTexture, lastImageMemoryBarrier.layout, newLayout,
                                                        lastImageMemoryBarrier.accessMask, VK_ACCESS_SHADER_READ_BIT,
                                                        lastImageMemoryBarrier.stageMask,
                                                        GetVkPipelineStageFlags(rBindingLayout.stageFlags));
                            }
                            else if (rBindingLayout.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
                                     rBindingLayout.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                            {
                                auto &rBufferInfo = pBufferInfos[lastBufferInfoIdx++];
                                assert(lastBufferInfoIdx < MAX_RESOURCES);
                                assert(pBinding != nullptr);
                                assert(pBinding->pBuffer != nullptr);
                                auto buffer = GetCurrentVkBuffer(pBinding->pBuffer);
                                rBufferInfo.buffer = buffer;
                                rBufferInfo.offset = 0;
                                rBufferInfo.range = VK_WHOLE_SIZE;
                                rSetWrite.pBufferInfo = &rBufferInfo;

                                auto lastBufferMemoryBarrier =
                                    VulkanGraphicsSystem::GetInstance()->GetLastBufferMemoryBarrier(buffer);
                                AddBufferMemoryBarrier(buffer, lastBufferMemoryBarrier.accessMask,
                                                       VK_ACCESS_SHADER_READ_BIT, lastBufferMemoryBarrier.stageMask,
                                                       GetVkPipelineStageFlags(rBindingLayout.stageFlags));
                            }
                            else
                            {
                                FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled descriptor type %d",
                                                       rBindingLayout.type);
                            }
                            rSetWrite.pTexelBufferView = nullptr;
                        }
                    }

                    if (setWritesCount > 0)
                    {
                        vkUpdateDescriptorSets(VulkanGraphicsSystem::GetInstance()->GetDevice(), setWritesCount,
                                               pSetWrites, 0, nullptr);
                    }
                }
            }

            auto TransitionRenderTargetToInitialLayout = [&](const VulkanTexture *pRenderTarget) {
                if (pRenderTarget == nullptr)
                {
                    return;
                }
                VkImageLayout newLayout;
                VkAccessFlags dstAccessMask;
                VkPipelineStageFlagBits dstStageMask;
                if (IsDepthFormat(pRenderTarget->GetFormat()))
                {
                    if (rPassBatch.renderInfo.depthWrite || rPassBatch.renderInfo.stencilWrite)
                    {
                        newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    }
                    else
                    {
                        newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                    }
                    dstAccessMask =
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                }
                else
                {
                    newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                }
                auto lastImageMemoryBarrier =
                    VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryBarrier(pRenderTarget);
                if (!IsVkReadOnlyAccessFlags(lastImageMemoryBarrier.accessMask))
                {
                    AddTextureMemoryBarrier(pRenderTarget, lastImageMemoryBarrier.layout, newLayout,
                                            lastImageMemoryBarrier.accessMask, dstAccessMask,
                                            lastImageMemoryBarrier.stageMask, dstStageMask);
                }
            };

            if (rPassBatch.type == PassType::RENDER)
            {
                FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tTransiting rendertargets to initial layouts (%u)",
                                   rPassBatch.renderInfo.renderTargetCount);

                std::for_each(rPassBatch.renderInfo.ppRenderTargets,
                              rPassBatch.renderInfo.ppRenderTargets + rPassBatch.renderInfo.renderTargetCount,
                              TransitionRenderTargetToInitialLayout);
                TransitionRenderTargetToInitialLayout(rPassBatch.renderInfo.pDepthStencilBuffer);
            }

            if (rPassBatch.lastPipelineBatchIdx == lastUsedPipelineBatchIdx)
            {
                continue;
            }

            FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tRecording invoke commands");

            if (rPassBatch.type == PassType::RENDER)
            {
                VkRenderPassBeginInfo renderPassBeginInfo;
                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.pNext = nullptr;
                renderPassBeginInfo.renderPass = rPassBatch.renderInfo.renderPass;
                renderPassBeginInfo.framebuffer = rPassBatch.renderInfo.frameBuffer;
                renderPassBeginInfo.renderArea = {{0, 0}, {rPassBatch.renderInfo.width, rPassBatch.renderInfo.height}};
                renderPassBeginInfo.clearValueCount = rPassBatch.renderInfo.clearValueCount;
                renderPassBeginInfo.pClearValues = rPassBatch.renderInfo.pClearValues;

                VkSubpassBeginInfoKHR subpassBeginInfo;
                subpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO_KHR;
                subpassBeginInfo.pNext = nullptr;
                subpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;

                VkExt::vkCmdBeginRenderPass2KHR(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                                &renderPassBeginInfo, &subpassBeginInfo);
            }

            for (size_t j = lastUsedPipelineBatchIdx, jc = 0;
                 lastUsedPipelineBatchIdx < rPassBatch.lastPipelineBatchIdx; ++lastUsedPipelineBatchIdx, ++jc)
            {
                FASTCG_UNUSED(j);
                FASTCG_UNUSED(jc);

                FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\tPipeline [%zu/%zu]", jc,
                                   rPassBatch.lastPipelineBatchIdx - j);

                auto &rPipelineBatch = mPipelineBatches[lastUsedPipelineBatchIdx];

                if (rPipelineBatch.lastInvokeCommandIdx == lastUsedInvokeCommandIdx)
                {
                    continue;
                }

                auto GetVkPipelineBindPoint = [](PassType passType) {
                    switch (passType)
                    {
                    case PassType::RENDER:
                        return VK_PIPELINE_BIND_POINT_GRAPHICS;
                    case PassType::COMPUTE:
                        return VK_PIPELINE_BIND_POINT_COMPUTE;
                    default:
                        FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled pass type %d", (int)passType);
                        return (VkPipelineBindPoint)0;
                    }
                };

                auto pipelineBindPoint = GetVkPipelineBindPoint(rPassBatch.type);

                vkCmdBindPipeline(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), pipelineBindPoint,
                                  rPipelineBatch.pipeline.pipeline);

                for (size_t k = lastUsedInvokeCommandIdx, kc = 0;
                     lastUsedInvokeCommandIdx < rPipelineBatch.lastInvokeCommandIdx; ++lastUsedInvokeCommandIdx, ++kc)
                {
                    FASTCG_UNUSED(k);
                    FASTCG_UNUSED(kc);

                    FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\tInvoke [%zu/%zu]", kc,
                                       rPipelineBatch.lastInvokeCommandIdx - k);

                    const auto &rInvokeCommand = mInvokeCommands[lastUsedInvokeCommandIdx];

#if _DEBUG
                    ProccessMarkers(rInvokeCommand.lastMarkerCommandIdx);
#endif

                    // TODO: avoid binding and setting stuff unecessarily

                    if (rPassBatch.type == PassType::RENDER)
                    {
                        // set dynamic graphics pipeline attributes

                        vkCmdSetViewport(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), 0, 1,
                                         &rInvokeCommand.drawInfo.viewport);
                        vkCmdSetScissor(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), 0, 1,
                                        &rInvokeCommand.drawInfo.scissor);

                        if (rInvokeCommand.drawInfo.vertexBufferCount > 0)
                        {
                            std::vector<VkDeviceSize> offsets(rInvokeCommand.drawInfo.vertexBufferCount);
                            vkCmdBindVertexBuffers(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), 0,
                                                   rInvokeCommand.drawInfo.vertexBufferCount,
                                                   &rInvokeCommand.drawInfo.pVertexBuffers[0], &offsets[0]);
                        }
                        if (rInvokeCommand.drawInfo.indexBuffer != VK_NULL_HANDLE)
                        {
                            vkCmdBindIndexBuffer(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                                 rInvokeCommand.drawInfo.indexBuffer, 0,
                                                 VK_INDEX_TYPE_UINT32); // TODO: support other index types
                        }
                    }

                    if (rInvokeCommand.setCount > 0)
                    {
                        vkCmdBindDescriptorSets(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                                pipelineBindPoint, rPipelineBatch.pipeline.layout, 0,
                                                rInvokeCommand.setCount, rInvokeCommand.pSets, 0, nullptr);
                    }

                    switch (rPassBatch.type)
                    {
                    case PassType::RENDER:
                        switch (rInvokeCommand.drawInfo.type)
                        {
                        case DrawCommandType::INSTANCED_INDEXED:
                            vkCmdDrawIndexed(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                             rInvokeCommand.drawInfo.indexCount, rInvokeCommand.drawInfo.instanceCount,
                                             rInvokeCommand.drawInfo.firstIndex, rInvokeCommand.drawInfo.vertexOffset,
                                             rInvokeCommand.drawInfo.firstInstance);
                            break;
                        default:
                            FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled draw command type %d",
                                                   (int)rInvokeCommand.drawInfo.type);
                            break;
                        }
                        break;
                    case PassType::COMPUTE:
                        vkCmdDispatch(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                      rInvokeCommand.dispatchInfo.groupCountX, rInvokeCommand.dispatchInfo.groupCountY,
                                      rInvokeCommand.dispatchInfo.groupCountZ);
                        break;
                    default:
                        FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled pass type %d", (int)rPassBatch.type);
                        break;
                    }
                }
            }

            if (rPassBatch.type == PassType::RENDER)
            {
                VkSubpassEndInfoKHR subpassEndInfo;
                subpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO_KHR;
                subpassEndInfo.pNext = nullptr;
                VkExt::vkCmdEndRenderPass2KHR(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                              &subpassEndInfo);
            }

            auto TransitionRenderTargetToFinalLayout = [&](const auto *pRenderTarget) {
                if (pRenderTarget == nullptr)
                {
                    return;
                }
                VkImageLayout newLayout;
                VkAccessFlags accessMask;
                VkPipelineStageFlags stageMask;
                if (IsDepthFormat(pRenderTarget->GetFormat()))
                {
                    if (rPassBatch.renderInfo.depthWrite || rPassBatch.renderInfo.stencilWrite)
                    {
                        newLayout = pRenderTarget->GetRestingLayout();
                    }
                    else
                    {
                        newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                    }
                    accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    stageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
                }
                else
                {
                    newLayout = pRenderTarget->GetRestingLayout();
                    accessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    stageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                }
                VulkanGraphicsSystem::GetInstance()->NotifyImageMemoryBarrier(pRenderTarget,
                                                                              {newLayout, accessMask, stageMask});
            };

            if (rPassBatch.type == PassType::RENDER)
            {
                std::for_each(rPassBatch.renderInfo.ppRenderTargets,
                              rPassBatch.renderInfo.ppRenderTargets + rPassBatch.renderInfo.renderTargetCount,
                              TransitionRenderTargetToFinalLayout);
                TransitionRenderTargetToFinalLayout(rPassBatch.renderInfo.pDepthStencilBuffer);

                FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tTransiting rendertargets to final layouts (%zu)",
                                   rPassBatch.renderInfo.renderTargets.size());
            }
        }

        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Processing end-of-frame clears (%zu)",
                           mClearCommands.size() - lastUsedClearCommandIdx);

        ProcessClears(mClearCommands.size());

        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Processing end-of-frame copies (%zu)",
                           mCopyCommands.size() - lastUsedCopyCommandIdx);

        ProcessCopies(mCopyCommands.size());
#if _DEBUG
        ProccessMarkers(mMarkerCommands.size());
#endif

        if (mAddMemoryBarrier)
        {
            // FIXME: too broad?
            VkMemoryBarrier barrier;
            barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
            barrier.pNext = nullptr;
            barrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT | VK_ACCESS_MEMORY_READ_BIT;
            vkCmdPipelineBarrier(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
                                 VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 1, &barrier,
                                 0, nullptr, 0, nullptr);
            mAddMemoryBarrier = false;
        }

#if !defined FASTCG_DISABLE_GPU_TIMING
        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Enqueing timestamp queries");

        EnqueueTimestampQuery(mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()].end,
                              VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
#endif

        mPassBatches.resize(0);
        mPipelineBatches.resize(0);
        mInvokeCommands.resize(0);
        mCopyCommands.resize(0);
        mClearCommands.resize(0);
#if _DEBUG
        mMarkerCommands.resize(0);
#endif

        FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Context ended (context: %s)", GetName().c_str());

        mEnded = true;
    }

#if !defined FASTCG_DISABLE_GPU_TIMING
    void VulkanGraphicsContext::InitializeTimeElapsedData()
    {
        mTimeElapsedQueries.resize(VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
        memset(&mTimeElapsedQueries[0], (int)~0u,
               VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames() * sizeof(TimeElapsedQuery));
        mElapsedTimes.resize(VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
        memset(&mElapsedTimes[0], 0, VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames() * sizeof(double));
    }

    void VulkanGraphicsContext::RetrieveElapsedTime(uint32_t frame)
    {
        if (mTimeElapsedQueries[frame].start != ~0u)
        {
            uint64_t timestamps[2]{0, 0};
            vkGetQueryPoolResults(VulkanGraphicsSystem::GetInstance()->GetDevice(),
                                  VulkanGraphicsSystem::GetInstance()->GetQueryPool(frame),
                                  mTimeElapsedQueries[frame].start, 2, sizeof(timestamps), timestamps, sizeof(uint64_t),
                                  VK_QUERY_RESULT_64_BIT);

            mElapsedTimes[frame] =
                (timestamps[1] - timestamps[0]) *
                VulkanGraphicsSystem::GetInstance()->GetPhysicalDeviceProperties().limits.timestampPeriod * 1e-9;
        }
        else
        {
            mElapsedTimes[frame] = 0;
        }
    }
#endif

    double VulkanGraphicsContext::GetElapsedTime(uint32_t frame) const
    {
#if !defined FASTCG_DISABLE_GPU_TIMING
        return mElapsedTimes[frame];
#else
        return 0;
#endif
    }

    void VulkanGraphicsContext::AddBufferMemoryBarrier(VkBuffer buffer, VkAccessFlags srcAccessMask,
                                                       VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask,
                                                       VkPipelineStageFlags dstStageMask)
    {
        VkBufferMemoryBarrier bufferMemoryBarrier;
        bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        bufferMemoryBarrier.pNext = nullptr;
        bufferMemoryBarrier.srcAccessMask = srcAccessMask;
        bufferMemoryBarrier.dstAccessMask = dstAccessMask;
        bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferMemoryBarrier.buffer = buffer;
        bufferMemoryBarrier.offset = 0;
        bufferMemoryBarrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), srcStageMask, dstStageMask,
                             0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);

        VulkanGraphicsSystem::GetInstance()->NotifyBufferMemoryBarrier(buffer, {dstAccessMask, dstStageMask});
    }

    void VulkanGraphicsContext::AddTextureMemoryBarrier(const VulkanTexture *pTexture, VkImageLayout oldLayout,
                                                        VkImageLayout newLayout, VkAccessFlags srcAccessMask,
                                                        VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask,
                                                        VkPipelineStageFlags dstStageMask)
    {
        VkImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
        imageMemoryBarrier.oldLayout = oldLayout;
        imageMemoryBarrier.newLayout = newLayout;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = pTexture->GetImage();
        imageMemoryBarrier.subresourceRange.aspectMask = pTexture->GetAspectFlags();
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imageMemoryBarrier.subresourceRange.levelCount = (uint32_t)pTexture->GetMipCount();
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.layerCount = pTexture->GetSlices();

        vkCmdPipelineBarrier(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), srcStageMask, dstStageMask,
                             0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);

        VulkanGraphicsSystem::GetInstance()->NotifyImageMemoryBarrier(pTexture,
                                                                      {newLayout, dstAccessMask, dstStageMask});
    }

    void VulkanGraphicsContext::EnqueueTimestampQuery(uint32_t query, VkPipelineStageFlagBits pipelineStage)
    {
        vkCmdWriteTimestamp(
            VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), pipelineStage,
            VulkanGraphicsSystem::GetInstance()->GetQueryPool(VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()),
            query);
    }
}

#endif