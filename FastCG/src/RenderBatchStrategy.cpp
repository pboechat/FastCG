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
        if (it != mRenderBatches.end())
        {
            AddToRenderBatch(*it, rRenderablesToAdd);
        }
        else
        {
            mRenderBatches.emplace_back(RenderBatch{RenderBatchType::MATERIAL_BASED, pMaterial, rRenderablesToAdd});
        }
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
        auto &rRenderables = rRenderBatch.renderables;
        rRenderables.insert(rRenderables.end(), rRenderablesToAdd.begin(), rRenderablesToAdd.end());
    }

    inline bool RenderBatchStrategy::RemoveFromRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable)
    {
        auto &rRenderables = rRenderBatch.renderables;
        auto it = std::find(rRenderables.cbegin(), rRenderables.cend(), pRenderable);
        if (it == rRenderables.cend())
        {
            return false;
        }
        rRenderables.erase(it);
        return true;
    }
}