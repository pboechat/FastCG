#include <FastCG/Rendering/RenderBatchStrategy.h>
#include <FastCG/Rendering/Renderable.h>

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
        AddToRenderBatch(GetShadowCastersRenderBatchIterator(), pRenderable);
    }

    void RenderBatchStrategy::RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable)
    {
        RemoveFromRenderBatch(GetShadowCastersRenderBatchIterator(), pRenderable);
    }

    void RenderBatchStrategy::AddToSkyboxRenderBatch(const Renderable *pRenderable)
    {
        auto skyboxRenderBatchIt = GetSkyboxRenderBatchIterator();
        if (skyboxRenderBatchIt->pMaterial != nullptr)
        {
            skyboxRenderBatchIt->renderablesPerMesh.clear();
        }
        skyboxRenderBatchIt->pMaterial = pRenderable->GetMaterial();
        skyboxRenderBatchIt->renderablesPerMesh.emplace(pRenderable->GetMesh(),
                                                        std::vector<const Renderable *>{pRenderable});
    }

    void RenderBatchStrategy::RemoveFromSkyboxRenderBatch(const Renderable *pRenderable)
    {
        auto skyboxRenderBatchIt = GetSkyboxRenderBatchIterator();
        skyboxRenderBatchIt->pMaterial = nullptr;
        skyboxRenderBatchIt->renderablesPerMesh.clear();
    }

    void RenderBatchStrategy::AddToMaterialRenderBatch(const Renderable *pRenderable)
    {
        const auto &rpMaterial = pRenderable->GetMaterial();
        auto materialRenderBatchIt = GetMaterialRenderBatchIterator(rpMaterial);
        if (materialRenderBatchIt == mRenderBatches.end())
        {
            materialRenderBatchIt =
                mRenderBatches.insert(mRenderBatches.end(), RenderBatch{rpMaterial->GetGraphicsContextState().blend
                                                                            ? RenderGroup::TRANSPARENT_MATERIAL
                                                                            : RenderGroup::OPAQUE_MATERIAL,
                                                                        rpMaterial});
            AddToRenderBatch(materialRenderBatchIt, pRenderable);
            std::sort(mRenderBatches.begin(), mRenderBatches.end(), RenderBatchComparer());
        }
        else
        {
            AddToRenderBatch(materialRenderBatchIt, pRenderable);
        }
    }

    void RenderBatchStrategy::RemoveFromMaterialRenderBatch(const Renderable *pRenderable)
    {
        const auto &rpMaterial = pRenderable->GetMaterial();
        auto materialRenderBatchIt = GetMaterialRenderBatchIterator(rpMaterial);
        assert(materialRenderBatchIt != mRenderBatches.end());
        RemoveFromRenderBatch(materialRenderBatchIt, pRenderable);
    }

    void RenderBatchStrategy::AddToRenderBatch(const RenderBatches::iterator &rRenderBatchIt,
                                               const Renderable *pRenderable)
    {
        assert(rRenderBatchIt != mRenderBatches.end());
        auto &rRenderablesPerMesh = rRenderBatchIt->renderablesPerMesh;
        const auto &rpMesh = pRenderable->GetMesh();
        auto renderablesPerMeshIt = rRenderablesPerMesh.find(rpMesh);
        if (renderablesPerMeshIt == rRenderablesPerMesh.end())
        {
            renderablesPerMeshIt = rRenderablesPerMesh.emplace(rpMesh, std::vector<const Renderable *>{}).first;
        }
        renderablesPerMeshIt->second.emplace_back(pRenderable);
    }

    inline bool RenderBatchStrategy::RemoveFromRenderBatch(const RenderBatches::iterator &rRenderBatchIt,
                                                           const Renderable *pRenderable)
    {
        assert(rRenderBatchIt != mRenderBatches.end());
        auto &rRenderablesPerMesh = rRenderBatchIt->renderablesPerMesh;
        const auto &rpMesh = pRenderable->GetMesh();
        auto renderablesPerMeshIt = rRenderablesPerMesh.find(rpMesh);
        assert(renderablesPerMeshIt != rRenderablesPerMesh.end());
        auto &rRenderables = renderablesPerMeshIt->second;
        auto renderableIt = std::find(rRenderables.cbegin(), rRenderables.cend(), pRenderable);
        assert(renderableIt != rRenderables.cend());
        rRenderables.erase(renderableIt);
        if (rRenderables.empty())
        {
            rRenderablesPerMesh.erase(renderablesPerMeshIt);
            if (rRenderablesPerMesh.empty())
            {
                if (IsMaterialRenderGroup(rRenderBatchIt->group))
                {
                    mRenderBatches.erase(rRenderBatchIt);
                }
            }
        }
        return true;
    }
}