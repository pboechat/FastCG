#include <FastCG/Rendering/RenderBatchStrategy.h>

namespace
{
    FastCG::RenderGroupInt GetOrder(const FastCG::RenderBatch &rRenderBatch)
    {
        auto order = (FastCG::RenderGroupInt)rRenderBatch.group;
        if (rRenderBatch.pMaterial != nullptr)
        {
            order += rRenderBatch.pMaterial->GetOrder();
        }
        return order;
    }

    struct RenderBatchComparer
    {
        bool operator()(const FastCG::RenderBatch &rLhs, const FastCG::RenderBatch &rRhs) const
        {
            return GetOrder(rLhs) < GetOrder(rRhs);
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

        if (pRenderable->IsSkybox())
        {
            AddToSkyboxRenderBatch(pRenderable);
        }
        else
        {
            AddToMaterialRenderBatch(pRenderable);
        }
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

        if (pRenderable->IsSkybox())
        {
            RemoveFromSkyboxRenderBatch(pRenderable);
        }
        else
        {
            RemoveFromMaterialRenderBatch(pRenderable);
        }
    }

    void RenderBatchStrategy::AddToShadowCastersRenderBatch(const Renderable *pRenderable)
    {
        AddToRenderBatch(*(mRenderBatches.begin() + (RenderGroupInt)RenderGroup::SHADOW_CASTERS), pRenderable);
    }

    void RenderBatchStrategy::RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable)
    {
        RemoveFromRenderBatch(*(mRenderBatches.begin() + (RenderGroupInt)RenderGroup::SHADOW_CASTERS), pRenderable);
    }

    void RenderBatchStrategy::AddToSkyboxRenderBatch(const Renderable *pRenderable)
    {
        auto skyboxRenderBatchIt = mRenderBatches.begin() + (RenderGroupInt)RenderGroup::SKYBOX;
        if (skyboxRenderBatchIt->pMaterial != nullptr)
        {
            skyboxRenderBatchIt->renderablesPerMesh.clear();
        }
        skyboxRenderBatchIt->pMaterial = pRenderable->GetMaterial().get();
        skyboxRenderBatchIt->renderablesPerMesh.emplace(pRenderable->GetMesh().get(), std::vector<const Renderable *>{pRenderable});
    }

    void RenderBatchStrategy::RemoveFromSkyboxRenderBatch(const Renderable *pRenderable)
    {
        auto skyboxRenderBatchIt = mRenderBatches.begin() + (RenderGroupInt)RenderGroup::SKYBOX;
        skyboxRenderBatchIt->pMaterial = nullptr;
        skyboxRenderBatchIt->renderablesPerMesh.clear();
    }

    void RenderBatchStrategy::AddToMaterialRenderBatch(const Renderable *pRenderable)
    {
        const auto *pMaterial = pRenderable->GetMaterial().get();
        auto it = std::find_if(mRenderBatches.begin(), mRenderBatches.end(), [pMaterial](const auto &rRenderBatch)
                               { return rRenderBatch.pMaterial == pMaterial; });
        if (it == mRenderBatches.end())
        {
            mRenderBatches.emplace_back(RenderBatch{pMaterial->GetGraphicsContextState().blend ? RenderGroup::TRANSPARENT_MATERIAL : RenderGroup::OPAQUE_MATERIAL, pMaterial});
            it = std::prev(mRenderBatches.end());
            AddToRenderBatch(*it, pRenderable);
            std::sort(mRenderBatches.begin(), mRenderBatches.end(), RenderBatchComparer());
        }
        else
        {
            AddToRenderBatch(*it, pRenderable);
        }
    }

    void RenderBatchStrategy::RemoveFromMaterialRenderBatch(const Renderable *pRenderable)
    {
        const auto *pMaterial = pRenderable->GetMaterial().get();
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
        const auto *pMesh = pRenderable->GetMesh().get();
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
        const auto *pMesh = pRenderable->GetMesh().get();
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
            if (IsMaterialRenderGroup(renderBatchIt->group))
            {
                mRenderBatches.erase(renderBatchIt);
            }
        }
        return true;
    }
}