#include <FastCG/RenderBatchStrategy.h>

#include <algorithm>

namespace
{
    struct RenderBatchComparer
    {
        bool operator()(const FastCG::RenderBatch &rLhs, const FastCG::RenderBatch &rRhs) const
        {
            return rLhs.type < rRhs.type;
        }
    };

}

namespace FastCG
{
    void RenderBatchStrategy::AddRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);

        if (pRenderable->GetMesh() == nullptr)
        {
            return;
        }

        if (pRenderable->IsShadowCaster())
        {
            AddToShadowCastersRenderBatch(pRenderable);
        }

        if (pRenderable->GetMaterial() == nullptr)
        {
            return;
        }

        AddToMaterialBasedRenderBatch(pRenderable);
    }

    void RenderBatchStrategy::RemoveRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);

        if (pRenderable->GetMesh() == nullptr)
        {
            return;
        }

        if (pRenderable->IsShadowCaster())
        {
            RemoveFromShadowCastersRenderBatch(pRenderable);
        }

        if (pRenderable->GetMaterial() == nullptr)
        {
            return;
        }

        RemoveFromMaterialBasedRenderBatch(pRenderable);
    }

    void RenderBatchStrategy::AddToShadowCastersRenderBatch(const Renderable *pRenderable)
    {
        AddToRenderBatch(*mRenderBatches.begin(), pRenderable);
    }

    void RenderBatchStrategy::RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable)
    {
        RemoveFromRenderBatch(*mRenderBatches.begin(), pRenderable);
    }

    void RenderBatchStrategy::AddToMaterialBasedRenderBatch(const Renderable *pRenderable)
    {
        const auto *pMaterial = pRenderable->GetMaterial();
        auto it = std::find_if(mRenderBatches.begin(), mRenderBatches.end(), [pMaterial](const auto &rRenderBatch)
                               { return rRenderBatch.pMaterial == pMaterial; });
        if (it == mRenderBatches.end())
        {
            mRenderBatches.emplace_back(RenderBatch{pMaterial->GetGraphicsContextState().blend ? RenderBatchType::TRANSPARENT_MATERIAL : RenderBatchType::OPAQUE_MATERIAL, pMaterial});
            it = std::prev(mRenderBatches.end());
            AddToRenderBatch(*it, pRenderable);
            std::sort(mRenderBatches.begin(), mRenderBatches.end(), RenderBatchComparer());
        }
        else
        {
            AddToRenderBatch(*it, pRenderable);
        }
    }

    void RenderBatchStrategy::RemoveFromMaterialBasedRenderBatch(const Renderable *pRenderable)
    {
        const auto *pMaterial = pRenderable->GetMaterial();
        auto it = std::find_if(mRenderBatches.begin(), mRenderBatches.end(), [pMaterial](const auto &rRenderBatch)
                               { return rRenderBatch.pMaterial == pMaterial; });
        if (it != mRenderBatches.end())
        {
            RemoveFromRenderBatch(*it, pRenderable);
        }
    }

    void RenderBatchStrategy::AddToRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable)
    {
        auto &rRenderablesPerMesh = rRenderBatch.renderablesPerMesh;
        const auto *pMesh = pRenderable->GetMesh();
        auto it = rRenderablesPerMesh.find(pMesh);
        if (it == rRenderablesPerMesh.end())
        {
            it = rRenderablesPerMesh.emplace(pMesh, std::vector<const Renderable *>{}).first;
        }
        it->second.emplace_back(pRenderable);
    }

    inline bool RenderBatchStrategy::RemoveFromRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable)
    {
        auto &rRenderablesPerMesh = rRenderBatch.renderablesPerMesh;
        const auto *pMesh = pRenderable->GetMesh();
        auto renderablesPerMeshIt = rRenderablesPerMesh.find(pMesh);
        if (renderablesPerMeshIt == rRenderablesPerMesh.end())
        {
            return false;
        }
        auto &rRenderables = renderablesPerMeshIt->second;
        auto renderableIt = std::find(rRenderables.cbegin(), rRenderables.cend(), pRenderable);
        assert(renderableIt != rRenderables.cend());
        rRenderables.erase(renderableIt);
        if (rRenderables.empty())
        {
            auto renderBatchIt = std::find_if(mRenderBatches.begin(), mRenderBatches.end(), [&rRenderBatch](const auto &rOtherRenderBatch)
                                              { return &rRenderBatch == &rOtherRenderBatch; });
            assert(renderBatchIt != mRenderBatches.end());
            if (renderBatchIt->type != RenderBatchType::SHADOW_CASTERS)
            {
                mRenderBatches.erase(renderBatchIt);
            }
        }
        return true;
    }
}