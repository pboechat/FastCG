#include <FastCG/MaterialBasedRenderBatchStrategy.h>

#include <cassert>

namespace FastCG
{
	void MaterialBasedRenderBatchStrategy::AddRenderable(const Renderable *pRenderable)
	{
		const auto *pMaterial = pRenderable->GetMaterial();
		assert(pMaterial != nullptr);

		const RenderBatch *pRenderBatch;
		if (FindRenderBatchByMaterial(pMaterial, pRenderBatch))
		{
			AddToRenderBatch(pRenderBatch, {pRenderable});
		}
		else
		{
			AddRenderBatch(pMaterial, {pRenderable});
		}
	}

	void MaterialBasedRenderBatchStrategy::RemoveRenderable(const Renderable *pRenderable)
	{
		const auto *pMaterial = pRenderable->GetMaterial();
		assert(pMaterial != nullptr);

		const RenderBatch *pRenderBatch;
		auto found = FindRenderBatchByMaterial(pMaterial, pRenderBatch);
		assert(found);
		auto removed = RemoveFromRenderBatch(pRenderBatch, pRenderable);
		assert(removed);
	}

}