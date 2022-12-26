#ifndef FASTCG_RENDER_BATCH_STRATEGY_H
#define FASTCG_RENDER_BATCH_STRATEGY_H

#include <FastCG/RenderBatch.h>
#include <FastCG/Renderable.h>

#include <vector>
#include <initializer_list>

namespace FastCG
{
	class RenderBatchStrategy
	{
	public:
		inline const auto &GetRenderBatches() const
		{
			return mRenderBatches;
		}

		void AddRenderable(const Renderable *pRenderable);
		void RemoveRenderable(const Renderable *pRenderable);

	private:
		std::vector<RenderBatch> mRenderBatches{{RenderBatchType::SHADOW_CASTERS}};

		void AddToShadowCastersRenderBatch(const Renderable *pRenderable);
		void RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable);
		void AddToMaterialBasedRenderBatch(const Renderable *pRenderable);
		void RemoveFromMaterialBasedRenderBatch(const Renderable *pRenderable);
		void AddToRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable);
		bool RemoveFromRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable);
	};

}

#endif