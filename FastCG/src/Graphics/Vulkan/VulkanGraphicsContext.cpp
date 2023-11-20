#ifdef FASTCG_VULKAN

#include <FastCG/Platform/Application.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsContext.h>
#include <FastCG/Graphics/Vulkan/VulkanGraphicsSystem.h>
#include <FastCG/Graphics/Vulkan/VulkanExceptions.h>
#include <FastCG/Core/Log.h>

#include <limits>
#include <cstring>
#include <algorithm>

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
		InitializeTimeElapsedData();
	}

	VulkanGraphicsContext::~VulkanGraphicsContext() = default;

	void VulkanGraphicsContext::Begin()
	{
		assert(mEnded);
		mRenderPassDescription.renderTargets.resize(0);
		mRenderPassDescription.pDepthStencilBuffer = nullptr;
		memset(&mPipelineDescription, 0, sizeof(mPipelineDescription));
		mViewport = {};
		mScissor = {};
		mVertexBuffers.resize(0);
		mpIndexBuffer = VK_NULL_HANDLE;
		mPipelineResourcesUsage.resize(0);
#if !defined FASTCG_DISABLE_GPU_TIMING
		if (mTimeElapsedQueries.size() != VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames())
		{
			InitializeTimeElapsedData();
		}
		mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()] = {VulkanGraphicsSystem::GetInstance()->NextQuery(), VulkanGraphicsSystem::GetInstance()->NextQuery()};
		EnqueueTimestampQuery(mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()].start);
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
		mViewport = VkViewport{(float)x, (float)y, (float)width, (float)height, 0, 1};
	}

	void VulkanGraphicsContext::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
	{
		mScissor = VkRect2D{x, y, width, height};
	}

	void VulkanGraphicsContext::SetBlend(bool blend)
	{
		mPipelineDescription.blend = blend;
	}

	void VulkanGraphicsContext::SetBlendFunc(BlendFunc color, BlendFunc alpha)
	{
		mPipelineDescription.blendState.colorOp = color;
		mPipelineDescription.blendState.alphaOp = alpha;
	}

	void VulkanGraphicsContext::SetBlendFactors(BlendFactor srcColor, BlendFactor dstColor, BlendFactor srcAlpha, BlendFactor dstAlpha)
	{
		mPipelineDescription.blendState.srcColorFactor = srcColor;
		mPipelineDescription.blendState.dstColorFactor = dstColor;
		mPipelineDescription.blendState.srcAlphaFactor = srcAlpha;
		mPipelineDescription.blendState.dstAlphaFactor = dstAlpha;
	}

	void VulkanGraphicsContext::SetStencilTest(bool stencilTest)
	{
		mPipelineDescription.stencilTest = stencilTest;
	}

	void VulkanGraphicsContext::SetStencilFunc(Face face, CompareOp stencilFunc, int32_t ref, uint32_t mask)
	{
		assert(face == Face::FRONT || face == Face::BACK || face == Face::FRONT_AND_BACK);
		if (face == Face::FRONT || face == Face::FRONT_AND_BACK)
		{
			mPipelineDescription.stencilFrontState.compareOp = stencilFunc;
			mPipelineDescription.stencilFrontState.reference = ref;
			mPipelineDescription.stencilFrontState.compareMask = mask;
		}
		if (face == Face::BACK || face == Face::FRONT_AND_BACK)
		{
			mPipelineDescription.stencilBackState.compareOp = stencilFunc;
			mPipelineDescription.stencilBackState.reference = ref;
			mPipelineDescription.stencilBackState.compareMask = mask;
		}
	}

	void VulkanGraphicsContext::SetStencilOp(Face face, StencilOp stencilFail, StencilOp depthFail, StencilOp depthPass)
	{
		assert(face == Face::FRONT || face == Face::BACK || face == Face::FRONT_AND_BACK);
		if (face == Face::FRONT || face == Face::FRONT_AND_BACK)
		{
			mPipelineDescription.stencilFrontState.stencilFailOp = stencilFail;
			mPipelineDescription.stencilFrontState.depthFailOp = depthFail;
			mPipelineDescription.stencilFrontState.passOp = depthPass;
		}
		if (face == Face::BACK || face == Face::FRONT_AND_BACK)
		{
			mPipelineDescription.stencilBackState.stencilFailOp = stencilFail;
			mPipelineDescription.stencilBackState.depthFailOp = depthFail;
			mPipelineDescription.stencilBackState.passOp = depthPass;
		}
	}

	void VulkanGraphicsContext::SetStencilWriteMask(Face face, uint32_t mask)
	{
		if (face == Face::FRONT || face == Face::FRONT_AND_BACK)
		{
			mPipelineDescription.stencilFrontState.writeMask = mask;
		}
		if (face == Face::BACK || face == Face::FRONT_AND_BACK)
		{
			mPipelineDescription.stencilBackState.writeMask = mask;
		}
	}

	void VulkanGraphicsContext::SetDepthTest(bool depthTest)
	{
		mPipelineDescription.depthTest = depthTest;
	}

	void VulkanGraphicsContext::SetDepthWrite(bool depthWrite)
	{
		mPipelineDescription.depthWrite = depthWrite;
	}

	void VulkanGraphicsContext::SetDepthFunc(CompareOp depthFunc)
	{
		mPipelineDescription.depthFunc = depthFunc;
	}

	void VulkanGraphicsContext::SetScissorTest(bool scissorTest)
	{
		mPipelineDescription.scissorTest = scissorTest;
	}

	void VulkanGraphicsContext::SetCullMode(Face face)
	{
		mPipelineDescription.cullMode = face;
	}

	void VulkanGraphicsContext::Copy(const VulkanBuffer *pBuffer, size_t dataSize, const void *pData)
	{
		assert(pBuffer != nullptr);
		uint32_t frameIndex;
		if (pBuffer->IsMultiFrame())
		{
			frameIndex = VulkanGraphicsSystem::GetInstance()->GetCurrentFrame();
		}
		else
		{
			frameIndex = 0;
		}
		Copy(pBuffer, frameIndex, dataSize, pData);
	}

	void VulkanGraphicsContext::Copy(const VulkanBuffer *pBuffer, uint32_t frameIndex, size_t dataSize, const void *pData)
	{
		assert(pBuffer != nullptr);
		assert(dataSize > 0);
		assert(pData != nullptr);

		auto &rBufferFrameData = pBuffer->GetFrameData(frameIndex);
		assert(rBufferFrameData.allocation != VK_NULL_HANDLE);

		if (pBuffer->UsesMappableMemory())
		{
			void *mappedData;
			FASTCG_CHECK_VK_RESULT(vmaMapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
												rBufferFrameData.allocation,
												&mappedData));
			memcpy(mappedData, pData, dataSize);
			vmaUnmapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
						   rBufferFrameData.allocation);

			VkMemoryPropertyFlags memPropFlags;
			vmaGetAllocationMemoryProperties(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
											 rBufferFrameData.allocation,
											 &memPropFlags);
			if ((memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
			{
				FASTCG_CHECK_VK_RESULT(vmaFlushAllocation(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
														  rBufferFrameData.allocation,
														  0,
														  dataSize));
			}
		}
		else
		{
			auto *pStagingBuffer = VulkanGraphicsSystem::GetInstance()->CreateBuffer({"Staging Buffer for " + pBuffer->GetName() + (frameIndex > 0 ? " (" + std::to_string(frameIndex) + ")" : ""),
																					  BufferUsageFlagBit::DYNAMIC,
																					  dataSize,
																					  pData,
																					  {},
																					  true});

			EnqueueCopyCommand(CopyCommandType::BUFFER_TO_BUFFER, CopyCommandArgs{{pStagingBuffer, 0}, {pBuffer, frameIndex}});

			VulkanGraphicsSystem::GetInstance()->DestroyBuffer(pStagingBuffer);
		}
	}

	void VulkanGraphicsContext::Copy(const VulkanTexture *pTexture, size_t dataSize, const void *pData)
	{
		assert(pTexture != nullptr);
		assert(pTexture->GetAllocation() != VK_NULL_HANDLE);
		assert(dataSize > 0);
		assert(pData != nullptr);

		if (pTexture->UsesMappableMemory())
		{
			void *mappedData;
			FASTCG_CHECK_VK_RESULT(vmaMapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
												pTexture->GetAllocation(),
												&mappedData));
			memcpy(mappedData, pData, dataSize);
			vmaUnmapMemory(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
						   pTexture->GetAllocation());

			VkMemoryPropertyFlags memPropFlags;
			vmaGetAllocationMemoryProperties(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
											 pTexture->GetAllocation(),
											 &memPropFlags);
			if ((memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
			{
				FASTCG_CHECK_VK_RESULT(vmaFlushAllocation(VulkanGraphicsSystem::GetInstance()->GetAllocator(),
														  pTexture->GetAllocation(),
														  0,
														  VK_WHOLE_SIZE));
			}
		}
		else
		{
			auto *pStagingBuffer = VulkanGraphicsSystem::GetInstance()->CreateBuffer({"Staging Buffer for " + pTexture->GetName(),
																					  BufferUsageFlagBit::DYNAMIC,
																					  dataSize,
																					  pData,
																					  {},
																					  true});

			EnqueueCopyCommand(CopyCommandType::BUFFER_TO_IMAGE, CopyCommandArgs{{pStagingBuffer, 0}, {pTexture}});

			VulkanGraphicsSystem::GetInstance()->DestroyBuffer(pStagingBuffer);
		}
	}

	void VulkanGraphicsContext::BindShader(const VulkanShader *pShader)
	{
		assert(pShader != nullptr);
		if (mPipelineDescription.pShader == pShader)
		{
			return;
		}

		mPipelineDescription.pShader = pShader;
		mPipelineResourcesUsage.resize(0);
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
		if (mPipelineResourcesUsage.size() <= location.set)
		{
			mPipelineResourcesUsage.resize(location.set + 1);
		}
		auto &rDescriptorSet = mPipelineResourcesUsage[location.set];
		if (rDescriptorSet.size() <= location.binding)
		{
			rDescriptorSet.resize(location.binding + 1);
		}
		auto &rBinding = rDescriptorSet[location.binding];
		rBinding.pBuffer = pBuffer;
	}

	void VulkanGraphicsContext::BindResource(const VulkanTexture *pTexture, const char *pName)
	{
		assert(pTexture != nullptr);
		assert(pName != nullptr);
		assert(mPipelineDescription.pShader != nullptr);
		auto location = mPipelineDescription.pShader->GetResourceLocation(pName);
		if (location.set == ~0u && location.binding == ~0u)
		{
			return;
		}
		if (mPipelineResourcesUsage.size() <= location.set)
		{
			mPipelineResourcesUsage.resize(location.set + 1);
		}
		auto &rDescriptorSet = mPipelineResourcesUsage[location.set];
		if (rDescriptorSet.size() <= location.binding)
		{
			rDescriptorSet.resize(location.binding + 1);
		}
		auto &rBinding = rDescriptorSet[location.binding];
		rBinding.pTexture = pTexture;
	}

	void VulkanGraphicsContext::Blit(const VulkanTexture *pSrc, const VulkanTexture *pDst)
	{
		assert(pSrc != nullptr);
		assert(pDst != nullptr);
		EnqueueCopyCommand(CopyCommandType::BLIT, CopyCommandArgs{{pSrc}, {pDst}});
	}

	void VulkanGraphicsContext::ClearRenderTarget(uint32_t renderTargetIndex, const glm::vec4 &rClearColor)
	{
		assert(renderTargetIndex < mRenderPassDescription.renderTargets.size());
		auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [&](const auto &rClearRequest)
							   { return rClearRequest.pTexture == mRenderPassDescription.renderTargets[renderTargetIndex]; });
		if (it == mClearRequests.end())
		{
			it = mClearRequests.emplace(mClearRequests.end());
			it->pTexture = mRenderPassDescription.renderTargets[renderTargetIndex];
		}
		it->value.color.float32[0] = rClearColor.r;
		it->value.color.float32[1] = rClearColor.g;
		it->value.color.float32[2] = rClearColor.b;
		it->value.color.float32[3] = rClearColor.a;
		it->flags = VulkanClearRequestFlagBit::COLOR_OR_DEPTH;
	}

	void VulkanGraphicsContext::ClearDepthStencilBuffer(float depth, int32_t stencil)
	{
		assert(mRenderPassDescription.pDepthStencilBuffer != nullptr);
		auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [&](const auto &rClearRequest)
							   { return rClearRequest.pTexture == mRenderPassDescription.pDepthStencilBuffer; });
		if (it == mClearRequests.end())
		{
			it = mClearRequests.emplace(mClearRequests.end());
			it->pTexture = mRenderPassDescription.pDepthStencilBuffer;
		}
		it->value.depthStencil.depth = depth;
		it->value.depthStencil.stencil = (uint32_t)stencil;
		it->flags = VulkanClearRequestFlagBit::COLOR_OR_DEPTH | VulkanClearRequestFlagBit::STENCIL;
	}

	void VulkanGraphicsContext::ClearDepthBuffer(float depth)
	{
		assert(mRenderPassDescription.pDepthStencilBuffer != nullptr);
		auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [&](const auto &rClearRequest)
							   { return rClearRequest.pTexture == mRenderPassDescription.pDepthStencilBuffer; });
		if (it == mClearRequests.end())
		{
			it = mClearRequests.emplace(mClearRequests.end());
			it->pTexture = mRenderPassDescription.pDepthStencilBuffer;
		}
		it->value.depthStencil.depth = depth;
		it->flags = VulkanClearRequestFlagBit::COLOR_OR_DEPTH;
	}

	void VulkanGraphicsContext::ClearStencilBuffer(int32_t stencil)
	{
		assert(mRenderPassDescription.pDepthStencilBuffer != nullptr);
		auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [&](const auto &rClearRequest)
							   { return rClearRequest.pTexture == mRenderPassDescription.pDepthStencilBuffer; });
		if (it == mClearRequests.end())
		{
			it = mClearRequests.emplace(mClearRequests.end());
			it->pTexture = mRenderPassDescription.pDepthStencilBuffer;
		}
		it->value.depthStencil.stencil = (uint32_t)stencil;
		it->flags = VulkanClearRequestFlagBit::STENCIL;
	}

	void VulkanGraphicsContext::SetRenderTargets(const VulkanTexture *const *pRenderTargets, uint32_t renderTargetCount, const VulkanTexture *pDepthStencilBuffer)
	{
		mRenderPassDescription.renderTargets.resize(renderTargetCount);
		for (uint32_t i = 0; i < renderTargetCount; ++i)
		{
			auto *pRenderTarget = pRenderTargets[i];
			if (pRenderTarget == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
			{
				pRenderTarget = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
			}
			mRenderPassDescription.renderTargets[i] = pRenderTarget;
		}
		mRenderPassDescription.pDepthStencilBuffer = pDepthStencilBuffer;
	}

	void VulkanGraphicsContext::SetVertexBuffers(const VulkanBuffer *const *pBuffers, uint32_t bufferCount)
	{
		assert(pBuffers != nullptr);
		mVertexBuffers.resize(bufferCount);
		memcpy(&mVertexBuffers[0], pBuffers, bufferCount * sizeof(const VulkanBuffer *));
	}

	void VulkanGraphicsContext::SetIndexBuffer(const VulkanBuffer *pBuffer)
	{
		assert(pBuffer != nullptr);
		mpIndexBuffer = GetCurrentVkBuffer(pBuffer);
		assert(mpIndexBuffer != VK_NULL_HANDLE);
	}

	void VulkanGraphicsContext::DrawIndexed(PrimitiveType primitiveType, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
	{
		EnqueueDrawCommand(DrawCommandType::INSTANCED_INDEXED, primitiveType, 0, 1, firstIndex, indexCount, vertexOffset);
	}

	void VulkanGraphicsContext::DrawInstancedIndexed(PrimitiveType primitiveType, uint32_t firstInstance, uint32_t instanceCount, uint32_t firstIndex, uint32_t indexCount, int32_t vertexOffset)
	{
		EnqueueDrawCommand(DrawCommandType::INSTANCED_INDEXED, primitiveType, firstInstance, instanceCount, firstIndex, indexCount, vertexOffset);
	}

	void VulkanGraphicsContext::EnqueueCopyCommand(CopyCommandType type, const CopyCommandArgs &rArgs)
	{
		mCopyCommands.emplace_back(CopyCommand {
#if _DEBUG
				mMarkerCommands.size(),
#endif
				type, 
				rArgs });
	}

	void VulkanGraphicsContext::EnqueueDrawCommand(DrawCommandType type,
												   PrimitiveType primitiveType,
												   uint32_t firstInstance,
												   uint32_t instanceCount,
												   uint32_t firstIndex,
												   uint32_t indexCount,
												   int32_t vertexOffset)
	{
		assert(mPipelineDescription.pShader != nullptr);

		auto &rFragmentOutputDescription = mPipelineDescription.pShader->GetFragmentOutputDescription();
		auto actualRenderPassDescription = mRenderPassDescription;
		for (size_t i = 0; i < actualRenderPassDescription.renderTargets.size(); ++i)
		{
			auto it = rFragmentOutputDescription.find((uint32_t)i);
			if (it == rFragmentOutputDescription.end())
			{
				actualRenderPassDescription.renderTargets[i] = nullptr;
			}
		}

		bool depthStencilWrite = mPipelineDescription.depthWrite || (mPipelineDescription.stencilTest && (mPipelineDescription.stencilBackState.writeMask != 0 || mPipelineDescription.stencilFrontState.writeMask != 0));

		auto renderPass = VulkanGraphicsSystem::GetInstance()->GetOrCreateRenderPass(actualRenderPassDescription, mClearRequests, depthStencilWrite).second;
		assert(renderPass != VK_NULL_HANDLE);

		auto frameBuffer = VulkanGraphicsSystem::GetInstance()->GetOrCreateFrameBuffer(actualRenderPassDescription, mClearRequests, depthStencilWrite).second;
		assert(frameBuffer != VK_NULL_HANDLE);

		RenderPassCommand *pRenderPassCommand;
		if (mRenderPassCommands.empty() || mRenderPassCommands.back().renderPass != renderPass)
		{
			mRenderPassCommands.emplace_back();
			pRenderPassCommand = &mRenderPassCommands.back();
			pRenderPassCommand->renderPass = renderPass;
			pRenderPassCommand->frameBuffer = frameBuffer;
			pRenderPassCommand->width = std::numeric_limits<uint32_t>::max();
			pRenderPassCommand->height = std::numeric_limits<uint32_t>::max();
			pRenderPassCommand->renderTargets.resize(0);
			pRenderPassCommand->clearValues.resize(0);
			pRenderPassCommand->depthStencilWrite = depthStencilWrite;

			auto CalculateRenderArea = [pRenderPassCommand](const VulkanTexture *pRenderTarget)
			{
				pRenderPassCommand->width = std::min(pRenderPassCommand->width, pRenderTarget->GetWidth());
				pRenderPassCommand->height = std::min(pRenderPassCommand->height, pRenderTarget->GetHeight());
			};

			for (auto *pRenderTarget : actualRenderPassDescription.renderTargets)
			{
				if (pRenderTarget == nullptr)
				{
					continue;
				}

				CalculateRenderArea(pRenderTarget);

				auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [pRenderTarget](const auto &rClearRequest)
									   { return rClearRequest.pTexture == pRenderTarget; });
				if (it != mClearRequests.end())
				{
					pRenderPassCommand->clearValues.emplace_back(it->value);
				}
				else
				{
					pRenderPassCommand->clearValues.emplace_back();
				}

				pRenderPassCommand->renderTargets.emplace_back(pRenderTarget);
			}

			if (actualRenderPassDescription.pDepthStencilBuffer != nullptr)
			{
				CalculateRenderArea(actualRenderPassDescription.pDepthStencilBuffer);

				auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [&actualRenderPassDescription](const auto &rClearRequest)
									   { return rClearRequest.pTexture == actualRenderPassDescription.pDepthStencilBuffer; });
				if (it != mClearRequests.end())
				{
					pRenderPassCommand->clearValues.emplace_back(it->value);
				}
				else
				{
					pRenderPassCommand->clearValues.emplace_back();
				}

				pRenderPassCommand->pDepthStencilBuffer = actualRenderPassDescription.pDepthStencilBuffer;
			}
		}
		else
		{
			pRenderPassCommand = &mRenderPassCommands.back();
		}

		uint32_t renderTargetCount = 0;
		for (const auto *pRenderTarget : actualRenderPassDescription.renderTargets)
		{
			if (pRenderTarget == nullptr)
			{
				continue;
			}

			renderTargetCount++;
		}

		if (mPrevRenderPass != VK_NULL_HANDLE && mPrevRenderPass != renderPass)
		{
			// erase clear requests only if switching renderpasses for reasons other
			// than indirect attachments clears. this avoids the unnecessary creation/starting
			// of renderpasses due to those clears.
			for (const auto *pRenderTarget : actualRenderPassDescription.renderTargets)
			{
				if (pRenderTarget == nullptr)
				{
					continue;
				}

				auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [pRenderTarget](const auto &rClearRequest)
									   { return rClearRequest.pTexture == pRenderTarget; });
				if (it != mClearRequests.end())
				{
					mClearRequests.erase(it);
				}
			}

			if (actualRenderPassDescription.pDepthStencilBuffer != nullptr)
			{
				auto it = std::find_if(mClearRequests.begin(), mClearRequests.end(), [&actualRenderPassDescription](const auto &rClearRequest)
									   { return rClearRequest.pTexture == actualRenderPassDescription.pDepthStencilBuffer; });
				if (it != mClearRequests.end())
				{
					mClearRequests.erase(it);
				}
			}
		}

		mPrevRenderPass = renderPass;

		auto pipeline = VulkanGraphicsSystem::GetInstance()->GetOrCreatePipeline(mPipelineDescription, renderPass, renderTargetCount, mVertexBuffers).second;
		assert(pipeline.pipeline != VK_NULL_HANDLE);

		PipelineCommand *pPipelineCommand;
		if (mPipelineCommands.empty() || mPipelineCommands.back().pipeline.pipeline != pipeline.pipeline)
		{
			mPipelineCommands.emplace_back();
			pPipelineCommand = &mPipelineCommands.back();
			pPipelineCommand->pipeline = pipeline;
			pPipelineCommand->pipelineResourcesLayout = mPipelineDescription.pShader->GetLayoutDescription();
		}
		else
		{
			pPipelineCommand = &mPipelineCommands.back();
		}

		pRenderPassCommand->lastCopyCommandIdx = mCopyCommands.size();
		pRenderPassCommand->lastPipelineCommandIdx = mPipelineCommands.size();

		mDrawCommands.emplace_back();
		auto *pDrawCommand = &mDrawCommands.back();

		pPipelineCommand->lastDrawCommandIdx = mDrawCommands.size();

#if _DEBUG
		pDrawCommand->lastMarkerCommandIdx = mMarkerCommands.size();
#endif

		pDrawCommand->primitiveType = primitiveType;
		pDrawCommand->firstInstance = firstInstance;
		pDrawCommand->instanceCount = instanceCount;
		pDrawCommand->firstIndex = firstIndex;
		pDrawCommand->indexCount = indexCount;
		pDrawCommand->vertexOffset = vertexOffset;
		pDrawCommand->viewport = {mViewport.x, mViewport.height - mViewport.y, mViewport.width, -mViewport.height, mViewport.minDepth, mViewport.maxDepth}; // flip the viewport y axis
		// pDrawCommand->viewport = {mViewport.x, mViewport.y, mViewport.width, mViewport.height, mViewport.minDepth, mViewport.maxDepth};
		if (mScissor.extent.width == 0 || mScissor.extent.height == 0)
		{
			pDrawCommand->scissor = {(int32_t)mViewport.x, (int32_t)mViewport.y, (uint32_t)mViewport.width, (uint32_t)mViewport.height};
		}
		else
		{
			pDrawCommand->scissor = mScissor;
		}
		pDrawCommand->pipelineResourcesUsage = mPipelineResourcesUsage;
		pDrawCommand->vertexBuffers.resize(mVertexBuffers.size());
		for (size_t i = 0; i < mVertexBuffers.size(); ++i)
		{
			pDrawCommand->vertexBuffers[i] = GetCurrentVkBuffer(mVertexBuffers[i]);
		}
		pDrawCommand->pIndexBuffer = mpIndexBuffer;
	}

	void VulkanGraphicsContext::End()
	{
		assert(!mEnded);

		FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Ending context (context: %s)", GetName().c_str());

		constexpr size_t MAX_WRITES = 128;
		constexpr size_t MAX_RESOURCES = MAX_WRITES;

		VkWriteDescriptorSet descriptorSetWrites[MAX_WRITES];
		VkDescriptorImageInfo descriptorImageInfos[MAX_RESOURCES];
		VkDescriptorBufferInfo descriptorBufferInfos[MAX_RESOURCES];

#if _DEBUG
		size_t lastMarkerCommandIdx = 0;
		auto ProccessMarkers = [&](size_t end)
		{
			for (; lastMarkerCommandIdx < end; ++lastMarkerCommandIdx)
			{
				auto &rMarkerCommand = mMarkerCommands[lastMarkerCommandIdx];
				switch (rMarkerCommand.type)
				{
				case MarkerCommandType::PUSH:
					VulkanGraphicsSystem::GetInstance()->PushDebugMarker(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), rMarkerCommand.name.c_str());
					break;
				case MarkerCommandType::POP:
					VulkanGraphicsSystem::GetInstance()->PopDebugMarker(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer());
					break;
				default:
					FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled marker command type %d", (int)rMarkerCommand.type);
					break;
				}
			}
		};
#endif

		size_t lastUsedCopyCommandIdx = 0;
		auto ProcessCopies = [&](size_t end)
		{
			for (size_t i = lastUsedCopyCommandIdx, ic = 0; lastUsedCopyCommandIdx < end; ++lastUsedCopyCommandIdx, ++ic)
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
				case CopyCommandType::BUFFER_TO_BUFFER:
				{
					FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Buffer %s -> Buffer %s", rCopyCommand.args.srcBufferData.pBuffer->GetName().c_str(), rCopyCommand.args.dstBufferData.pBuffer->GetName().c_str());
					VkBufferCopy copyRegion;
					copyRegion.srcOffset = 0;
					copyRegion.dstOffset = 0;
					copyRegion.size = rCopyCommand.args.srcBufferData.pBuffer->GetDataSize();
					auto &rSrcBufferFrameData = rCopyCommand.args.srcBufferData.pBuffer->GetFrameData(rCopyCommand.args.srcBufferData.frameIndex);
					auto &rDstBufferFrameData = rCopyCommand.args.dstBufferData.pBuffer->GetFrameData(rCopyCommand.args.dstBufferData.frameIndex);
					vkCmdCopyBuffer(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
									rSrcBufferFrameData.buffer,
									rDstBufferFrameData.buffer,
									1,
									&copyRegion);

					AddBufferMemoryBarrier(rSrcBufferFrameData.buffer,
										   VK_ACCESS_TRANSFER_WRITE_BIT,
										   rCopyCommand.args.dstBufferData.pBuffer->GetDefaultAccessFlags(),
										   VK_PIPELINE_STAGE_TRANSFER_BIT,
										   rCopyCommand.args.dstBufferData.pBuffer->GetDefaultStageFlags());
				}
				break;
				case CopyCommandType::BUFFER_TO_IMAGE:
				{
					auto *pDstTexture = rCopyCommand.args.dstTextureData.pTexture;
					if (pDstTexture == VulkanGraphicsSystem::GetInstance()->GetBackbuffer())
					{
						pDstTexture = VulkanGraphicsSystem::GetInstance()->GetCurrentSwapChainTexture();
					}

					FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Buffer %s -> Image %s", rCopyCommand.args.srcBufferData.pBuffer->GetName().c_str(), pDstTexture->GetName().c_str());

					AddTextureMemoryBarrier(pDstTexture,
											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											VK_ACCESS_TRANSFER_WRITE_BIT,
											pDstTexture->GetDefaultAccessFlags(),
											VK_PIPELINE_STAGE_TRANSFER_BIT,
											VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT);

					// TODO: support array and mipped textures
					VkBufferImageCopy copyRegion;
					copyRegion.bufferOffset = 0;
					copyRegion.bufferRowLength = 0;
					copyRegion.bufferImageHeight = 0;
					copyRegion.imageSubresource.aspectMask = pDstTexture->GetAspectFlags();
					copyRegion.imageSubresource.mipLevel = 0;
					copyRegion.imageSubresource.baseArrayLayer = 0;
					copyRegion.imageSubresource.layerCount = 1;
					copyRegion.imageOffset = {0, 0, 0};
					copyRegion.imageExtent = {pDstTexture->GetWidth(), pDstTexture->GetHeight(), 1};

					vkCmdCopyBufferToImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
										   rCopyCommand.args.srcBufferData.pBuffer->GetFrameData(rCopyCommand.args.srcBufferData.frameIndex).buffer,
										   pDstTexture->GetImage(),
										   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
										   1,
										   &copyRegion);
				}
				break;
				case CopyCommandType::IMAGE_TO_IMAGE:
				{
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

					FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Image %s -> Image %s (Copy)", pSrcTexture->GetName().c_str(), pDstTexture->GetName().c_str());

					AddTextureMemoryBarrier(pSrcTexture,
											VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
											0,
											VK_ACCESS_TRANSFER_READ_BIT,
											VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
											VK_PIPELINE_STAGE_TRANSFER_BIT);
					AddTextureMemoryBarrier(pDstTexture,
											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											0,
											VK_ACCESS_TRANSFER_WRITE_BIT,
											VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
											VK_PIPELINE_STAGE_TRANSFER_BIT);

					// TODO: support array, mipped and 3D textures
					VkImageCopy imageCopyRegion;
					imageCopyRegion.srcSubresource.aspectMask = pSrcTexture->GetAspectFlags();
					imageCopyRegion.srcSubresource.baseArrayLayer = 0;
					imageCopyRegion.srcSubresource.layerCount = 1;
					imageCopyRegion.srcSubresource.mipLevel = 0;
					imageCopyRegion.srcOffset = {0, 0, 0};
					imageCopyRegion.dstSubresource.aspectMask = pDstTexture->GetAspectFlags();
					imageCopyRegion.dstSubresource.baseArrayLayer = 0;
					imageCopyRegion.dstSubresource.layerCount = 1;
					imageCopyRegion.dstSubresource.mipLevel = 0;
					imageCopyRegion.dstOffset = {0, 0, 0};
					imageCopyRegion.extent.width = pSrcTexture->GetWidth();
					imageCopyRegion.extent.height = pSrcTexture->GetHeight();
					imageCopyRegion.extent.depth = 1;

					vkCmdCopyImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
								   pSrcTexture->GetImage(),
								   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
								   pDstTexture->GetImage(),
								   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								   1,
								   &imageCopyRegion);
				}
				break;
				case CopyCommandType::BLIT:
				{
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

					FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t Image %s -> Image %s (Blit)", pSrcTexture->GetName().c_str(), pDstTexture->GetName().c_str());

					AddTextureMemoryBarrier(pSrcTexture,
											VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
											0,
											VK_ACCESS_TRANSFER_READ_BIT,
											VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
											VK_PIPELINE_STAGE_TRANSFER_BIT);
					AddTextureMemoryBarrier(pDstTexture,
											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											0,
											VK_ACCESS_TRANSFER_WRITE_BIT,
											VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
											VK_PIPELINE_STAGE_TRANSFER_BIT);

					VkImageBlit imageBlit{};
					imageBlit.srcSubresource.aspectMask = pSrcTexture->GetAspectFlags();
					imageBlit.srcSubresource.baseArrayLayer = 0;
					imageBlit.srcSubresource.layerCount = 1;
					imageBlit.srcSubresource.mipLevel = 0;
					imageBlit.srcOffsets[0] = {0, 0, 0};
					imageBlit.srcOffsets[1] = {(int32_t)pSrcTexture->GetWidth(), (int32_t)pSrcTexture->GetHeight(), 1};
					imageBlit.dstSubresource.aspectMask = pDstTexture->GetAspectFlags();
					imageBlit.dstSubresource.baseArrayLayer = 0;
					imageBlit.dstSubresource.layerCount = 1;
					imageBlit.dstSubresource.mipLevel = 0;
					imageBlit.dstOffsets[0] = {0, 0, 0};
					imageBlit.dstOffsets[1] = {(int32_t)pDstTexture->GetWidth(), (int32_t)pDstTexture->GetHeight(), 1};

					vkCmdBlitImage(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
								   pSrcTexture->GetImage(),
								   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
								   pDstTexture->GetImage(),
								   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								   1,
								   &imageBlit,
								   GetVkFilter(pSrcTexture->GetFilter()));
				}
				break;
				default:
					FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled copy command type %d", (int)rCopyCommand.type);
					continue;
				}
			}
		};

		size_t lastUsedPipelineCommandIdx = 0;
		size_t lastUsedDrawCommandIdx = 0;

		FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Iterating over renderpasses (%zu)", mRenderPassCommands.size());

		for (size_t i = 0; i < mRenderPassCommands.size(); ++i)
		{
			FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tRenderpass [%zu/%zu]", i, mRenderPassCommands.size() - 1);

			auto &rRenderPassCommand = mRenderPassCommands[i];

			FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tProcessing pre-renderpass copies (%zu)", rRenderPassCommand.lastCopyCommandIdx - lastUsedCopyCommandIdx);

			ProcessCopies(rRenderPassCommand.lastCopyCommandIdx);

			FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tSetting up pipeline resources (%zu)", rRenderPassCommand.lastPipelineCommandIdx - lastUsedPipelineCommandIdx);

			for (size_t j = lastUsedPipelineCommandIdx, jc = 0; j < rRenderPassCommand.lastPipelineCommandIdx; ++j, ++jc)
			{
				FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\tPipeline [%zu/%zu]", jc, rRenderPassCommand.lastPipelineCommandIdx - lastUsedPipelineCommandIdx);

				auto &rPipelineCommand = mPipelineCommands[j];

				for (size_t k = lastUsedDrawCommandIdx, kc = 0; k < rPipelineCommand.lastDrawCommandIdx; ++k, ++kc)
				{
					FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\tDraw [%zu/%zu]", kc, rPipelineCommand.lastDrawCommandIdx - lastUsedDrawCommandIdx);

					auto &rDrawCommand = mDrawCommands[k];

					rDrawCommand.descriptorSets.resize(rPipelineCommand.pipelineResourcesLayout.size());

					uint32_t descriptorSetWritesCount = 0;
					uint32_t lastDescriptorImageInfoIdx = 0;
					uint32_t lastDescriptorBufferInfoIdx = 0;
					for (size_t l = 0; l < rPipelineCommand.pipelineResourcesLayout.size(); ++l)
					{
						FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\t\tPipeline resource [%zu/%zu]", l, rPipelineCommand.pipelineResourcesLayout.size() - 1);

						auto &rDescriptorSetLayout = rPipelineCommand.pipelineResourcesLayout[l];
						auto &rDescriptorSetUsage = rDrawCommand.pipelineResourcesUsage[l];
						auto &rDescriptorSet = rDrawCommand.descriptorSets[l];
						if (rDescriptorSetLayout.empty())
						{
							rDescriptorSet = VK_NULL_HANDLE;
							continue;
						}
						rDescriptorSet = VulkanGraphicsSystem::GetInstance()->GetOrCreateDescriptorSet(rDescriptorSetLayout).second;
						for (size_t m = 0; m < rDescriptorSetLayout.size(); ++m)
						{
							auto &rBindingLayout = rDescriptorSetLayout[m];
							auto *pBinding = rDescriptorSetUsage.size() > rBindingLayout.binding ? &rDescriptorSetUsage[rBindingLayout.binding] : nullptr;
							auto &rDescriptorSetWrite = descriptorSetWrites[descriptorSetWritesCount++];
							rDescriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							rDescriptorSetWrite.pNext = nullptr;
							rDescriptorSetWrite.descriptorCount = 1;
							rDescriptorSetWrite.descriptorType = rBindingLayout.type;
							rDescriptorSetWrite.dstArrayElement = 0;
							rDescriptorSetWrite.dstBinding = rBindingLayout.binding;
							rDescriptorSetWrite.dstSet = rDescriptorSet;
							if (rBindingLayout.type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
							{
								auto &rDescriptorImageInfo = descriptorImageInfos[lastDescriptorImageInfoIdx++];
								assert(lastDescriptorImageInfoIdx < MAX_RESOURCES);
								const VulkanTexture *pTexture;
								if (pBinding != nullptr)
								{
									pTexture = pBinding->pTexture;
								}
								else
								{
									// TODO: support other texture types!
									pTexture = VulkanGraphicsSystem::GetInstance()->GetMissingTexture(TextureType::TEXTURE_2D);
								}
								rDescriptorImageInfo.sampler = pTexture->GetDefaultSampler();
								rDescriptorImageInfo.imageView = pTexture->GetDefaultImageView();

								VkImageLayout newLayout;
								if (pTexture == rRenderPassCommand.pDepthStencilBuffer)
								{
									assert(!rRenderPassCommand.depthStencilWrite); // read-only
									newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
								}
								else
								{
									assert(std::find(rRenderPassCommand.renderTargets.begin(), rRenderPassCommand.renderTargets.end(), pTexture) == rRenderPassCommand.renderTargets.end());
									newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
								}

								rDescriptorImageInfo.imageLayout = newLayout;
								rDescriptorSetWrite.pImageInfo = &rDescriptorImageInfo;

								auto imageMemoryTransition = VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryTransition(pTexture);
								if (imageMemoryTransition.layout != newLayout)
								{
									AddTextureMemoryBarrier(pTexture,
															newLayout,
															imageMemoryTransition.accessMask,
															VK_ACCESS_SHADER_READ_BIT,
															imageMemoryTransition.stageMask,
															VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
								}
							}
							else if (rBindingLayout.type == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ||
									 rBindingLayout.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
							{
								auto &rDescriptorBufferInfo = descriptorBufferInfos[lastDescriptorBufferInfoIdx++];
								assert(lastDescriptorBufferInfoIdx < MAX_RESOURCES);
								assert(pBinding != nullptr);
								assert(pBinding->pBuffer != nullptr);
								rDescriptorBufferInfo.buffer = GetCurrentVkBuffer(pBinding->pBuffer);
								rDescriptorBufferInfo.offset = 0;
								rDescriptorBufferInfo.range = VK_WHOLE_SIZE;
								rDescriptorSetWrite.pBufferInfo = &rDescriptorBufferInfo;
							}
							else
							{
								FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled descriptor type %d", rBindingLayout.type);
							}
							rDescriptorSetWrite.pTexelBufferView = nullptr;
						}
					}

					if (descriptorSetWritesCount > 0)
					{
						vkUpdateDescriptorSets(VulkanGraphicsSystem::GetInstance()->GetDevice(),
											   descriptorSetWritesCount,
											   descriptorSetWrites,
											   0,
											   nullptr);
					}
				}
			}

			FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tTransiting rendertargets to initial layouts (%zu)", rRenderPassCommand.renderTargets.size());

			auto TransitionRenderTargetToInitialLayout = [&](const VulkanTexture *pRenderTarget)
			{
				if (pRenderTarget == nullptr)
				{
					return;
				}
				VkImageLayout newLayout;
				VkAccessFlags dstAccessMask;
				VkPipelineStageFlagBits dstStageMask;
				if (IsDepthFormat(pRenderTarget->GetFormat()))
				{
					if (rRenderPassCommand.depthStencilWrite)
					{
						newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
					}
					else
					{
						newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
					}
					dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
					dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
				}
				else
				{
					newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
					dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				}
				auto imageMemoryTransition = VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryTransition(pRenderTarget);
				if (imageMemoryTransition.layout != newLayout)
				{
					AddTextureMemoryBarrier(pRenderTarget,
											newLayout,
											imageMemoryTransition.accessMask,
											dstAccessMask,
											imageMemoryTransition.stageMask,
											dstStageMask);
				}
			};

			std::for_each(rRenderPassCommand.renderTargets.begin(), rRenderPassCommand.renderTargets.end(), TransitionRenderTargetToInitialLayout);
			TransitionRenderTargetToInitialLayout(rRenderPassCommand.pDepthStencilBuffer);

			if (rRenderPassCommand.lastPipelineCommandIdx == lastUsedPipelineCommandIdx)
			{
				continue;
			}

			FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tRecording draw commands");

			VkRenderPassBeginInfo renderPassBeginInfo;
			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.pNext = nullptr;
			renderPassBeginInfo.renderPass = rRenderPassCommand.renderPass;
			renderPassBeginInfo.framebuffer = rRenderPassCommand.frameBuffer;
			renderPassBeginInfo.renderArea = {{0, 0}, {rRenderPassCommand.width, rRenderPassCommand.height}};
			renderPassBeginInfo.clearValueCount = (uint32_t)rRenderPassCommand.clearValues.size();
			renderPassBeginInfo.pClearValues = rRenderPassCommand.clearValues.empty() ? nullptr : &rRenderPassCommand.clearValues[0];

			VkSubpassBeginInfoKHR subpassBeginInfo;
			subpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO_KHR;
			subpassBeginInfo.pNext = nullptr;
			subpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;

			VkExt::vkCmdBeginRenderPass2KHR(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
											&renderPassBeginInfo,
											&subpassBeginInfo);

			for (size_t j = lastUsedPipelineCommandIdx, jc = 0; lastUsedPipelineCommandIdx < rRenderPassCommand.lastPipelineCommandIdx; ++lastUsedPipelineCommandIdx, ++jc)
			{
				FASTCG_UNUSED(j);
				FASTCG_UNUSED(jc);

				FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\tPipeline [%zu/%zu]", jc, rRenderPassCommand.lastPipelineCommandIdx - j);

				auto &rPipelineCommand = mPipelineCommands[lastUsedPipelineCommandIdx];

				if (rPipelineCommand.lastDrawCommandIdx == lastUsedDrawCommandIdx)
				{
					continue;
				}

				vkCmdBindPipeline(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
								  VK_PIPELINE_BIND_POINT_GRAPHICS,
								  rPipelineCommand.pipeline.pipeline);

				for (size_t k = lastUsedDrawCommandIdx, kc = 0; lastUsedDrawCommandIdx < rPipelineCommand.lastDrawCommandIdx; ++lastUsedDrawCommandIdx, ++kc)
				{
					FASTCG_UNUSED(k);
					FASTCG_UNUSED(kc);

					FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\t\t\tDraw [%zu/%zu]", kc, rPipelineCommand.lastDrawCommandIdx - k);

					const auto &rDrawCommand = mDrawCommands[lastUsedDrawCommandIdx];

#if _DEBUG
					ProccessMarkers(rDrawCommand.lastMarkerCommandIdx);
#endif

					// TODO: avoid binding and setting stuff unecessarily

					vkCmdSetViewport(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), 0, 1, &rDrawCommand.viewport);
					vkCmdSetScissor(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), 0, 1, &rDrawCommand.scissor);

					if (!rDrawCommand.vertexBuffers.empty())
					{
						std::vector<VkDeviceSize> offsets(rDrawCommand.vertexBuffers.size());
						vkCmdBindVertexBuffers(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
											   0,
											   (uint32_t)rDrawCommand.vertexBuffers.size(),
											   &rDrawCommand.vertexBuffers[0],
											   &offsets[0]);
					}
					if (rDrawCommand.pIndexBuffer != VK_NULL_HANDLE)
					{
						vkCmdBindIndexBuffer(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
											 rDrawCommand.pIndexBuffer,
											 0,
											 VK_INDEX_TYPE_UINT32); // TODO: support other index types
					}

					if (!rDrawCommand.descriptorSets.empty())
					{
						vkCmdBindDescriptorSets(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
												VK_PIPELINE_BIND_POINT_GRAPHICS,
												rPipelineCommand.pipeline.layout,
												0,
												(uint32_t)rDrawCommand.descriptorSets.size(),
												&rDrawCommand.descriptorSets[0],
												0,
												nullptr);
					}

					switch (rDrawCommand.type)
					{
					case DrawCommandType::INSTANCED_INDEXED:
						vkCmdDrawIndexed(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
										 rDrawCommand.indexCount,
										 rDrawCommand.instanceCount,
										 rDrawCommand.firstIndex,
										 rDrawCommand.vertexOffset,
										 rDrawCommand.firstInstance);
						break;
					default:
						FASTCG_THROW_EXCEPTION(Exception, "Vulkan: Unhandled draw command type %d", (int)rDrawCommand.type);
						break;
					}
				}
			}

			VkSubpassEndInfoKHR subpassEndInfo;
			subpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO_KHR;
			subpassEndInfo.pNext = nullptr;
			VkExt::vkCmdEndRenderPass2KHR(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(), &subpassEndInfo);

			auto TransitionRenderTargetToFinalLayout = [&](const auto *pRenderTarget)
			{
				if (pRenderTarget == nullptr)
				{
					return;
				}
				VkImageLayout newLayout;
				VkAccessFlags accessMask;
				VkPipelineStageFlags stageMask;
				if (IsDepthFormat(pRenderTarget->GetFormat()))
				{
					if (rRenderPassCommand.depthStencilWrite)
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
				VulkanGraphicsSystem::GetInstance()->NotifyImageMemoryTransition(pRenderTarget, {newLayout, accessMask, stageMask});
			};

			std::for_each(rRenderPassCommand.renderTargets.begin(), rRenderPassCommand.renderTargets.end(), TransitionRenderTargetToFinalLayout);
			TransitionRenderTargetToFinalLayout(rRenderPassCommand.pDepthStencilBuffer);

			FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "\tTransiting rendertargets to final layouts (%zu)", rRenderPassCommand.renderTargets.size());
		}

		FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Processing frame-end copies (%zu)", mCopyCommands.size() - lastUsedCopyCommandIdx);

		ProcessCopies(mCopyCommands.size());
#if _DEBUG
		ProccessMarkers(mMarkerCommands.size());
#endif

#if !defined FASTCG_DISABLE_GPU_TIMING
		FASTCG_LOG_VERBOSE(VulkanGraphicsContext, "Enqueing timestamp queries");

		EnqueueTimestampQuery(mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()].end);
#endif

		mRenderPassCommands.resize(0);
		mPipelineCommands.resize(0);
		mDrawCommands.resize(0);
		mCopyCommands.resize(0);
		mClearRequests.resize(0);
		mPrevRenderPass = VK_NULL_HANDLE;
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
		memset(&mTimeElapsedQueries[0], (int)~0u, VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames() * sizeof(TimeElapsedQuery));
		mElapsedTimes.resize(VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames());
		memset(&mElapsedTimes[0], 0, VulkanGraphicsSystem::GetInstance()->GetMaxSimultaneousFrames() * sizeof(double));
	}

	void VulkanGraphicsContext::RetrieveElapsedTime()
	{
		assert(mEnded);

		if (mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()].start != ~0u)
		{
			uint64_t timestamps[2];
			vkGetQueryPoolResults(VulkanGraphicsSystem::GetInstance()->GetDevice(),
								  VulkanGraphicsSystem::GetInstance()->GetCurrentQueryPool(),
								  mTimeElapsedQueries[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()].start,
								  2,
								  sizeof(timestamps),
								  timestamps,
								  sizeof(uint64_t),
								  VK_QUERY_RESULT_64_BIT);

			mElapsedTimes[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()] = (timestamps[1] - timestamps[0]) * VulkanGraphicsSystem::GetInstance()->GetPhysicalDeviceProperties().limits.timestampPeriod * 1e-9;
		}
		else
		{
			mElapsedTimes[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()] = 0;
		}
	}
#endif

	double VulkanGraphicsContext::GetElapsedTime() const
	{
#if !defined FASTCG_DISABLE_GPU_TIMING
		return mElapsedTimes[VulkanGraphicsSystem::GetInstance()->GetCurrentFrame()];
#else
		return 0;
#endif
	}

	void VulkanGraphicsContext::AddBufferMemoryBarrier(VkBuffer buffer,
													   VkAccessFlags srcAccessMask,
													   VkAccessFlags dstAccessMask,
													   VkPipelineStageFlags srcStageMask,
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

		vkCmdPipelineBarrier(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
							 srcStageMask,
							 dstStageMask,
							 0,
							 0,
							 nullptr,
							 1,
							 &bufferMemoryBarrier,
							 0,
							 nullptr);
	}

	void VulkanGraphicsContext::AddTextureMemoryBarrier(const VulkanTexture *pTexture,
														VkImageLayout newLayout,
														VkAccessFlags srcAccessMask,
														VkAccessFlags dstAccessMask,
														VkPipelineStageFlags srcStageMask,
														VkPipelineStageFlags dstStageMask)
	{
		VkImageMemoryBarrier imageMemoryBarrier;
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;
		imageMemoryBarrier.srcAccessMask = srcAccessMask;
		imageMemoryBarrier.dstAccessMask = dstAccessMask;
		imageMemoryBarrier.oldLayout = VulkanGraphicsSystem::GetInstance()->GetLastImageMemoryTransition(pTexture).layout;
		imageMemoryBarrier.newLayout = newLayout;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = pTexture->GetImage();
		imageMemoryBarrier.subresourceRange.aspectMask = pTexture->GetAspectFlags();
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
							 srcStageMask,
							 dstStageMask,
							 0,
							 0,
							 nullptr,
							 0,
							 nullptr,
							 1,
							 &imageMemoryBarrier);

		VulkanGraphicsSystem::GetInstance()->NotifyImageMemoryTransition(pTexture, {newLayout, dstAccessMask, dstStageMask});
	}

	void VulkanGraphicsContext::EnqueueTimestampQuery(uint32_t query)
	{
		vkCmdWriteTimestamp(VulkanGraphicsSystem::GetInstance()->GetCurrentCommandBuffer(),
							VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
							VulkanGraphicsSystem::GetInstance()->GetCurrentQueryPool(),
							query);
	}
}

#endif