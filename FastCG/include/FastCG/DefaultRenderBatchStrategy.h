#ifndef FASTCG_DEFAULT_RENDER_BATCH_STRATEGY_H
#define FASTCG_DEFAULT_RENDER_BATCH_STRATEGY_H

#include <FastCG/RenderBatchStrategy.h>

namespace FastCG
{
	class DefaultRenderBatchStrategy : public RenderBatchStrategy
	{
	public:
		void AddRenderable(const Renderable *pRenderable) override;
		void RemoveRenderable(const Renderable *pRenderable) override;
	};

}

#endif