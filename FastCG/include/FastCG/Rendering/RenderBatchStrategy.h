#ifndef FASTCG_RENDER_BATCH_STRATEGY_H
#define FASTCG_RENDER_BATCH_STRATEGY_H

#include <FastCG/Rendering/Material.h>
#include <FastCG/Rendering/Mesh.h>
#include <FastCG/Rendering/RenderingUtils.h>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace FastCG
{
    class Renderable;

    struct RenderBatch
    {
        RenderGroup group;
        std::shared_ptr<Material> pMaterial{nullptr};
        std::unordered_map<std::shared_ptr<Mesh>, std::vector<const Renderable *>> renderablesPerMesh{};
    };

    class RenderBatchStrategy final
    {
    public:
        using RenderBatches = std::vector<RenderBatch>;

        inline const RenderBatch &GetShadowCastersRenderBatch() const
        {
            return *GetShadowCastersRenderBatchIterator();
        }
        inline const RenderBatch &GetSkyboxRenderBatch() const
        {
            return *GetSkyboxRenderBatchIterator();
        }
        inline RenderBatches::const_iterator GetFirstOpaqueMaterialRenderBatchIterator() const
        {
            return mRenderBatches.begin() + (RenderGroupInt)RenderGroup::OPAQUE_MATERIAL;
        }
        inline RenderBatches::const_iterator GetFirstTransparentMaterialRenderBatchIterator() const
        {
            return std::find_if(mRenderBatches.cbegin(), mRenderBatches.cend(), [](const auto &rRenderBatch) {
                return rRenderBatch.group == RenderGroup::TRANSPARENT_MATERIAL;
            });
        }
        inline RenderBatches::const_iterator GetLastRenderBatchIterator() const
        {
            return mRenderBatches.cend();
        }

        void AddRenderable(const Renderable *pRenderable);
        void RemoveRenderable(const Renderable *pRenderable);

    private:
        RenderBatches mRenderBatches{{RenderGroup::SHADOW_CASTERS}, {RenderGroup::SKYBOX}, {RenderGroup::RESERVED}};

        inline RenderBatches::iterator GetShadowCastersRenderBatchIterator()
        {
            return mRenderBatches.begin() + (RenderGroupInt)RenderGroup::SHADOW_CASTERS;
        }
        inline RenderBatches::const_iterator GetShadowCastersRenderBatchIterator() const
        {
            return mRenderBatches.cbegin() + (RenderGroupInt)RenderGroup::SHADOW_CASTERS;
        }
        inline RenderBatches::iterator GetSkyboxRenderBatchIterator()
        {
            return mRenderBatches.begin() + (RenderGroupInt)RenderGroup::SKYBOX;
        }
        inline RenderBatches::const_iterator GetSkyboxRenderBatchIterator() const
        {
            return mRenderBatches.cbegin() + (RenderGroupInt)RenderGroup::SKYBOX;
        }
        inline RenderBatches::iterator GetMaterialRenderBatchIterator(const std::shared_ptr<Material> &rpMaterial)
        {
            return std::find_if(mRenderBatches.begin(), mRenderBatches.end(), [&rpMaterial](const auto &rRenderBatch) {
                return rRenderBatch.pMaterial == rpMaterial;
            });
        }
        void AddToShadowCastersRenderBatch(const Renderable *pRenderable);
        void RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable);
        void AddToSkyboxRenderBatch(const Renderable *pRenderable);
        void RemoveFromSkyboxRenderBatch(const Renderable *pRenderable);
        void AddToMaterialRenderBatch(const Renderable *pRenderable);
        void RemoveFromMaterialRenderBatch(const Renderable *pRenderable);
        void AddToRenderBatch(const RenderBatches::iterator &rRenderBatchIt, const Renderable *pRenderable);
        bool RemoveFromRenderBatch(const RenderBatches::iterator &rRenderBatchIt, const Renderable *pRenderable);
    };

}

#endif