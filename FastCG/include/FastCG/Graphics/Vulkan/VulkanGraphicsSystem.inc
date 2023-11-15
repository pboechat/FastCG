namespace FastCG
{
    void VulkanGraphicsSystem::DestroyBuffer(const VulkanBuffer *pBuffer)
    {
        assert(pBuffer != nullptr);
        mDeferredDestroyRequests.emplace_back(DeferredDestroyRequest{mCurrentFrame, pBuffer});
    }

    void VulkanGraphicsSystem::DestroyShader(const VulkanShader *pShader)
    {
        assert(pShader != nullptr);
        mDeferredDestroyRequests.emplace_back(DeferredDestroyRequest{mCurrentFrame, pShader});
    }

    void VulkanGraphicsSystem::DestroyTexture(const VulkanTexture *pTexture)
    {
        assert(pTexture != nullptr);
        {
            auto it = mLastImageMemoryTransitions.find(pTexture->GetImage());
            if (it != mLastImageMemoryTransitions.end())
            {
                mLastImageMemoryTransitions.erase(it);
            }
        }
        {
            auto it = mRenderTargetToFrameBufferHash.find(pTexture->GetImage());
            if (it != mRenderTargetToFrameBufferHash.end())
            {
                DestroyFrameBuffer(it->second); // IMPORTANT: 'it' becomes unusable after this!
            }
        }
        mDeferredDestroyRequests.emplace_back(DeferredDestroyRequest{mCurrentFrame, pTexture});
    }

    void VulkanGraphicsSystem::DestroyFrameBuffer(size_t frameBufferHash)
    {
        auto it1 = mFrameBuffers.find(frameBufferHash);
        assert(it1 != mFrameBuffers.end());
        auto it2 = mFrameBufferHashToRenderTargets.find(frameBufferHash);
        assert(it2 != mFrameBufferHashToRenderTargets.end());
        for (auto image : it2->second)
        {
            auto it3 = mRenderTargetToFrameBufferHash.find(image);
            assert(it3 != mRenderTargetToFrameBufferHash.end());
            mRenderTargetToFrameBufferHash.erase(it3);
        }
        mFrameBufferHashToRenderTargets.erase(it2);
        auto frameBuffer = it1->second;
        mFrameBuffers.erase(it1);
        mDeferredDestroyRequests.emplace_back(DeferredDestroyRequest{mCurrentFrame, frameBuffer});
    }

    VulkanImageMemoryTransition VulkanGraphicsSystem::GetLastImageMemoryTransition(const VulkanTexture *pTexture) const
    {
        assert(pTexture != nullptr);
        auto it = mLastImageMemoryTransitions.find(pTexture->GetImage());
        if (it == mLastImageMemoryTransitions.end())
        {
            return {VK_IMAGE_LAYOUT_UNDEFINED, 0, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};
        }
        else
        {
            return it->second;
        }
    }

    void VulkanGraphicsSystem::NotifyImageMemoryTransition(const VulkanTexture *pTexture, const VulkanImageMemoryTransition &rImageMemoryTransition)
    {
        assert(pTexture != nullptr);
        auto image = pTexture->GetImage();
        assert(image != VK_NULL_HANDLE);
        mLastImageMemoryTransitions[image] = rImageMemoryTransition;
    }

    VkInstance VulkanGraphicsSystem::GetVulkanInstance() const
    {
        return mInstance;
    }

    VkDevice VulkanGraphicsSystem::GetDevice() const
    {
        return mDevice;
    }

    VmaAllocator VulkanGraphicsSystem::GetAllocator() const
    {
        return mAllocator;
    }

    const VulkanTexture *VulkanGraphicsSystem::GetCurrentSwapChainTexture() const
    {
        return mSwapChainTextures[mSwapChainIndex];
    }

    VulkanGraphicsContext *VulkanGraphicsSystem::GetImmediateGraphicsContext() const
    {
        return mpImmediateGraphicsContext;
    }

    VkCommandBuffer VulkanGraphicsSystem::GetCurrentCommandBuffer() const
    {
        return mCommandBuffers[mCurrentFrame];
    }

    VkAllocationCallbacks *VulkanGraphicsSystem::GetAllocationCallbacks() const
    {
        return mAllocationCallbacks.get();
    }

    const VkFormatProperties *VulkanGraphicsSystem::GetFormatProperties(VkFormat format) const
    {
        assert(format <= LAST_FORMAT);
        return &mFormatProperties[format];
    }

    const VkPhysicalDeviceProperties &VulkanGraphicsSystem::GetPhysicalDeviceProperties() const
    {
        return mPhysicalDeviceProperties;
    }

    VkQueryPool VulkanGraphicsSystem::GetCurrentQueryPool() const
    {
        return mQueryPools[mCurrentFrame];
    }

    uint32_t VulkanGraphicsSystem::NextQuery()
    {
        // TODO: make this less brittle and possibly dynamic
        assert(mNextQueries[mCurrentFrame] < 128);
        return mNextQueries[mCurrentFrame]++;
    }

}