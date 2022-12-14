#include <FastCG/RenderBatchStrategy.h>

namespace FastCG
{
    void RenderBatchStrategy::AddRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);

        if (pRenderable->IsShadowCaster())
        {
            AddToShadowCastersRenderBatch({pRenderable});
        }

        const auto *pMaterial = pRenderable->GetMaterial();
        if (pMaterial != nullptr)
        {
            AddToMaterialBasedRenderBatch(pMaterial, {pRenderable});
        }
    }

    void RenderBatchStrategy::RemoveRenderable(const Renderable *pRenderable)
    {
        assert(pRenderable != nullptr);

        const auto *pMaterial = pRenderable->GetMaterial();
        if (pMaterial != nullptr)
        {
            RemoveFromMaterialBasedRenderBatch(pMaterial, pRenderable);
        }

        if (pRenderable->IsShadowCaster())
        {
            RemoveFromShadowCastersRenderBatch(pRenderable);
        }
    }

    void RenderBatchStrategy::AddToShadowCastersRenderBatch(const std::initializer_list<const Renderable *> &rRenderablesToAdd)
    {
        AddToRenderBatch(mRenderBatches[0], rRenderablesToAdd);
    }

    void RenderBatchStrategy::RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable)
    {
        RemoveFromRenderBatch(mRenderBatches[0], pRenderable);
    }

    void RenderBatchStrategy::AddToMaterialBasedRenderBatch(const Material *pMaterial, const std::initializer_list<const Renderable *> &rRenderablesToAdd)
    {
        auto it = FindMaterialBasedRenderBatch(pMaterial);
        if (it == mRenderBatches.end())
        {
            mRenderBatches.emplace_back(RenderBatch{RenderBatchType::MATERIAL_BASED, pMaterial});
            it = mRenderBatches.end() - 1;
        }
        AddToRenderBatch(*it, rRenderablesToAdd);
    }

    void RenderBatchStrategy::RemoveFromMaterialBasedRenderBatch(const Material *pMaterial, const Renderable *pRenderable)
    {
        auto it = FindMaterialBasedRenderBatch(pMaterial);
        if (it != mRenderBatches.end())
        {
            auto removed = RemoveFromRenderBatch(*it, pRenderable);
            assert(removed);
        }
    }

    void RenderBatchStrategy::AddToRenderBatch(RenderBatch &rRenderBatch, const std::initializer_list<const Renderable *> &rRenderablesToAdd)
    {
        auto &rRenderablesPerMesh = rRenderBatch.renderablesPerMesh;
        for (const auto *pRenderable : rRenderablesToAdd)
        {
            const auto *pMesh = pRenderable->GetMesh();
            auto it = rRenderablesPerMesh.find(pMesh);
            if (it == rRenderablesPerMesh.end())
            {
                it = rRenderablesPerMesh.emplace(pMesh, std::vector<const Renderable *>{}).first;
            }
            it->second.emplace_back(pRenderable);
        }
    }

    inline bool RenderBatchStrategy::RemoveFromRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable)
    {
        auto &rRenderablesPerMesh = rRenderBatch.renderablesPerMesh;
        const auto *pMesh = pRenderable->GetMesh();
        auto renderablesPerMeshIt = rRenderablesPerMesh.find(pMesh);
        assert(renderablesPerMeshIt != rRenderablesPerMesh.end());
        auto& rRenderables = renderablesPerMeshIt->second;
        auto renderableIt = std::find(rRenderables.cbegin(), rRenderables.cend(), pRenderable);
        assert(renderableIt != rRenderables.cend());
        rRenderables.erase(renderableIt);
        return true;
    }
}