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
		virtual ~RenderBatchStrategy()
		{
			for (const auto *pRenderBatch : mRenderBatches)
			{
				delete pRenderBatch;
			}
			mRenderBatches.clear();
		}

		inline const std::vector<const RenderBatch *> &GetRenderBatches() const
		{
			return mRenderBatches;
		}

		virtual void AddRenderable(const Renderable *pRenderable) = 0;
		virtual void RemoveRenderable(const Renderable *pRenderable) = 0;

	protected:
		inline bool FindRenderBatchByMaterial(const Material *pMaterial, const RenderBatch *&rpRenderBatch) const
		{
			auto it = std::find_if(mRenderBatches.cbegin(), mRenderBatches.cend(), [pMaterial](const auto *pRenderBatch)
								   { return pRenderBatch->pMaterial == pMaterial; });
			if (it == mRenderBatches.cend())
			{
				return false;
			}
			rpRenderBatch = *it;
			return true;
		}

		inline void AddRenderBatch(const Material *pMaterial, const std::initializer_list<const Renderable *> &rRenderables)
		{
			mRenderBatches.emplace_back(new RenderBatch{pMaterial, rRenderables});
		}

		inline void AddToRenderBatch(const RenderBatch *pRenderBatch, const std::initializer_list<const Renderable *> &rRenderablesToAdd)
		{
			auto &rRenderables = const_cast<RenderBatch *>(pRenderBatch)->renderables;
			rRenderables.insert(rRenderables.end(), rRenderablesToAdd.begin(), rRenderablesToAdd.end());
		}

		inline bool RemoveFromRenderBatch(const RenderBatch *pRenderBatch, const Renderable *pRenderable)
		{
			auto &rRenderables = const_cast<RenderBatch *>(pRenderBatch)->renderables;
			auto it = std::find(rRenderables.cbegin(), rRenderables.cend(), pRenderable);
			if (it == rRenderables.cend())
			{
				return false;
			}
			rRenderables.erase(it);
			return true;
		}

	private:
		std::vector<const RenderBatch *> mRenderBatches;
	};

}

#endif