#ifndef FASTCG_RENDER_BATCH_STRATEGY_H
#define FASTCG_RENDER_BATCH_STRATEGY_H

#include <FastCG/RenderBatch.h>
#include <FastCG/Renderable.h>

#include <vector>
#include <initializer_list>
#include <algorithm>

namespace FastCG
{
	class RenderBatchStrategy
	{
	public:
		inline const std::vector<RenderBatch> &GetRenderBatches() const
		{
			return mRenderBatches;
		}

		void AddRenderable(const Renderable *pRenderable);
		void RemoveRenderable(const Renderable *pRenderable);

	private:
		std::vector<RenderBatch> mRenderBatches{{RenderBatchType::SHADOW_CASTERS}};

		void AddToShadowCastersRenderBatch(const std::initializer_list<const Renderable *> &rRenderablesToAdd);
		void RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable);
		inline auto FindMaterialBasedRenderBatch(const Material *pMaterial)
		{
			return std::find_if(mRenderBatches.begin(), mRenderBatches.end(), [pMaterial](const auto &rRenderBatch)
								{ return rRenderBatch.type == RenderBatchType::MATERIAL_BASED && rRenderBatch.pMaterial == pMaterial; });
		}
		void AddToMaterialBasedRenderBatch(const Material *pMaterial, const std::initializer_list<const Renderable *> &rRenderablesToAdd);
		void RemoveFromMaterialBasedRenderBatch(const Material *pMaterial, const Renderable *pRenderable);
		void AddToRenderBatch(RenderBatch &rRenderBatch, const std::initializer_list<const Renderable *> &rRenderablesToAdd);
		bool RemoveFromRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable);
	};

}

#endif