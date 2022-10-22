#ifndef FASTCG_MATERIAL_BASED_RENDER_BATCH_STRATEGY_H
#define FASTCG_MATERIAL_BASED_RENDER_BATCH_STRATEGY_H

#include <FastCG/RenderBatchStrategy.h>

namespace FastCG
{
	class MaterialBasedRenderBatchStrategy : public RenderBatchStrategy
	{
	public:
		void AddRenderable(const Renderable *pRenderable) override;
		void RemoveRenderable(const Renderable *pRenderable) override;
	};

}

#endif