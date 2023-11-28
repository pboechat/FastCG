#ifndef FASTCG_RENDER_BATCH_STRATEGY_H
#define FASTCG_RENDER_BATCH_STRATEGY_H

#include <FastCG/Rendering/RenderBatch.h>
#include <FastCG/Rendering/Renderable.h>

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
		std::vector<RenderBatch> mRenderBatches{{RenderGroup::SHADOW_CASTERS}, {RenderGroup::SKYBOX}, {RenderGroup::RESERVED}};

		void AddToShadowCastersRenderBatch(const Renderable *pRenderable);
		void RemoveFromShadowCastersRenderBatch(const Renderable *pRenderable);
		void AddToSkyboxRenderBatch(const Renderable *pRenderable);
		void RemoveFromSkyboxRenderBatch(const Renderable *pRenderable);
		void AddToMaterialRenderBatch(const Renderable *pRenderable);
		void RemoveFromMaterialRenderBatch(const Renderable *pRenderable);
		void AddToRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable);
		bool RemoveFromRenderBatch(RenderBatch &rRenderBatch, const Renderable *pRenderable);
	};

}

#endif